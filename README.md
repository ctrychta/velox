velox
=====

This is a C++ micro-benchmarking library inspired by Haskell's [criterion](https://hackage.haskell.org/package/criterion).

##Usage
Velox is available as a single header, [velox_amalgamation.h](https://raw.githubusercontent.com/ctrychta/velox/master/amalgamation/velox_amalgamation.h), which can be included in your project.  It should work with gcc 4.8+, clang 3.4+, and VS2013+.

##Examples
```cpp
#include "velox_amalgamation.h"
#include <fstream>
#include <iostream>
#include <thread>

int main() {
  velox::TextReporter text_reporter(std::cout);
  std::ofstream fout("example.html");
  velox::HtmlReporter html_reporter(fout);
  velox::MultiReporter reporter(text_reporter, html_reporter);

  velox::Velox<> v(reporter);
  v.bench("vector allocation", [] { std::vector<int>(20000); });
}
```

The `Velox` constructor takes a reporter and a configuration.  In this example a configuration wasn't provided so the default was used.  The template parameter is used to specify which clock should be used for taking measurements.  Currently, the clock defaults to std::chrono::high_resolution_clock for gcc and clang, and a custom clock based on QueryPerformanceCounter for MSVC (once the high_resolution clock is fixed in the next version of visual studio that will be the default).

Once a `Velox` object is created benchmarks can be run by calling the `bench` member function and providing a benchmark name and a callable to benchmark.

If the function being benchmarked needs to perform setup or teardown logic it can take a `velox::Stopwatch&` parameter and call the `measure` member function as in the below example:

```cpp
#include "velox_amalgamation.h"
#include <iostream>
#include <fstream>

std::vector<int> create_test_vector() {
  std::vector<int> v(10000);
  std::iota(v.begin(), v.end(), 0);
  std::shuffle(v.begin(), v.end(), std::mt19937(42));
  return v;
}

int compare_ints_desc(const void *a, const void *b) {
  int lhs = *static_cast<const int *>(a);
  int rhs = *static_cast<const int *>(b);
  if (lhs > rhs)
    return -1;
  if (lhs < rhs)
    return 1;
  return 0;
}

void c_sort(velox::Stopwatch &sw) {
  auto v = create_test_vector();
  sw.measure([&] { std::qsort(v.data(), v.size(), sizeof(int), compare_ints_desc); });
}

void cpp_sort(velox::Stopwatch &sw) {
  auto v = create_test_vector();
  sw.measure([&] { std::sort(v.begin(), v.end(), std::greater<int>()); });
}

int main() {
  velox::TextReporter text_reporter{std::cout};
  std::ofstream fout("sort_example.html");
  velox::HtmlReporter html_reporter{fout};
  velox::MultiReporter reporter(text_reporter, html_reporter);

  velox::Velox<> v(reporter);
  v.bench("qsort", c_sort).bench("std::sort", cpp_sort);

  return 0;
}
```

##Reporters
Velox comes with a few built in reporters and more can be easily created by deriving from `velox::Reporter`.

##Optimizer
With small microbenchmarks it's not uncommon for the optimizer to determine that some or all of the code being benchmarked is not being used and eliminate it.  To help prevent this velox provides the `velox::optimization_barrier` function which can be called with a variable to prevent it being removed by dead code elimination or other optimizations.  Currently, this function is only available when using gcc or clang.  I'm still investigating how to implement this for Visual Studio (if you have any ideas please let me know). 

Of course, whether or not you use `velox::optimization_barrier`, it's always a good idea to look at the generated assembly of your benchmarks to make sure they are testing what you expect.

##License
velox is released under the [MIT](https://tldrlegal.com/license/mit-license) license.  The HtmlReporter uses the [jQuery](http://jquery.com/) and [HighCharts](http://www.highcharts.com/) libraries which are released under the MIT and [CC BY-NC 3.0](http://creativecommons.org/licenses/by-nc/3.0/) licenses respectively.
