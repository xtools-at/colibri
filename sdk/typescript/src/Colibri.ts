import { ColibriBleInterface } from './ColibriBleInterface'
import type { ColibriInterface, ColibriMethods, ColibriStatus } from './types'

const RPC_METHOD_PING = 'ping'
const RPC_METHOD_LIST_METHODS = 'listMethods'
const RPC_METHOD_SET_PW = 'setPassword'
const RPC_METHOD_UNLOCK = 'unlock'
const RPC_METHOD_LOCK = 'lock'
const RPC_METHOD_WIPE = 'wipe'
const RPC_METHOD_DELETE_PAIRED_DEVICES = 'deletePairedDevices'
const RPC_METHOD_GET_STATUS = 'getStatus'
const RPC_METHOD_GET_WALLET = 'getSelectedWallet'
const RPC_METHOD_EXPORT_XPUB_ROOT = 'exportRootXPub'
const RPC_METHOD_EXPORT_XPUB_ACCOUNT = 'exportAccountXPub'
const RPC_METHOD_GET_INFO = 'getDeviceInfo'
const RPC_METHOD_CREATE_MNEMONIC = 'createSeedPhrase'
const RPC_METHOD_ADD_MNEMONIC = 'addSeedPhrase'
const RPC_METHOD_SELECT_WALLET = 'selectWallet'
const RPC_METHOD_SIGN_MSG = 'signMessage'
const RPC_METHOD_ETH_SIGN_TYPED_DATA_HASH = 'eth_signTypedDataHash'
const RPC_METHOD_ETH_SIGN_TX = 'eth_signTransaction'

export class Colibri implements ColibriMethods {
  private ble: ColibriBleInterface
  private connection: ColibriInterface | undefined
  private stateCallback: ((status: ColibriStatus) => void) | undefined

  public isUnlocked = false
  public isSeedPhraseSet = false
  public isPasswordSet = false
  public deviceName = ''
  public fwVersion = ''
  public boardType = ''
  public displayType = ''
  public storedWallets = 0
  public walletId = 0
  public chainType = 0
  public address = ''
  public pubKey = ''
  public hdPath = ''

  constructor() {
    this.ble = new ColibriBleInterface(this.stateChanged)
  }

  private reset() {
    this.isUnlocked = false
    this.isSeedPhraseSet = false
    this.isPasswordSet = false
    this.deviceName = ''
    this.fwVersion = ''
    this.boardType = ''
    this.displayType = ''
    this.storedWallets = 0
    this.walletId = 0
    this.chainType = 0
    this.address = ''
    this.pubKey = ''
    this.hdPath = ''

    this.connection = undefined
  }

  connectBle = async () => {
    try {
      await this.ble.connect()
      this.connection = this.ble
    } catch (error) {
      this.disconnectBle()
    }

    await this.getStatus()
    this.stateChanged()
  }

  disconnectBle = () => {
    this.ble.disconnect()
    this.connection = undefined
    this.stateChanged()
  }

  isBleAvailable = () => {
    return this.ble.isAvailable()
  }

  isBleConnected = () => {
    return this.ble.isConnected()
  }

  public onChange = (
    callback: ((status: ColibriStatus) => void) | undefined
  ) => {
    this.stateCallback = callback
    if (callback) {
      this.stateChanged()
    }
  }

  private stateChanged = () => {
    if (!this.isBleConnected() || this.connection === undefined) {
      this.reset()
    }

    if (this.stateCallback) {
      const {
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
      } = this

      this.stateCallback({
        isBleAvailable: this.isBleAvailable(),
        isBleConnected: this.isBleConnected(),
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
      })
    }
  }

  private rpcCall = async (method: string, params: any[] = []) => {
    if (!this.connection) {
      throw new Error('Not connected')
    }

    const res = await this.connection.rpcCall(method, params)
    if (res) {
      if (res.error) {
        throw new Error(res.error.message)
      }
      return res.result
    }

    throw new Error('Request timed out')
  }

  ping = async (): Promise<boolean> => {
    const res = await this.rpcCall(RPC_METHOD_PING) // returns string 'pong'
    return !!res?.length
  }

  listMethods = async (): Promise<Array<Record<string, string>>> => {
    const res = await this.rpcCall(RPC_METHOD_LIST_METHODS)
    return res as unknown as Array<Record<string, string>>
  }

  setPassword = async (password: string): Promise<boolean> => {
    const res = await this.rpcCall(RPC_METHOD_SET_PW, [password])

    await this.getStatus()

    return res as unknown as boolean
  }

  getStatus = async () => {
    const res = await this.rpcCall(RPC_METHOD_GET_STATUS)
    const [unlocked, seedPhraseSet, passwordSet] = res as unknown as [
      boolean,
      boolean,
      boolean,
    ]

    this.isUnlocked = unlocked
    this.isSeedPhraseSet = seedPhraseSet
    this.isPasswordSet = passwordSet
    this.stateChanged()

    return {
      unlocked,
      seedPhraseSet,
      passwordSet,
    }
  }

