// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rlp.h"

static bool decode_length_unchecked(const uint8_t* buf, size_t buf_len, size_t* offset, size_t* len, bool* is_list) {
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

// the encoded item, header and payload, must fit in the remaining buffer
static bool decode_length(const uint8_t* buf, size_t buf_len, size_t* offset, size_t* len, bool* is_list) {
  if (!decode_length_unchecked(buf, buf_len, offset, len, is_list)) return false;
  return *offset <= buf_len && *len <= buf_len - *offset;
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
  p->length = p->offset + len;
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
  item->isList = is_list;

  p->offset += offset + len;

  return true;
}

static uint64_t rlp_to_uint64(const RLPItem* item) {
  uint64_t value = 0;

  for (size_t i = 0; i < item->len; i++) value = (value << 8) | item->ptr[i];

  return value;
}

// next item as a byte string of at most maxLen bytes
static bool rlp_next_bytes(RLPParser* p, RLPItem* item, size_t maxLen) {
  return rlp_next(p, item) && !item->isList && item->len <= maxLen;
}

static bool rlp_copy(RLPParser* p, uint8_t* dst, size_t* dstSize, size_t maxLen, const char* name) {
  RLPItem item;
  if (!rlp_next_bytes(p, &item, maxLen)) {
    log_e("Invalid tx field: %s", name);
    return false;
  }
  memcpy(dst, item.ptr, item.len);
  *dstSize = item.len;
  printf("%s: %s\n", name, toHex(item.ptr, item.len, true).c_str());
  return true;
}

static bool rlp_chain_id(RLPParser* p, uint64_t* chainId) {
  RLPItem item;
  if (!rlp_next_bytes(p, &item, sizeof(uint64_t))) {
    log_e("Invalid tx field: chainId");
    return false;
  }
  *chainId = rlp_to_uint64(&item);
  printf("chainId: %llu\n", *chainId);
  return true;
}

static bool rlp_to(RLPParser* p, EthTxData& tx) {
  RLPItem item;
  // contract creation (empty `to`) is not supported
  if (!rlp_next_bytes(p, &item, ADDRESS_LENGTH) || item.len != ADDRESS_LENGTH) {
    log_e("Invalid tx field: to");
    return false;
  }
  memcpy(tx.to, item.ptr, ADDRESS_LENGTH);
  printf("to: %s\n", toHex(item.ptr, item.len, true).c_str());
  return true;
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
  size_t list_len;
  if (!rlp_enter_list(&parser, &list_len)) return false;

  return rlp_copy(&parser, tx.nonce, &tx.nonceSize, sizeof(tx.nonce), "nonce") &&
         rlp_copy(&parser, tx.maxGasFee, &tx.maxGasFeeSize, sizeof(tx.maxGasFee), "gasPrice") &&
         rlp_copy(&parser, tx.gasLimit, &tx.gasLimitSize, sizeof(tx.gasLimit), "gasLimit") && rlp_to(&parser, tx) &&
         rlp_copy(&parser, tx.value, &tx.valueSize, sizeof(tx.value), "value") &&
         rlp_copy(&parser, tx.data, &tx.dataSize, sizeof(tx.data), "data") && rlp_chain_id(&parser, &tx.chainId);
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
  size_t list_len;
  if (!rlp_enter_list(&parser, &list_len)) return false;

  return rlp_chain_id(&parser, &tx.chainId) &&
         rlp_copy(&parser, tx.nonce, &tx.nonceSize, sizeof(tx.nonce), "nonce") &&
         rlp_copy(&parser, tx.maxPriorityFeePerGas, &tx.maxPriorityFeePerGasSize, sizeof(tx.maxPriorityFeePerGas),
                  "maxPriorityFeePerGas") &&
         rlp_copy(&parser, tx.maxGasFee, &tx.maxGasFeeSize, sizeof(tx.maxGasFee), "maxFeePerGas") &&
         rlp_copy(&parser, tx.gasLimit, &tx.gasLimitSize, sizeof(tx.gasLimit), "gasLimit") && rlp_to(&parser, tx) &&
         rlp_copy(&parser, tx.value, &tx.valueSize, sizeof(tx.value), "value") &&
         rlp_copy(&parser, tx.data, &tx.dataSize, sizeof(tx.data), "data");
}
