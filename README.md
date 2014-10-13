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

The `Velox` constructor takes a reporter and a configuration.  In this example a configuration wasn't provided so the default was used.  The template parameter is used to specify which clock should be used for taking measurements.  Currently, the clock defaults to std::chrono::high_resolution_clock for gcc and clang, and a custom clock based on QueryPerformanceCounter for MSVC (once the high_resolution clock is fixed in the next version of Visual Studio that will be the default).

Once a `Velox` object is created benchmarks can be run by calling the `bench` member function and providing a benchmark name and a callable to benchmark.

If the function being benchmarked needs to perform setup or teardown logic it can take a `velox::Stopwatch&` parameter and call the `measure` member function as in the below example:

```cpp
#include "velox_amalgamation.h"
#include <iostream>
#include <fstream>

std::vector<int> create_test_vector() {
  std::vector<int> v(5000);
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

###TextReporter
Outputs a summary of the different statistics calculated for a particular benchmark.  For the sort example the output may look something like this:

```
Benchmarking with `std::chrono::_V2::system_clock` which is unsteady
Benchmarking qsort
> Warming up for 5000 ms
> Collecting 100 measurements in estimated 10.758 s
> Found 10 outliers among 100 measurements (10%)
  > 3 (3%) high mild
  > 7 (7%) high severe
> estimating statistics
  > bootstrapping sample with 100000 resamples
  > mean   192.18 us +/- 357.98 ns [191.55 us 192.94 us] 95% CI
  > median 191.01 us +/- 131.88 ns [190.75 us 191.24 us] 95% CI
  > SD     3.5976 us +/- 797.00 ns [1.9055 us 5.0148 us] 95% CI
  > MAD    887.10 ns +/- 166.35 ns [597.84 ns 1.2642 us] 95% CI
  > LLS    190.75 us +/- 78.311 ns [190.62 us 190.92 us] 95% CI
  > r^2    0.9999856 +/- 3.728778e-06 [0.9999774 0.9999917] 95% CI

Benchmarking std::sort
> Warming up for 5000 ms
> Collecting 100 measurements in estimated 10.287 s
> Found 9 outliers among 100 measurements (9%)
  > 2 (2%) high mild
  > 7 (7%) high severe
> estimating statistics
  > bootstrapping sample with 100000 resamples
  > mean   77.274 us +/- 94.431 ns [77.107 us 77.474 us] 95% CI
  > median 76.960 us +/- 37.374 ns [76.890 us 77.053 us] 95% CI
  > SD     946.10 ns +/- 202.18 ns [510.73 ns 1.3030 us] 95% CI
  > MAD    239.26 ns +/- 48.988 ns [146.12 ns 333.07 ns] 95% CI
  > LLS    76.918 us +/- 25.987 ns [76.872 us 76.973 us] 95% CI
  > r^2    0.9999909 +/- 2.117352e-06 [0.9999865 0.9999948] 95% CI

```

For each benchmark velox begins by warming up for a configurable duration, it then determines how many iterations to run for each measurement, estimates the measurement time, and collects the measurements.

Once the measurements are collected outliers are classified as either low severe(Q1 - 3 * IQR), low mild(Q1 - 1.5 * IQR), high mild(Q3 + 1.5 * IQR), or high severe(Q3 + 3.0 * IQR).

Next, the collected measurements are [bootstrapped](http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29) with a configurable number of resamples(100,000 by default).  After the bootstrapping is complete the [mean](http://en.wikipedia.org/wiki/Mean), [median](http://en.wikipedia.org/wiki/Median), [standard deviation](http://en.wikipedia.org/wiki/Standard_deviation),  [median absolute deviation](http://en.wikipedia.org/wiki/Median_absolute_deviation),  [linear least squares](http://en.wikipedia.org/wiki/Ordinary_least_squares), and [r^2](http://en.wikipedia.org/wiki/Coefficient_of_determination) are output along the the calculated [confidence intervals](http://en.wikipedia.org/wiki/Confidence_interval).

###HtmlReporter
Generates an [HTML report](http://ctrychta.github.io/velox/sort_example.html) containing the same information the TextReporter outputs and a few different charts of the collected data.

The [kernel density estimate](http://en.wikipedia.org/wiki/Kernel_density_estimation) shows the probability of a particular measurement occurring.  The higher the probability density line the more likely a measurement is to occur.

The samples chart plots each of the measurements taken as the `total time / number of iterations`.  It also has lines to show any outliers.

The raw measurements chart shows the different measurements collected while benchmarking the function and the calculated regression line.

###MultiReporter
A helper class which can be constructed from multiple reporters which will forward calls to all of the contained reporters.  This is used because currently the `Velox` class supports a single reporter.

##Optimizer
With microbenchmarks it's not uncommon for the optimizer to determine that some or all of the code being benchmarked is not being used and eliminate it.  To help prevent this velox provides the `velox::optimization_barrier` function which can be called with a variable to prevent it being removed by dead code elimination or other optimizations.  Currently, this function is only available when using gcc or clang.  I'm still investigating how to implement this for Visual Studio (if you have any ideas please let me know). 

Of course, whether or not you use `velox::optimization_barrier`, it's always a good idea to look at the generated assembly of your benchmarks to make sure they are testing what you expect.

##License
velox is released under the [MIT](https://tldrlegal.com/license/mit-license) license.  The HtmlReporter uses the [jQuery](http://jquery.com/) and [HighCharts](http://www.highcharts.com/) libraries which are released under the MIT and [CC BY-NC 3.0](http://creativecommons.org/licenses/by-nc/3.0/) licenses respectively.
