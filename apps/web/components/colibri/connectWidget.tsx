'use client'

import { useState, useCallback, useEffect } from 'react'
import { useColibri } from '@colibriwallet/sdk/react'
import { Widget } from './widget'
import { Button } from '../core'
import { Link } from 'nextra-theme-docs'

export const ConnectWidget = () => {
  // Colibri context
  const { isBleAvailable, isBleConnected, connectBle, disconnectBle } =
    useColibri()

  // state
  const [statusMsg, setStatusMsg] = useState('')
  const [statusType, setStatusType] = useState<'error' | 'info' | 'default'>(
    'default'
  )
  const [isLinuxUser, setIsLinuxUser] = useState(false)

  // prevent hydration error
  useEffect(() => {
    setIsLinuxUser(
      typeof window != 'undefined' &&
        window.navigator.userAgent.includes('Linux ')
    )
  }, [])

  // click handler
  const onButtonClick = useCallback(async () => {
    setStatusMsg('Requesting Bluetooth devices...')
    setStatusType('info')

    if (!isBleConnected) {
      try {
        await connectBle()
        setStatusMsg('Successfully connected!')
        setStatusType('default')
        return
      } catch (err) {
        setStatusMsg(
          'Error while connecting to wallet: request failed or cancelled by user'
        )
        setStatusType('error')
        return
      }
    } else {
      try {
        disconnectBle()
        setStatusMsg('Successfully disconnected!')
        setStatusType('default')
        return
      } catch (err) {
        // ignore
      }
    }

    setStatusMsg('')
    setStatusType('default')
  }, [isBleConnected, connectBle, disconnectBle])

  // no bluetooth
  if (!isBleAvailable) {
    return (
      <Widget
        calloutMsg="This browser doesn't support Bluetooth"
        calloutType="warning"
      >
        <p className="mb-4">
          Please try one of the browsers below to connect to your wallet:
        </p>
        <ul className="mb-4 list-inside list-disc pr-2">
          <li>Desktop: Chrome, Edge</li>
          <li>Android: Chrome, Samsung Internet</li>
          <li>
            iOS:{' '}
            <Link
              href="https://itunes.apple.com/us/app/webble/id1193531073"
              target="_blank"
              rel="noopener noreferrer nofollow"
            >
              WebBLE Browser
            </Link>{' '}
            (paid app)
          </li>
        </ul>
        {isLinuxUser && (
          <p className="text-sm">
            On <i>Linux</i> only, you&apos;d additionally have set this browser
            flag to enable Bluetooth support:{' '}
            <code>
              chrome://flags/#enable-experimental-web-platform-features
            </code>
          </p>
        )}
      </Widget>
    )
  }

  const footnote = !isBleConnected ? (
    <>
      <p className="mb-2 mt-4">
        Make sure your wallet is plugged in, and <b>paired</b> via your
        operating system&apos;s Bluetooth settings <i>before</i> trying to
        connect.
      </p>
      <p>
        If your operating system did <b>not</b> ask you for a PIN while pairing,
        your wallet may not be connected properly. The default PIN{' '}
        <code>200913</code> can be changed in the firmware&apos;s{' '}
        <code>config_user.h</code>
      </p>
    </>
  ) : undefined

  return (
    <Widget
      calloutMsg={isBleConnected ? 'Your wallet is connected!' : undefined}
      statusMsg={statusMsg}
      statusType={statusType}
      footnote={footnote}
    >
      <Button onClick={onButtonClick} disabled={!isBleAvailable}>
        {isBleConnected ? 'Disconnect' : 'Connect wallet'}
      </Button>
    </Widget>
  )
}
