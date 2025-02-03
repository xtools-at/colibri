# Colibri TypeScript SDK

Zero-dependency TypeScript SDK for the [Colibri.diy](https://colibri.diy) hardware wallet, for Web and React. This package is part of the [Colibri monorepo](https://github.com/xtools-at/colibri).

## Getting started

First, install the SDK from [npm](https://www.npmjs.com/package/@colibriwallet/sdk):

```bash
pnpm i @colibriwallet/sdk
```

### TypeScript

In your TypeScript- or Javascript file, import and instantiate the `Colibri` class to use it:

```typescript
import { Colibri } from '@colibriwallet/sdk'

const colibri = new Colibri()

// init a connect button
if (colibri.isBleAvailable()) {
  document
    .querySelector('#connectButton')
    ?.addEventListener('click', colibri.connectBle)
}

// send commands
if (colibri.isBleConnected()) {
  const unlocked = await colibri.unlock('MyPassword')

  if (unlocked) {
    const { walletId, seedPhrase } = await colibri.createSeedPhrase(24)
  }
}
```

### React

The SDK includes a React context provider, and a hook to access wallet state and all SDK methods. React >= 18 is required as a peer-dependency for this to work.

You'll need to wrap your root layout with the `ColibriProvider` like this:

```tsx
// e.g. your app/layout.tsx or pages/_app.tsx
import { ColibriProvider } from '@colibriwallet/sdk/react'

export default function RootLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <ColibriProvider>
      <html>
        <body>{children}</body>
      </html>
    </ColibriProvider>
  )
}
```

Then you can call the `useColibri()` hook in your component or page:

```tsx
import { useColibri } from '@colibriwallet/sdk/react'

export const ConnectButton = () => {
  const { isBleAvailable, isBleConnected, connectBle, disconnectBle } =
    useColibri()

  return (
    <button
      onClick={() => {
        if (isBleConnected) {
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
```

## License

Copyright © 2025 by [The Colibri Project](https://colibri.diy). This package is licensed under the [MIT](https://github.com/xtools-at/colibri/blob/main/sdk/typescript/LICENSE.md) license.
