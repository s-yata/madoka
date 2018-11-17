---
layout: default
title: "Count-Min Sketch Library"
lang: ja
---

# Count-Min Sketch Library

## Introduction

Madoka は Count-Min sketch というデータ構造のライブラリであり，大規模なデータに含まれるアイテムの頻度を求めたいときなどに有用です．Madoka は Conservative update という手法を使ってスケッチの精度を高めています．また，不要なカウントを抑える手法や近似的なカウンタなどの導入により，高い空間効率を実現しています．詳細については，以下のサイトを参照してください．

* Count-Min sketch と Conservative update について
  * [Count-Min sketch - Wikipedia, the free encyclopedia](http://en.wikipedia.org/wiki/Count-Min_sketch)
  * [Count-Min Sketch](https://sites.google.com/site/countminsketch/)
* 近似的なカウンタについて
  * [Approximate counting algorithm - Wikipedia, the free encyclopedia](http://en.wikipedia.org/wiki/Approximate_counting_algorithm)

Madoka は Groonga プロジェクト（ [Groonga - カラムストア機能付き全文検索エンジン](http://groonga.org/ja/) ）の一環として開発されました．

## Installation

```
$ wget https://github.com/downloads/s-yata/madoka/madoka-0.0.2.tar.gz
$ tar zxf madoka-0.0.2.tar.gz
$ cd madoka-0.0.2/
$ ./configure
$ make
$ make check
$ sudo make install
$ sudo ldconfig
```

[ソースコードのアーカイブ](https://github.com/downloads/s-yata/madoka/madoka-0.0.2.tar.gz) をダウンロードして展開した後，<kbd>configure</kbd> と <kbd>make</kbd> を実行すればビルド・インストールできます．環境によっては <kbd>ldconfig</kbd> が必要になるかもしれません．

ソースコードを [GitHub](http://github.com/s-yata/madoka) 上のリポジトリから得たときは，<kbd>configure</kbd> と <kbd>make</kbd> の前に <kbd>autoreconf -i</kbd> を実行して <kbd>configure</kbd> ファイルを生成する必要があります．

Madoka は Ubuntu 11.10 においてテストされています．ビルド・インストールにおいて問題が起きたときは， [GitHub](http://github.com/s-yata/madoka/issues) から報告していただけると助かります．

## Example

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

Madoka の基本的な使い方を示した C++ のサンプルコードです．この例では，デフォルトの設定を使ってスケッチを作成し，<var>"Madoka"</var> と <var>"Homura"</var> の頻度をカウントした後で，結果を出力しています．実行結果は <var>"Madoka: 1"</var>, <var>"Homura: 2"</var>, <var>"Mami: 0"</var> となります．

## Interface

Count-Min sketch の基本機能はアイテムの頻度を求めることですが，スケッチ同士の内積を推定できることから，スケッチを特徴ベクトルとして使うことも可能です．さらに，Madoka ではスケッチの合成や縮小をサポートしています．詳細については，以下のドキュメントを参照してください．

* 英語
  * [C API Documentation - 準備中](doc/c-api.html)
  * [C++ API Documentation](doc/cpp-api.html)
* 日本語
  * [C API Documentation - 準備中](doc/c-api.ja.html)
  * [C++ API Documentation](doc/cpp-api.ja.html)

## Implementation

* 日本語
  * [第 9 回自然言語処理勉強会](http://atnd.org/events/25020) の発表スライド ([PowerPoint](https://github.com/downloads/s-yata/madoka/TokyoNLP-09-madoka.pptx), [PDF](https://github.com/downloads/s-yata/madoka/TokyoNLP-09-madoka.pdf))

## License

Madoka は二条項 BSD ライセンス（ [The BSD 2-Clause License](http://www.opensource.org/licenses/bsd-license.php) ）を採用しています．

Copyright (c) 2012, Susumu Yata. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