  unlock = async (password: string): Promise<boolean> => {
    const res = await this.rpcCall(RPC_METHOD_UNLOCK, [password])
    const unlocked = res as unknown as boolean

    if (unlocked) {
      this.isUnlocked = true

      await this.getDeviceInfo()
      await this.getSelectedWallet()
    }

    return unlocked
  }

  lock = async (): Promise<boolean> => {
    const res = await this.rpcCall(RPC_METHOD_LOCK)

    this.isUnlocked = false
    await this.getStatus()

    return res as unknown as boolean
  }

  wipe = async (): Promise<boolean> => {
    this.rpcCall(RPC_METHOD_WIPE)
    this.reset()
    return true
  }

  deletePairedDevices = async (): Promise<boolean> => {
    const res = await this.rpcCall(RPC_METHOD_DELETE_PAIRED_DEVICES)
    return res as unknown as boolean
  }

  createSeedPhrase = async (
    words?: 12 | 18 | 24,
    overwriteWalletId?: number
  ) => {
    const params = [words, overwriteWalletId]
    const res = await this.rpcCall(RPC_METHOD_CREATE_MNEMONIC, params)
    const [walletId, seedPhrase] = res as unknown as [number, string]
    return {
      walletId,
      seedPhrase,
    }
  }

  addSeedPhrase = async (
    seedPhrase: string,
    overwriteWalletId?: number
  ): Promise<number> => {
    const params = [seedPhrase, overwriteWalletId]
    const res = await this.rpcCall(RPC_METHOD_ADD_MNEMONIC, params)
    // return walletId
    return res as unknown as number
  }

  getDeviceInfo = async () => {
    const res = await this.rpcCall(RPC_METHOD_GET_INFO)
    const [deviceName, fwVersion, boardType, displayType, storedWallets] =
      res as unknown as [string, string, string, string, number]

    this.deviceName = deviceName
    this.fwVersion = fwVersion
    this.boardType = boardType
    this.displayType = displayType
    this.storedWallets = storedWallets
    this.stateChanged()

    return {
      deviceName,
      fwVersion,
      boardType,
      displayType,
      storedWallets,
    }
  }

  getSelectedWallet = async () => {
    const res = await this.rpcCall(RPC_METHOD_GET_WALLET)
    const [walletId, chainType, address, pubKey, hdPath] = res as unknown as [
      number,
      number,
      string,
      string,
      string,
    ]

    this.walletId = walletId
    this.chainType = chainType
    this.address = address
    this.pubKey = pubKey
    this.hdPath = hdPath
    this.stateChanged()

    return {
      walletId,
      chainType,
      address,
      pubKey,
      hdPath,
    }
  }

  exportRootXPub = async () => {
    const res = await this.rpcCall(RPC_METHOD_EXPORT_XPUB_ROOT)
    const [xPub, fingerprint] = res as unknown as [string, number]
    return {
      xPub,
      fingerprint,
    }
  }

  exportAccountXPub = async () => {
    const res = await this.rpcCall(RPC_METHOD_EXPORT_XPUB_ACCOUNT)
    const [xPub, fingerprint] = res as unknown as [string, number]
    return {
      xPub,
      fingerprint,
    }
  }

  selectWallet = async (
    _walletId: number,
    _hdPath?: string,
    _passphrase?: string,
    _chainTypeOverride?: number
  ) => {
    const res = await this.rpcCall(RPC_METHOD_SELECT_WALLET, [
      _walletId,
      _hdPath,
      _passphrase,
      _chainTypeOverride,
    ])
    const [walletId, chainType, address, pubKey, hdPath] = res as unknown as [
      number,
      number,
      string,
      string,
      string,
    ]

    this.walletId = walletId
    this.chainType = chainType
    this.address = address
    this.pubKey = pubKey
    this.hdPath = hdPath
    this.stateChanged()

    return {
      walletId,
      chainType,
      address,
      pubKey,
      hdPath,
    }
  }

  signMessage = async (message: string): Promise<string> => {
    const res = await this.rpcCall(RPC_METHOD_SIGN_MSG, [message])
    return res as unknown as string
  }

  ethSignTypedDataHash = async (
    domainSeparatorHash: string,
    messageHash: string
  ): Promise<string> => {
    const res = await this.rpcCall(RPC_METHOD_ETH_SIGN_TYPED_DATA_HASH, [
      domainSeparatorHash,
      messageHash,
    ])
    return res as unknown as string
  }

  ethSignTransaction = async (
    chainId: string,
    to: string,
    value: string,
    data: string,
    nonce: string,
    gasLimit: string,
    gasPrice: string,
    maxPriorityFee?: string
  ): Promise<string> => {
    const res = await this.rpcCall(RPC_METHOD_ETH_SIGN_TX, [
      chainId,
      to,
      value,
      data,
      nonce,
      gasLimit,
      gasPrice,
      maxPriorityFee,
    ])
    return res as unknown as string
  }
}
