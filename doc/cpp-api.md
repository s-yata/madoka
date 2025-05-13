---
layout: default
title: "C++ API Documentation"
lang: en
---

# C++ API Documentation

## Draw a sketch

```cpp
#include <iostream>
#include <string>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.create();

  std::string key;
  while (std::getline(std::cin, key)) {
    sketch.inc(key.c_str(), key.length());
  }
  sketch.save(argv[1]);
  return 0;
}
```

```
$ g++ draw-a-sketch.cc -lmadoka
$ ./a.out SKETCH < KEYSET
```

Let's try to draw a sketch. This example reads a keyset from standard input (<var>std::cin</var>) and draws a sketch (<code>madoka::Sketch</code>). Then, this example saves the sketch to a file specified by the 1st command line argument (<var>argv[1]</var>). The following are the points of this example.

* <kbd>madoka.h</kbd>
  * <code>#include <madoka.h></code> is needed to use Madoka. Data types, constants and classes are defined in <kbd>madoka.h</kbd>.
* <code>madoka::Sketch</code>
  * <code>madoka::Sketch</code> represents a sketch and provides functions for sketching.
* <code>madoka::Sketch::create()</code>
  * <code>create()</code> is a function to make a sketch. You must not start sketching before making a sketch.
* <code>madoka::Sketch::inc()</code>
  * <code>inc()</code> is a function to increment a value associated with a key. The 1st argument specifies the starting address and the 2nd argument specifies the length in bytes of the key. Yeah, this is sketching.
  * Note that <code>inc()</code> does nothing if the current value is saturated.
* <code>madoka::Sketch::save()</code>
  * <code>save()</code> is a function to save a sketch. The 1st argument specifies the path of the file.

Note that an option <kbd>-lmadoka</kbd> is needed to build this example. If you have installed Madoka, <kbd>pkg-config madoka --libs</kbd> is available to get the required options.

## Look at a sketch

```cpp
#include <iostream>
#include <string>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.load(argv[1]);
  // sketch.open(argv[1]);

  std::string key;
  while (std::getline(std::cin, key)) {
    std::cout << key << ": "
              << sketch.get(key.c_str(), key.length()) << std::endl;
  }
  return 0;
}
```

```
$ g++ look-at-a-sketch.cc -lmadoka
$ ./a.out SKETCH < KEYSET
```

Next, let's try to look at a sketch. This example loads a sketch from a file specified by the 1st command line argument (<var>argv[1]</var>). Then, this example looks up keys read from standard input (<var>std::cin</var>). The following are the points of this example.

* <code>madoka::Sketch::load()</code>
  * <code>load()</code> is a function to load a sketch from a file. The 1st argument specifies the path of the file.
  * Note that <code>open()</code> is also available to access a sketch file when you don't want to load the whole file. <code>open()</code> uses memory mapped I/O instead of reading the whole sketch into memory.
* <code>madoka::Sketch::get()</code>
  * <code>get()</code> is a function to get the value associated with a key. The 1st argument specifies the starting address and the 2nd argument specifies the length in bytes of the key.

h3. Use other brushes

```cpp
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.create();

  sketch.set("QB", 2, 10);
  std::cout << "QB: "
            << sketch.add("QB", 2, 5) << std::endl;
  sketch.set("QB", 2, 7);
  std::cout << "QB: "
            << sketch.get("QB", 2) << std::endl;
  return 0;
}
```

```
$ g++ use-other-brushes.cc -lmadoka
$ ./a.out
QB: 15
QB: 15
```

<code>madoka::Sketch</code> provides other drawing functions named <code>set()</code> and <code>add()</code>. This example shows how these functions work.

* <code>madoka::Sketch::set()</code>
  * <code>set()</code> is a function to update a value associated with a key. The 1st argument specifies the starting address and the 2nd argument specifies the length in bytes of the key. The 3rd argument specifies the value.
  * Note that <code>set()</code> does nothing when the specified value is not greater than the current associated value.
  * Also note that the new value is saturated when the specified value is greater than the upper limit.
