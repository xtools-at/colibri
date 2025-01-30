'use client'

import { useContext } from 'react'
import { BleContext, BleContextType } from '../contexts/BleContext'

export const useBle = (): BleContextType => {
  const context = useContext(BleContext)
  if (!context) {
    throw new Error('useBle must be used within a BleProvider')
  }
  return context
}
