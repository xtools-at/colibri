'use client'

import type { FC, ReactNode } from 'react'
import { useBle } from 'hooks/useBle'

export const Connect: FC<{ children: ReactNode }> = ({ children }) => {
  const { isAvailable, isConnected, connect } = useBle()

  return (
    <button onClick={() => connect()} disabled={!isAvailable || isConnected}>
      {children}
    </button>
  )
}