* <code>madoka::Sketch::add()</code>
  * <code>add()</code> is a function to perform an addition. The 1st argument specifies the starting address and the 2nd argument specifies the length in bytes of the key. The 3rd argument specifies the value to be added. The return value of <code>add()</code> is the result of the addition.
  * Note that the result is saturated when the sum is greater than the upper limit.

In this example, the 1st <code>set()</code> changes the value associated with <var>"QB"</var> from <var>0</var> to <var>10</var>. Then, <code>add()</code> adds <var>5</var> to that value (from <var>10</var> to <var>15</var>). After that, the 2nd <code>set()</code> does nothing because the specified value (<var>7</var>) is less than the current value (<var>15</var>).

## Customize a sketch

```cpp
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  const madoka::UInt64 MY_WIDTH = 1ULL << 24;
  const madoka::UInt64 MY_MAX_VALUE = 10;

  madoka::Sketch sketch;
  sketch.create(MY_WIDTH, MY_MAX_VALUE);

  std::cout << "width: " << sketch.width() << std::endl;
  std::cout << "max_value: " << sketch.max_value() << std::endl;
  std::cout << "size: " << sketch.file_size() << std::endl;
  return 0;
}
```

```
$ g++ customize-a-sketch.cc -lmadoka
$ ./a.out
width: 16777216
max_value: 15
size: 25165904
```

Let's customize a sketch for your application. A [Count-Min sketch](https://sites.google.com/site/countminsketch/) is a probabilistic data structure and the accuracy depends on its parameters, <var>width</var> and <var>depth</var>, and the target data stream.

Basically, accurate sketching requires a large <var>width</var> and a longer stream requires a larger <var>width</var>, but a larger <var>width</var> requires a larger memory space and increases cache misses. The other parameter, <var>depth</var>, also has an effect on accuracy but Madoka uses a fixed <var>depth</var>, actually <var>madoka::SKETCH_DEPTH</var> (<var>3</var>), based on benchmarks. Instead, Madoka has another parameter, <var>max_value</var>, that specifies the upper limit of the values. By using a small <var>max_value</var>, you can save memory.

To customize a sketch, specify <var>width</var> and <var>max_value</var> when creating a sketch. See the following for more details.

* <var>width</var>
  * <var>0</var> is replaced with the default value <var>madoka::SKETCH_DEFAULT_WIDTH</var> (<var>2<sup>20</sup></var>).
  * <var>width</var> must not be greater than <var>madoka::SKETCH_MAX_WIDTH</var> (<var>2<sup>42</sup></var>).
  * Note that you can quickly draw and look at a sketch if the <var>width</var> of the sketch is a power of 2.
* <var>max_value</var>
  * <var>0</var> is replaced with the default value <var>madoka::SKETCH_DEFAULT_MAX_VALUE</var> (<var>2<sup>45</sup> - 1</var>).
  * <var>max_value</var> must not be greater than <var>madoka::SKETCH_MAX_MAX_VALUE</var> (<var>2<sup>45</sup> - 1</var>).
  * <code>create()</code> rounds out a given <var>max_value</var> to <var>1</var>, <var>3</var>, <var>15</var>, <var>255</var>, <var>65535</var> or <var>2<sup>45</sup> - 1</var>.

This example creates a customized sketch and prints the size. Note that the size in bytes of a sketch is approximately <var>width x 8</var> if <var>max_value == 2<sup>45</sup> - 1</var>, or otherwise <var>width x depth x log<sub>2</sub>(max_value + 1) / 8</var>.

## Clear a sketch

```cpp
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.create();

  sketch.set("Sayaka", 6, 100);
  sketch.clear();
  std::cout << "Sayaka: "
            << sketch.get("Sayaka", 6) << std::endl;
  return 0;
}
```

```
$ g++ clear-a-sketch.cc -lmadoka
$ ./a.out
Sayaka: 0
```

