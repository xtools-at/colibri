'use client'

import React, { createContext, useState, useEffect } from 'react'
import { Colibri } from '../Colibri'
import type { ColibriContextType, ColibriStatus } from '../types'

export const ColibriContext = createContext<ColibriContextType>(
  {} as ColibriContextType
)

export const ColibriProvider: React.FC<{ children: React.ReactNode }> = ({
  children,
}) => {
  // init states
  const [colibri] = useState(new Colibri())
  const [isBleAvailable, setIsBleAvailable] = useState(false)
  const [isBleConnected, setIsBleConnected] = useState(false)
  const [isUnlocked, setIsUnlocked] = useState(false)
  const [isSeedPhraseSet, setIsSeedPhraseSet] = useState(false)
  const [isPasswordSet, setIsPasswordSet] = useState(false)
  const [deviceName, setDeviceName] = useState('')
  const [fwVersion, setFwVersion] = useState('')
  const [boardType, setBoardType] = useState('')
  const [displayType, setDisplayType] = useState('')
  const [storedWallets, setStoredWallets] = useState(0)
  const [walletId, setWalletId] = useState(0)
  const [chainType, setChainType] = useState(0)
  const [address, setAddress] = useState('')
  const [pubKey, setPubKey] = useState('')
  const [hdPath, setHdPath] = useState('')
  const [remainingAttempts, setRemainingAttempts] = useState(0)

  // update React states on change
  useEffect(() => {
    const changeHandler = (data: ColibriStatus) => {
      setIsBleAvailable(data.isBleAvailable)
      setIsBleConnected(data.isBleConnected)
      setIsUnlocked(data.isUnlocked)
      setIsSeedPhraseSet(data.isSeedPhraseSet)
      setIsPasswordSet(data.isPasswordSet)
      setDeviceName(data.deviceName)
      setFwVersion(data.fwVersion)
      setBoardType(data.boardType)
      setDisplayType(data.displayType)
      setStoredWallets(data.storedWallets)
      setWalletId(data.walletId)
      setChainType(data.chainType)
      setAddress(data.address)
      setPubKey(data.pubKey)
      setHdPath(data.hdPath)
      setRemainingAttempts(data.remainingAttempts)
    }

    colibri.onChange(changeHandler)

    return () => {
      colibri.onChange(undefined)
    }
  }, [colibri])

  // expose methods
  const {
    connectBle,
    disconnectBle,
    ping,
    listMethods,
    setPassword,
    getStatus,
    unlock,
    lock,
    wipe,
    deletePairedDevices,
    createSeedPhrase,
    addSeedPhrase,
    getDeviceInfo,
    getSelectedWallet,
    exportRootXPub,
    exportAccountXPub,
    selectWallet,
    signMessage,
    ethSignTypedDataHash,
    ethSignTransaction,
  } = colibri

  return (
    <ColibriContext.Provider
      value={{
        isBleAvailable,
        isBleConnected,
        isUnlocked,
        isSeedPhraseSet,
        isPasswordSet,
        deviceName,
        fwVersion,
        boardType,
        displayType,
        storedWallets,
        walletId,
        chainType,
        address,
        pubKey,
        hdPath,
        remainingAttempts,
        connectBle,
        disconnectBle,
        ping,
        listMethods,
        setPassword,
        getStatus,
        unlock,
        lock,
        wipe,
        deletePairedDevices,
        createSeedPhrase,
        addSeedPhrase,
        getDeviceInfo,
        getSelectedWallet,
        exportRootXPub,
        exportAccountXPub,
        selectWallet,
        signMessage,
        ethSignTypedDataHash,
        ethSignTransaction,
      }}
    >
      {children}
    </ColibriContext.Provider>
  )
}
