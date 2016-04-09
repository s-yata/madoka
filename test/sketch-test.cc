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
#include <vector>

#include <madoka/sketch.h>
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

void basic_test(madoka::UInt64 max_value,
                const std::vector<std::string> &keys,
                const std::vector<madoka::UInt64> &original_freqs,
                const std::vector<std::size_t> &ids) {
  const char PATH[] = "sketch-test.temp.1";

  std::remove(PATH);

  madoka::Sketch sketch;

  sketch.create(keys.size(), max_value, PATH);
  MADOKA_THROW_IF(sketch.width() != keys.size());
  MADOKA_THROW_IF(sketch.depth() != madoka::SKETCH_DEPTH);
  MADOKA_THROW_IF(sketch.max_value() != max_value);
  MADOKA_THROW_IF(sketch.seed() != 0);

  std::vector<madoka::UInt64> freqs;
  for (std::size_t i = 0; i < original_freqs.size(); ++i) {
    freqs.push_back((original_freqs[i] < sketch.max_value()) ?
                    original_freqs[i] : sketch.max_value());
  }

  for (std::size_t i = 0; i < keys.size(); ++i) {
    sketch.set(keys[i].c_str(), keys[i].length(), original_freqs[i]);
    MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  sketch.close();

  sketch.open(PATH, madoka::FILE_PRIVATE);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }

  sketch.set("query", 5, sketch.max_value());
  if (sketch.mode() == madoka::SKETCH_EXACT_MODE) {
    MADOKA_THROW_IF(sketch.get("query", 5) != sketch.max_value());
    MADOKA_THROW_IF(sketch.inc("query", 5) != sketch.max_value());
    MADOKA_THROW_IF(sketch.get("query", 5) != sketch.max_value());
  } else {
    MADOKA_THROW_IF(madoka::Approx::encode(sketch.get("query", 5)) !=
                    madoka::Approx::encode(sketch.max_value()));
    MADOKA_THROW_IF(madoka::Approx::encode(sketch.inc("query", 5)) !=
                    madoka::Approx::encode(sketch.max_value()));
    MADOKA_THROW_IF(madoka::Approx::encode(sketch.get("query", 5)) !=
                    madoka::Approx::encode(sketch.max_value()));
  }

  sketch.clear();
  sketch.close();

  sketch.open(PATH);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) < freqs[i]);
  }
  sketch.clear();
  sketch.close();

  sketch.load(PATH);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) != 0);
  }

  sketch.create(keys.size() + 13, max_value, NULL, 0, 123456789);
  MADOKA_THROW_IF(sketch.width() != (keys.size() + 13));
  MADOKA_THROW_IF(sketch.depth() != madoka::SKETCH_DEPTH);
  MADOKA_THROW_IF(sketch.seed() != 123456789);

  for (std::size_t i = 0; i < ids.size(); ++i) {
    const std::string &key = keys[ids[i]];
    const madoka::UInt64 freq = sketch.add(key.c_str(), key.length(), 1);
    MADOKA_THROW_IF(freq == 0);
    if (sketch.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch.get(key.c_str(), key.length()) != freq);
    } else {
      const madoka::UInt64 freq_approx = madoka::Approx::encode(freq);
      const madoka::UInt64 sketch_approx =
          madoka::Approx::encode(sketch.get(key.c_str(), key.length()));
      MADOKA_THROW_IF(sketch_approx != freq_approx);
    }
  }
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (sketch.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) <
                      freqs[i]);
    } else {
      MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) <
                      (freqs[i] * 0.975));
    }
  }
  sketch.save(PATH, madoka::FILE_TRUNCATE);
  sketch.close();

  sketch.open(PATH);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (sketch.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) <
                      freqs[i]);
    } else {
      MADOKA_THROW_IF(sketch.get(keys[i].c_str(), keys[i].length()) <
                      (freqs[i] * 0.975));
    }
  }
  sketch.close();

  MADOKA_THROW_IF(std::remove(PATH) == -1);
}

