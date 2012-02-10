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

#include "approx.h"

namespace madoka {

const UInt64 Approx::OFFSET_TABLE[APPROX_MAX_EXPONENT + 1] = {
           0, 1ULL << 14, 1ULL << 15, 1ULL << 16, 1ULL << 17, 1ULL << 18,
  1ULL << 19, 1ULL << 20, 1ULL << 21, 1ULL << 22, 1ULL << 23, 1ULL << 24,
  1ULL << 25, 1ULL << 26, 1ULL << 27, 1ULL << 28, 1ULL << 29, 1ULL << 30,
  1ULL << 31, 1ULL << 32, 1ULL << 33, 1ULL << 34, 1ULL << 35, 1ULL << 36,
  1ULL << 37, 1ULL << 38, 1ULL << 39, 1ULL << 40, 1ULL << 41, 1ULL << 42,
  1ULL << 43, 1ULL << 44
};

const UInt8 Approx::SHIFT_TABLE[APPROX_MAX_EXPONENT + 1] = {
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
};

const UInt32 Approx::MASK_TABLE[APPROX_MAX_EXPONENT + 1] = {
  0x00000000, 0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000F,
  0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF, 0x000001FF, 0x000003FF,
  0x000007FF, 0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
  0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF, 0x001FFFFF, 0x003FFFFF,
  0x007FFFFF, 0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
  0x1FFFFFFF, 0x3FFFFFFF
};

}  // namespace madoka
