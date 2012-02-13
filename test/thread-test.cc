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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <madoka/sketch.h>
#include <madoka/random.h>

namespace {

const std::size_t NUM_KEYS       = 1 << 17;
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

void do_approx_count(madoka::UInt64 count,
                     madoka::UInt64 *approx, madoka::Random *random) {
  for (madoka::UInt64 i = 0; i < count; ++i) {
    *approx = madoka::Approx::inc(*approx, random);
  }
}

void test_approx() {
  std::cout << "info: Approx: catastrophic" << std::endl;

  std::cout.setf(std::ios::fixed);

  for (madoka::UInt64 count = 1 << 20; count <= (1 << 24); count <<= 1) {
    std::cout << "info: " << std::setw(8) << count << ':' << std::flush;
    for (int num_threads = 1; num_threads <= 8; ++num_threads) {
      madoka::UInt64 approx = 0;
      madoka::Random random;

      std::vector<std::thread> threads;
      for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(do_approx_count, count / num_threads,
                                      &approx, &random));
      }

      for (std::size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
      }

      const madoka::UInt64 value = madoka::Approx::decode(approx);
      std::cout << ' ' << std::setw(5) << std::setprecision(3)
                << (static_cast<double>(value) / count) << std::flush;
    }
    std::cout << std::endl;
  }
}

void do_sketch_count(const std::vector<std::string> &keys,
                     std::vector<std::size_t>::const_iterator begin,
                     std::vector<std::size_t>::const_iterator end,
                     madoka::Sketch *sketch) {
  for (std::vector<std::size_t>::const_iterator it = begin; it != end; ++it) {
    sketch->inc(keys[*it].c_str(), keys[*it].length());
  }
}

void test_sketch() {
  std::vector<std::string> keys;
  std::vector<madoka::UInt64> freqs;
  std::vector<std::size_t> ids;
  generate_keys(&keys, &freqs, &ids);

  MADOKA_THROW_IF(keys.size() != NUM_KEYS);
  MADOKA_THROW_IF(freqs.size() != NUM_KEYS);

  std::cout << "info: Sketch: Zipf distribution: "
            << "#keys = " << keys.size()
            << ", #queries = " << ids.size() << std::endl;

  std::cout.setf(std::ios::fixed);

  madoka::Random random;
  for (madoka::UInt64 width = keys.size() / 4;
       width <= keys.size() * 4; width *= 2) {
    std::cout << "info: " << std::setw(6) << width << ':' << std::flush;
    for (int num_threads = 1; num_threads <= 8; ++num_threads) {
      madoka::Sketch sketch;
      sketch.create(width);

      std::vector<std::thread> threads;
      for (int i = 0; i < num_threads; ++i) {
        const auto begin = ids.begin() + (ids.size() / num_threads) * i;
        const auto end = ids.begin() + (ids.size() / num_threads) * (i + 1);
        threads.push_back(std::thread(do_sketch_count, keys, begin, end,
                                      &sketch));
      }

      for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
      }

      madoka::UInt64 diff = 0;
      for (std::size_t i = 0; i < keys.size(); ++i) {
        const madoka::UInt64 freq =
            sketch.get(keys[i].c_str(), keys[i].length());
        diff += std::llabs(freq - freqs[i]);
      }
      std::cout << ' ' << std::setw(6) << std::setprecision(3)
                << (100.0 * diff / ids.size()) << '%' << std::flush;
    }
    std::cout << std::endl;
  }
}

void test_merge() {
  std::vector<std::string> keys;
  std::vector<madoka::UInt64> freqs;
  std::vector<std::size_t> ids;
  generate_keys(&keys, &freqs, &ids);

  MADOKA_THROW_IF(keys.size() != NUM_KEYS);
  MADOKA_THROW_IF(freqs.size() != NUM_KEYS);

  std::cout << "info: Sketch (merge): Zipf distribution: "
            << "#keys = " << keys.size()
            << ", #queries = " << ids.size() << std::endl;

  std::cout.setf(std::ios::fixed);

  madoka::Random random;
  for (madoka::UInt64 width = keys.size() / 4;
       width <= keys.size() * 4; width *= 2) {
    std::cout << "info: " << std::setw(6) << width << ':' << std::flush;
    for (int num_threads = 1; num_threads <= 8; ++num_threads) {
      madoka::Sketch * const sketches = new madoka::Sketch[num_threads];
      for (int i = 0; i < num_threads; ++i) {
        sketches[i].create(width);
      }

      std::vector<std::thread> threads;
      for (int i = 0; i < num_threads; ++i) {
        const auto begin = ids.begin() + (ids.size() / num_threads) * i;
        const auto end = ids.begin() + (ids.size() / num_threads) * (i + 1);
        threads.push_back(std::thread(do_sketch_count, keys, begin, end,
                                      &sketches[i]));
      }

      for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
      }

      for (int i = 1; i < num_threads; ++i) {
        sketches[0].merge(sketches[i]);
      }

      madoka::UInt64 diff = 0;
      for (std::size_t i = 0; i < keys.size(); ++i) {
        const madoka::UInt64 freq =
            sketches[0].get(keys[i].c_str(), keys[i].length());
        diff += std::llabs(freq - freqs[i]);
      }
      std::cout << ' ' << std::setw(6) << std::setprecision(3)
                << (100.0 * diff / ids.size()) << '%' << std::flush;

      delete [] sketches;
    }
    std::cout << std::endl;
  }
}

}  // namespace

int main() try {
  test_approx();
  test_sketch();
  test_merge();

  return 0;
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
