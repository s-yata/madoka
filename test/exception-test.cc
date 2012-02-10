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

#include <cstring>
#include <iostream>

#include <madoka/exception.h>

int main() try {
  bool ignored = false;
  try {
    MADOKA_THROW("error message");
    ignored = true;
  } catch (const madoka::Exception &ex) {
    MADOKA_THROW_IF(
        std::strcmp(ex.what(), "exception-test.cc:33: error message") != 0);
  }
  MADOKA_THROW_IF(ignored);

  try {
    MADOKA_THROW_IF(!ignored);
    ignored = true;
  } catch (const madoka::Exception &ex) {
    MADOKA_THROW_IF(
        std::strcmp(ex.what(), "exception-test.cc:42: !ignored") != 0);
  }
  MADOKA_THROW_IF(ignored);

  return 0;
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
