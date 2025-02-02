'use client'

import { useColibri } from '@colibriwallet/sdk/react'

export const ConnectButton = () => {
  const { isBleAvailable, isBleConnected, connectBle, disconnectBle } =
    useColibri()

  return (
    <button
      onClick={() => {
        if (!isBleConnected) {
          connectBle()
        } else {
          disconnectBle()
        }
      }}
      disabled={!isBleAvailable}
    >
      {isBleConnected ? 'Disconnect from BLE' : 'Connect via BLE'}
    </button>
  )
}
