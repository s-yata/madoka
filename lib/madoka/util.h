// Copyright (c) 2012, Susumu Yata
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.

#ifndef MADOKA_UTIL_H
#define MADOKA_UTIL_H

#ifdef __cplusplus
 #include <cstddef>
#else  // __cplusplus
 #include <stddef.h>
#endif  // __cplusplus

#ifdef _MSC_VER
 #ifdef __cplusplus
  #include <intrin.h>
  #ifdef WIN64
   #pragma intrinsic(_BitScanReverse64)
  #else  // WIN64
   #pragma intrinsic(_BitScanReverse)
  #endif  // WIN64
 #endif  // __cplusplus
#else  // _MSC_VER
 #include <stdint.h>
#endif  // _MSC_VER

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifdef _MSC_VER
typedef unsigned __int8  madoka_uint8;
typedef unsigned __int16 madoka_uint16;
typedef unsigned __int32 madoka_uint32;
typedef unsigned __int64 madoka_uint64;
#else  // _MSC_VER
typedef uint8_t  madoka_uint8;
typedef uint16_t madoka_uint16;
typedef uint32_t madoka_uint32;
typedef uint64_t madoka_uint64;
#endif  // _MSC_VER

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
namespace madoka {

typedef ::madoka_uint8  UInt8;
typedef ::madoka_uint16 UInt16;
typedef ::madoka_uint32 UInt32;
typedef ::madoka_uint64 UInt64;

namespace util {

template <typename T>
inline void swap(T &lhs, T &rhs) throw() {
  const T temp = lhs;
  lhs = rhs;
  rhs = temp;
}

// bit_scan_reverse() returns the index of the most significant 1 bit of
// `value'. Note that bit_scan_reverse() returns 0 or 64 if `value' == 0.
inline UInt64 bit_scan_reverse(UInt64 value) throw() {
#ifdef _MSC_VER
  unsigned long index;
 #ifdef WIN64
  ::_BitScanReverse64(&index, value);
  return index;
 #else
  if (::_BitScanReverse(&index, static_cast<unsigned long>(value)) != 0) {
    return index;
  }
  ::_BitScanReverse(&index, static_cast<unsigned long>(value >> 32));
  return index + 32;
 #endif  // WIN64
#else  // _MSC_VER
  return ::__builtin_clzll(value);
#endif  // _MSC_VER
}

}  // namespace util
}  // namespace madoka
#endif  // __cplusplus

#endif  // MADOKA_UTIL_H
