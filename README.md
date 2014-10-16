velox
=====

A C++ micro-benchmarking library inspired by Haskell's [criterion](https://hackage.haskell.org/package/criterion).

##Usage
Velox is available as a single header, [velox_amalgamation.h](https://raw.githubusercontent.com/ctrychta/velox/master/amalgamation/velox_amalgamation.h), which can be included in your project.  It should work with gcc 4.8+, clang 3.4+, and VS2013+.

##Examples
```cpp
#include "velox_amalgamation.h"
#include <fstream>
#include <iostream>

int main() {
  velox::TextReporter text_reporter(std::cout);
  std::ofstream fout("example.html");
  velox::HtmlReporter html_reporter(fout);
  velox::MultiReporter reporter(text_reporter, html_reporter);

  velox::Velox<> v(reporter);
  v.bench("vector allocation", [] { std::vector<int>(20000); });
}
```

The `Velox` constructor takes a reporter and a configuration.  In this example a configuration wasn't provided so the default was used.  The template parameter is used to specify which clock should be used for taking measurements.  Currently, the clock defaults to `std::chrono::high_resolution_clock` for gcc and clang, and a custom clock based on QueryPerformanceCounter for MSVC (once the `high_resolution clock` is fixed in the next version of Visual Studio that will be the default).

Once a `Velox` object is created benchmarks can be run by calling the `bench` member function and providing a benchmark name and a callable to benchmark.

If the function being benchmarked needs to perform setup or teardown logic it can take a `velox::Stopwatch&` parameter and call the `measure` member function as in the below example:

```cpp
#include "velox_amalgamation.h"
#include <iostream>
#include <fstream>

std::vector<int> create_test_vector() {
  std::vector<int> v(1000000);
  std::iota(v.begin(), v.end(), 0);
  return v;
}

void linear_search(velox::Stopwatch &sw) {
  auto v = create_test_vector();
  sw.measure([&] { velox::optimization_barrier(std::find(v.begin(), v.end(), 984760)); });
}

void binary_search(velox::Stopwatch &sw) {
  auto v = create_test_vector();
  sw.measure([&] { velox::optimization_barrier(std::binary_search(v.begin(), v.end(), 984760)); });
}

int main() {
  velox::TextReporter text_reporter{std::cout};
  std::ofstream fout("search_example.html");
  velox::HtmlReporter html_reporter{fout};
  velox::MultiReporter reporter(text_reporter, html_reporter);

  velox::Velox<> v(reporter);
  v.bench("linear search", linear_search).bench("binary search", binary_search);

  return 0;
}
```

##Reporters
Velox comes with a few built in reporters and more can be easily created by deriving from `velox::Reporter`.

###TextReporter
Outputs a summary of the different statistics calculated for a particular benchmark.  For the sort example the output may look something like this:

```
Benchmarking with `std::chrono::_V2::system_clock` which is unsteady
Benchmarking linear search
> Warming up for 5000 ms
> Collecting 100 measurements in estimated 10.981 s
> Found 13 outliers among 100 measurements (13%)
  > 7 (7%) high mild
  > 6 (6%) high severe
> estimating statistics
  > bootstrapping sample with 100000 resamples
  > mean   236.31 us +/- 157.76 ns [236.03 us 236.65 us] 95% CI
  > median 235.81 us +/- 62.929 ns [235.70 us 235.98 us] 95% CI
  > SD     1.5892 us +/- 364.25 ns [924.04 ns 2.2553 us] 95% CI
  > MAD    568.98 ns +/- 78.268 ns [408.01 ns 721.51 ns] 95% CI
  > LLS    235.98 us +/- 121.75 ns [235.77 us 236.24 us] 95% CI
  > r^2    0.9999847 +/- 6.430088e-06 [0.9999709 0.9999954] 95% CI

Benchmarking binary search
> Warming up for 5000 ms
> Collecting 100 measurements in estimated 10.198 s
> Found 13 outliers among 100 measurements (13%)
  > 5 (5%) high mild
  > 8 (8%) high severe
> estimating statistics
  > bootstrapping sample with 100000 resamples
  > mean   21.112 ns +/- 7.3282 ps [21.100 ns 21.128 ns] 95% CI
  > median 21.091 ns +/- 1.8364 ps [21.088 ns 21.094 ns] 95% CI
  > SD     73.463 ps +/- 20.490 ps [36.438 ps 109.48 ps] 95% CI
  > MAD    18.101 ps +/- 2.7952 ps [12.442 ps 23.078 ps] 95% CI
  > LLS    21.097 ns +/- 3.0996 ps [21.092 ns 21.104 ns] 95% CI
  > r^2    0.9999979 +/- 1.217583e-06 [0.999995 0.9999994] 95% CI
```

For each benchmark velox begins by warming up for a configurable duration, it then determines how many iterations to run for each measurement, estimates the measurement time, and collects the measurements.

Once the measurements are collected outliers are classified as either low severe(Q1 - 3 * IQR), low mild(Q1 - 1.5 * IQR), high mild(Q3 + 1.5 * IQR), or high severe(Q3 + 3 * IQR).

Next, the collected measurements are [bootstrapped](http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29) with a configurable number of resamples(100,000 by default).  After the bootstrapping is complete the [mean](http://en.wikipedia.org/wiki/Mean), [median](http://en.wikipedia.org/wiki/Median), [standard deviation](http://en.wikipedia.org/wiki/Standard_deviation),  [median absolute deviation](http://en.wikipedia.org/wiki/Median_absolute_deviation),  [linear least squares](http://en.wikipedia.org/wiki/Ordinary_least_squares), and [r^2](http://en.wikipedia.org/wiki/Coefficient_of_determination) are output along with the calculated [confidence intervals](http://en.wikipedia.org/wiki/Confidence_interval).

###HtmlReporter
Generates an [HTML report](http://ctrychta.github.io/velox/search_example.html) containing the same information the TextReporter outputs and a few different charts of the collected data.

The [kernel density estimate](http://en.wikipedia.org/wiki/Kernel_density_estimation) shows the probability of a particular measurement occurring.  The higher the probability density line the more likely a measurement is to occur.

The samples chart plots each of the measurements taken as the `total time / number of iterations`.  It also has lines to show any outliers according to the IQR criteria.

The raw measurements chart shows the different measurements collected while benchmarking the function and the calculated regression line.

###MultiReporter
A helper class which can be constructed from multiple reporters which will forward calls to all of the contained reporters.  This is used because currently the `Velox` class supports a single reporter.

##optimization_barrier
With micro-benchmarks it's not uncommon for the optimizer to determine that some or all of the code being benchmarked is not being used and eliminate it.  To help prevent this velox provides the `velox::optimization_barrier` function which can be used to tell the compiler that a variable or return value is used in order to prevent it being removed by [DCE](http://en.wikipedia.org/wiki/Dead_code_elimination).  The implementation of this function for gcc and clang should have no overhead, while the implementation for MSVC has a small amount of overhead (a couple of mov's and a test).

Of course, whether or not you use `velox::optimization_barrier`, it's always a good idea to look at the generated assembly of your benchmarks to ensure they are testing what you expect.

##License
velox is released under the [MIT](https://tldrlegal.com/license/mit-license) license.  The HtmlReporter uses the [jQuery](http://jquery.com/) and [HighCharts](http://www.highcharts.com/) libraries which are released under the [MIT](https://tldrlegal.com/license/mit-license) and [CC BY-NC 3.0](https://tldrlegal.com/license/creative-commons-attribution-noncommercial-%28cc-nc%29#summary) licenses respectively.
