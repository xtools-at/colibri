# Colibri Typescript SDK

for Web and React

## Install

```bash
pnpm i @colibriwallet/sdk
```

## Use

### Typescript

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

Wrap your root layout with the `ColibriProvider`:

```tsx
// e.g. src/app/layout.tsx
import { ColibriProvider } from '@colibriwallet/sdk/react'

export default function RootLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <html>
      <body>
        <ColibriProvider>{children}</ColibriProvider>
      </body>
    </html>
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

licensed under the [MIT](https://github.com/xtools-at/colibri/blob/main/sdk/typescript/LICENSE.md) license.
