'use client'

import { useContext } from 'react'
import { ColibriBleContext, ColibriBleContextType } from './ColibriBleContext'

export const useColibriBle = (): ColibriBleContextType => {
  const context = useContext(ColibriBleContext)
  if (!context) {
    throw new Error('useColibriBle must be used within a BleProvider')
  }
  return context
}
