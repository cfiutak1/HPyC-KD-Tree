#include <chrono>


#define                                                         \
TIMING_INIT_VARS()                                              \
    std::chrono::_V2::steady_clock::time_point timing_start;    \
    std::chrono::_V2::steady_clock::time_point timing_end;      \
    std::chrono::duration<double> timing_diff;


#define                                                         \
TIMING_START()                                                  \
    timing_start = std::chrono::steady_clock::now();


#define                                                         \
TIMING_END()                                                    \
    timing_end = std::chrono::steady_clock::now();

#define                                                         \
TIMING_CALCULATE_DIFF()                                         \
    timing_diff = timing_end - timing_start;                    \
    float timing_recorded = timing_diff.count();
