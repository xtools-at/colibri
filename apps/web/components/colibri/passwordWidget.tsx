'use client'

import { useState, useCallback, useEffect, type FC } from 'react'
import { useColibri } from '@colibriwallet/sdk/react'
import { Widget } from './widget'
import {
  NoConnectionErrorWidget,
  LockedErrorWidget,
  NoPasswordErrorWidget,
  NoSeedPhraseErrorWidget,
} from './errorWidget'
import { Button, Input } from '../core'
import { Link } from 'nextra-theme-docs'

export const PasswordWidget: FC<{
  mode?: 'set' | 'unlock'
}> = ({ mode = 'set' }) => {
  // Colibri context
  const {
    isBleConnected,
    isPasswordSet,
    isSeedPhraseSet,
    isUnlocked,
    remainingAttempts,
    setPassword,
    unlock,
    lock,
  } = useColibri()

  // state
  const [statusMsg, setStatusMsg] = useState('')
  const [statusType, setStatusType] = useState<'error' | 'info' | 'default'>(
    'default'
  )
  const [input, setInput] = useState('')
  const [inputInvalid, setInputInvalid] = useState(false)
  const useUnlockMode = mode === 'unlock'

  // input validation
  useEffect(() => {
    if (input.length && (input.length < 12 || input.length > 255)) {
      return setInputInvalid(true)
    }

    setInputInvalid(false)
  }, [input])

  // click handler
  const onButtonClick = useCallback(async () => {
    setStatusMsg('Sending request...')
    setStatusType('info')

    try {
      if (useUnlockMode) {
        if (!isUnlocked) {
          await unlock(input)
        } else {
          await lock()
        }
      } else {
        await setPassword(input)
      }

      // clear input on success
      setInput('')
      setStatusMsg('Success!')
      setStatusType('default')
    } catch (err) {
      setStatusMsg(err.message)
      setStatusType('error')
    }
  }, [setPassword, input, isUnlocked, unlock, lock, useUnlockMode])

  // connection TODO:
  if (isBleConnected) {
    return <NoConnectionErrorWidget />
  }

  // needs to be unlocked to change password
  if (!useUnlockMode && isPasswordSet && isSeedPhraseSet && !isUnlocked) {
    return <LockedErrorWidget />
  }

  // needs to be set up to unlock TODO:
  if (useUnlockMode) {
    if (isPasswordSet) {
      return <NoPasswordErrorWidget />
    }
    if (isSeedPhraseSet) {
      return <NoSeedPhraseErrorWidget />
    }
  }

  let footnote =
    useUnlockMode && remainingAttempts ? (
      <p className="mb-2 mt-4">
        {remainingAttempts} remaining unlock attempts before wallet is erased.
      </p>
    ) : undefined

  if (!useUnlockMode) {
    footnote = (
      <>
        <p className="mb-2 mt-4">input security</p>
        <p>
          <Link
            href="https://diceware.dmuth.org"
            target="_blank"
            rel="noopener noreferrer"
          >
            Diceware
          </Link>
        </p>
      </>
    )
  }

  let calloutMsg = isPasswordSet
    ? 'Your password is already set, you can change it here'
    : undefined
  if (useUnlockMode && isUnlocked) {
    calloutMsg = 'Your wallet is unlocked'
  }

  let buttonText = isPasswordSet ? 'Change password' : 'Set password'
  if (useUnlockMode) {
    buttonText = isUnlocked ? 'Lock Wallet' : 'Unlock wallet'
  }

  return (
    <Widget
      calloutMsg={calloutMsg}
      calloutType={'default'}
      statusMsg={statusMsg}
      statusType={statusType}
      footnote={footnote}
    >
      <div className="mb-6">
        <Input
          id={useUnlockMode ? 'input-unlock' : 'input-password'}
          label={useUnlockMode ? 'Unlock with password' : 'Set a password'}
          type="password"
          placeholder="Enter password"
          minLength={12}
          maxLength={255}
          value={input}
          onChange={(e) => setInput(e.target.value)}
          isInvalid={inputInvalid}
          message={
            inputInvalid ? 'Password must be at least 12 characters' : undefined
          }
        />
      </div>

      <Button onClick={onButtonClick} disabled={inputInvalid || !input.length}>
        {buttonText}
      </Button>
    </Widget>
  )
}
