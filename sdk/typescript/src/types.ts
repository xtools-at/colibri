export interface ColibriContextType {
  isBleAvailable: boolean
  isBleConnected: boolean
  connectBle: () => Promise<void>
  disconnectBle: () => void
}

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
