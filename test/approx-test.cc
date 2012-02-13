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

#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <madoka/approx.h>
#include <madoka/exception.h>
#include <madoka/random.h>

int main() try {
  for (madoka::UInt64 approx = 0; approx <= madoka::APPROX_MASK; ++approx) {
    const madoka::UInt64 value = madoka::Approx::decode(approx);
    MADOKA_THROW_IF(value > madoka::APPROX_MAX_VALUE);
    MADOKA_THROW_IF(value < approx);
    MADOKA_THROW_IF(madoka::Approx::encode(value) != approx);
  }

  madoka::Random random;
  for (madoka::UInt64 i = 0; i < (1ULL << 16); ++i) {
    madoka::UInt64 value = random();
    madoka::UInt64 approx = madoka::Approx::encode(value);

    madoka::UInt64 diff = value - madoka::Approx::decode(approx);
    MADOKA_THROW_IF(diff > (value / (madoka::APPROX_MAX_SIGNIFICAND + 1)));

    diff = std::llabs(value - madoka::Approx::decode(approx, &random));
    MADOKA_THROW_IF(diff > (value / (madoka::APPROX_MAX_SIGNIFICAND + 1)));

    value = ((value << 32) | random()) & madoka::APPROX_VALUE_MASK;
    approx = madoka::Approx::encode(value);

    diff = value - madoka::Approx::decode(approx);
    MADOKA_THROW_IF(diff > (value / (madoka::APPROX_MAX_SIGNIFICAND + 1)));

    diff = std::llabs(value - madoka::Approx::decode(approx, &random));
    MADOKA_THROW_IF(diff > (value / (madoka::APPROX_MAX_SIGNIFICAND + 1)));
  }

  std::cout.setf(std::ios::fixed);

  for (madoka::UInt64 count = 1 << 15; count <= (1 << 23); count <<= 1) {
    std::cout << "info: " << std::setw(7) << count << ':' << std::flush;
    for (int i = 0; i < 8; ++i) {
      madoka::UInt64 approx = 0;
      for (madoka::UInt64 j = 0; j < count; ++j) {
        approx = madoka::Approx::inc(approx, &random);
      }
      const madoka::UInt64 value = madoka::Approx::decode(approx);
      std::cout << ' ' << std::setw(7) << std::setprecision(5)
                << (static_cast<double>(value) / count) << std::flush;
      MADOKA_THROW_IF(value < (count * 0.975));
      MADOKA_THROW_IF(value > (count * 1.025));
    }
    std::cout << std::endl;
  }

  return 0;
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
