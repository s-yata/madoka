---
layout: default
title: "C++ API Documentation"
lang: ja
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

手始めにスケッチを描いてみます．サンプルは，標準入力（<var>std::cin</var>）からキーを一つずつ読み込み，スケッチ（<code>madoka::Sketch</code>）を描いた後，完成したスケッチをコマンドの第一引数（<var>argv[1]</var>）で指定されたファイルに保存しています．注目すべき箇所は以下の通りです．

* <kbd>madoka.h</kbd>
  * Madoka を構成するクラスや定数は <kbd>madoka.h</kbd> の中で定義されています．そのため，<code>#include <madoka.h></code> が必要となります．
* <code>madoka::Sketch</code>
  * <code>madoka::Sketch</code> はスケッチのクラスであり，スケッチの描画・合成やファイル入出力のインタフェースを持ちます．
* <code>madoka::Sketch::create()</code>
  * <code>create()</code> は白紙のスケッチを作成する関数です．スケッチを作成せずに描画しようとするとマミるので注意してください．
* <code>madoka::Sketch::inc()</code>
  * <code>inc()</code> はスケッチを描画するための関数です．正確には，指定されたキーに対応する値をインクリメントします．第一引数にはキーの開始アドレス，第二引数にはキーのバイト数を指定するようになっています．
  * インクリメントの対象が既に飽和しているときは何もしないので，オーバーフローを気にせずスケッチを描くことができます．
* <code>madoka::Sketch::save()</code>
  * <code>save()</code> はスケッチをファイルに保存する関数です．第一引数にはファイルのパスを指定するようになっています．

サンプルのビルドには <kbd>-lmadoka</kbd> というオプションが必要なことに注意してください．Madoka がインストールされている環境であれば，<kbd>pkg-config madoka --libs</kbd> によってオプションを得ることができます．

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

次に，描いたスケッチを眺めます．サンプルでは，コマンドの第一引数（<var>argv[1]</var>）で指定されたファイルからスケッチを入力し，標準入力（<var>std::cin</var>）から読み込んだキーに対応する値をスケッチから読み取っています．注目すべき箇所は以下の通りです．

* <code>madoka::Sketch::load()</code>
  * <code>load()</code> はスケッチをファイルから入力する関数です．第一引数にはファイルのパスを指定するようになっています．
  * <code>open()</code> も同様の関数ですが，ファイル全体を読み込みたくない状況で使うことを想定しています．メモリマップド I/O を使うため，描画した内容が他のプロセスに反映されたり，他のプロセスが描画した内容が反映されたりすることに注意してください．
* <code>madoka::Sketch::get()</code>
  * <code>get()</code> はキーに対応する値をスケッチから読み取る関数です．第一引数にはキーの開始アドレス，第二引数にはキーのバイト数を指定するようになっています．読み取った値が戻り値になります．

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

<code>madoka::Sketch</code> が提供する描画用の関数には，<code>inc()</code> のほかに <code>set()</code> と <code>add()</code> があります．サンプルはそれぞれの役割を示しています．

* <code>madoka::Sketch::set()</code>
  * <code>set()</code> はキーと対応する値を更新する関数です．第一引数にはキーの開始アドレス，第二引数にはキーのバイト数を指定するようになっています．第三引数には新しい値を指定します．
  * 新しい値が現在の値以下のときは何もしないことに注意してください．
  * 指定した新しい値が最大値を超えているときは，キーと対応する値を最大値へと更新します．オーバーフローの心配はありません．
* <code>madoka::Sketch::add()</code>
  * <code>add()</code> は加算の関数です．第一引数にはキーの開始アドレス，第二引数にはキーのバイト数を指定するようになっています．第三引数には加える値を指定します．加算の結果が戻り値になります．
  * オーバーフローしそうなときは，加算の結果を最大値へと補正します．

