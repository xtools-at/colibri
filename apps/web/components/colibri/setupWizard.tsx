'use client'

import { Steps } from 'nextra/components'
import { useColibri } from '@colibriwallet/sdk/react'
import { ConnectWidget } from './connectWidget'
import { PasswordWidget } from './passwordWidget'
import { SeedPhraseWidget } from './seedPhraseWidget'

export const SetupWizard = () => {
  // Colibri context
  const { isBleConnected, isPasswordSet, isSeedPhraseSet } = useColibri()

  return (
    <Steps>
      <h3>Connect Wallet</h3>
      <ConnectWidget />

      {isBleConnected && isPasswordSet && isSeedPhraseSet && (
        <>
          <h3>Unlock wallet</h3>
          <PasswordWidget mode="unlock" />
        </>
      )}

      <h3>{!isPasswordSet ? 'Set your password' : 'Change your password'}</h3>
      <PasswordWidget mode="set" />

      <h3>Create or add a seed phrase</h3>
      <SeedPhraseWidget />

      <h3>Access Wallet</h3>
      <ConnectWidget />
    </Steps>
  )
}
