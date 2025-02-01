interface JsonRpcError {
  code: number
  message: string
  data?: any
}

interface JsonRpcErrorResponse {
  jsonrpc: '2.0'
  id: number | string | null
  error: JsonRpcError
}

interface JsonRpcSuccessResponse<T = any> {
  jsonrpc: '2.0'
  id: number | string | null
  result: T
}

type JsonRpcResponse<T = any> = JsonRpcErrorResponse | JsonRpcSuccessResponse<T>

const BLE_SERVICE_UUID = '31415926-5358-9793-2384-626433832795'
const BLE_CHARACTERISTIC_INPUT = 'C001'
const BLE_CHARACTERISTIC_OUTPUT = 'C000'
const BLE_CHARACTERISTIC_CHUNK_SIZE = 'BB73'
const BLE_DEFAULT_CHUNK_SIZE = 20

const BLE_DEFAULT_REQUEST_OPTIONS: RequestDeviceOptions = {
  filters: [{ services: [BLE_SERVICE_UUID] }],
}

export class ColibriBleService {
  private device: BluetoothDevice | null = null
  private gatt: BluetoothRemoteGATTServer | undefined = undefined
  private bluetooth: Bluetooth | undefined = undefined
  private maxChunkSize: number = BLE_DEFAULT_CHUNK_SIZE
  private responseBuffer: string = ''

  public connected = false
  public response: JsonRpcResponse | undefined

  constructor(bluetoothModule?: Bluetooth) {
    if (typeof window !== 'undefined') {
      this.bluetooth = navigator.bluetooth
    }
    if (bluetoothModule) {
      this.bluetooth = bluetoothModule
    }
  }

  isAvailable(): boolean {
    return !!this.bluetooth?.requestDevice
  }

  isConnected(): boolean {
    this.connected = !!this.gatt?.connected
    return this.connected
  }

  async connect(options?: RequestDeviceOptions): Promise<void> {
    if (!this.isAvailable()) {
      throw new Error('Web Bluetooth API is not available')
    }

    if (this.connected) {
      throw new Error('Already connected')
    }

    if (!options) options = BLE_DEFAULT_REQUEST_OPTIONS

    try {
      const device = await this.bluetooth!.requestDevice(options)
      const gatt = await device.gatt?.connect()

      this.device = device
      this.gatt = gatt

      this.connected = true

      setTimeout(() => this.updateMaxChunkSize(), 2000)

      this.startDeviceListeners()
      this.startNotifications()
    } catch (error) {
      console.error(`Failed to connect: ${error}`)
    }

    this.connected = false
  }

  async updateMaxChunkSize(): Promise<void> {
    try {
      const chunkBytes = await this.readRaw(
        BLE_SERVICE_UUID,
        BLE_CHARACTERISTIC_CHUNK_SIZE
      )
      this.maxChunkSize = chunkBytes.getUint16(0, true)
    } catch (error) {
      console.error(error)
      this.maxChunkSize = BLE_DEFAULT_CHUNK_SIZE
    }
  }

  disconnect() {
    this.stopNotifications()
    this.stopDeviceListeners()

    if (this.gatt?.connected) {
      this.gatt.disconnect()
    }

    this.device = null
    this.gatt = undefined
    this.maxChunkSize = BLE_DEFAULT_CHUNK_SIZE
    this.response = undefined
    this.responseBuffer = ''

    this.connected = false
  }

  private async getCharacteristic(
    serviceId: string,
    characteristicId: string
  ): Promise<BluetoothRemoteGATTCharacteristic> {
    if (!this.gatt) throw new Error('No device connected')

    const service = await this.gatt.getPrimaryService(serviceId)
    const characteristic = await service.getCharacteristic(characteristicId)
    return characteristic
  }

  private stringToBuffer(str: string): ArrayBuffer {
    // we don't need encoding, only char->byte conversion
    const buffer = new ArrayBuffer(str.length)
    const view = new Uint8Array(buffer)
    for (let i = 0; i < str.length; i++) {
      view[i] = str.charCodeAt(i)
    }
    return buffer
  }