サンプルにおいて，最初の <code>set()</code> は <var>"QB"</var> に対応する値を <var>0</var> から <var>10</var> に更新します．次に，<code>add()</code> が <var>5</var> を加算するため，<var>"QB"</var> に対応する値は <var>15</var> になります．二回目の <code>set()</code> に指定された値（<var>7</var>）は <var>15</var> より小さいため，値の更新はおこなわれず，<code>get()</code> の戻り値は <var>15</var> となります．

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

[Count-Min sketch](https://sites.google.com/site/countminsketch/) は確率的なデータ構造なので，スケッチによって得られる値には誤差が含まれます．そして，誤差の大きさは，スケッチを作成するときに指定するパラメータである <var>width</var> と <var>depth</var>，およびに入力するキーの分布に依存します．そのため，Count-Min sketch の性能を引き出すには，適切なパラメータを与えることが重要になります．

基本的に，正確なスケッチを描くには大きな <var>width</var> が必要となり，入力するキーが多ければ大きな <var>width</var> が必要となります．しかし，<var>width</var> を大きくすると，メモリ使用量が増えるだけでなく，キャッシュミスも増えてしまいます．

残るパラメータである <var>depth</var> については，<var>width</var> と同じく精度に影響を与える存在であるものの，Madoka では固定値を採用しています．具体的には <var>madoka::SKETCH_DEPTH</var> （<var>3</var>）です．この値は，実装の都合と実験の結果を勘案して選択しました．

<var>depth</var> の固定によってパラメータを減らした代わりに，Madoka は新たなパラメータとして <var>max_value</var> を採用しています．<var>max_value</var> はスケッチに保存できる値の上限であり，小さい値を指定することによってスケッチのメモリ使用量を抑えることができます．

特製のスケッチが欲しいときは，<code>create()</code> に対して <var>width</var> と <var>max_value</var> を指定します．以下の情報を参考に，いろいろと試してみてください．

* <var>width</var>
  * <var>0</var> を指定したときは，デフォルトの値である <var>madoka::SKETCH_DEFAULT_WIDTH</var> （<var>2<sup>20</sup></var>）が採用されます．
  * <var>width</var> には <var>madoka::SKETCH_MAX_WIDTH</var> （<var>2<sup>42</sup></var>）以下の値を指定する必要があります．
  * <var>width</var> を 2 のべき乗にすることによって，スケッチの描画にかかる時間を短縮できます．
* <var>max_value</var>
  * <var>0</var> を指定したときは，デフォルトの値である <var>madoka::SKETCH_DEFAULT_MAX_VALUE</var> （<var>2<sup>45</sup> - 1</var>）が採用されます．
  * <var>max_value</var> には <var>madoka::SKETCH_MAX_MAX_VALUE</var> （<var>2<sup>45</sup> - 1</var>）以下の値を指定する必要があります．
  * <code>create()</code> は <var>max_value</var> を <var>1</var>, <var>3</var>, <var>15</var>, <var>255</var>, <var>65535</var>, <var>2<sup>45</sup> - 1</var> のいずれかに切り上げます．

サンプルでは，特製のスケッチを作成した後，そのサイズを出力しています．スケッチのサイズについては，<var>max_value</var> が <var>madoka::SKETCH_MAX_MAX_VALUE</var> であれば <var>width x 8</var> によって，<var>max_value</var> が <var>madoka::SKETCH_MAX_MAX_VALUE</var> でなければ <var>width x depth x log<sub>2</sub>(max_value + 1) / 8</var> によって計算できます．

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

<code>madoka::Sketch</code> にはスケッチを白紙の状態に戻すためのインタフェースがあります．サンプルでは，作成したスケッチに更新を加えてから白紙に戻し，更新によって <var>100</var> になった値が <var>0</var> に戻っていることを確認しています．

* <code>madoka::Sketch::clear()</code>
  * <code>clear()</code> はスケッチを白紙に戻す関数であり，すべての値を <var>0</var> にします．
  * <var>width</var> と <var>max_value</var> はそのままになります．

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

<code>madoka::Sketch</code> にはスケッチを複製するためのインタフェースがあります．サンプルでは，スケッチの複製をスナップショットとして残し，オリジナルのスケッチを更新しています．

* <code>madoka::Sketch::copy()</code>
  * <code>copy()</code> はスケッチを複製する関数です．第一引数には複製元のスケッチを指定するようになっています．

サンプルでおこなっているように，<code>copy()</code> はメモリ上でスナップショットを作成する用途に使えます．スナップショットをファイルとして残したいときは，<code>save()</code> を使う方が簡単です．

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

<code>madoka::Sketch</code> にはフィルタという機能があり，誤差の抑制や減衰のシミュレーションなどに利用できます．サンプルでは，スケッチに含まれるすべての値を <var>1/2</var> にするフィルタを適用しています．

* <code>madoka::Sketch::filter()</code>
  * <code>filter()</code> はスケッチにフィルタを適用する関数です．具体的には，フィルタとして渡された関数をスケッチに含まれるすべての値に適用します．第一引数にはフィルタを指定するようになっています．
  * フィルタとして指定できるのは，現在の値（<code>madoka::UInt64</code>）を受け取り，適用後の値（<code>madoka::UInt64</code>）を返す関数へのポインタです．
  * 関数オブジェクトを渡すことも可能ですが，引数と戻り値の型を <code>madoka::UInt64</code> にする必要があります．
  * フィルタが <var>max_value</var> を超える値を返したときは自動的に <var>max_value</var> へと補正するため，<code>UINT64_MAX</code> を超えなければ問題にはなりません．
  * <var>NULL</var> を引数にしたときは何もしません．

フィルタ機能を用いれば， [Lossy Conservative Update (PDF)](http://www.umiacs.umd.edu/%7Eamit/Papers/goyalLCUSketchAAAI11.pdf) を実装することができます．また，カウンタが飽和したときに全体を <var>1/2</var> にすることでスケッチの延命を図るという使い方があります．さらに，すべての値を対数に変換することで圧縮するなどの使い方もあります．

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

<code>madoka::Sketch</code> にはスケッチを縮小するためのインタフェースがあります．スケッチの縮小は <var>width</var> と <var>max_value</var> の設定にも関わる機能です．

たとえば，スケッチに割り当てた <var>width</var> もしくは <var>max_value</var> が大きすぎると分かったときは，スケッチを縮小することができます．正確には，新たに小さなスケッチを作成して，既存のスケッチを構成する値を新しいスケッチに対して設定しなおすという手順になります．

* <code>madoka::Sketch::shrink()</code>
  * <code>shrink()</code> はスケッチを縮小する関数です．第一引数には元のスケッチを指定するようになっています．第二引数には新しいスケッチの <var>width</var>，第三引数には新しいスケッチの <var>max_value</var> を指定するようになっています．第四引数には，値の再設定に際して適用するフィルタを指定することができます．
  * 新しいスケッチの <var>width</var> は，元になるスケッチの <var>width</var> を割り切れる値にする必要があります．一方の <var>max_value</var> には制約がありません．元になるスケッチの <var>max_value</var> より大きな値でも指定することができます．

サンプルでは，<var>"Akemi: 256"</var> と <var>"Homura: 16777216"</var> を描き込んだスケッチを縮小しています．新しいスケッチの <var>width</var> と <var>max_value</var> はそれぞれ <var>10</var> と <var>15</var> です．<code>logarimize()</code> をフィルタとして適用しているため，新しいスケッチから読み取ることができる値は対数になります．たとえば，<var>"Akemi: 256"</var> は <var>"Akemi: 8"</var> となります．ただし，新しいスケッチの <var>max_value</var> は <var>15</var> であることから，<var>"Homura: 16777216"</var> は <var>"Homura: 15"</var> となります．

スケッチを縮小できるという特徴により，描画の時点では大きめの <var>width</var> と <var>max_value</var> を採用し，描画の後で用途に応じた <var>width</var> と <var>max_value</var> を選ぶという使い方ができます．また，対数への圧縮や閾値による二値化など，さまざまな使い方が考えられます．

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

<code>madoka::Sketch</code> はスケッチの合成をサポートしています．スケッチの合成はベクトルの加算と同様の操作であり，一方のスケッチ（右）を構成する値を他方のスケッチ（左）を構成する値に足し合わせます．

* <code>madoka::Sketch::merge()</code>
  * <code>merge()</code> はスケッチを合成する関数です．第一引数には右スケッチを指定するようになっています．第二引数には左スケッチから値を読み取るときに適用するフィルタ，第三引数には右スケッチから値を読み取るときに適用するフィルタを指定することができます．
  * <var>width</var> が等しいスケッチの組に対してのみ有効です．

サンプルでは，<var>"Mami: 1"</var> を描き込んだスケッチ同士を合成しています．そのため，合成により得られたスケッチからは <var>"Mami: 2"</var> を読み取ることができます．このサンプルでは <code>copy()</code> を使うことで元のスケッチを残していますが，上書きが問題にならない状況であれば，直接 <code>merge()</code> を呼び出しても問題ありません．

スケッチを合成する機能により，分散してスケッチを描画することができます．たとえば，入力を分割することができれば，それぞれに対するスケッチを描画してから合成することにより，最終的には一つのスケッチを得ることができます．

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

Count-Min sketch は内積の推定をサポートしています．<code>madoka::Sketch</code> には，内積の推定とともにスケッチの長さ（仮）を推定できるインタフェースがあります．実際にスケッチの長さという概念が存在するわけではありませんが，コサイン類似度の推定に用いることができます．

* <code>madoka::Sketch::inner_product()</code>
  * <code>inner_product()</code> は内積を推定する関数です．第一引数には右スケッチを指定するようになっています．第二引数には左スケッチの長さを受け取る変数，第三引数には右スケッチの長さを受け取る変数を指定することができます．内積の推定値が戻り値になります．
  * <var>width</var> が等しいスケッチの組に対してのみ有効です．

サンプルでは，描画したスケッチの内積を求めています．また，<code>inner_product()</code> の結果を用いてコサイン類似度を推定しています．

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

<code>create()</code>, <code>open()</code>, <code>load()</code>, <code>save()</code>, <code>copy()</code>, <code>shrink()</code> は <var>path</var> と <var>flags</var> を引数として指定できるようになっています．<var>path</var> に対して <var>NULL</var> を指定したときは，ファイルとの関連付けをせずにメモリを確保します．<var>flags</var> にはファイルの操作やマッピングの作成に関する振る舞いを指定することができます．

* <var>madoka::FILE_CREATE</var>
  * <var>FILE_CREATE</var> はファイルを作成するときに暗黙的に指定されるフラグです．
* <var>madoka::FILE_TRUNCATE</var>
  * <var>FILE_TRUNCATE</var> は <code>create()</code>, <code>copy()</code>, <code>shrink()</code> において <var>path</var> が <var>NULL</var> でないときに指定できるフラグです．
  * <var>FILE_TRUNCATE</var> はファイルの上書きを許可するフラグです．<var>FILE_TRUNCATE</var> が指定されていないときにファイルが存在していれば，例外が投げられます．
* <var>madoka::FILE_READONLY</var>
  * <var>FILE_READONLY</var> は <code>open()</code> において指定できるフラグです．
  * <var>FILE_READONLY</var> は更新を禁止するフラグです．読み込み専用のスケッチを更新しようとするとマミるので注意してください．
* <var>madoka::FILE_WRITABLE</var>
  * <var>FILE_WRITABLE</var> は書き込み可能なマッピングを作成するときに暗黙的に指定されるフラグです．
* <var>madoka::FILE_SHARED</var>
  * <var>FILE_SHARED</var> は更新内容を他のプロセスに反映させるために暗黙的に指定されるフラグです．
* <var>madoka::FILE_PRIVATE</var>
  * <var>FILE_PRIVATE</var> は <code>open()</code> において指定できるフラグです．
  * <var>FILE_PRIVATE</var> は更新内容を他のプロセスに反映させないためのフラグです．
* <var>madoka::FILE_ANONYMOUS</var>
  * <var>FILE_ANONYMOUS</var> はファイルとの関連付けをおこなわないときに暗黙的に指定されるフラグです．
* <var>madoka::FILE_HUGETLB</var>
  * <var>FILE_HUGETLB</var> は <code>create()</code>, <code>open()</code>, <code>load()</code>, <code>save()</code>, <code>copy()</code>, <code>shrink()</code> において指定できるフラグです．
  * <var>FILE_HUGETLB</var> は HugePage の使用を許可するフラグです．詳細については次のセクションを参照してください．
* <var>madoka::FILE_PRELOAD</var>
  * <var>FILE_PRELOAD</var> は <code>open()</code> において指定できるフラグです．
  * <var>FILE_PRELOAD</var> はファイル全体の読み込みを指示するフラグです．ディスクに対するランダムアクセスを回避したいときに有用です．

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

概略を述べると，Count-Min sketch とは <var>depth</var> 個のハッシュ表を組み合わせたデータ構造です．そして，<code>madoka::Sketch</code> では <var>depth</var> を <var>3</var> に固定しているため，<code>get()</code> 以外の基本操作 <code>set()</code>, <code>inc()</code>, <code>add()</code> は <var>3</var> 回の [ランダムアクセス](https://ja.wikipedia.org/wiki/%E3%83%A9%E3%83%B3%E3%83%80%E3%83%A0%E3%82%A2%E3%82%AF%E3%82%BB%E3%82%B9) をおこないます．

スケッチの描画はランダムアクセスを必要とするため，スケッチが [CPU キャッシュ](https://ja.wikipedia.org/wiki/%E3%82%AD%E3%83%A3%E3%83%83%E3%82%B7%E3%83%A5%E3%83%A1%E3%83%A2%E3%83%AA) に収まらなければ，キャッシュミスが発生するようになり， [メモリのレイテンシ](https://ja.wikipedia.org/wiki/%E3%83%AC%E3%82%A4%E3%83%86%E3%83%B3%E3%82%B7) がボトルネックになります．さらにスケッチを大きくすると， [TLB](https://ja.wikipedia.org/wiki/%E3%83%88%E3%83%A9%E3%83%B3%E3%82%B9%E3%83%AC%E3%83%BC%E3%82%B7%E3%83%A7%E3%83%B3%E3%83%BB%E3%83%AB%E3%83%83%E3%82%AF%E3%82%A2%E3%82%B5%E3%82%A4%E3%83%89%E3%83%BB%E3%83%90%E3%83%83%E3%83%95%E3%82%A1) ミスが発生するようになり，描画のスループットを低下させてしまいます．

<var>madoka::FILE_HUGETLB</var> は [HugePage](https://en.wikipedia.org/wiki/Page_%28computer_memory%29#Huge_pages) の使用を許可するフラグです．HugePage を使えば，スケッチの描画における TLB ミスを減らすことができます．スケッチの作成に際して <var>madoka::FILE_HUGETLB</var> を指定すると，HugePage の使用を試みます．このとき，HugePage が使えない状況であれば，通常のページを使います．

サンプルは HugePage の使い方を示しています．まず，<kbd>/proc/meminfo</kbd> を見ることにより，HugePage が有効になっているかどうかを確認できます．無効になっているときは，<kbd>/proc/sys/vm/nr_hugepages</kbd> を編集することで有効にすることができます．HugePage を有功にするには root 権限が必要なことに注意してください．HugePage の有効な環境を用意できれば，<code>madoka::Sketch</code> で HugePage を使うことができます．ただし，ファイルと関連付けたスケッチについては，ファイルシステムが HugePage をサポートしていない限りは HugePage を使うことができません．詳細については， [HugePage のサポートに関する情報](https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt) を参照してください．

## Specify a seed

<code>madoka::Sketch</code> では，ハッシュ関数および疑似乱数生成器の種を指定することができます．指定方法は <code>create()</code> の第五引数です．既に存在するスケッチの種を変更することはできません．種の異なるスケッチ同士を合成することはできないという制約があるため，デフォルトの種に統一しておいた方が使いやすくなります．基本的に使わない機能です．

## Get information of a sketch

* <code>madoka::Sketch::width()</code>
  * <code>width()</code> は <var>width</var> を返します．
* <code>madoka::Sketch::width_mask()</code>
  * <code>width_mask()</code> は，<var>width</var> が 2 のべき乗であれば <var>width - 1</var> を返し，そうでなければ <var>0</var> を返します．
* <code>madoka::Sketch::depth()</code>
  * <code>depth()</code> は <var>madoka::SKETCH_DEPTH</var> を返します．
* <code>madoka::Sketch::max_value()</code>
  * <code>max_value()</code> は <var>max_value</var> を返します．
* <code>madoka::Sketch::value_mask()</code>
  * <code>value_mask()</code> は <var>max_value</var> を返します．
* <code>madoka::Sketch::value_size()</code>
  * <code>value_size()</code> は <var>log<sub>2</sub>(max_value + 1)</var> を返します．
* <code>madoka::Sketch::seed()</code>
  * <code>seed()</code> は <var>seed</var> を返します．
* <code>madoka::Sketch::table_size()</code>
  * <code>table_size()</code> はスケッチのサイズをバイト単位で返します．
* <code>madoka::Sketch::file_size()</code>
  * <code>file_size()</code> はスケッチのファイルサイズをバイト単位で返します．ファイルサイズはヘッダのサイズとスケッチのサイズを加算することによって求められます．
* <code>madoka::Sketch::flags()</code>
  * <code>flags()</code> はメモリマップド I/O に関するフラグの論理和を返します．
* <code>madoka::Sketch::mode()</code>
  * <code>mode()</code> は， <var>value_size</var> が <var>madoka::SKETCH_APPROX_VALUE_SIZE</var> であれば <var>madoka::SKETCH_APPROX_MODE</var> を返し，そうでなければ <var>madoka::SKETCH_EXACT_MODE</var> を返します．

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

Madoka はエラーが起きると例外を投げます．例外のクラスは <code>madoka::Exception</code> です．サンプルは例外の捕まえ方を示しています．

* <code>madoka::Exception::what()</code>
  * <code>what()</code> はエラーメッセージを返す関数です．エラーメッセージの書式は，<code>__FILE__</code> << <var>":"</var> << <code>__LINE__</code> << <var>": "</var> << <var>the-reason-of-the-error</var> となっています．
  * <kbd>madoka/sketch.h</kbd> の例外指定子を確認することにより，どの関数が例外を投げるのかを確認できます．

サンプルでは，指定した <var>width</var> が大きすぎるため，<code>create()</code> が例外を投げます．エラーが起きたときは <code>madoka::Sketch</code> の内容が更新されないようになっています．

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

<code>madoka::Croquis</code> は <code>madoka::Sketch</code> を単純化したクラスです．<code>inc()</code>, <code>copy()</code>, <code>filter()</code>, <code>shrink()</code>, <code>merge()</code>, <code>inner_product()</code> は使えません．その代わり，テンプレート引数によって値の型を指定することができます．

サンプルでは，<code>madoka::Croquis<float></code> とすることにより， [浮動小数点数](https://ja.wikipedia.org/wiki/%E6%B5%AE%E5%8B%95%E5%B0%8F%E6%95%B0%E7%82%B9%E6%95%B0) を値の型として使っています．使い方は <code>madoka::Sketch</code> とほぼ同じですが，丸めによる誤差があるため，小さい値を <code>add()</code> に渡すと切り捨てられる可能性があることに注意してください．
