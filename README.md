## Introduction

Madoka is an implementation of the Count-Min sketch, a data structure for summarizing data streams. Madoka uses a conservative update mechanism to improve the accurary of sketches. In addition, Madoka uses an authorized update mechanism and an approximate counting algorithm for ultimate sketching. For more information, see the following web sites.

* Count-Min sketch
  * https://en.wikipedia.org/wiki/Count-Min_sketch
  * https://sites.google.com/site/countminsketch/
* Approximate counting algorithm
  * https://en.wikipedia.org/wiki/Approximate_counting_algorithm

Madoka was developed as a part of the groonga project (http://groonga.org/), special thanks to Brazil Inc.

## Documentation

* English
  * https://s-yata.github.com/madoka/
* Japanese
  * https://s-yata.github.com/madoka/index.ja.html

## Example

This is an example of using Madoka through its C++ interface. This example creates a sketch with default parameters and then increments values associated with "Madoka" and "Homura". The example will print "Madoka: 1", "Homura: 2" and "Mami: 0".

```cpp
#include <iostream>
#include <madoka.h>

int main() {
  madoka::Sketch sketch;
  sketch.create();

  sketch.inc("Madoka", 6);
  sketch.inc("Homura", 6);
  sketch.inc("Homura", 6);

  std::cout << "Madoka: " << sketch.get("Madoka", 6) << std::endl;
  std::cout << "Homura: " << sketch.get("Homura", 6) << std::endl;
  std::cout << "Mami: " << sketch.get("Mami", 4) << std::endl;

  return 0;
}
```

## License

Madoka is licensed under the Simplified BSD License.

Copyright (c) 2012, Susumu Yata
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
