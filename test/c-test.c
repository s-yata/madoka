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

#include <assert.h>
#include <stdio.h>

#include <madoka.h>

static madoka_uint64 divide_by_2(madoka_uint64 value) {
  return value / 2;
}

int main(void) {
  const char PATH_1[] = "c-test.temp.1";
  const char PATH_2[] = "c-test.temp.2";

  remove(PATH_1);

  madoka_sketch *sketch;

  const char *what = NULL;
  sketch = madoka_create(100, 3, PATH_1, 0, 0, &what);
  assert(sketch != NULL);
  assert(what == NULL);

  assert(madoka_create(100, 3, PATH_1, 0, 0, &what) == NULL);
  assert(what != NULL);
  printf("log: %s:%d: %s\n", __FILE__, __LINE__, what);

  assert(madoka_get_width(sketch) == 100);
  assert(madoka_get_depth(sketch) == 3);
  assert(madoka_get_max_value(sketch) == 3);

  madoka_set(sketch, "banana", 6, 2);
  assert(madoka_get(sketch, "banana", 6) == 2);

  assert(madoka_inc(sketch, "apple", 5) == 1);
  assert(madoka_inc(sketch, "apple", 5) == 2);
  assert(madoka_inc(sketch, "apple", 5) == 3);
  assert(madoka_inc(sketch, "apple", 5) == 3);

  assert(madoka_add(sketch, "orange", 6, 2) == 2);
  assert(madoka_add(sketch, "orange", 6, 100) == 3);

  madoka_close(sketch);

  assert(madoka_open(PATH_2, 0, &what) == NULL);
  printf("log: %s:%d: %s\n", __FILE__, __LINE__, what);

  sketch = madoka_open(PATH_1, 0, &what);
  assert(sketch != NULL);

  assert(madoka_get(sketch, "banana", 6) == 2);
  assert(madoka_get(sketch, "apple", 5) == 3);
  assert(madoka_get(sketch, "orange", 6) == 3);

  assert(madoka_save(sketch, PATH_2, 0, &what) == 0);
  madoka_close(sketch);

  sketch = madoka_load(PATH_2, 0, &what);
  assert(sketch != NULL);

  assert(madoka_get(sketch, "banana", 6) == 2);
  assert(madoka_get(sketch, "apple", 5) == 3);
  assert(madoka_get(sketch, "orange", 6) == 3);

  madoka_filter(sketch, divide_by_2);

  assert(madoka_get(sketch, "banana", 6) == 1);
  assert(madoka_get(sketch, "apple", 5) == 1);
  assert(madoka_get(sketch, "orange", 6) == 1);

  madoka_clear(sketch);

  assert(madoka_get(sketch, "banana", 6) == 0);
  assert(madoka_get(sketch, "apple", 5) == 0);
  assert(madoka_get(sketch, "orange", 6) == 0);

  madoka_close(sketch);

  sketch = madoka_load(PATH_2, 0, &what);
  assert(sketch != NULL);

  assert(madoka_get(sketch, "banana", 6) == 2);
  assert(madoka_get(sketch, "apple", 5) == 3);
  assert(madoka_get(sketch, "orange", 6) == 3);

  madoka_sketch *new_sketch = madoka_shrink(sketch, 50, 15, divide_by_2,
                                            PATH_2, MADOKA_FILE_TRUNCATE,
                                            &what);
  assert(new_sketch != NULL);

  madoka_swap(sketch, new_sketch);
  madoka_close(new_sketch);

  assert(madoka_get_width(sketch) == 50);
  assert(madoka_get_depth(sketch) == 3);
  assert(madoka_get_max_value(sketch) == 15);

  assert(madoka_get(sketch, "banana", 6) == 1);
  assert(madoka_get(sketch, "apple", 5) == 1);
  assert(madoka_get(sketch, "orange", 6) == 1);

  madoka_set(sketch, "banana", 6, 10);
  assert(madoka_get(sketch, "banana", 6) == 10);

  assert(madoka_inc(sketch, "apple", 5) == 2);
  assert(madoka_inc(sketch, "apple", 5) == 3);
  assert(madoka_inc(sketch, "apple", 5) == 4);

  assert(madoka_add(sketch, "orange", 6, 10) == 11);
  assert(madoka_add(sketch, "orange", 6, 100) == 15);

  assert(madoka_shrink(sketch, 17, 1, NULL, NULL, 0, &what) == NULL);
  printf("log: %s:%d: %s\n", __FILE__, __LINE__, what);

  madoka_close(sketch);

  assert(remove(PATH_1) == 0);
  assert(remove(PATH_2) == 0);
  return 0;
}
