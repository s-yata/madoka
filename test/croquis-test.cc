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

#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <madoka/croquis.h>
#include <madoka/random.h>

namespace {

const std::size_t NUM_KEYS       = 1 << 16;
const std::size_t MIN_KEY_LENGTH = 1;
const std::size_t MAX_KEY_LENGTH = 16;

void generate_keys(std::vector<std::string> *keys,
                   std::vector<madoka::UInt64> *freqs,
                   std::vector<std::size_t> *ids) {
  madoka::Random random;

  std::set<std::string> unique_keys;
  std::string key;
  while (unique_keys.size() < NUM_KEYS) {
    const std::size_t key_length = MIN_KEY_LENGTH +
        (random() % (MAX_KEY_LENGTH - MIN_KEY_LENGTH + 1));
    key.resize(key_length);
    for (std::size_t j = 0; j < key_length; ++j) {
      key[j] = 'A' + (random() % 26);
    }
    unique_keys.insert(key);
  }
  std::vector<std::string>(unique_keys.begin(), unique_keys.end()).swap(*keys);

  for (std::size_t i = 0; i < NUM_KEYS; ++i) {
    const std::size_t freq = NUM_KEYS / (i + 1);
    freqs->push_back(freq);
    for (std::size_t j = 0; j < freq; ++j) {
      ids->push_back(i);
    }
  }
  std::random_shuffle(ids->begin(), ids->end(), random);
}

template <typename T>
void test_croquis(const std::vector<std::string> &keys,
                  const std::vector<madoka::UInt64> &original_freqs,
                  const std::vector<std::size_t> &ids) {
  const char PATH[] = "croquis-test.temp.1";

  std::remove(PATH);

  madoka::Croquis<T> croquis;
  MADOKA_THROW_IF(croquis.max_value() != std::numeric_limits<T>::max());

  std::vector<T> freqs;
  for (std::size_t i = 0; i < original_freqs.size(); ++i) {
    freqs.push_back((original_freqs[i] < croquis.max_value()) ?
                    original_freqs[i] : croquis.max_value());
  }
  MADOKA_THROW_IF(freqs.size() != original_freqs.size());

  croquis.create(keys.size(), 3, PATH, madoka::FILE_TRUNCATE);
  MADOKA_THROW_IF(croquis.width() != keys.size());
  MADOKA_THROW_IF(croquis.depth() != 3);
  MADOKA_THROW_IF(croquis.seed() != 0);

  for (std::size_t i = 0; i < keys.size(); ++i) {
    croquis.set(keys[i].c_str(), keys[i].length(), freqs[i]);
    MADOKA_THROW_IF(croquis.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(croquis.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  croquis.close();

  croquis.open(PATH, madoka::FILE_PRIVATE);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(croquis.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  croquis.clear();
  croquis.close();

  croquis.open(PATH);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(croquis.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  croquis.clear();
  croquis.close();

  croquis.load(PATH);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(croquis.get(keys[i].c_str(), keys[i].length()) != 0);
  }

  croquis.create(keys.size() + 13, 5, NULL, 0, 123456789);
  MADOKA_THROW_IF(croquis.width() != (keys.size() + 13));
  MADOKA_THROW_IF(croquis.depth() != 5);
  MADOKA_THROW_IF(croquis.seed() != 123456789);

  for (std::size_t i = 0; i < ids.size(); ++i) {
    const std::string &key = keys[ids[i]];
    const T freq = croquis.add(key.c_str(), key.length(), 1);
    MADOKA_THROW_IF(freq == 0);
    MADOKA_THROW_IF(croquis.get(key.c_str(), key.length()) != freq);
  }
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(croquis.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  croquis.save(PATH, madoka::FILE_TRUNCATE);
  croquis.close();

  croquis.open(PATH);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(croquis.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  croquis.close();

  MADOKA_THROW_IF(std::remove(PATH) == -1);
}

void benchmark_croquis(const std::vector<std::string> &keys,
                       const std::vector<madoka::UInt64> &freqs,
                       const std::vector<std::size_t> &ids) {
  std::cout << "info: Zipf distribution: "
            << "#keys = " << keys.size()
            << ", #queries = " << ids.size() << std::endl;

  std::cout.setf(std::ios::fixed);

  madoka::Random random;
  for (madoka::UInt64 width = keys.size() / 8;
       width <= keys.size() * 8; width *= 2) {
    std::cout << "info: " << std::setw(6) << width << ':' << std::flush;
    for (int i = 0; i < 8; ++i) {
      madoka::Croquis<madoka::UInt32> croquis;
      croquis.create(width, 0, NULL, 0, random());
      for (std::size_t i = 0; i < ids.size(); ++i) {
        croquis.add(keys[ids[i]].c_str(), keys[ids[i]].length(), 1);
      }
      madoka::UInt64 diff = 0;
      for (std::size_t i = 0; i < keys.size(); ++i) {
        const madoka::UInt64 freq =
            croquis.get(keys[i].c_str(), keys[i].length());
        diff += std::llabs(freq - freqs[i]);
      }
      std::cout << ' ' << std::setw(6) << std::setprecision(3)
                << (100.0 * diff / ids.size()) << '%' << std::flush;
    }
    std::cout << std::endl;
  }
}

}  // namespace

int main() try {
  std::vector<std::string> keys;
  std::vector<madoka::UInt64> freqs;
  std::vector<std::size_t> ids;
  generate_keys(&keys, &freqs, &ids);

  MADOKA_THROW_IF(keys.size() != NUM_KEYS);
  MADOKA_THROW_IF(freqs.size() != NUM_KEYS);

#define TEST_CROQUIS(type) \
  ((std::cout << "log: " << __FILE__ << ':' << __LINE__ << ": " \
              << "test_croquis<" #type ">()" << std::endl), \
   test_croquis<type>(keys, freqs, ids))

  TEST_CROQUIS(madoka::UInt8);
  TEST_CROQUIS(madoka::UInt16);
  TEST_CROQUIS(madoka::UInt32);
  TEST_CROQUIS(madoka::UInt64);
  TEST_CROQUIS(bool);
  TEST_CROQUIS(float);
  TEST_CROQUIS(double);

#undef TEST_CROQUIS

  benchmark_croquis(keys, freqs, ids);

  return 0;
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
