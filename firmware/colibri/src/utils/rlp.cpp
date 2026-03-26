// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rlp.h"

static bool decode_length(const uint8_t* buf, size_t buf_len, size_t* offset, size_t* len, bool* is_list) {
  if (buf_len == 0) return false;

  uint8_t prefix = buf[0];

  if (prefix <= 0x7f) {
    *offset = 0;
    *len = 1;
    *is_list = false;
    return true;
  }

  if (prefix <= 0xb7) {
    *offset = 1;
    *len = prefix - 0x80;
    *is_list = false;
    return true;
  }

  if (prefix <= 0xbf) {
    size_t len_of_len = prefix - 0xb7;
    if (buf_len < 1 + len_of_len) return false;

    size_t l = 0;
    for (size_t i = 0; i < len_of_len; i++) l = (l << 8) | buf[1 + i];

    *offset = 1 + len_of_len;
    *len = l;
    *is_list = false;
    return true;
  }

  if (prefix <= 0xf7) {
    *offset = 1;
    *len = prefix - 0xc0;
    *is_list = true;
    return true;
  }

  size_t len_of_len = prefix - 0xf7;

  if (buf_len < 1 + len_of_len) return false;

  size_t l = 0;
  for (size_t i = 0; i < len_of_len; i++) l = (l << 8) | buf[1 + i];

  *offset = 1 + len_of_len;
  *len = l;
  *is_list = true;

  return true;
}

static bool rlp_init(RLPParser* p, const uint8_t* buffer, size_t length) {
  p->buffer = buffer;
  p->length = length;
  p->offset = 0;
  return true;
}

static bool rlp_enter_list(RLPParser* p, size_t* list_len) {
  size_t offset, len;
  bool is_list;

  if (!decode_length(p->buffer + p->offset, p->length - p->offset, &offset, &len, &is_list)) return false;

  if (!is_list) return false;

  p->offset += offset;
  *list_len = len;

  return true;
}

static bool rlp_next(RLPParser* p, RLPItem* item) {
  if (p->offset >= p->length) return false;

  size_t offset, len;
  bool is_list;

  if (!decode_length(p->buffer + p->offset, p->length - p->offset, &offset, &len, &is_list)) return false;

  item->ptr = p->buffer + p->offset + offset;
  item->len = len;

  p->offset += offset + len;

  return true;
}

static uint64_t rlp_to_uint64(const RLPItem* item) {
  uint64_t value = 0;

  for (size_t i = 0; i < item->len; i++) value = (value << 8) | item->ptr[i];

  return value;
}

bool decodeLegacyTx(const uint8_t* data, size_t dataLen, EthTxData& tx) {
  // legacy tx
  /*
    [
      nonce,
      gasPrice, // wei
      gasLimit, // e.g. 21000
      to,
      value, // wei
      data,
      chainId,
      0,
      0
    ]
  */

  tx.isEip1559 = false;

  // parse RLP data
  RLPParser parser;
  rlp_init(&parser, data, dataLen);
  RLPItem item;
  size_t list_len;
  rlp_enter_list(&parser, &list_len);

  // nonce
  rlp_next(&parser, &item);
  tx.nonceSize = item.len;
  memcpy(tx.nonce, item.ptr, item.len);
  printf("nonce: %llu\n", rlp_to_uint64(&item));

  // gas price
  rlp_next(&parser, &item);
  tx.maxGasFeeSize = item.len;
  memcpy(tx.maxGasFee, item.ptr, item.len);
  printf("maxGasFee: %llu\n", rlp_to_uint64(&item));

  // gas limit
  rlp_next(&parser, &item);
  tx.gasLimitSize = item.len;
  memcpy(tx.gasLimit, item.ptr, item.len);
  printf("gasLimit: %llu\n", rlp_to_uint64(&item));

  // to address
  rlp_next(&parser, &item);
  memcpy(tx.to, item.ptr, ADDRESS_LENGTH);
  printf("to: ");
  printf(toHex(item.ptr, item.len, true).c_str());
  printf("\n");

  // value
  rlp_next(&parser, &item);
  tx.valueSize = item.len;
  memcpy(tx.value, item.ptr, item.len);
  printf("value: %llu\n", rlp_to_uint64(&item));

  // data
  rlp_next(&parser, &item);
  tx.dataSize = item.len;
  memcpy(tx.data, item.ptr, item.len);
  printf("data: ");
  printf(toHex(item.ptr, item.len, true).c_str());
  printf("\n");

  // chainId
  rlp_next(&parser, &item);
  tx.chainId = rlp_to_uint64(&item);
  printf("chainId: %llu\n", tx.chainId);
}

bool decodeEip1559Tx(const uint8_t* data, size_t dataLen, EthTxData& tx) {
  // EIP-1559 typed tx
  /*
    [
      chainId,
      nonce,
      maxPriorityFeePerGas,
      maxFeePerGas,
      gasLimit,
      to,
      value,
      data,
      accessList
    ]
  */

  tx.isEip1559 = true;

  // parse RLP data
  RLPParser parser;
  rlp_init(&parser, data, dataLen);
  RLPItem item;
  size_t list_len;
  rlp_enter_list(&parser, &list_len);

  // chainId
  rlp_next(&parser, &item);
  tx.chainId = rlp_to_uint64(&item);
  printf("chainId: %llu\n", tx.chainId);

  // nonce
  rlp_next(&parser, &item);
  tx.nonceSize = item.len;
  memcpy(tx.nonce, item.ptr, item.len);
  printf("nonce: %llu\n", rlp_to_uint64(&item));

  // maxPriorityFeePerGas
  rlp_next(&parser, &item);
  tx.maxPriorityFeePerGasSize = item.len;
  memcpy(tx.maxPriorityFeePerGas, item.ptr, item.len);
  printf("maxPriorityFeePerGas: %llu\n", rlp_to_uint64(&item));

  // maxGasFee
  rlp_next(&parser, &item);
  tx.maxGasFeeSize = item.len;
  memcpy(tx.maxGasFee, item.ptr, item.len);
  printf("maxGasFee: %llu\n", rlp_to_uint64(&item));

  // gas limit
  rlp_next(&parser, &item);
  tx.gasLimitSize = item.len;
  memcpy(tx.gasLimit, item.ptr, item.len);
  printf("gasLimit: %llu\n", rlp_to_uint64(&item));

  // to address
  rlp_next(&parser, &item);
  memcpy(tx.to, item.ptr, ADDRESS_LENGTH);
  printf("to: ");
  printf(toHex(item.ptr, item.len, true).c_str());
  printf("\n");

  // value
  rlp_next(&parser, &item);
  tx.valueSize = item.len;
  memcpy(tx.value, item.ptr, item.len);
  printf("value: %llu\n", rlp_to_uint64(&item));

  // data
  rlp_next(&parser, &item);
  tx.dataSize = item.len;
  memcpy(tx.data, item.ptr, item.len);
  printf("data: ");
  printf(toHex(item.ptr, item.len, true).c_str());
  printf("\n");
}
