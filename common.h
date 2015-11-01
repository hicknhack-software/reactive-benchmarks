#include "benchmark/benchmark.h"

#define ITERATION_COUNT 10000000

#if defined(COMPLEX)
#include <complex>
#define ELEMENT_TYPE std::complex<double>
#define ADD_VALUE(X) X + 1.
#define FILTER_VALUE(X) static_cast<int>(X.real()) % 2 == 0
/*
 * MinGW 4.9.2
Run on (1 X 2594.28 MHz CPU )
10/25/15 02:09:19
Benchmark    Time(ns)    CPU(ns) Iterations
-------------------------------------------
PlainLoop    37795251   36458333         18
RxCppTest1  110626395  111979167          6
VectorLoop   82908594   82812500         10
ListLoop    113992474  113636364         11
 */
#elif defined(DOUBLE)
#define ELEMENT_TYPE double
#define ADD_VALUE(X) X + 1.
#define FILTER_VALUE(X) static_cast<int>(X) % 2 == 0
/*
 * MinGW 4.9.2
Run on (1 X 2594.47 MHz CPU )
10/25/15 02:07:50
Benchmark    Time(ns)    CPU(ns) Iterations
-------------------------------------------
PlainLoop    37987149   38651316         19
RxCppTest1  110982630  111979167          6
VectorLoop   81959414   82812500         10
ListLoop    105696668  105113636         11
 */
#else
#define ELEMENT_TYPE size_t
#define ADD_VALUE(X) X + 1
#define FILTER_VALUE(X) X % 2 == 0
/*
 * MinGW 4.9.2
Run on (1 X 2594.15 MHz CPU )
10/27/15 10:20:21
Benchmark     Time(ns)    CPU(ns) Iterations
--------------------------------------------
PlainLoop      5906469    5902778         90
LambdaLoop     5898085    5859375        112
LambdaRange    5740264    5719866        112
RxCppTest1    93106621   93750000          7

 * MSVC2015 PGO
Run on (1 X 2595.62 MHz CPU )
10/27/15 10:18:38
Benchmark       Time(ns)    CPU(ns) Iterations
----------------------------------------------
PlainLoop        4232888    4147377        162
LambdaLoop       4259974    4235693        166
LambdaRange      4268113    4235693        166
CoroutineLoop  117110292  117187500          6
RxCppTest1     356721163  359375000          2
*/
#endif

template< typename T>
void doNotOptimize(T k) {
#if defined(_MSC_VER) && !defined(__clang__)
    //static volatile T m; m = k;
    //__asm { xor eax, k }
    __asm { nop }; (void)k;
#else
    benchmark::DoNotOptimize(k);
#endif
}
