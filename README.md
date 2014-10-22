velox
=====

A C++ micro-benchmarking library inspired by Haskell's [criterion](https://hackage.haskell.org/package/criterion).  Velox calculates various statistics for the functions being benchmarked and comes with reporters to generate html reports and textual summaries.

##Example
```cpp
#include "velox_amalgamation.h"
#include <iostream>
#include <fstream>

std::vector<int> create_test_vector(unsigned i) {
  std::vector<int> v(i);
  std::iota(v.begin(), v.end(), 0);
  return v;
}

void linear_search(velox::Stopwatch &sw, unsigned i) {
  auto v = create_test_vector(i);
  sw.measure([&] { velox::optimization_barrier(std::find(v.begin(), v.end(), i - 1)); });
}

void binary_search(velox::Stopwatch &sw, unsigned i) {
  auto v = create_test_vector(i);
  sw.measure([&] { velox::optimization_barrier(std::binary_search(v.begin(), v.end(), i - 1)); });
}

int main() {
  velox::TextReporter text_reporter{std::cout};
  std::ofstream fout("search_example.html");
  velox::HtmlReporter html_reporter{fout};
  velox::MultiReporter reporter(text_reporter, html_reporter);

  velox::Velox<> v(reporter);
  v.bench_with_arg("linear search", linear_search, {4u, 8u, 16u, 32u, 64u})
      .bench_with_arg("binary search", binary_search, {4u, 8u, 16u, 32u, 64u});
}
```

