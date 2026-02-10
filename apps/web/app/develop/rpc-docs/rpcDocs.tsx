'use client'

import { Cards } from 'nextra/components'
import type { FC, ReactNode } from 'react'
import { rpcMethods } from './rpcMethods'

export const RpcDocsCard: FC<{
  children: ReactNode
  id: string
}> = ({ children, id }) => {
  return (
    <li id={id}>
      <Cards.Card
        title=""
        href={`#${id}`}
        className="pointer-events-none my-4 cursor-default"
      >
        <div className="px-4 pt-6">{children}</div>
      </Cards.Card>
    </li>
  )
}

export const RpcDocs: FC<{
  methods: typeof rpcMethods
}> = ({ methods }) => {
  return (
    <ul>
      {methods.map((method) => (
        <RpcDocsCard key={method.method} id={method.method}>
          <h3 className="text-lg">{method.method}</h3>

          <p>
            <b>Parameters:</b> <i>{method.params || '<no input parameters>'}</i>
          </p>

          <p>
            <b>Result:</b> <i>{method.result}</i>
          </p>
        </RpcDocsCard>
      ))}
    </ul>
  )
}
