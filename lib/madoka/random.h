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

#ifndef MADOKA_RANDOM_H
#define MADOKA_RANDOM_H

#include "util.h"

#ifdef __cplusplus
namespace madoka {

class Random {
 public:
  explicit Random(UInt64 seed = 0) noexcept
    : x_(123456789U ^ static_cast<UInt32>(seed & 0xFFFF)),
      y_(362436069U ^ static_cast<UInt32>((seed >> 16) & 0xFFFF)),
      z_(521288629U ^ static_cast<UInt32>((seed >> 32) & 0xFFFF)),
      w_(88675123U ^ static_cast<UInt32>(seed >> 48)) {}
  ~Random() noexcept {}

  Random(const Random &random) noexcept
    : x_(random.x_), y_(random.y_), z_(random.z_), w_(random.w_) {}

  Random &operator=(const Random &rhs) noexcept {
    x_ = rhs.x_;
    y_ = rhs.y_;
    z_ = rhs.z_;
    w_ = rhs.w_;
    return *this;
  }

  void reset(UInt64 seed = 0) noexcept {
    x_ = 123456789U ^ static_cast<UInt32>(seed & 0xFFFF);
    y_ = 362436069U ^ static_cast<UInt32>((seed >> 16) & 0xFFFF);
    z_ = 521288629U ^ static_cast<UInt32>((seed >> 32) & 0xFFFF);
    w_ = 88675123U ^ static_cast<UInt32>(seed >> 48);
  }

  UInt32 operator()() noexcept {
    const UInt32 t = x_ ^ (x_ << 11);
    x_ = y_;
    y_ = z_;
    z_ = w_;
    w_ = (w_ ^ (w_ >> 19)) ^ (t ^ (t >> 8));
    return w_;
  }

  UInt32 operator()(UInt32 x) noexcept {
    return operator()() % x;
  }

  void swap(Random *random) noexcept {
    util::swap(x_, random->x_);
    util::swap(y_, random->y_);
    util::swap(z_, random->z_);
    util::swap(w_, random->w_);
  }

 private:
  UInt32 x_;
  UInt32 y_;
  UInt32 z_;
  UInt32 w_;
};

}  // namespace madoka
#endif  // __cplusplus

#endif  // MADOKA_RANDOM_H