The above code generates both an [html report](http://ctrychta.github.io/velox/search_example.html) and a summary which looks like this:
```
Benchmarking with `std::chrono::_V2::system_clock` which is unsteady
Benchmarking linear search / 4
> Warming up for 5000 ms
> Collecting 100 measurements in estimated 10.198 s
> Found 14 outliers among 100 measurements (14%)
  > 5 (5%) high mild
  > 9 (9%) high severe
> estimating statistics
  > bootstrapping sample with 100000 resamples
  > mean   1.1905 ns +/- 1.2334 ps [1.1883 ns 1.1931 ns] 95% CI
  > median 1.1862 ns +/- 0.1874 ps [1.1860 ns 1.1867 ns] 95% CI
  > SD     12.400 ps +/- 2.8081 ps [6.1391 ps 17.250 ps] 95% CI
  > MAD    0.7500 ps +/- 0.2612 ps [0.4594 ps 1.4803 ps] 95% CI
  > LLS    1.1874 ns +/- 0.3524 ps [1.1868 ns 1.1882 ns] 95% CI
  > r^2    0.9999822 +/- 1.023804e-05 [0.999958 0.9999962] 95% CI
--- truncated ---
``` 

##Usage
Velox is available as a single header, [velox_amalgamation.h](https://raw.githubusercontent.com/ctrychta/velox/master/amalgamation/velox_amalgamation.h), which can be included in your project.  It should work with gcc 4.8+, clang 3.4+, and VS2013+.

##Documentation
###VeloxConfig
- `warm_up_time`: The number of milliseconds to run the function being benchmarked before taking any measurements.  Besides allowing the OS/CPU to adapt to the function this warm up period is used to estimate how long a single call to the function takes.
- `measurement_time`: The number of milliseconds to run each benchmark.  This is not a strict limit and, depending on the function, the actual time may be much larger.
- `num_measurements`: The number of measurements to take.  Each measurement will consist of a different number of iterations of the function.  The first measurement will always be at least two iterations and the number of iterations will increase by at least one per measurement.  So, for 100 measurements the function being benchmarked will be called at least 5150 times (which is the reason the `measurement_time` is not a strict upper bound).
- `num_resamples`: The number of resamples to use when [bootstrapping](http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29) the calculated statistics.
- `confidence_level`: Used when calculating [confidence intervals](https://en.wikipedia.org/wiki/Confidence_interval) of the various statistics.
- `estimate_clock_cost`: Whether or not to estimate the clock cost.  The cost is not used in any calculations so it will just be reported.

###DefaultClock
The default clock used when benchmarking functions.  On linux this is `std::chrono::high_resolution_clock` and on windows this is `velox::WindowsHighResolutionClock`.  The windows clock is implemented using QueryPerformanceCounter and is needed because the `std::chrono::high_resolution_clock` provided with VS2013 is not actually high resolution.  The clocks provided with the next version of visual studio have been [fixed](http://blogs.msdn.com/b/vcblog/archive/2014/06/06/c-14-stl-features-fixes-and-breaking-changes-in-visual-studio-14-ctp1.aspx) so that will be the default for windows once VS14 is released.

###ProcessCPUClock
A clock which measures CPU time instead of wall clock time.  This means that time spent waiting on I/O, sleeping, etc. will not be counted.  This clock tends to be lower resolution then DefaultClock so measuring intervals less than several hundred nanoseconds will not work very well.  Be aware that when using this clock the warm up and measurement periods may take much more wall clock time then specified.

###Velox
The benchmark manager.  The clock to use for measurements is specified by the template parameter.  The constructor takes a reporter, which receives different callbacks throughout the benchmarking process, and an optional `VeloxConfig` if you want to override the default configuration.

- `bench`: Benchmarks a function.  By default the function will be called with no parameters.  However, if the function has any setup/teardown logic it can take a `velox::Stopwatch &` parameter and use it like this:
```cpp
void foo(velox::Stopwatch &sw) {
  // optional setup
  sw.measure([&] { 
    // code to benchmark
  });
  // optional teardown
}
```
The setup/teardown happens once per measurement, which will consist of multiple calls to the code passed to `sw.measure`. 
- `bench_with_arg`: Benchmarks a function with different arguments.  The function may optionally take a `velox::Stopwatch &` as a first parameter.  By default the benchmark name will look like this: `name / arg`.  If `arg` does not have an `operator<<` or you would like to use custom formatting logic you can call the overload which takes a custom formatter.  The formatter will be passed an `std::ostream&` and the argument.
```cpp
v.bench_with_arg("foo", [](int i) { /*code using i*/ }, {2, 4, 8});
```
VS2013 seems to have problems with the braces for the `std::initializer_list` so you may need to qualify the type by using "std::initializer_list<int>{2, 4, 8}`.
- `bench_with_args`: The same as `bench_with_arg` except multiple arguments can be passed in as tuples.

###optimization_barrier
With micro-benchmarks it's not uncommon for the optimizer to determine that some or all of the code being benchmarked is not being used and eliminate it.  To help prevent this velox provides the `velox::optimization_barrier` function which can be used to tell the compiler that a variable or return value is used in order to prevent it being removed by [DCE](http://en.wikipedia.org/wiki/Dead_code_elimination).  The implementation of this function for gcc and clang should have no overhead, while the implementation for MSVC has a small amount of overhead (a couple of mov's and a test).  I don't completely trust the MSVC implementation so please report any bugs you run into to.

Of course, whether or not you use `velox::optimization_barrier`, it's always a good idea to look at the generated assembly of your benchmarks to ensure they are testing what you expect.

###Reporter
All reporters are derived from this class.  The various functions are called under the following conditions:
- `suite_starting`: Called in the `Velox` constructor
- `estimate_clock_cost_starting`: If `Velox` is configured to estimate the clock cost this function will be called before the estimation begins.
- `estimate_clock_cost_ended`: Called when the clock cost estimation is complete.  The parameter is the estimated cost (currently the median of the measurements).
- `benchmark_starting`: Called before each benchmark starts.  This will be called for each individual argument to a function when `bench_with_arg(s)` is used.
- `warm_up_starting`: Called before the warm up period begins.  The parameter is how long the warm up will last.  The duration is tied to the clock being used so it may be wall clock time, or it may be something else.
- `warm_up_ended`: Called if the warm up completes successfully.  The parameter is the number of iterations and their duration which will be used when calculating the number of iterations each measurement will consist of.
- `warm_up_failed`: Called if the warm up failed.  The failure may be due to measuring an extremely quick function which overflows the 64-bit unsigned integer that holds the number of iterations or the measured duration being zero (likely due to a function taking a `velox::Stopwatch&` and not calling measure).  If the warm up failed no further reporter functions will be called for that particular benchmark.
- `measurement_collection_starting`: Called before the measurements are collected.  The first parameter is the number of measurements which will be taken and the second is the estimated time the collection will take.
- `measurement_collection_ended`: Called once all of the measurements have been collected.  The first parameter contains the number of iterations and duration of each measurement.  The second parameter contains the estimated times for a single call to the function being benchmarked.  The third parameter is the outlier classification of the single call times according to the following criteria: low severe(Q1 - 3 * IQR), low mild(Q1 - 1.5 * IQR), high mild(Q3 + 1.5 * IQR), or high severe(Q3 + 3 * IQR).
- `estimate_statistics_starting`: Called before running the [bootstrap](http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29) analysis of the collected measurements.  The parameter is the number of resamples to use when running the bootstrap.
- `estimate_statistics_ended`: Called once the bootstrap is complete.  The parameter contains the calculated [mean](http://en.wikipedia.org/wiki/Mean), [median](http://en.wikipedia.org/wiki/Median), [standard deviation](http://en.wikipedia.org/wiki/Standard_deviation),  [median absolute deviation](http://en.wikipedia.org/wiki/Median_absolute_deviation),  [linear least squares](http://en.wikipedia.org/wiki/Ordinary_least_squares), and [r^2](http://en.wikipedia.org/wiki/Coefficient_of_determination) along with their calculated [confidence intervals](http://en.wikipedia.org/wiki/Confidence_interval).
- `benchmark_ended`: Called when a benchmark is complete.
- `suite_ended`: Called in the `Velox` destructor.

###TextReporter
Outputs a textual summary of various benchmark data.

###HtmlReporter
Outputs an HTML report of the benchmarks with charts to easily visual the different runs.  The three charts are:

####Kernel Density Estimate
Shows the probability of a particular measurement occurring. The higher the probability density line the more likely a measurement is to occur. 
####Samples
Plots the per iteration time of each measurement (total time / number of iterations). If any of the values are classified as outliers according to the IQR criteria the relevant lines are shown as well. 
####Raw Measurements
The raw measurements(number of iterations and duration) which were collected when benchmarking a function. The regression line is created from the calculated LLS value. All points should be on or very near the regression line.

###MultiReporter
A helper class which can be constructed from multiple reporters which will forward calls to all of the contained reporters.  This is used because currently the `Velox` class supports a single reporter.

##License
velox is released under the [MIT](https://tldrlegal.com/license/mit-license) license.  The HtmlReporter uses the [jQuery](http://jquery.com/) and [HighCharts](http://www.highcharts.com/) libraries which are released under the [MIT](https://tldrlegal.com/license/mit-license) and [CC BY-NC 3.0](https://tldrlegal.com/license/creative-commons-attribution-noncommercial-%28cc-nc%29#summary) licenses respectively.