  private bufferToString(buffer: ArrayBuffer): string {
    const view = new Uint8Array(buffer)
    let str = ''
    for (let i = 0; i < view.length; i++) {
      if (view[i] != undefined) {
        str += String.fromCharCode(view[i] as number)
      }
    }
    return str.trim()
  }

  private dataViewToBuffer(dataView: DataView): ArrayBuffer {
    return dataView.buffer.slice(
      dataView.byteOffset,
      dataView.byteOffset + dataView.byteLength
    ) as ArrayBuffer
  }

  private async writeRaw(
    serviceId: string,
    characteristicId: string,
    data: ArrayBuffer
  ): Promise<void> {
    const characteristic = await this.getCharacteristic(
      serviceId,
      characteristicId
    )
    await characteristic.writeValue(data)
  }

  private async writeString(text: string): Promise<void> {
    const chunks: ArrayBuffer[] = []
    for (let i = 0; i < text.length; i += this.maxChunkSize) {
      const textChunk = text.substring(i, i + this.maxChunkSize)
      chunks.push(this.stringToBuffer(textChunk))
    }

    for (const chunk of chunks) {
      await this.writeRaw(BLE_SERVICE_UUID, BLE_CHARACTERISTIC_INPUT, chunk)
      await new Promise((resolve) => setTimeout(resolve, 100))
    }
  }

  private async writeJsonRpc(
    method: string,
    params: any[] = []
  ): Promise<JsonRpcResponse | undefined> {
    const id = Math.abs(Math.random() * 100)
    const input = {
      id,
      method,
      params,
    }
    const str = JSON.stringify(input)
    this.response = undefined

    await this.writeString(str)

    let counter = 0
    const timeout = 35
    const timeoutMax = 30_000
    while (!this.response || (this.response as JsonRpcResponse).id !== id) {
      if (counter > timeoutMax) break

      await new Promise((resolve) => setTimeout(resolve, timeout))
      counter += timeout
    }

    return this.response
  }

  async ping(): Promise<boolean> {
    const response = await this.writeJsonRpc('ping')
    return !!response
  }

  private async readRaw(
    serviceId: string,
    characteristicId: string
  ): Promise<DataView> {
    const characteristic = await this.getCharacteristic(
      serviceId,
      characteristicId
    )
    return characteristic.readValue()
  }

  private notificationHandler(event: Event) {
    const value = (event.target as BluetoothRemoteGATTCharacteristic).value
    if (value) {
      this.onNotification(value)
    }
  }

  private deviceDisconnectHandler() {
    this.disconnect()
  }

  private onNotification(data: DataView) {
    try {
      const str = this.bufferToString(this.dataViewToBuffer(data))

      if (str.startsWith('{')) {
        this.responseBuffer = ''
      }
      this.responseBuffer += str

      if (this.responseBuffer.endsWith('}')) {
        const json = JSON.parse(this.responseBuffer)
        this.response = json
        this.responseBuffer = ''
      }
    } catch (error) {
      console.error('Error handling BLE notification:', error)
    }
  }

  private async startNotifications(): Promise<void> {
    try {
      const characteristic = await this.getCharacteristic(
        BLE_SERVICE_UUID,
        BLE_CHARACTERISTIC_OUTPUT
      )

      // Start notifications
      await characteristic.startNotifications()

      // Add event listener
      characteristic.addEventListener(
        'characteristicvaluechanged',
        this.notificationHandler
      )
    } catch (error) {
      console.error('Failed to start notifications:', error)
      throw error
    }
  }

  private async stopNotifications(): Promise<void> {
    try {
      const characteristic = await this.getCharacteristic(
        BLE_SERVICE_UUID,
        BLE_CHARACTERISTIC_OUTPUT
      )

      characteristic.removeEventListener(
        'characteristicvaluechanged',
        this.notificationHandler
      )
      await characteristic.stopNotifications()
    } catch (error) {
      console.error('Failed to stop notifications:', error)
      throw error
    }
  }

  private startDeviceListeners() {
    if (!this.device) return

    this.device.addEventListener(
      'gattserverdisconnected',
      this.deviceDisconnectHandler
    )
  }

  private stopDeviceListeners() {
    if (!this.device) return

    this.device.removeEventListener(
      'gattserverdisconnected',
      this.deviceDisconnectHandler
    )
  }
}
