'use client'

import { Callout } from 'nextra/components'
import type { FC, ReactNode } from 'react'

export const Widget: FC<{
  children: ReactNode
  calloutMsg?: string
  calloutType?: 'info' | 'warning' | 'error' | 'default'
  statusMsg?: string
  statusType?: 'info' | 'warning' | 'error' | 'default'
  footnote?: ReactNode | string
}> = ({
  children,
  calloutMsg,
  calloutType = 'info',
  statusMsg,
  statusType = 'default',
  footnote,
}) => {
  return (
    <article className="pb-2">
      {calloutMsg && <Callout type={calloutType}>{calloutMsg}</Callout>}

      <div className="my-4">{children}</div>

      {statusMsg && <Callout type={statusType}>{statusMsg}</Callout>}

      {footnote && <div className="my-4 text-sm">{footnote}</div>}
    </article>
  )
}
