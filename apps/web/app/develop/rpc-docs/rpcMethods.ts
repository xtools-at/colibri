export const rpcMethods = [
  {
    method: 'addSeedPhrase',
    params:
      '[string: 12/18/24 word mnemonic phrase, number: (opt.) existing wallet id to overwrite]',
    result: 'number: wallet id',
  },
  {
    method: 'createSeedPhrase',
    params:
      '[number: (opt.) length in words (12|18|24=default), number: (opt.) existing wallet id to overwrite]',
    result: '[number: wallet id, string: mnemonic phrase]',
  },
  { method: 'deletePairedDevices', params: '', result: 'bool: success' },
  {
    method: 'eth_signTransaction',
    params:
      '[string: chainId, string: to, string: value, string: data, string: nonce, string: gasLimit, string: gasPrice/maxFeePerGas, string: (opt.) maxPriorityFeePerGas (EIP-1559)]',
    result: 'string: signature',
  },
  {
    method: 'eth_signTypedDataHash',
    params: '[string: hex domain separator hash, string: hex message hash]',
    result: 'string: signature',
  },
  {
    method: 'exportAccountXPub',
    params: '',
    result: '[string: xpub, number: fingerprint]',
  },
  {
    method: 'exportRootXPub',
    params: '',
    result: '[string: xpub, number: fingerprint]',
  },
  {
    method: 'getDeviceInfo',
    params: '',
    result:
      '[string: name, string: firmware version, string: board type, string: display type, number: stored wallets]',
  },
  {
    method: 'getSelectedWallet',
    params: '',
    result:
      '[number: wallet id, number: chain type (1=BTC|2=ETH), string: address, string: pubkey, string: hdPath]',
  },
  {
    method: 'getStatus',
    params: '',
    result:
      '[bool: unlocked, bool: key set, bool: password set, number: remaining login attempts]',
  },
  {
    method: 'listMethods',
    params: '',
    result: '[...object: {string: method, string: params, string: result}]',
  },
  { method: 'lock', params: '', result: 'bool: success' },
  { method: 'ping', params: '', result: 'string: result' },
  {
    method: 'selectWallet',
    params:
      '[number: wallet id, string: (opt.) hd path, string: (opt.) bip32 passphrase, number: (opt.) force chain type (1=BTC|2=ETH)]',
    result:
      '[number: wallet id, number: chain type (1=BTC|2=ETH), string: address, string: pubkey, string: hdPath]',
  },
  {
    method: 'setPassword',
    params: '[string: password]',
    result: 'bool: success',
  },
  {
    method: 'signHash',
    params: '[string: hex digest]',
    result: 'string: signature',
  },
  {
    method: 'signMessage',
    params:
      '[string: plaintext message to sign, number: (opt.) chain type override]',
    result: 'string: signature',
  },
  { method: 'unlock', params: '[string: password]', result: 'bool: success' },
  { method: 'wipe', params: '', result: 'bool: success' },
]
