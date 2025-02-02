'use client'

import React, { createContext, useState, useEffect } from 'react'
import { Colibri } from '../Colibri'
import type { ColibriContextType } from '../types'

export const ColibriContext = createContext<ColibriContextType>({
  isBleAvailable: false,
  isBleConnected: false,
  connectBle: () => Promise.resolve(),
  disconnectBle: () => Promise.resolve(),
})

export const ColibriProvider: React.FC<{ children: React.ReactNode }> = ({
  children,
}) => {
  const colibri = new Colibri()

  /*
  const [response, setResponse] = useState<JsonRpcResponse | undefined>(
    undefined
  )

  useEffect(() => {
    setResponse(ble.response)
  }, [ble.response])
  */

  return (
    <ColibriContext.Provider
      value={{
        isBleAvailable: colibri.isBleAvailable(),
        isBleConnected: colibri.ble.connected,
        connectBle: colibri.connectBle,
        disconnectBle: colibri.disconnectBle,
      }}
    >
      {children}
    </ColibriContext.Provider>
  )
}
