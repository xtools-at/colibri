export interface BleDevice {
  device: BluetoothDevice
  gatt?: BluetoothRemoteGATTServer
}

export class BleService {
  private device: BleDevice | null = null
  private bluetooth: Bluetooth | undefined = undefined

  constructor(bluetoothModule?: Bluetooth) {
    if (typeof window !== 'undefined') {
      this.bluetooth = navigator.bluetooth
    }
    if (bluetoothModule) {
      this.bluetooth = bluetoothModule
    }
  }

  isAvailable(): boolean {
    return !!this.bluetooth && !!this.bluetooth.requestDevice
  }

  async connect(options: RequestDeviceOptions): Promise<BleDevice> {
    if (!this.isAvailable()) {
      throw new Error('Web Bluetooth API is not available')
    }

    try {
      const device = await this.bluetooth!.requestDevice(options)
      const gatt = await device.gatt?.connect()

      this.device = { device, gatt }
      return this.device
    } catch (error) {
      throw new Error(`Failed to connect: ${error}`)
    }
  }

  async disconnect(): Promise<void> {
    if (this.device?.gatt?.connected) {
      this.device.gatt.disconnect()
    }
    this.device = null
  }

  async write(
    serviceId: string,
    characteristicId: string,
    data: ArrayBuffer
  ): Promise<void> {
    if (!this.device?.gatt) throw new Error('No device connected')

    const service = await this.device.gatt.getPrimaryService(serviceId)
    const characteristic = await service.getCharacteristic(characteristicId)
    await characteristic.writeValue(data)
  }

  async read(serviceId: string, characteristicId: string): Promise<DataView> {
    if (!this.device?.gatt) throw new Error('No device connected')

    const service = await this.device.gatt.getPrimaryService(serviceId)
    const characteristic = await service.getCharacteristic(characteristicId)
    return characteristic.readValue()
  }
}
