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

#include <cstdio>
#include <cstring>
#include <iostream>

#include <madoka/file.h>

int main() try {
  const char PATH_1[] = "file-test.temp.1";
  const char PATH_2[] = "file-test.temp.2";

  std::remove(PATH_1);
  std::remove(PATH_2);

  madoka::File file;

  file.create(NULL, 1 << 16);
  MADOKA_THROW_IF(file.addr() == NULL);
  MADOKA_THROW_IF(file.size() != (1 << 16));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_WRITABLE | madoka::FILE_PRIVATE | madoka::FILE_ANONYMOUS));
  std::memset(file.addr(), 0x01, file.size());
  file.close();

  try {
    file.open(PATH_1);
    MADOKA_THROW("madoka::File::open() succeeded");
  } catch (const madoka::Exception &ex) {
    std::cout << "log: " << __FILE__ << ':' << __LINE__ << ": "
              << ex.what() << std::endl;
  }
  file.close();

  file.create(PATH_1, 1 << 17);
  MADOKA_THROW_IF(file.size() != (1 << 17));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_CREATE | madoka::FILE_WRITABLE | madoka::FILE_SHARED));
  std::memset(file.addr(), 0x02, file.size());
  file.close();

  bool ignored = false;
  try {
    file.create(PATH_1, 1 << 18);
    ignored = true;
  } catch (const madoka::Exception &ex) {
    std::cout << "log: " << __FILE__ << ':' << __LINE__ << ": "
              << ex.what() << std::endl;
  }
  MADOKA_THROW_IF(ignored);

  file.open(PATH_1);
  MADOKA_THROW_IF(*static_cast<const madoka::UInt8 *>(file.addr()) != 0x02);
  MADOKA_THROW_IF(file.size() != (1 << 17));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_WRITABLE | madoka::FILE_SHARED));
  std::memset(file.addr(), 0x03, file.size());
  file.close();

  file.open(PATH_1, madoka::FILE_READONLY);
  MADOKA_THROW_IF(*static_cast<const madoka::UInt8 *>(file.addr()) != 0x03);
  MADOKA_THROW_IF(file.size() != (1 << 17));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_READONLY | madoka::FILE_SHARED));
  file.close();

  file.open(PATH_1, madoka::FILE_PRELOAD);
  MADOKA_THROW_IF(*static_cast<const madoka::UInt8 *>(file.addr()) != 0x03);
  MADOKA_THROW_IF(file.size() != (1 << 17));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_WRITABLE | madoka::FILE_SHARED | madoka::FILE_PRELOAD));
  file.close();

  file.open(PATH_1, madoka::FILE_PRIVATE);
  MADOKA_THROW_IF(*static_cast<const madoka::UInt8 *>(file.addr()) != 0x03);
  MADOKA_THROW_IF(file.size() != (1 << 17));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_WRITABLE | madoka::FILE_PRIVATE));
  std::memset(file.addr(), 0x04, file.size());
  file.close();

  file.load(PATH_1);
  MADOKA_THROW_IF(*static_cast<const madoka::UInt8 *>(file.addr()) != 0x03);
  MADOKA_THROW_IF(file.size() != (1 << 17));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_WRITABLE | madoka::FILE_PRIVATE | madoka::FILE_ANONYMOUS));
  std::memset(file.addr(), 0x05, file.size());
  file.close();

  file.open(PATH_1);
  MADOKA_THROW_IF(*static_cast<const madoka::UInt8 *>(file.addr()) != 0x03);
  MADOKA_THROW_IF(file.size() != (1 << 17));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_WRITABLE | madoka::FILE_SHARED));
  file.close();

  file.create(PATH_1, 1 << 19, madoka::FILE_TRUNCATE);
  MADOKA_THROW_IF(file.size() != (1 << 19));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_CREATE | madoka::FILE_TRUNCATE | madoka::FILE_WRITABLE |
       madoka::FILE_SHARED));
  std::memset(file.addr(), 0x06, file.size());

  file.save(PATH_2);
  try {
    file.save(PATH_2);
    ignored = true;
  } catch (const madoka::Exception &ex) {
    std::cout << "log: " << __FILE__ << ':' << __LINE__ << ": "
              << ex.what() << std::endl;
  }
  MADOKA_THROW_IF(ignored);
  file.close();

  file.open(PATH_2);
  MADOKA_THROW_IF(*static_cast<const madoka::UInt8 *>(file.addr()) != 0x06);
  MADOKA_THROW_IF(file.size() != (1 << 19));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_WRITABLE | madoka::FILE_SHARED));

  std::memset(file.addr(), 0x07, file.size());
  file.save(PATH_1, madoka::FILE_TRUNCATE);
  file.close();

  file.open(PATH_1);
  MADOKA_THROW_IF(*static_cast<const madoka::UInt8 *>(file.addr()) != 0x07);
  MADOKA_THROW_IF(file.size() != (1 << 19));
  MADOKA_THROW_IF(file.flags() !=
      (madoka::FILE_WRITABLE | madoka::FILE_SHARED));
  file.close();

  MADOKA_THROW_IF(std::remove(PATH_1) == -1);
  MADOKA_THROW_IF(std::remove(PATH_2) == -1);

  return 0;
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
