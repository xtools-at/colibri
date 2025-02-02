'use client'

import { useContext } from 'react'
import { ColibriContext } from './ColibriProvider'
import type { ColibriContextType } from '../types'

export const useColibri = (): ColibriContextType => {
  const context = useContext(ColibriContext)
  if (!context) {
    throw new Error('useColibri must be used within a ColibriProvider')
  }
  return context
}
