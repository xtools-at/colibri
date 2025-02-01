'use client'

import { useColibriBle } from 'colibri/useColibriBle'

export const ConnectButton = () => {
  const { isAvailable, isConnected, connect, disconnect } = useColibriBle()

  return (
    <button
      onClick={() => {
        if (isConnected) {
          disconnect()
        } else {
          connect()
        }
      }}
      disabled={!isAvailable}
    >
      {isConnected ? 'Disconnect from BLE' : 'Connect via BLE'}
    </button>
  )
}
