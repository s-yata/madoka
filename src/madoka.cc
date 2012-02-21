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
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <madoka.h>

#include "../config.h"

namespace {

enum Mode {
  MODE_CREATE,
  MODE_GET,
  MODE_SET,
  MODE_INC,
  MODE_ADD,
  MODE_LIST
};

Mode MODE = MODE_LIST;

const char *SKETCH_PATH = NULL;

madoka::UInt64 WIDTH = 0;
madoka::UInt64 MAX_VALUE = 0;
madoka::UInt64 SEED = 0;

bool TRUNCATE_FLAG = false;
bool PRELOAD_FLAG = false;

madoka::UInt64 to_uint64(const char *arg, madoka::UInt64 min_value = 0,
                         madoka::UInt64 max_value = 0xFFFFFFFFFFFFFFFFULL) {
  char *value_end;
  const long long value = std::strtoll(arg, &value_end, 10);
  MADOKA_THROW_IF(*value_end != '\0');
  MADOKA_THROW_IF(value < 0);
  MADOKA_THROW_IF(static_cast<madoka::UInt64>(value) < min_value);
  MADOKA_THROW_IF(static_cast<madoka::UInt64>(value) > max_value);
  return static_cast<madoka::UInt64>(value);
}

int mode_create_main(int, char *[]) {
  madoka::Sketch sketch;
  sketch.create(WIDTH, MAX_VALUE, SKETCH_PATH,
                TRUNCATE_FLAG ? madoka::FILE_TRUNCATE : 0, SEED);
  return 0;
}

void mode_get_sub(const madoka::Sketch &sketch, std::istream *stream) {
  std::string key;
  while (std::getline(*stream, key)) {
    const madoka::UInt64 value = sketch.get(key.c_str(), key.length());
    std::cout << key << '\t' << value << '\n';
  }
}

int mode_get_main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.open(SKETCH_PATH, madoka::FILE_READONLY |
              (PRELOAD_FLAG ? madoka::FILE_PRELOAD : 0));
  if (::optind == argc) {
    mode_get_sub(sketch, &std::cin);
  }
  for (int i = ::optind; i < argc; ++i) {
    std::ifstream file(argv[i], std::ios::binary);
    MADOKA_THROW_IF(!file);
    mode_get_sub(sketch, &file);
  }
  return 0;
}

void mode_set_sub(std::istream *stream, madoka::Sketch *sketch) {
  std::string key;
  while (std::getline(*stream, key)) {
    const std::string::size_type delim_pos = key.find_last_of('\t');
    MADOKA_THROW_IF(delim_pos == std::string::npos);
    const madoka::UInt64 value = to_uint64(key.c_str() + delim_pos + 1);
    sketch->set(key.c_str(), delim_pos, value);
  }
}

int mode_set_main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.open(SKETCH_PATH, PRELOAD_FLAG ? madoka::FILE_PRELOAD : 0);
  if (::optind == argc) {
    mode_set_sub(&std::cin, &sketch);
  }
  for (int i = ::optind; i < argc; ++i) {
    std::ifstream file(argv[i], std::ios::binary);
    MADOKA_THROW_IF(!file);
    mode_set_sub(&file, &sketch);
  }
  return 0;
}

void mode_inc_sub(std::istream *stream, madoka::Sketch *sketch) {
  std::string key;
  while (std::getline(*stream, key)) {
    sketch->inc(key.c_str(), key.length());
  }
}

int mode_inc_main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.open(SKETCH_PATH, PRELOAD_FLAG ? madoka::FILE_PRELOAD : 0);
  if (::optind == argc) {
    mode_inc_sub(&std::cin, &sketch);
  }
  for (int i = ::optind; i < argc; ++i) {
    std::ifstream file(argv[i], std::ios::binary);
    MADOKA_THROW_IF(!file);
    mode_inc_sub(&file, &sketch);
  }
  return 0;
}

void mode_add_sub(std::istream *stream, madoka::Sketch *sketch) {
  std::string key;
  while (std::getline(*stream, key)) {
    const std::string::size_type delim_pos = key.find_last_of('\t');
    MADOKA_THROW_IF(delim_pos == std::string::npos);
    const madoka::UInt64 value = to_uint64(key.c_str() + delim_pos + 1);
    sketch->add(key.c_str(), delim_pos, value);
  }
}

int mode_add_main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.open(SKETCH_PATH, PRELOAD_FLAG ? madoka::FILE_PRELOAD : 0);
  if (::optind == argc) {
    mode_add_sub(&std::cin, &sketch);
  }
  for (int i = ::optind; i < argc; ++i) {
    std::ifstream file(argv[i], std::ios::binary);
    MADOKA_THROW_IF(!file);
    mode_add_sub(&file, &sketch);
  }
  return 0;
}