<code>madoka::Sketch</code> provides an interface to clear a sketch. This example creates a sketch and updates the value associated with <var>"Sayaka"</var> from <var>0</var> to <var>100</var>, but <code>clear()</code> fills the sketch with <var>0</var>s. As a result, <code>get()</code> returns <var>0</var> for <var>"Sayaka"</var>.

* <code>madoka::Sketch::clear()</code>
  * <code>clear()</code> is a function to clear a sketch, or more precisely, <code>clear()</code> fills a sketch with <var>0</var>s.
  * Note that <var>width</var> and <var>max_value</var> of the sketch remain unchanged.

## Copy a sketch

```cpp
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.create();

  sketch.set("Kyoko", 5, 100);

  madoka::Sketch snapshot;
  snapshot.copy(sketch);

  sketch.add("Kyoko", 5, 50);

  std::cout << "Kyoko (original): "
            << sketch.get("Kyoko", 5) << std::endl;
  std::cout << "Kyoko (snapshot): "
            << snapshot.get("Kyoko", 5) << std::endl;
  return 0;
}
```

```
$ g++ copy-a-sketch.cc -lmadoka
$ ./a.out
Kyoko (original): 150
Kyoko (snapshot): 100
```

<code>madoka::Sketch</code> provides an interface to copy a sketch. This example draws a sketch and creates its copy as a snapshot. Then, this example updates the original sketch.

* <code>madoka::Sketch::copy()</code>
  * <code>copy()</code> is a function to create a copy of a sketch. The 1st argument specifies the source sketch.

As shown in this example, <code>copy()</code> is useful to create a snapshot in memory. If you want to save a snapshot as a file, <code>save()</code> is an easier choice.

## Apply a filter

```cpp
#include <iostream>

#include <madoka.h>

madoka::UInt64 divide_by_2(madoka::UInt64 value) {
  return value / 2;
}

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.create();

  sketch.set("Kaname", 6, 8);
  sketch.set("Madoka", 6, 15);

  sketch.filter(divide_by_2);
  // sketch.filter([](madoka::UInt64 value) { return value / 2; });

  std::cout << "Kaname: "
            << sketch.get("Kaname", 6) << std::endl;
  std::cout << "Madoka: "
            << sketch.get("Madoka", 6) << std::endl;
  return 0;
}
```

```
$ g++ apply-a-filter.cc -lmadoka
$ ./a.out
Kaname: 4
Madoka: 7
```

<code>madoka::Sketch</code> provides a filter feature which can be used to reduce errors and to simulate decays. This example uses a filter for dividing all the values in a sketch by <var>2</var>.

* <code>madoka::Sketch::filter()</code>
  * <code>filter()</code> is a function to apply a filter to a sketch, or more precisely, <code>filter()</code> applies a filter to all the values in a sketch. The 1st argument specifies the filter.
  * <code>filter()</code> accepts a pointer to a function that takes the current value (<code>madoka::UInt64</code>) and returns the filtered value (<code>madoka::UInt64</code>).
  * A function object is also acceptable if the type of its argument and return value is <code>madoka::UInt64</code>.
  * Note that if the filter returns a value greater than <var>max_value</var>, that value is replaced with <var>max_value</var>.
  * <code>filter()</code> does nothing if the argument is <var>NULL</var>.

