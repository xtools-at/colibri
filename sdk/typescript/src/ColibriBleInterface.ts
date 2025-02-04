import type { JsonRpcResponse, ColibriInterface } from './types'
import { stringToBuffer, bufferToString, dataViewToBuffer } from './utils'

const BLE_SERVICE_UUID = '31415926-5358-9793-2384-626433832795'
const BLE_CHARACTERISTIC_INPUT = 0xc001
const BLE_CHARACTERISTIC_OUTPUT = 0xc000
const BLE_DEFAULT_CHUNK_SIZE = 248

const BLE_DEFAULT_REQUEST_OPTIONS: RequestDeviceOptions = {
  filters: [{ services: [BLE_SERVICE_UUID] }],
}

export class ColibriBleInterface implements ColibriInterface {
  private bluetooth: Bluetooth | undefined = undefined
  private device: BluetoothDevice | null = null
  private gatt: BluetoothRemoteGATTServer | undefined = undefined
  private responseBuffer: string = ''
  private response: JsonRpcResponse | undefined = undefined
  private stateCallback: (() => void) | undefined

  public connected = false

  constructor(stateCallback?: (() => void) | undefined) {
    if (typeof window !== 'undefined') {
      this.bluetooth = navigator.bluetooth
    }

    this.stateCallback = stateCallback
  }

  isAvailable = (): boolean => {
    return !!this.bluetooth?.requestDevice
  }

  isConnected = (): boolean => {
    this.connected = !!this.gatt?.connected
    return this.connected
  }

  connect = async (options?: RequestDeviceOptions): Promise<void> => {
    if (!this.isAvailable()) {
      throw new Error('Web Bluetooth API is not available')
    }

    if (this.connected) {
      throw new Error('BLE device already connected')
    }

    if (!options) options = BLE_DEFAULT_REQUEST_OPTIONS

    this.reset()

    try {
      const device = await this.bluetooth!.requestDevice(options)
      const gatt = await device.gatt?.connect()

      if (!gatt) {
        throw new Error('Failed to connect to GATT server')
      }

      this.device = device
      this.gatt = gatt
      this.connected = true

      this.startDeviceListeners()
      this.startNotifications()
    } catch (error) {
      console.warn(`Failed to connect: ${error}`)
      throw error
    }
  }

  private reset = () => {
    this.device = null
    this.gatt = undefined
    this.response = undefined
    this.responseBuffer = ''

    this.connected = false
  }

  disconnect = () => {
    this.stopNotifications()
    this.stopDeviceListeners()

    if (this.gatt?.connected) {
      this.gatt.disconnect()
    }

    if (this.stateCallback) {
      this.stateCallback()
    }

    this.reset()
  }

  private getCharacteristic = async (
    serviceId: string,
    characteristicId: number
  ): Promise<BluetoothRemoteGATTCharacteristic> => {
    if (!this.gatt) throw new Error('No device connected')

    const service = await this.gatt.getPrimaryService(serviceId)
    const characteristic = await service.getCharacteristic(characteristicId)
    return characteristic
  }

  private writeRaw = async (
    serviceId: string,
    characteristicId: number,
    data: ArrayBuffer
  ): Promise<void> => {
    const characteristic = await this.getCharacteristic(
      serviceId,
      characteristicId
    )
    await characteristic.writeValue(data)
  }

  private writeString = async (text: string): Promise<void> => {
    const chunks: ArrayBuffer[] = []
    for (let i = 0; i < text.length; i += BLE_DEFAULT_CHUNK_SIZE) {
      const textChunk = text.substring(i, i + BLE_DEFAULT_CHUNK_SIZE)
      chunks.push(stringToBuffer(textChunk))
    }

    for (const chunk of chunks) {
      await this.writeRaw(BLE_SERVICE_UUID, BLE_CHARACTERISTIC_INPUT, chunk)
      await new Promise((resolve) => setTimeout(resolve, 100))
    }
  }

  rpcCall = async (
    method: string,
    params: any[] = []
  ): Promise<JsonRpcResponse | undefined> => {
    const id = Math.round(Math.random() * 100)
    const input = {
      id,
      method,
      params,
    }
    const str = JSON.stringify(input)

    this.responseBuffer = ''
    this.response = undefined

    await this.writeString(str)

    let counter = 0
    const timeout = 35
    const timeoutMax = 20_000
    while (!this.response || (this.response as JsonRpcResponse).id !== id) {
      if (counter > timeoutMax) break

      await new Promise((resolve) => setTimeout(resolve, timeout))
      counter += timeout
    }

    if (!this.response) {
      console.warn('Request timed out')
      return
    }

    return this.response as JsonRpcResponse
  }

  private readRaw = async (
    serviceId: string,
    characteristicId: number
  ): Promise<DataView> => {
    const characteristic = await this.getCharacteristic(
      serviceId,
      characteristicId
    )
    return characteristic.readValue()
  }

  private onNotification = (event: Event) => {
    const data = (event.target as BluetoothRemoteGATTCharacteristic).value
    if (!data) return

    try {
      const str = bufferToString(dataViewToBuffer(data))

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

  private startNotifications = async (): Promise<void> => {
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
        this.onNotification
      )
    } catch (error) {
      console.warn('Failed to start notifications:', error)
      throw error
    }
  }

  private stopNotifications = async (): Promise<void> => {
    try {
      const characteristic = await this.getCharacteristic(
        BLE_SERVICE_UUID,
        BLE_CHARACTERISTIC_OUTPUT
      )

      characteristic.removeEventListener(
        'characteristicvaluechanged',
        this.onNotification
      )
      await characteristic.stopNotifications()
    } catch (error) {
      console.warn('Failed to stop notifications:', error)
      throw error
    }
  }

  private startDeviceListeners = () => {
    if (!this.device) return

    this.device.addEventListener('gattserverdisconnected', this.disconnect)
  }

  private stopDeviceListeners = () => {
    if (!this.device) return

    this.device.removeEventListener('gattserverdisconnected', this.disconnect)
  }
}
