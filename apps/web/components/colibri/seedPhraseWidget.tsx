'use client'

import { useState, useCallback, useEffect, type FC } from 'react'
import { useColibri } from '@colibriwallet/sdk/react'
import { Widget } from './widget'
import {
  NoConnectionErrorWidget,
  LockedErrorWidget,
  NoPasswordErrorWidget,
} from './errorWidget'
import { Button, Input, Select } from '../core'
import { Link } from 'nextra-theme-docs'

export const SeedPhraseWidget: FC = () => {
  // Colibri context
  const {
    isBleConnected,
    isPasswordSet,
    storedWallets,
    isUnlocked,
    addSeedPhrase,
    createSeedPhrase,
  } = useColibri()

  // states
  const [statusMsg, setStatusMsg] = useState('')
  const [statusType, setStatusType] = useState<'error' | 'info' | 'default'>(
    'default'
  )
  const [createStatusMsg, setCreateStatusMsg] = useState('')
  const [createStatusType, setCreateStatusType] = useState<
    'error' | 'info' | 'default'
  >('default')
  const [input, setInput] = useState('')
  const [inputInvalid, setInputInvalid] = useState(false)
  const [words, setWords] = useState(24)

  // input validation
  useEffect(() => {
    if (input.length) {
      if (input.includes(',') || input.length > 196) {
        return setInputInvalid(true)
      }
    }

    setInputInvalid(false)
  }, [input])

  // click handler
  const onAddClick = useCallback(async () => {
    const words = input.trim().split(' ')
    if (![12, 18, 24].includes(words.length)) {
      return setInputInvalid(true)
    }

    setStatusMsg('Sending request...')
    setStatusType('info')

    try {
      const walletId = await addSeedPhrase(input)

      // clear input on success
      setInput('')
      setStatusMsg(
        `Successfully added seed phrase to wallet! New ID: #${walletId}`
      )
      setStatusType('default')
    } catch (err) {
      setStatusMsg(err.message)
      setStatusType('error')
    }
  }, [input, addSeedPhrase])

  const onCreateClick = useCallback(async () => {
    setCreateStatusMsg('Sending request...')
    setCreateStatusType('info')

    try {
      const { walletId, seedPhrase } = await createSeedPhrase(
        words as 12 | 18 | 24
      )

      // clear input on success
      setCreateStatusMsg(
        `Successfully created seed phrase on wallet! New ID: #${walletId}; \n\nSeed phrase: ${seedPhrase}`
      )
      setCreateStatusType('default')
    } catch (err) {
      setCreateStatusMsg(err.message)
      setCreateStatusType('error')
    }
  }, [createSeedPhrase, words])

  // connection TODO:
  if (isBleConnected) {
    return <NoConnectionErrorWidget />
  }

  // pw needs to be set up to add seed phrase  TODO:
  if (isPasswordSet) {
    return <NoPasswordErrorWidget />
  }

  // needs to be unlocked to add seed phrase  TODO:
  if (isUnlocked) {
    return <LockedErrorWidget />
  }

  const calloutMsg =
    storedWallets > 0 ? `Found ${storedWallets} stored seed phrases` : undefined

  const footnote = (
    <>
      <p className="mb-2 mt-4">Seed security</p>
      <p>
        <Link
          href="https://jlopp.github.io/metal-bitcoin-storage-reviews/reviews/safu-ninja"
          target="_blank"
          rel="noopener noreferrer"
        >
          SAFU Ninja
        </Link>
      </p>
    </>
  )

  const isBusy = statusType === 'info' || createStatusType === 'info'

  return (
    <>
      <Widget statusMsg={createStatusMsg} statusType={createStatusType}>
        <div className="mb-6 max-w-48">
          <Select
            id="input-words"
            label="Pick seed phrase length"
            onChange={(e) => setWords(parseInt(e.target.value))}
          >
            <option value="24" defaultChecked={true}>
              24 words
            </option>
            <option value="18">18 words</option>
            <option value="12">12 words</option>
          </Select>
        </div>
        <Button onClick={onCreateClick} disabled={isBusy}>
          Create a seed phrase
        </Button>
      </Widget>

      <div className="mb-4">- OR -</div>

      <Widget
        calloutMsg={calloutMsg}
        calloutType={'default'}
        statusMsg={statusMsg}
        statusType={statusType}
        footnote={footnote}
      >
        <div className="mb-6">
          <Input
            id="input-seedphrase"
            label="Add existing seed phrase"
            type="textarea"
            placeholder="Enter your 12/18/24 word seed phrase"
            maxLength={196}
            value={input}
            onChange={(e) => setInput(e.target.value)}
            isInvalid={inputInvalid}
            message={
              inputInvalid
                ? 'Seed phrase must have 12/18/24 words, separated by spaces'
                : undefined
            }
          />
        </div>

        <Button
          onClick={onAddClick}
          disabled={inputInvalid || !input.length || isBusy}
        >
          Add your seed phrase
        </Button>
      </Widget>
    </>
  )
}