void extra_test(madoka::UInt64 max_value,
                const std::vector<std::string> &keys,
                const std::vector<madoka::UInt64> &original_freqs,
                const std::vector<std::size_t> &) {
  const char PATH_1[] = "sketch-test.temp.1";
  const char PATH_2[] = "sketch-test.temp.2";

  std::remove(PATH_1);
  std::remove(PATH_2);

  madoka::Sketch sketch;
  sketch.create(keys.size(), max_value);

  std::vector<madoka::UInt64> freqs;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    sketch.set(keys[i].c_str(), keys[i].length(), original_freqs[i]);
    freqs.push_back(sketch.get(keys[i].c_str(), keys[i].length()));
  }

  madoka::Sketch sketch_1;
  sketch_1.copy(sketch, PATH_1);
  MADOKA_THROW_IF(sketch_1.width() != sketch.width());
  MADOKA_THROW_IF(sketch_1.max_value() != sketch.max_value());
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (sketch_1.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch_1.get(keys[i].c_str(), keys[i].length()) !=
                      freqs[i]);
    } else {
      const madoka::UInt64 freq_approx = madoka::Approx::encode(freqs[i]);
      const madoka::UInt64 sketch_approx = madoka::Approx::encode(
          sketch_1.get(keys[i].c_str(), keys[i].length()));
      MADOKA_THROW_IF(sketch_approx != freq_approx);
    }
  }

  sketch_1.filter(NULL);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (sketch_1.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch_1.get(keys[i].c_str(), keys[i].length()) !=
                      freqs[i]);
    } else {
      const madoka::UInt64 freq_approx = madoka::Approx::encode(freqs[i]);
      const madoka::UInt64 sketch_approx = madoka::Approx::encode(
          sketch_1.get(keys[i].c_str(), keys[i].length()));
      MADOKA_THROW_IF(sketch_approx != freq_approx);
    }
  }

  sketch_1.filter([](madoka::UInt64 x) { return x / 2; });
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (sketch_1.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch_1.get(keys[i].c_str(), keys[i].length()) !=
                      (freqs[i] / 2));
    } else {
      const madoka::UInt64 freq_approx = madoka::Approx::encode(freqs[i] / 2);
      const madoka::UInt64 sketch_approx = madoka::Approx::encode(
          sketch_1.get(keys[i].c_str(), keys[i].length()));
      MADOKA_THROW_IF(sketch_approx != freq_approx);
    }
  }

  madoka::Sketch sketch_2;
  sketch_2.shrink(sketch);
  MADOKA_THROW_IF(sketch_2.width() != sketch.width());
  MADOKA_THROW_IF(sketch_2.max_value() != sketch.max_value());
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (sketch_2.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch_2.get(keys[i].c_str(), keys[i].length()) !=
                      freqs[i]);
    } else {
      const madoka::UInt64 freq_approx = madoka::Approx::encode(freqs[i]);
      const madoka::UInt64 sketch_approx = madoka::Approx::encode(
          sketch_2.get(keys[i].c_str(), keys[i].length()));
      MADOKA_THROW_IF(sketch_approx != freq_approx);
    }
  }

  sketch_2.shrink(sketch, sketch.width() / 2);
  MADOKA_THROW_IF(sketch_2.width() != (sketch.width() / 2));
  MADOKA_THROW_IF(sketch_2.max_value() != sketch.max_value());
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (sketch_2.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch_2.get(keys[i].c_str(), keys[i].length()) <
                      freqs[i]);
    } else {
      const madoka::UInt64 freq_approx = madoka::Approx::encode(freqs[i]);
      const madoka::UInt64 sketch_approx = madoka::Approx::encode(
          sketch_2.get(keys[i].c_str(), keys[i].length()));
      MADOKA_THROW_IF(sketch_approx < freq_approx);
    }
  }

  sketch_2.shrink(sketch, 0, 15);
  MADOKA_THROW_IF(sketch_2.width() != sketch.width());
  MADOKA_THROW_IF(sketch_2.max_value() != 15);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(sketch_2.get(keys[i].c_str(), keys[i].length()) !=
                    std::min(freqs[i], sketch_2.max_value()));
  }

  sketch_2.shrink(sketch, 0, 1,
                  [](madoka::UInt64 x) -> madoka::UInt64 { return x > 10; });
  MADOKA_THROW_IF(sketch_2.width() != sketch.width());
  MADOKA_THROW_IF(sketch_2.max_value() != 1);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    MADOKA_THROW_IF(sketch_2.get(keys[i].c_str(), keys[i].length()) !=
                    (freqs[i] > 10));
  }

  sketch_2.copy(sketch, PATH_2, madoka::FILE_TRUNCATE);
  sketch_2.merge(sketch_1, [](madoka::UInt64 x) { return x / 2; }, NULL);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (sketch_2.mode() == madoka::SKETCH_EXACT_MODE) {
      MADOKA_THROW_IF(sketch_2.get(keys[i].c_str(), keys[i].length()) !=
                      ((freqs[i] / 2) * 2));
    } else {
      const madoka::UInt64 freq_approx =
          madoka::Approx::encode((freqs[i] / 2) * 2);
      const madoka::UInt64 sketch_approx = madoka::Approx::encode(
          sketch_2.get(keys[i].c_str(), keys[i].length()));
      MADOKA_THROW_IF(sketch_approx != freq_approx);
    }
  }

  double lhs_square_length;
  double rhs_square_length;
  const double inner_product =
      sketch.inner_product(sketch_2, &lhs_square_length, &rhs_square_length);
  const double lhs_length = std::sqrt(lhs_square_length);
  const double rhs_length = std::sqrt(rhs_square_length);
  if ((lhs_length != 0.0) && (rhs_length != 0.0)) {
    const double cosine = inner_product / lhs_length / rhs_length;
    MADOKA_THROW_IF(cosine < 0.9);
    MADOKA_THROW_IF(cosine > (1.0 + 1e-9));
  }

  sketch_1.close();
  sketch_2.close();

  MADOKA_THROW_IF(std::remove(PATH_1) == -1);
  MADOKA_THROW_IF(std::remove(PATH_2) == -1);
}