For example, by using this feature, you can implement a variety of [lossy conservative updates](https://www.aaai.org/ocs/index.php/AAAI/AAAI11/paper/view/3757). Also, you can divide all the values by <var>2</var> when one of the values reaches <var>max_value</var>. In addition, you can replace all the values with their binary logarithms for compression, etc.

## Shrink a sketch

```cpp
#include <iostream>

#include <madoka.h>

madoka::UInt64 logarithmize(madoka::UInt64 value) {
  return 63 - ::__builtin_clzll(value | 1);
}

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.create(100);

  sketch.set("Akemi", 5, 256);
  sketch.set("Homura", 6, 16777216);

  madoka::Sketch new_sketch;
  new_sketch.shrink(sketch, 10, 15, logarithmize);

  std::cout << "width: "
            << new_sketch.width() << std::endl;
  std::cout << "max_value: "
            << new_sketch.max_value() << std::endl;

  std::cout << "Akemi: "
            << new_sketch.get("Akemi", 5) << std::endl;
  std::cout << "Homura: "
            << new_sketch.get("Homura", 6) << std::endl;
  return 0;
}
```

```
$ g++ shrink-a-sketch.cc -lmadoka
$ ./a.out
width: 10
max_value: 15
Akemi: 8
Homura: 15
```

Let's shrink a sketch for saving memory. This is a reasonable answer to the question "How can I determine the best values for <var>width</var> and <var>max_value</var>?".

During or after sketching, you may find that <var>width</var> and <var>max_value</var> are too large. In such a case, you can shrink the sketch, or more precisely, you can create a smaller sketch and copies the contents of the source sketch to the smaller sketch.

* <code>madoka::Sketch::shrink()</code>
  * <code>shrink()</code> is a function to shrink a sketch. The 1st argument specifies the source sketch. The 2nd argument specifies <var>width</var> and the 3rd argument specifies <var>max_value</var> of the new sketch. The 4th argument specifies a filter to be applied in shrinking.
  * Note that the new <var>width</var> must be a factor of the source <var>width</var>. On the other hand, a <var>max_value</var> greater than the source <var>max_value</var> is acceptable.

This example creates a sketch and shrinks the sketch to a smaller sketch. The new <var>width</var> and the new <var>max_value</var> are <var>10</var> and <var>15</var> respectively. Note that the values are replaced with their logarithms by <code>logarimize()</code>. For example, <var>"Akemi: 256"</var> is replaced with <var>"Akemi: 8"</var>. Also note that <var>"Homura: 16777216"</var> is replaced with <var>"Homura: 24"</var> but the output is <var>"Homura: 15"</var> because of saturation.

Due to this feature, you can use large <var>width</var> and <var>max_value</var> during sketching, and after that, you can adjust <var>width</var> and <var>max_value</var> to your application. Also, you can compress a sketch as shown in this example, binarize a sketch based on a threshold, etc.

## Merge sketches

```cpp
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch_1, sketch_2;
  sketch_1.create();
  sketch_2.create();

  sketch_1.inc("Tomoe", 5);
  sketch_1.inc("Mami", 4);
  sketch_2.inc("Mami", 4);

  madoka::Sketch sketch;
  sketch.copy(sketch_1);
  sketch.merge(sketch_2);

  std::cout << "Tomoe: "
            << sketch.get("Tomoe", 5) << std::endl;
  std::cout << "Mami: "
            << sketch.get("Mami", 4) << std::endl;
  return 0;
}
```

```
$ g++ merge-sketches.cc -lmadoka
$ ./a.out
Tomoe: 1
Mami: 2
```

<code>madoka::Sketch</code> provides an interface to merge sketches. A sketch merging works like vector addition. It simply adds the values of the right-hand side sketch (rhs-sketch) to the values of the left-hand side sketch (lhs-sketch).

* <code>madoka::Sketch::merge()</code>
  * <code>merge()</code> is a function to merge sketches. The 1st argument specifies the rhs-sketch. The 2nd argument specifies a filter that is applied to the lhs-sketch values. The 3rd argument specifies a filter that is applied to the rhs-sketch values.
  * Note that the sketches must have the same <var>width</var>.

This example creates two sketches with <var>"Mami: 1"</var> and merges the sketches. So, the resultant sketch has <var>"Mami: 2"</var>. Note that this example uses <code>copy()</code> to keep the lhs-sketch. If you don't mind overwriting the lhs-sketch, you can call <code>merge()</code> directly.

The merging feature allows you to draw a sketch in a distributed manner. For example, you can divide an input data stream into several streams and draw sketches for each stream in parallel. Then, you can merge the temporary sketches to get the final sketch.

## Estimate the inner product

```cpp
#include <cmath>
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch_1, sketch_2;
  sketch_1.create();
  sketch_2.create();

  sketch_1.add("Charlotte", 9, 3);
  sketch_1.add("Oktavia", 7, 2);
  sketch_2.add("Gretchen", 8, 5);
  sketch_2.add("Charlotte", 9, 4);

  double length_1, length_2;
  double inner_product =
      sketch_1.inner_product(sketch_2, &length_1, &length_2);
  length_1 = std::sqrt(length_1);
  length_2 = std::sqrt(length_2);

  std::cout << "inner_product: " << inner_product << std::endl;
  std::cout << "length_1: " << length_1 << std::endl;
  std::cout << "length_2: " << length_2 << std::endl;
  std::cout << "cosine: "
            << (inner_product / length_1 / length_2) << std::endl;
  return 0;
}
```

```
$ g++ estimate-the-inner-product.cc -lmadoka
$ ./a.out
inner_product: 12
length_1: 3.60555
length_2: 6.40312
cosine: 0.519778
```

A Count-Min sketch supports inner product estimation. <code>madoka::Sketch</code> provides an interface to estimate the inner product with the length of sketches, actually length is not defined for sketches. This interface is useful to estimate the cosine similarity.

* <code>madoka::Sketch::inner_product()</code>
  * <code>inner_product()</code> is a function to estimate inner product. The 1st argument specifies the rhs-sketch. The 2nd argument can be used to get the estimated squared length of the lhs-sketch. The 3rd argument can be used to get the estimated squared length of the rhs-sketch. The return value of <code>inner_product()</code> is the estimated inner product.
  * Note that the sketches must have the same <var>width</var>.

This example creates two sketches and estimates the inner product between the sketches. This example also computes the cosine similarity from the obtained values.

## Configure memory mapping

```cpp
namespace madoka {

enum FileFlag {
  FILE_CREATE    = 1 << 0,
  FILE_TRUNCATE  = 1 << 1,
  FILE_READONLY  = 1 << 2,
  FILE_WRITABLE  = 1 << 3,
  FILE_SHARED    = 1 << 4,
  FILE_PRIVATE   = 1 << 5,
  FILE_ANONYMOUS = 1 << 6,
  FILE_HUGETLB   = 1 << 7,
  FILE_PRELOAD   = 1 << 8
};

}  // namespace madoka
```

<code>create()</code>, <code>open()</code>, <code>load()</code>, <code>save()</code>, <code>copy()</code> and <code>shrink()</code> have arguments named <var>path</var> and <var>flags</var>. The <var>path</var> argument specifies the target file. Note that <var>NULL</var> specifies to create an anonymous memory mapping. The <var>flags</var> argument specifies the behavior as follows.

* <var>madoka::FILE_CREATE</var>
  * <var>FILE_CREATE</var> is used internally to create a file.
* <var>madoka::FILE_TRUNCATE</var>
  * <code>create()</code>, <code>copy()</code> and <code>shrink()</code> accept <var>FILE_TRUNCATE</var> unless <var>path == NULL</var>.
  * <var>FILE_TRUNCATE</var> is used to overwrite an existing file. Note that a creation without <var>FILE_TRUNCATE</var> to an existing file throws an exception.
* <var>madoka::FILE_READONLY</var>
  * <code>open()</code> accepts <var>FILE_READONLY</var>.
  * <var>FILE_READONLY</var> is used to disable modifications. Note that a modification to a read-only sketch will cause a segmentation fault.
* <var>madoka::FILE_WRITABLE</var>
  * <var>FILE_WRITABLE</var> is used internally to enable modifications.
* <var>madoka::FILE_SHARED</var>
  * <var>FILE_SHARED</var> is used internally to make modifications visible to other processes.
* <var>madoka::FILE_PRIVATE</var>
  * <code>open()</code> accepts <var>FILE_PRIVATE</var>.
  * <var>FILE_PRIVATE</var> is used to make modifications invisible to other processes.
* <var>madoka::FILE_ANONYMOUS</var>
  * <var>FILE_ANONYMOUS</var> is used internally to map memory not backed by a file.
* <var>madoka::FILE_HUGETLB</var>
  * <code>create()</code>, <code>open()</code>, <code>load()</code>, <code>save()</code>, <code>copy()</code> and <code>shrink()</code> accept <var>FILE_HUGETLB</var>.
  * <var>FILE_HUGETLB</var> is used to enable huge pages. See the next section for details.
* <var>madoka::FILE_PRELOAD</var>
  * <code>open()</code> accepts <var>FILE_PRELOAD</var>.
  * <var>FILE_PRELOAD</var> is used to preload the entire file after mapping a file. Preloading is useful to avoid random disk access.

## Use huge pages

```cpp
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  sketch.create(0, 0, NULL, madoka::FILE_HUGETLB);

  if (sketch.flags() & madoka::FILE_HUGETLB) {
    std::cout << "HugeTLB: on" << std::endl;
  } else {
    std::cout << "HugeTLB: off" << std::endl;
  }
  return 0;
}
```

```
$ g++ use-huge-pages.cc -lmadoka
$ grep HugePages_Free /proc/meminfo
HugePages_Free:        0
$ ./a.out
HugeTLB: off
$ sudo sysctl -w vm.nr_hugepages=512
vm.nr_hugepages = 512
$ grep HugePages_Free /proc/meminfo
HugePages_Free:      512
$ ./a.out
HugeTLB: on
```

Roughly speaking, a Count-Min sketch is composed of hash tables and the number of hash tables is equal to its <var>depth</var>. The <var>depth</var> of <code>madoka::Sketch</code> is fixed to <var>3</var> and thus basic operations, <code>set()</code>, <code>inc()</code> and <code>add()</code>, except <code>get()</code>, perform at least <var>3</var> random accesses.

Because of the random access nature, if a sketch is larger than the [CPU cache](https://en.wikipedia.org/wiki/CPU_cache), cache misses occur in sketching and the memory access latency becomes the bottleneck. In addition, if a sketch is much, much, much larger than the CPU cache, [TLB](https://en.wikipedia.org/wiki/Translation_lookaside_buffer) misses reduce the throughput of sketching.

<var>madoka::FILE_HUGETLB</var> is an optional flag to enable the use of [huge pages](https://en.wikipedia.org/wiki/Page_%28computer_memory%29#Huge_pages). The use of huge pages reduces TLB misses in sketching. If a sketch is created with <var>madoka::FILE_HUGETLB</var>, the sketch tries to use huge pages. If huge pages are not available, the sketch uses regular pages.

This example shows how to use huge pages. You can check whether huge pages are available or not by reading <kbd>/proc/meminfo</kbd>. If disabled, you can enable huge pages by editing <kbd>/proc/sys/vm/nr_hugepages</kbd>. Note that only a user with root authority can enable huge pages. After that, <code>madoka::Sketch</code> can use huge pages. Remember that a sketch backed by a file does not support huge pages unless the file system supports huge pages. For more details, see [information about huge page support](https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt).

## Specify a seed

<code>madoka::Sketch</code> provides an interface to specify a seed, which is used to calculate hash values and to initialize a random number generator. The 5th argument of <code>create()</code> specifies the seed. However, basically there is no need to specify a user-defined seed. Note that you cannot merge sketches having different seeds.

## Get information of a sketch

* <code>madoka::Sketch::width()</code>
  * <code>width()</code> returns <var>width</var> of the sketch.
* <code>madoka::Sketch::width_mask()</code>
  * <code>width_mask()</code> returns <var>width - 1</var> if <var>width</var> is a power of 2, otherwise returns <var>0</var>.
* <code>madoka::Sketch::depth()</code>
  * <code>depth()</code> returns <var>madoka::SKETCH_DEPTH</var>.
* <code>madoka::Sketch::max_value()</code>
  * <code>max_value()</code> returns <var>max_value</var> of the sketch.
* <code>madoka::Sketch::value_mask()</code>
  * <code>value_mask()</code> returns <var>max_value</var>.
* <code>madoka::Sketch::value_size()</code>
  * <code>value_size()</code> returns <var>log<sub>2</sub>(max_value + 1)</var>.
* <code>madoka::Sketch::seed()</code>
  * <code>seed()</code> returns <var>seed</var> of the sketch.
* <code>madoka::Sketch::table_size()</code>
  * <code>table_size()</code> returns the size in bytes of the sketch.
* <code>madoka::Sketch::file_size()</code>
  * <code>file_size()</code> returns the file size in bytes of the sketch. The file size is equal to the sum of the header size and the sketch size.
* <code>madoka::Sketch::flags()</code>
  * <code>flags()</code> returns memory mapping related flags.
* <code>madoka::Sketch::mode()</code>
  * <code>mode()</code> returns <var>madoka::SKETCH_APPROX_MODE</var> if <var>value_size == madoka::SKETCH_APPROX_VALUE_SIZE</var>, otherwise returns <var>madoka::SKETCH_EXACT_MODE</var>.

## Catch an exception

```cpp
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Sketch sketch;
  try {
    sketch.create(madoka::SKETCH_MAX_WIDTH + 1);
  } catch (const madoka::Exception &ex) {
    std::cerr << "error: "
              << ex.what() << std::endl;
    return -1;
  }
  return 0;
}
```

```
$ g++ catch-and-exception.cc -lmadoka
$ ./a.out
error: madoka/sketch.cc:453: width > SKETCH_MAX_WIDTH
```

Madoka throws an exception when an error occurs. The exception class is <code>madoka::Exception</code>. This example shows how to catch an exception.

* <code>madoka::Exception::what()</code>
  * <code>what()</code> is a function to get an error message. The format is <code>__FILE__</code> << <var>":"</var> << <code>__LINE__</code> << <var>": "</var> << <var>the-reason-of-the-error</var>.
  * Note that exception specifiers in <kbd>madoka/sketch.h</kbd> tell you which function is possible to throw an exception.

In this example, <code>create()</code> fails to create a sketch because the specifed <var>width</var> is too large. Remember that <code>madoka::Sketch</code> is guaranteed to be unchanged when an error has occurred.

## Draw a croquis

```cpp
#include <iostream>

#include <madoka.h>

int main(int argc, char *argv[]) {
  madoka::Croquis<float> croquis;
  croquis.create();

  croquis.set("Madoka", 6, 1.25);
  croquis.set("Hiroshi", 7, 2.5);
  croquis.add("Madoka", 6, 0.5);

  std::cout << "Madoka: "
            << croquis.get("Madoka", 6) << std::endl;
  std::cout << "Hiroshi: "
            << croquis.get("Hiroshi", 7) << std::endl;
  return 0;
}
```

```
$ g++ draw-a-croquis.cc -lmadoka
$ ./a.out
Madoka: 1.75
Hiroshi: 2.5
```

<code>madoka::Croquis</code> is a simplified version of <code>madoka::Sketch</code>. It does not provide <code>inc()</code>, <code>copy()</code>, <code>filter()</code>, <code>shrink()</code>, <code>merge()</code> and <code>inner_product()</code>. Instead, <code>madoka::Croquis</code> has a template parameter that specifies the type of cells. 

This example uses [floating point numbers](https://en.wikipedia.org/wiki/Floating_point) with <code>madoka::Croquis&lt;float&gt;</code>. The usage is same as that of <code>madoka::Sketch</code>, but remember that a tiny value may be truncated in <code>add()</code> because of the rounding.
