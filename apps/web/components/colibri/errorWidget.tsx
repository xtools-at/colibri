'use client'

import type { FC, ReactNode } from 'react'
import { Widget } from './widget'

const ErrorWidget: FC<{
  children?: ReactNode
  errorMsg: string
}> = ({ children, errorMsg }) => {
  return (
    <Widget calloutMsg={errorMsg} calloutType="info">
      {children}
    </Widget>
  )
}

export const NoConnectionErrorWidget: FC<{
  children?: ReactNode
}> = ({ children }) => {
  return (
    <ErrorWidget errorMsg="Please connect your wallet first">
      {children}
    </ErrorWidget>
  )
}

export const NoPasswordErrorWidget: FC<{
  children?: ReactNode
}> = ({ children }) => {
  return (
    <ErrorWidget errorMsg="Please set a password first">{children}</ErrorWidget>
  )
}

export const NoSeedPhraseErrorWidget: FC<{
  children?: ReactNode
}> = ({ children }) => {
  return (
    <ErrorWidget errorMsg="Please set up a seed phrase first">
      {children}
    </ErrorWidget>
  )
}

export const LockedErrorWidget: FC<{
  children?: ReactNode
}> = ({ children }) => {
  return (
    <ErrorWidget errorMsg="Please unlock your wallet first">
      {children}
    </ErrorWidget>
  )
}
