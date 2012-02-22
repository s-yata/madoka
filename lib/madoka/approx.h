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

#ifndef MADOKA_APPROX_H
#define MADOKA_APPROX_H

#include "random.h"

#ifdef __cplusplus
namespace madoka {

const UInt64 APPROX_SIGNIFICAND_SIZE  = 14;
const UInt64 APPROX_MAX_SIGNIFICAND   = (1ULL << APPROX_SIGNIFICAND_SIZE) - 1;
const UInt64 APPROX_SIGNIFICAND_MASK  = APPROX_MAX_SIGNIFICAND;
const UInt64 APPROX_SIGNIFICAND_SHIFT = 0;

const UInt64 APPROX_EXPONENT_SIZE     = 5;
const UInt64 APPROX_MAX_EXPONENT      = (1ULL << APPROX_EXPONENT_SIZE) - 1;
const UInt64 APPROX_EXPONENT_MASK     = APPROX_MAX_EXPONENT;
const UInt64 APPROX_EXPONENT_SHIFT    = APPROX_SIGNIFICAND_SIZE;

const UInt64 APPROX_SIZE              =
    APPROX_EXPONENT_SIZE + APPROX_SIGNIFICAND_SIZE;
const UInt64 APPROX_MASK              = (1ULL << APPROX_SIZE) - 1;

const UInt64 APPROX_VALUE_SIZE        =
    APPROX_SIGNIFICAND_SIZE + (1 << APPROX_EXPONENT_SIZE) - 1;
const UInt64 APPROX_MAX_VALUE         = (1ULL << APPROX_VALUE_SIZE) - 1;
const UInt64 APPROX_VALUE_MASK        = APPROX_MAX_VALUE;

class Approx {
 public:
  static const UInt64 OFFSET_TABLE[APPROX_MAX_EXPONENT + 1];
  static const UInt8 SHIFT_TABLE[APPROX_MAX_EXPONENT + 1];
  static const UInt32 MASK_TABLE[APPROX_MAX_EXPONENT + 1];

  static UInt64 encode(UInt64 value) throw() {
    value &= APPROX_VALUE_MASK;
    const UInt64 exponent =
        50 - util::bit_scan_reverse(value | APPROX_SIGNIFICAND_MASK);
    return (exponent << APPROX_EXPONENT_SHIFT) |
        ((value >> SHIFT_TABLE[exponent]) & APPROX_SIGNIFICAND_MASK);
  }

  static UInt64 decode(UInt64 approx) throw() {
    const UInt64 significand =
        (approx >> APPROX_SIGNIFICAND_SHIFT) & APPROX_SIGNIFICAND_MASK;
    const UInt64 exponent =
        (approx >> APPROX_EXPONENT_SHIFT) & APPROX_EXPONENT_MASK;
    return OFFSET_TABLE[exponent] | (significand << SHIFT_TABLE[exponent]);
  }

  static UInt64 decode(UInt64 approx, Random *random) throw() {
    const UInt64 exponent =
        (approx >> APPROX_EXPONENT_SHIFT) & APPROX_EXPONENT_MASK;
    const UInt64 significand =
        (approx >> APPROX_SIGNIFICAND_SHIFT) & APPROX_SIGNIFICAND_MASK;
    return OFFSET_TABLE[exponent] | (significand << SHIFT_TABLE[exponent]) |
        ((*random)() & MASK_TABLE[exponent]);
  }

  static UInt64 inc(UInt64 approx, Random *random) throw() {
    const UInt64 exponent =
        (approx >> APPROX_EXPONENT_SHIFT) & APPROX_EXPONENT_MASK;
    approx += (((*random)() & MASK_TABLE[exponent]) + 1) >>
        SHIFT_TABLE[exponent];
    return approx;
  }
};

}  // namespace madoka
#endif  // __cplusplus

#endif  // MADOKA_APPROX_H
