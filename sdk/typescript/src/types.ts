export interface ColibriStatus {
  isBleAvailable: boolean
  isBleConnected: boolean
  isUnlocked: boolean
  isSeedPhraseSet: boolean
  isPasswordSet: boolean
  deviceName: string
  fwVersion: string
  boardType: string
  displayType: string
  storedWallets: number
  walletId: number
  chainType: number
  address: string
  pubKey: string
  hdPath: string
}

export interface ColibriMethods {
  connectBle: () => Promise<void>
  disconnectBle: () => void
  ping: () => Promise<boolean>
  listMethods: () => Promise<Array<Record<string, string>>>
  setPassword: (password: string) => Promise<boolean>
  getStatus: () => Promise<{
    unlocked: boolean
    seedPhraseSet: boolean
    passwordSet: boolean
  }>
  unlock: (password: string) => Promise<boolean>
  lock: () => Promise<boolean>
  wipe: () => Promise<boolean>
  deletePairedDevices: () => Promise<boolean>
  createSeedPhrase: (
    words?: 12 | 18 | 24,
    overwriteWalletId?: number
  ) => Promise<{ walletId: number; seedPhrase: string }>
  addSeedPhrase: (
    seedPhrase: string,
    overwriteWalletId?: number
  ) => Promise<number>
  getDeviceInfo: () => Promise<{
    deviceName: string
    fwVersion: string
    boardType: string
    displayType: string
    storedWallets: number
  }>
  getSelectedWallet: () => Promise<{
    walletId: number
    chainType: number
    address: string
    pubKey: string
    hdPath: string
  }>
  exportRootXPub: () => Promise<{ xPub: string; fingerprint: number }>
  exportAccountXPub: () => Promise<{ xPub: string; fingerprint: number }>
  selectWallet: (
    _walletId: number,
    _hdPath?: string,
    _passphrase?: string,
    _chainTypeOverride?: number
  ) => Promise<{
    walletId: number
    chainType: number
    address: string
    pubKey: string
    hdPath: string
  }>
  signMessage: (message: string) => Promise<string>
  ethSignTypedDataHash: (
    domainSeparatorHash: string,
    messageHash: string
  ) => Promise<string>
  ethSignTransaction: (
    chainId: string,
    to: string,
    value: string,
    data: string,
    nonce: string,
    gasLimit: string,
    gasPrice: string,
    maxPriorityFee?: string
  ) => Promise<string>
}

export interface ColibriContextType extends ColibriStatus, ColibriMethods {}

interface JsonRpcError {
  code: number
  message: string
  data?: any
}

interface JsonRpcBase {
  jsonrpc: '2.0'
  id: number | null
}

interface JsonRpcErrorResponse extends JsonRpcBase {
  error: JsonRpcError
}

interface JsonRpcSuccessResponse<T = any> extends JsonRpcBase {
  result: T
}

export interface JsonRpcResponse<T = any>
  extends JsonRpcErrorResponse,
    JsonRpcSuccessResponse<T> {}

export interface JsonRpcRequest extends JsonRpcBase {
  method: string
  params?: Array<any>
}

export interface ColibriInterface {
  connected: boolean
  isAvailable(): boolean
  isConnected(): boolean
  connect(): Promise<void>
  disconnect(): void
  rpcCall(method: string, params?: any[]): Promise<JsonRpcResponse | undefined>
}
