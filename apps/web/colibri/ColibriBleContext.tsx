'use client'

import React, { createContext, useState, useEffect } from 'react'
import { ColibriBleService } from './ColibriBleService'

const bleService = new ColibriBleService()
export interface ColibriBleContextType {
  isAvailable: boolean
  isConnected: boolean
  error: string | null
  connect: () => Promise<void>
  disconnect: () => void
}

export const ColibriBleContext = createContext<ColibriBleContextType>({
  isAvailable: bleService.isAvailable(),
  isConnected: bleService.connected,
  error: null,
  connect: bleService.connect,
  disconnect: bleService.disconnect,
})

export const ColibriBleProvider: React.FC<{ children: React.ReactNode }> = ({
  children,
}) => {
  const [isAvailable, setIsAvailable] = useState(false)
  const [isConnected, setIsConnected] = useState(false)
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    setIsAvailable(bleService.isAvailable())
  }, [])

  return (
    <ColibriBleContext.Provider
      value={{
        isAvailable,
        isConnected,
        error,
        connect: bleService.connect,
        disconnect: bleService.disconnect,
      }}
    >
      {children}
    </ColibriBleContext.Provider>
  )
}
