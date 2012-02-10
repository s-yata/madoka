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

#include "file.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <limits>

namespace madoka {

File::File() throw() : fd_(-1), addr_(NULL), size_(0), flags_(0) {}

File::~File() throw() {
  if ((addr_ != NULL) && (size_ != 0)) {
    ::munmap(addr_, size_);
  }
  if (fd_ != -1) {
    ::close(fd_);
  }
}

void File::create(const char *path, std::size_t size,
                  int flags) throw(Exception) {
  File new_file;
  new_file.create_(path, size, flags);
  new_file.swap(this);
}

void File::open(const char *path, int flags) throw(Exception) {
  File new_file;
  new_file.open_(path, flags);
  new_file.swap(this);
}

void File::close() throw() {
  File().swap(this);
}

void File::load(const char *path, int flags) throw(Exception) {
  File new_file;
  new_file.load_(path, flags);
  new_file.swap(this);
}

void File::save(const char *path, int flags) const throw(Exception) {
  File file;
  file.create(path, size(), flags);
  std::memcpy(file.addr(), addr(), size());
}

void File::swap(File *file) throw() {
  util::swap(fd_, file->fd_);
  util::swap(addr_, file->addr_);
  util::swap(size_, file->size_);
  util::swap(flags_, file->flags_);
}

void File::create_(const char *path, std::size_t size,
                   int flags) throw(Exception) {
  const int VALID_FLAGS = FILE_TRUNCATE | FILE_HUGETLB | FILE_PRELOAD;
  MADOKA_THROW_IF(flags & ~VALID_FLAGS);

  flags |= FILE_CREATE | FILE_WRITABLE | FILE_SHARED;

  if (path == NULL) {
    flags |= FILE_ANONYMOUS;
  } else {
    if (~flags & FILE_TRUNCATE) {
      struct stat stat;
      if (::stat(path, &stat) == 0) {
        MADOKA_THROW("file already exists");
      }
    }

    fd_ = ::open(path, get_open_flags(flags), 0666);
    if (fd_ == -1) {
      MADOKA_THROW("::open() failed");
    }

    if (size != 0) {
      if (::ftruncate(fd_, size) == -1) {
        MADOKA_THROW("::ftruncate() failed");
      }
    }
  }

  if (size == 0) {
    static char DUMMY_BUF[1];
    addr_ = DUMMY_BUF;
  } else {
    void *map_addr = ::mmap(NULL, size, get_prot_flags(flags),
                            get_map_flags(flags), fd_, 0);
#ifdef MAP_HUGETLB
    if ((map_addr == MAP_FAILED) && (flags & FILE_HUGETLB)) {
      flags &= ~FILE_HUGETLB;
      map_addr = ::mmap(NULL, size, get_prot_flags(flags),
                        get_map_flags(flags), fd_, 0);
    }
#endif  // MAP_HUGETLB
    if (map_addr == MAP_FAILED) {
      MADOKA_THROW("::mmap() failed");
    }
    addr_ = map_addr;
  }
  size_ = size;
  flags_ = flags;

  if (flags & FILE_PRELOAD) {
    volatile madoka::UInt64 count = 0;
    for (std::size_t offset = 0; offset < size_; offset += 1024) {
      count += *static_cast<UInt8 *>(addr_) + offset;
    }
  }
}

void File::open_(const char *path, int flags) throw(Exception) {
  MADOKA_THROW_IF(path == NULL);

  const int VALID_FLAGS = FILE_READONLY | FILE_PRIVATE |
                          FILE_HUGETLB | FILE_PRELOAD;
  MADOKA_THROW_IF(flags & ~VALID_FLAGS);

  if (~flags & FILE_READONLY) {
    flags |= FILE_WRITABLE;
  }
  if (~flags & FILE_PRIVATE) {
    flags |= FILE_SHARED;
  }

  struct stat stat;
  if (::stat(path, &stat) == -1) {
    MADOKA_THROW("::stat() failed");
  }
  const std::size_t size = stat.st_size;
  MADOKA_THROW_IF(size > std::numeric_limits<std::size_t>::max());

  fd_ = ::open(path, get_open_flags(flags));
  if (fd_ == -1) {
    MADOKA_THROW("::open() failed");
  }

  if (size == 0) {
    static char DUMMY_BUF[1];
    addr_ = DUMMY_BUF;
  } else {
    void *map_addr = ::mmap(NULL, size, get_prot_flags(flags),
                            get_map_flags(flags), fd_, 0);
#ifdef MAP_HUGETLB
    if ((map_addr == MAP_FAILED) && (flags & FILE_HUGETLB)) {
      flags &= ~FILE_HUGETLB;
      map_addr = ::mmap(NULL, size, get_prot_flags(flags),
                        get_map_flags(flags), fd_, 0);
    }
#endif  // MAP_HUGETLB
    if (map_addr == MAP_FAILED) {
      MADOKA_THROW("::mmap() failed");
    }
    addr_ = map_addr;
  }
  size_ = size;
  flags_ = flags;

  if (flags & FILE_PRELOAD) {
    volatile madoka::UInt64 count = 0;
    for (std::size_t offset = 0; offset < size_; offset += 1024) {
      count += *static_cast<UInt8 *>(addr_) + offset;
    }
  }
}

void File::load_(const char *path, int flags) throw(Exception) {
  MADOKA_THROW_IF(path == NULL);

  const int VALID_FLAGS = FILE_HUGETLB;
  MADOKA_THROW_IF(flags & ~VALID_FLAGS);

  File file;
  file.open(path);

  create(NULL, file.size(), flags);
  std::memcpy(addr(), file.addr(), size());
}

int File::get_open_flags(int flags) throw() {
  int open_flags = 0;
  if (flags & FILE_CREATE) {
    open_flags |= O_CREAT;
  }
  if (flags & FILE_TRUNCATE) {
    open_flags |= O_TRUNC;
  }
  if (flags & FILE_READONLY) {
    open_flags |= O_RDONLY;
  }
  if (flags & FILE_WRITABLE) {
    open_flags |= O_RDWR;
  }
  return open_flags;
}

int File::get_prot_flags(int flags) throw() {
  int prot_flags = PROT_READ;
  if (flags & FILE_WRITABLE) {
    prot_flags |= PROT_WRITE;
  }
  return prot_flags;
}

int File::get_map_flags(int flags) throw() {
  int map_flags = 0;
  if (flags & FILE_SHARED) {
    map_flags |= MAP_SHARED;
  }
  if (flags & FILE_PRIVATE) {
    map_flags |= MAP_PRIVATE;
  }
  if (flags & FILE_ANONYMOUS) {
    map_flags |= MAP_ANONYMOUS;
  }
#ifdef MAP_HUGETLB
  if (flags & FILE_HUGETLB) {
    map_flags |= MAP_HUGETLB;
  }
#endif  // MAP_HUGETLB
  return map_flags;
}

}  // namespace madoka