int mode_list_main(int, char *[]) {
  madoka::Sketch sketch;
  sketch.open(SKETCH_PATH, madoka::FILE_READONLY);
  std::cout << "Path: " << SKETCH_PATH << std::endl;
  std::cout << "Width: " << sketch.width() << " "
            << ((sketch.width_mask() != 0) ? "(FAST)" : "(SLOW)") << std::endl;
  std::cout << "Depth: " << sketch.depth() << std::endl;
  std::cout << "MaxValue: " << sketch.max_value() << std::endl;
  std::cout << "Seed: " << sketch.seed() << std::endl;
  std::cout << "FileSize: " << sketch.file_size() << std::endl;
  std::cout << "Mode: "
            << ((sketch.mode() == madoka::SKETCH_EXACT_MODE) ?
                "EXACT_MODE" : "APPROX_MODE") << std::endl;
  return 0;
}

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
            << "  -c, --create   create a new sketch\n"
            << "    -w, --width=[N]      "
            << "specify the width of the new sketch\n"
            << "    -m, --max-value=[N]  "
            << "specify the maximum value of the new sketch\n"
            << "    -S, --seed=[N]       "
            << "specify the seed of the new sketch\n"
            << "    -t, --truncate       "
            << "force creation when the sketch already exists\n"
            << "  -g, --get      print given keys with their values\n"
            << "  -s, --set      set given key-value pairs\n"
            << "  -i, --inc      increment values of given keys\n"
            << "  -a, --add      add given values to given keys\n"
            << "    -p, --preload        "
            << "preload the whole sketch\n"
            << "  -l, --list     list information of a sketch\n"
            << "  -v, --version  print the version\n"
            << "  -h, --help     print this message\n"
            << "\n"
            << "Report bugs to <" << PACKAGE_BUGREPORT << ">"
            << std::endl;
}

}  // namespace

int main(int argc, char *argv[]) try {
  const struct option long_options[] = {
    { "create", 0, NULL, 'c' },
      { "width", 1, NULL, 'w' },
      { "max-value", 1, NULL, 'm' },
      { "seed", 1, NULL, 'S' },
      { "truncate", 1, NULL, 't' },
    { "get", 0, NULL, 'g' },
    { "set", 0, NULL, 's' },
    { "inc", 0, NULL, 'i' },
    { "add", 0, NULL, 'a' },
      { "preload", 0, NULL, 'p' },
    { "list", 0, NULL, 'l' },
    { "version", 0, NULL, 'v' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, 0 }
  };

  int option_label;
  while ((option_label = ::getopt_long(argc, argv, "cw:m:S:tgsiaplvh",
                                       long_options, NULL)) != -1) {
    switch (option_label) {
      case 'c': {
        MODE = MODE_CREATE;
        break;
      }
      case 'w': {
        WIDTH = to_uint64(::optarg, 0, madoka::SKETCH_MAX_WIDTH);
        break;
      }
      case 'm': {
        MAX_VALUE = to_uint64(::optarg, 0, madoka::SKETCH_MAX_MAX_VALUE);
        break;
      }
      case 'S': {
        SEED = to_uint64(::optarg);
        break;
      }
      case 't': {
        TRUNCATE_FLAG = true;
        break;
      }
      case 'g': {
        MODE = MODE_GET;
        break;
      }
      case 's': {
        MODE = MODE_SET;
        break;
      }
      case 'i': {
        MODE = MODE_INC;
        break;
      }
      case 'a': {
        MODE = MODE_ADD;
        break;
      }
      case 'p': {
        PRELOAD_FLAG = true;
        break;
      }
      case 'l': {
        MODE = MODE_LIST;
        break;
      }
      case 'v': {
        print_version();
        return 0;
      }
      case 'h': {
        print_help(argv[0]);
        return 0;
      }
      default: {
        return 1;
      }
    }
  }

  if (::optind == argc) {
    print_help(argv[0]);
    return 0;
  }

  SKETCH_PATH = argv[::optind++];

  if (::isatty(STDIN_FILENO) == 0) {
    std::ios::sync_with_stdio(false);
  }

  switch (MODE) {
    case MODE_CREATE: {
      return mode_create_main(argc, argv);
    }
    case MODE_GET: {
      return mode_get_main(argc, argv);
    }
    case MODE_SET: {
      return mode_set_main(argc, argv);
    }
    case MODE_INC: {
      return mode_inc_main(argc, argv);
    }
    case MODE_ADD: {
      return mode_add_main(argc, argv);
    }
    case MODE_LIST: {
      return mode_list_main(argc, argv);
    }
    default: {
      MADOKA_THROW("invalid mode");
    }
  }
} catch (const madoka::Exception &ex) {
  std::cerr << "error: " << ex.what() << std::endl;
  return 1;
}
