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

#include <iostream>

#include <madoka/exception.h>
#include <madoka/header.h>

int main() try {
  madoka::Header header;

  header.set_width(1ULL << 30);
  header.set_depth(3);
  header.set_max_value((1ULL << 28) - 1);
  header.set_value_size(28);
  header.set_seed(123456789);
  header.set_table_size(1ULL << 32);
  header.set_file_size((1ULL << 32) + sizeof(madoka::Header));

  MADOKA_THROW_IF(header.width() != (1ULL << 30));
  MADOKA_THROW_IF(header.width_mask() != ((1ULL << 30) - 1));
  MADOKA_THROW_IF(header.depth() != (3));
  MADOKA_THROW_IF(header.max_value() != ((1ULL << 28) - 1));
  MADOKA_THROW_IF(header.value_size() != 28);
  MADOKA_THROW_IF(header.seed() != 123456789);
  MADOKA_THROW_IF(header.table_size() != (1ULL << 32));
  MADOKA_THROW_IF(header.file_size() !=
                  ((1ULL << 32) + sizeof(madoka::Header)));

  header.set_width(123456789);
  MADOKA_THROW_IF(header.width() != 123456789);
  MADOKA_THROW_IF(header.width_mask() != 0);

  return 0;
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