void benchmark_sketch(const std::vector<std::string> &keys,
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
      madoka::Sketch sketch;
      sketch.create(width, 0, NULL, 0, random());
      for (std::size_t i = 0; i < ids.size(); ++i) {
        sketch.inc(keys[ids[i]].c_str(), keys[ids[i]].length());
      }
      madoka::UInt64 diff = 0;
      for (std::size_t j = 0; j < keys.size(); ++j) {
        const madoka::UInt64 freq =
            sketch.get(keys[j].c_str(), keys[j].length());
        diff += std::llabs(freq - freqs[j]);
      }
      std::cout << ' ' << std::setw(6) << std::setprecision(3)
                << (100.0 * diff / ids.size()) << '%' << std::flush;
    }
    std::cout << std::endl;
  }
}

void benchmark_shrink(const std::vector<std::string> &keys,
                      const std::vector<madoka::UInt64> &freqs,
                      const std::vector<std::size_t> &ids) {
  std::cout << "info: Zipf distribution: "
            << "#keys = " << keys.size()
            << ", #queries = " << ids.size() << std::endl;

  std::cout.setf(std::ios::fixed);

  std::cout << "info:      -:";
  for (madoka::UInt64 width = keys.size() / 16;
       width <= keys.size() * 8; width *= 2) {
    std::cout << ' ' << std::setw(7) << width;
  }
  std::cout << std::endl;

  for (madoka::UInt64 width = keys.size() / 16;
       width <= keys.size() * 8; width *= 2) {
    std::cout << "info: " << std::setw(6) << width << ':' << std::flush;

    madoka::Sketch sketch;
    sketch.create(width, 0, NULL, 0);
    for (std::size_t i = 0; i < ids.size(); ++i) {
      sketch.inc(keys[ids[i]].c_str(), keys[ids[i]].length());
    }

    for (madoka::UInt64 shrinked_width = keys.size() / 16;
         shrinked_width <= width; shrinked_width *= 2) {
      madoka::Sketch shrinked_sketch;
      shrinked_sketch.shrink(sketch, shrinked_width);

      madoka::UInt64 diff = 0;
      for (std::size_t i = 0; i < keys.size(); ++i) {
        const madoka::UInt64 freq =
            shrinked_sketch.get(keys[i].c_str(), keys[i].length());
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

#define BASIC_TEST(max_value) \
  ((std::cout << "log: " << __FILE__ << ':' << __LINE__ << ": " \
              << "basic_test(" #max_value ")" << std::endl), \
   basic_test(max_value, keys, freqs, ids))

  BASIC_TEST(1);
  BASIC_TEST(3);
  BASIC_TEST(15);
  BASIC_TEST(255);
  BASIC_TEST(65535);
  BASIC_TEST(madoka::SKETCH_MAX_MAX_VALUE);

#undef BASIC_TEST

#define EXTRA_TEST(max_value) \
  ((std::cout << "log: " << __FILE__ << ':' << __LINE__ << ": " \
              << "extra_test(" #max_value ")" << std::endl), \
   extra_test(max_value, keys, freqs, ids))

  EXTRA_TEST(1);
  EXTRA_TEST(3);
  EXTRA_TEST(15);
  EXTRA_TEST(255);
  EXTRA_TEST(65535);
  EXTRA_TEST(madoka::SKETCH_MAX_MAX_VALUE);

#undef EXTRA_TEST

  benchmark_sketch(keys, freqs, ids);
  benchmark_shrink(keys, freqs, ids);

  return 0;
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
