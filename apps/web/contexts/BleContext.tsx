'use client'

import React, { createContext, useState, useEffect } from 'react'
import { BleService, BleDevice } from './BleService'

export interface BleContextType {
  isAvailable: boolean
  isConnected: boolean
  device: BleDevice | null
  error: string | null
  connect: () => Promise<void>
  disconnect: () => Promise<void>
  write: (
    serviceId: string,
    characteristicId: string,
    data: ArrayBuffer
  ) => Promise<void>
  read: (serviceId: string, characteristicId: string) => Promise<DataView>
}

export const BleContext = createContext<BleContextType>({
  isAvailable: false,
  isConnected: false,
  device: null,
  error: null,
  connect: async () => {},
  disconnect: async () => {},
  write: async () => {},
  read: async () => new DataView(new ArrayBuffer(0)),
})
const bleService = new BleService()

export const BleProvider: React.FC<{ children: React.ReactNode }> = ({
  children,
}) => {
  const [isAvailable, setIsAvailable] = useState(false)
  const [isConnected, setIsConnected] = useState(false)
  const [device, setDevice] = useState<BleDevice | null>(null)
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    setIsAvailable(bleService.isAvailable())
  }, [])

  const connect = async () => {
    try {
      const device = await bleService.connect({
        filters: [
          // Customize these filters based on your BLE device
          { services: ['battery_service'] },
        ],
      })
      setDevice(device)
      setIsConnected(true)
      setError(null)
    } catch (err) {
      setError(err.message)
    }
  }

  const disconnect = async () => {
    try {
      await bleService.disconnect()
      setDevice(null)
      setIsConnected(false)
      setError(null)
    } catch (err) {
      setError(err.message)
    }
  }

  const write = async (
    serviceId: string,
    characteristicId: string,
    data: ArrayBuffer
  ) => {
    try {
      await bleService.write(serviceId, characteristicId, data)
      setError(null)
    } catch (err) {
      setError(err.message)
    }
  }

  const read = async (serviceId: string, characteristicId: string) => {
    try {
      const value = await bleService.read(serviceId, characteristicId)
      setError(null)
      return value
    } catch (err) {
      setError(err.message)
      throw err
    }
  }

  return (
    <BleContext.Provider
      value={{
        isAvailable,
        isConnected,
        device,
        error,
        connect,
        disconnect,
        write,
        read,
      }}
    >
      {children}
    </BleContext.Provider>
  )
}
