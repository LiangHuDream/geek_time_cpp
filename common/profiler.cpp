#include "profiler.h"
#include <iostream>  // std::cout
#include <vector>    // std::vector
#include <assert.h>  // assert
#include <stddef.h>  // size_t
#include <stdint.h>  // uint64_t
#include "rdtsc.h"   // rdtsc

namespace {

struct profiling_data {
    int number{-1};
    int call_count{};
    uint64_t call_duration{};
};

class profiler {
public:
    profiler();
    ~profiler();

#if __cplusplus >= 201103L
    profiler(const profiler&) = delete;
    profiler& operator=(const profiler&) = delete;
#endif

    void add_data(int number, uint64_t duration);

private:
    std::vector<profiling_data> data_;
};

profiler::profiler()
{
    size_t len = 0;
    for (;;) {
        if (!name_map[len].name) {
            break;
        }
        ++len;
    }
    data_.resize(len);
    int i = 0;
    for (auto& item : data_) {
        assert(i == name_map[i].number);
        item.number = i;
        ++i;
    }
}

profiler::~profiler()
{
#ifndef NDEBUG
    for (auto& item : data_) {
        if (item.call_count == 0) {
            continue;
        }
        std::cout << item.number << " " << name_map[item.number].name
                  << ":\n";
        std::cout << "  Call count: " << item.call_count << '\n';
        std::cout << "  Call duration: " << item.call_duration << '\n';
        std::cout << "  Average duration: "
                  << static_cast<double>(item.call_duration) /
                         (item.call_count != 0 ? item.call_count : 1)
                  << '\n';
    }
#endif
}

void profiler::add_data(int number, uint64_t duration)
{
    assert(number >= 0 && number < static_cast<int>(data_.size()));
    data_[number].call_count++;
    data_[number].call_duration += duration;
}

profiler profiler_instance;

}  // unnamed namespace

profiling_checker::~profiling_checker()
{
    auto end_time = rdtsc();
    profiler_instance.add_data(number_, end_time - start_time_);
}
