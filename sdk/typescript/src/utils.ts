export function stringToBuffer(str: string): ArrayBuffer {
  // we don't need encoding, only char->byte conversion
  const buffer = new ArrayBuffer(str.length)
  const view = new Uint8Array(buffer)
  for (let i = 0; i < str.length; i++) {
    view[i] = str.charCodeAt(i)
  }
  return buffer
}

export function bufferToString(buffer: ArrayBuffer): string {
  const view = new Uint8Array(buffer)
  let str = ''
  for (let i = 0; i < view.length; i++) {
    if (view[i] != undefined) {
      str += String.fromCharCode(view[i] as number)
    }
  }
  return str
}

export function dataViewToBuffer(dataView: DataView): ArrayBuffer {
  return dataView.buffer.slice(
    dataView.byteOffset,
    dataView.byteOffset + dataView.byteLength
  ) as ArrayBuffer
}
