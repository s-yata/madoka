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

#include <getopt.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <madoka.h>

#include "../config.h"
#include "timer.h"

namespace {

const madoka::UInt64 NUM_TRIALS = 5;

std::vector<const char *> KEYSET_PATHS;
const char *SKETCH_PATH = NULL;

madoka::UInt64 MIN_LENGTH = 6;
madoka::UInt64 MAX_LENGTH = 12;
madoka::UInt64 NUM_KEYS = 1 << 16;

madoka::UInt64 THRESHOLD = 0;

madoka::UInt64 MIN_WIDTH = 0;
madoka::UInt64 MAX_WIDTH = 0;

double ZIPF_FACTOR = 1.0;
double FREQ_FACTOR = 1.0;
double WIDTH_FACTOR = 1.4142135623730951;

int FILE_FLAGS = 0;

void print_version() {
  std::cout << "Version: " << PACKAGE_STRING << "\n"
            << "Description: "
            << "Count-Min sketch-based approximate counting"
            << std::endl;
}

void print_help(const char *name) {
  std::cout << "Usage: " << name << " [OPTION]... [SKETCH] [FILE]...\n"
            << "\n"
            << "Options:\n"
            << "  -l, --min-length=[N]    specify the minimum key length\n"
            << "  -L, --max-length=[N]    specify the maximum key length\n"
            << "  -k, --num-keys=[N]      "
            << "specify the number of distinct keys\n"
            << "  -t, --threshold=[N]     specify the frequency threshold\n"
            << "  -w, --min-width=[N]     specify the minimum sketch width\n"
            << "  -W, --max-width=[N]     specify the maximum sketch width\n"
            << "  -s, --zipf-factor=[N]   "
            << "specify the value of characterizing the Zipfian\n"
            << "                          "
            << "distribution, see the following page for more details\n"
            << "                          "
            << "http://en.wikipedia.org/wiki/Zipf%27s_law\n"
            << "  -r, --freq-factor=[N]   "
            << "specify the factor of frequency reduction\n"
            << "  -e, --width-factor=[N]  "
            << "specify the factor of width expansion\n"
            << "  -i, --input=[PATH]      specify the path of a keyset file\n"
            << "  -o, --output=[PATH]     specify the path of sketches\n"
            << "  -v, --version           print the version\n"
            << "  -h, --help              print this message\n"
            << "\n"
            << "Report bugs to <" << PACKAGE_BUGREPORT << ">"
            << std::endl;
}

template <typename T, typename U>
T parse_str(const char *str, T min_value, U max_value) {
  T value = static_cast<T>(0);
  std::stringstream(str) >> value;
  MADOKA_THROW_IF(value < min_value);
  MADOKA_THROW_IF(value > max_value);
  return value;
}

void parse_args(int argc, char *argv[]) {
  const struct option long_options[] = {
    { "min-length", 1, NULL, 'l' },
    { "max-length", 1, NULL, 'L' },
    { "num-keys", 1, NULL, 'k' },
    { "threshold", 1, NULL, 't' },
    { "min-width", 1, NULL, 'w' },
    { "max-width", 1, NULL, 'W' },
    { "zipf-factor", 1, NULL, 's' },
    { "freq-factor", 1, NULL, 'r' },
    { "width-factor", 1, NULL, 'e' },
    { "input", 1, NULL, 'i' },
    { "output", 1, NULL, 'o' },
    { "version", 0, NULL, 'v' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, 0 }
  };

  int option_label = -1;
  while ((option_label = ::getopt_long(argc, argv, "l:L:k:t:w:W:s:r:e:i:o:vh",
                                       long_options, NULL)) != -1) {
    switch (option_label) {
      case 'l': {
        MIN_LENGTH = parse_str(::optarg, 1ULL, 100ULL);
        break;
      }
      case 'L': {
        MAX_LENGTH = parse_str(::optarg, 6ULL, 100ULL);
        break;
      }
      case 'k': {
        NUM_KEYS = parse_str(::optarg, 1ULL, 1ULL << 30);
        break;
      }
      case 't': {
        THRESHOLD = parse_str(::optarg, 1ULL, madoka::SKETCH_MAX_MAX_VALUE);
        break;
      }
      case 'w': {
        MIN_WIDTH = parse_str(::optarg, 1ULL, madoka::SKETCH_MAX_WIDTH);
        break;
      }
      case 'W': {
        MAX_WIDTH = parse_str(::optarg, 1ULL, madoka::SKETCH_MAX_WIDTH);
        break;
      }
      case 's': {
        ZIPF_FACTOR = parse_str(::optarg, 0.0, 3.0);
        break;
      }
      case 'r': {
        FREQ_FACTOR = parse_str(::optarg, 0.0, 1.0);
        break;
      }
      case 'e': {
        WIDTH_FACTOR = parse_str(::optarg, 1.0, 2.0);
        break;
      }
      case 'i': {
        KEYSET_PATHS.push_back(::optarg);
        break;
      }
      case 'o': {
        SKETCH_PATH = ::optarg;
        break;
      }
      case 'v': {
        print_version();
        std::exit(0);
      }
      case 'h': {
        print_help(argv[0]);
        std::exit(0);
      }
      default: {
        std::exit(1);
      }
    }
  }

  MADOKA_THROW_IF(MIN_LENGTH > MAX_LENGTH);

  if ((MIN_WIDTH != 0) && (MAX_WIDTH != 0)) {
    MADOKA_THROW_IF(MIN_WIDTH > MAX_WIDTH);
  }

  for (int i = ::optind; i < argc; ++i) {
    KEYSET_PATHS.push_back(argv[i]);
  }

  for (std::size_t i = 0; i < KEYSET_PATHS.size(); ++i) {
    std::cerr << "KEYSET_PATH[" << i << "]: " << KEYSET_PATHS[i] << std::endl;
  }

  if (SKETCH_PATH != NULL) {
    std::cerr << "SKETCH_PATH: " << SKETCH_PATH << std::endl;
    FILE_FLAGS |= madoka::FILE_TRUNCATE;
  }
}

void gen_keys(std::vector<std::string> *keys,
              std::vector<madoka::UInt64> *freqs,
              std::vector<madoka::UInt64> *ids) {
  madoka::Random random;

  std::string key;
  std::set<std::string> keyset;
  while (keyset.size() != NUM_KEYS) {
    const madoka::UInt64 length =
        MIN_LENGTH + (random() % (MAX_LENGTH - MIN_LENGTH - 1));
    MADOKA_THROW_IF(length < MIN_LENGTH);
    MADOKA_THROW_IF(length > MAX_LENGTH);

    key.resize(static_cast<std::size_t>(length));
    for (madoka::UInt64 i = 0; i < length; ++i) {
      key[i] = static_cast<char>('A' + (random() % 26));
    }
    keyset.insert(key);
  }
  std::vector<std::string>(keyset.begin(), keyset.end()).swap(*keys);

  const double NUMERATOR = static_cast<double>(NUM_KEYS);
  for (std::size_t i = 0; i < keys->size(); ++i) {
    const madoka::UInt64 freq = static_cast<madoka::UInt64>(
        std::pow(NUMERATOR / (i + 1), ZIPF_FACTOR));
    MADOKA_THROW_IF(freq == 0);

    freqs->push_back(freq);
    for (madoka::UInt64 j = 0; j < freq; ++j) {
      ids->push_back(i);
    }
  }
  std::random_shuffle(ids->begin(), ids->end(), random);
}

void load_keys(std::vector<std::string> *keys,
               std::vector<madoka::UInt64> *freqs,
               std::vector<madoka::UInt64> *ids) {
  typedef std::map<std::string, madoka::UInt64> KeyMap;
  KeyMap keymap;
  for (std::size_t i = 0; i < KEYSET_PATHS.size(); ++i) {
    std::ifstream file(KEYSET_PATHS[i], std::ios::binary);
    MADOKA_THROW_IF(!file);

    std::string line;
    while (std::getline(file, line)) {
      const std::string::size_type delim_pos = line.find_last_of('\t');
      if (delim_pos == line.npos) {
        std::pair<KeyMap::iterator, bool> result =
            keymap.insert(std::make_pair(line, 1));
        if (result.second) {
          ++result.first->second;
        }
      } else {
        const madoka::UInt64 freq = parse_str(line.c_str() + delim_pos + 1,
            1ULL, madoka::SKETCH_MAX_MAX_VALUE);
        std::pair<KeyMap::iterator, bool> result =
            keymap.insert(std::make_pair(line.substr(0, delim_pos), freq));
        if (result.second) {
          result.first->second += freq;
        }
      }
    }
  }

  MIN_LENGTH = std::numeric_limits<madoka::UInt64>::max();
  MAX_LENGTH = 0;

  for (KeyMap::iterator it = keymap.begin(); it != keymap.end(); ++it) {
    const madoka::UInt64 freq =
        static_cast<madoka::UInt64>(it->second * FREQ_FACTOR);
    if (freq > 0) {
      keys->push_back(it->first);
      freqs->push_back(freq);

      if (it->first.length() < MIN_LENGTH) {
        MIN_LENGTH = it->first.length();
      }
      if (it->first.length() > MAX_LENGTH) {
        MAX_LENGTH = it->first.length();
      }
    }
  }

  for (madoka::UInt64 i = 0; i < keys->size(); ++i) {
    const madoka::UInt64 freq = (*freqs)[i];
    for (madoka::UInt64 j = 0; j < freq; ++j) {
      ids->push_back(i);
    }
  }

  madoka::Random random;
  std::random_shuffle(ids->begin(), ids->end(), random);
}

madoka::UInt64 threshold_filter(madoka::UInt64 value) {
  return value >= THRESHOLD;
}

void benchmark_filter(const std::vector<std::string> &keys,
                      const std::vector<madoka::UInt64> &freqs,
                      const std::vector<madoka::UInt64> &ids) {
  std::printf(
      "---------+---------------+-----------------+---------+----------\n"
      "    width       file size      average time  accuracy  reduction\n"
      "                     [KB]              [ns]       [%%]        [%%]\n"
      "           sketch  filter      inc      get                     \n"
      "---------+-------+-------+--------+--------+---------+----------\n");

  madoka::Sketch sketch;
  madoka::UInt64 width = MIN_WIDTH;
  for (int i = 0; width <= MAX_WIDTH; ++i) {
    std::vector<double> times;
    for (madoka::UInt64 trial_id = 0; trial_id < NUM_TRIALS; ++trial_id) {
      sketch.create(width, THRESHOLD, SKETCH_PATH, FILE_FLAGS);
      madoka::Timer timer;
      for (std::size_t j = 0; j < ids.size(); ++j) {
        sketch.inc(keys[ids[j]].c_str(), keys[ids[j]].length());
      }
      times.push_back(timer.elapsed());
    }
    std::sort(times.begin(), times.end());
    const double average_inc_time =
        1000000000.0 * times[times.size() / 2] / ids.size();

    madoka::Sketch filter;
    filter.shrink(sketch, 0, 1, threshold_filter);

    madoka::UInt64 num_matches = 0;
    madoka::UInt64 num_corrects = 0;

    times.clear();
    for (madoka::UInt64 trial_id = 0; trial_id < NUM_TRIALS; ++trial_id) {
      madoka::Timer timer;
      for (std::size_t j = 0; j < ids.size(); ++j) {
        filter.get(keys[ids[j]].c_str(), keys[ids[j]].length());
      }
      times.push_back(timer.elapsed());
    }
    std::sort(times.begin(), times.end());
    const double average_get_time =
        1000000000.0 * times[times.size() / 2] / ids.size();

    for (std::size_t j = 0; j < keys.size(); ++j) {
      const bool estimated_greater_than =
          (filter.get(keys[j].c_str(), keys[j].length()) != 0);
      if (estimated_greater_than) {
        ++num_matches;
      }

      const bool correct_greater_than = (freqs[j] >= THRESHOLD);
      if (estimated_greater_than == correct_greater_than) {
        ++num_corrects;
      }
    }

    std::printf("%9lu %7lu %7lu %8.2lf %8.2lf %9.2lf %10.2lf\n",
                static_cast<unsigned long>(width),
                static_cast<unsigned long>(sketch.file_size() >> 10),
                static_cast<unsigned long>(filter.file_size() >> 10),
                average_inc_time, average_get_time,
                100.0 * num_corrects / keys.size(),
                100.0 * num_matches / keys.size());

    width = static_cast<madoka::UInt64>(
        MIN_WIDTH * std::pow(WIDTH_FACTOR, i + 1));
  }

  printf(
      "---------+-------+-------+--------+--------+---------+----------\n");
}

void benchmark_counter(const std::vector<std::string> &keys,
                       const std::vector<madoka::UInt64> &freqs,
                       const std::vector<madoka::UInt64> &ids) {
  std::printf(
      "---------+-------+--------+---------------+---------------\n"
      "    width    size     time   average error   average error\n"
      "                       inc     set     inc     set     inc\n"
      "             [KB]     [ns]         [1/key]        [1/freq]\n"
      "---------+-------+--------+-------+-------+-------+-------\n");

  madoka::Sketch sketch;
  madoka::UInt64 width = MIN_WIDTH;
  for (int i = 0; width <= MAX_WIDTH; ++i) {
    for (madoka::UInt64 trial_id = 0; trial_id < NUM_TRIALS; ++trial_id) {
      sketch.create(width, 0, SKETCH_PATH, FILE_FLAGS);
      for (std::size_t j = 0; j < keys.size(); ++j) {
        sketch.set(keys[j].c_str(), keys[j].length(), freqs[j]);
      }
    }

    madoka::UInt64 total_set_error = 0;
    for (std::size_t j = 0; j < keys.size(); ++j) {
      const madoka::UInt64 estimated_freq =
          sketch.get(keys[j].c_str(), keys[j].length());
      total_set_error += std::llabs(freqs[j] - estimated_freq);
    }

    std::vector<double> times;
    for (madoka::UInt64 trial_id = 0; trial_id < NUM_TRIALS; ++trial_id) {
      sketch.create(width, 0, SKETCH_PATH, FILE_FLAGS);
      madoka::Timer timer;
      for (std::size_t j = 0; j < ids.size(); ++j) {
        sketch.inc(keys[ids[j]].c_str(), keys[ids[j]].length());
      }
      times.push_back(timer.elapsed());
    }
    std::sort(times.begin(), times.end());
    const double average_inc_time =
        1000000000.0 * times[times.size() / 2] / ids.size();

    madoka::UInt64 total_inc_error = 0;
    for (std::size_t j = 0; j < keys.size(); ++j) {
      const madoka::UInt64 estimated_freq =
          sketch.get(keys[j].c_str(), keys[j].length());
      total_inc_error += std::llabs(freqs[j] - estimated_freq);
    }

    std::printf("%9lu %7lu %8.2lf %7.3lf %7.3lf %7.3lf %7.3lf\n",
                static_cast<unsigned long>(width),
                static_cast<unsigned long>(sketch.file_size() >> 10),
                average_inc_time,
                1.0 * total_set_error / keys.size(),
                1.0 * total_inc_error / keys.size(),
                1.0 * total_set_error / ids.size(),
                1.0 * total_inc_error / ids.size());

    width = static_cast<madoka::UInt64>(
        MIN_WIDTH * std::pow(WIDTH_FACTOR, i + 1));
  }

  std::printf(
      "---------+-------+--------+-------+-------+-------+-------\n");
}

void benchmark() {
  std::vector<std::string> keys;
  std::vector<madoka::UInt64> freqs;
  std::vector<madoka::UInt64> ids;
  if (KEYSET_PATHS.empty()) {
    gen_keys(&keys, &freqs, &ids);
  } else {
    load_keys(&keys, &freqs, &ids);
  }

  MADOKA_THROW_IF(keys.size() == 0);
  MADOKA_THROW_IF(ids.size() == 0);

  if (MIN_WIDTH == 0) {
    if (THRESHOLD == 0) {
      MIN_WIDTH = std::max(keys.size() >> 3, std::size_t(1));
    } else {
      MIN_WIDTH = std::max(keys.size() / THRESHOLD, madoka::UInt64(1));
    }
  }

  if (MAX_WIDTH == 0) {
    MAX_WIDTH = MIN_WIDTH << 4;
  } else if (MAX_WIDTH < MIN_WIDTH) {
    MAX_WIDTH = MIN_WIDTH;
  }

  std::cerr << "LENGTH: " << MIN_LENGTH << " - " << MAX_LENGTH << std::endl;
  std::cerr << "NUM_KEYS: " << keys.size() << std::endl;
  std::cerr << "TOTAL_FREQ: " << ids.size()
            << " (x " << FREQ_FACTOR << ")" << std::endl;
  std::cerr << "WIDTH: " << MIN_WIDTH << " - " << MAX_WIDTH
            << " (x " << WIDTH_FACTOR << ")" << std::endl;

  if (THRESHOLD != 0) {
    std::cerr << "THRESHOLD: " << THRESHOLD << std::endl;
    benchmark_filter(keys, freqs, ids);
  } else {
    benchmark_counter(keys, freqs, ids);
  }

  if (SKETCH_PATH != NULL) {
    MADOKA_THROW_IF(std::remove(SKETCH_PATH) == -1);
  }
}

}  // namespace

int main(int argc, char *argv[]) try {
  parse_args(argc, argv);
  benchmark();
  return 0;
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
} catch (const std::exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
