import { ColibriBleInterface } from './ColibriBleInterface'
import type { ColibriInterface } from './types'

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

export class Colibri {
  public ble: ColibriBleInterface
  private connection: ColibriInterface | undefined

  constructor(webBleLibrary?: Bluetooth) {
    this.ble = new ColibriBleInterface(webBleLibrary)
  }

  async connectBle() {
    await this.ble.connect()
    this.connection = this.ble
  }

  disconnectBle() {
    this.ble.disconnect()
    this.connection = undefined
  }

  isBleAvailable() {
    return this.ble.isAvailable()
  }

  isBleConnected() {
    return this.ble.isConnected()
  }

  private async rpcCall(method: string, params: any[] = []) {
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

  async ping(): Promise<boolean> {
    const res = await this.rpcCall(RPC_METHOD_PING) // returns string 'pong'
    return !!res?.length
  }

  async listMethods(): Promise<Array<Record<string, string>>> {
    const res = await this.rpcCall(RPC_METHOD_LIST_METHODS)
    return res as unknown as Array<Record<string, string>>
  }

  async setPassword(password: string): Promise<boolean> {
    const res = await this.rpcCall(RPC_METHOD_SET_PW, [password])
    return res as unknown as boolean
  }

  async getStatus() {
    const res = await this.rpcCall(RPC_METHOD_GET_STATUS)
    const [unlocked, seedPhraseSet, passwordSet] = res as unknown as [
      boolean,
      boolean,
      boolean,
    ]
    return {
      unlocked,
      seedPhraseSet,
      passwordSet,
    }
  }

  async unlock(password: string): Promise<boolean> {
    const res = await this.rpcCall(RPC_METHOD_UNLOCK, [password])
    return res as unknown as boolean
  }

  async lock(): Promise<boolean> {
    const res = await this.rpcCall(RPC_METHOD_LOCK)
    return res as unknown as boolean
  }

  async wipe(): Promise<boolean> {
    const res = await this.rpcCall(RPC_METHOD_WIPE)
    return res as unknown as boolean
  }

  async deletePairedDevices(): Promise<boolean> {
    const res = await this.rpcCall(RPC_METHOD_DELETE_PAIRED_DEVICES)
    return res as unknown as boolean
  }

  async createSeedPhrase(words?: 12 | 18 | 24, overwriteWalletId?: number) {
    const params = [words, overwriteWalletId]
    const res = await this.rpcCall(RPC_METHOD_CREATE_MNEMONIC, params)
    const [walletId, seedPhrase] = res as unknown as [number, string]
    return {
      walletId,
      seedPhrase,
    }
  }

  async addSeedPhrase(
    seedPhrase: string,
    overwriteWalletId?: number
  ): Promise<number> {
    const params = [seedPhrase, overwriteWalletId]
    const res = await this.rpcCall(RPC_METHOD_ADD_MNEMONIC, params)
    // return walletId
    return res as unknown as number
  }

  async getDeviceInfo() {
    const res = await this.rpcCall(RPC_METHOD_GET_INFO)
    const [name, fwVersion, boardType, displayType, storedWallets] =
      res as unknown as [string, string, string, string, number]

    return {
      name,
      fwVersion,
      boardType,
      displayType,
      storedWallets,
    }
  }

  async getSelectedWallet() {
    const res = await this.rpcCall(RPC_METHOD_GET_WALLET)
    const [walletId, chainType, address, pubKey, hdPath] = res as unknown as [
      number,
      number,
      string,
      string,
      string,
    ]

    return {
      walletId,
      chainType,
      address,
      pubKey,
      hdPath,
    }
  }

  async exportRootXPub() {
    const res = await this.rpcCall(RPC_METHOD_EXPORT_XPUB_ROOT)
    const [xPub, fingerprint] = res as unknown as [string, number]
    return {
      xPub,
      fingerprint,
    }
  }

  async exportAccountXPub() {
    const res = await this.rpcCall(RPC_METHOD_EXPORT_XPUB_ACCOUNT)
    const [xPub, fingerprint] = res as unknown as [string, number]
    return {
      xPub,
      fingerprint,
    }
  }

  async selectWallet(
    _walletId: number,
    _hdPath?: string,
    _passphrase?: string,
    _chainTypeOverride?: number
  ) {
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

    return {
      walletId,
      chainType,
      address,
      pubKey,
      hdPath,
    }
  }

  async signMessage(
    message: string,
    chainTypeOverride?: number
  ): Promise<string> {
    const res = await this.rpcCall(RPC_METHOD_SIGN_MSG, [
      message,
      chainTypeOverride,
    ])
    return res as unknown as string
  }

  async ethSignTypedDataHash(
    domainSeparatorHash: string,
    messageHash: string
  ): Promise<string> {
    const res = await this.rpcCall(RPC_METHOD_ETH_SIGN_TYPED_DATA_HASH, [
      domainSeparatorHash,
      messageHash,
    ])
    return res as unknown as string
  }

  async ethSignTransaction(
    chainId: string,
    to: string,
    value: string,
    data: string,
    nonce: string,
    gasLimit: string,
    gasPrice: string,
    maxPriorityFee?: string
  ): Promise<string> {
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
