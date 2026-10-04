#pragma once

#include <exception>
#include <mutex>
#include <thread>
#include <vector>

namespace sfm {

// fn(t) for t in [0, nt), each on its own thread (inline when nt <= 1). The
// first exception is rethrown once every thread has joined: one leaving a
// std::thread is std::terminate, and SfM runs inside the GUI process.
template <class F>
void runThreads(int nt, F&& fn) {
    if (nt <= 1) {
        fn(0);
        return;
    }
    std::mutex mu;
    std::exception_ptr first;
    auto record = [&] {
        std::lock_guard<std::mutex> lk(mu);
        if (!first) first = std::current_exception();
    };
    auto guarded = [&](int t) {
        try {
            fn(t);
        } catch (...) {
            record();
        }
    };
    std::vector<std::thread> pool;
    pool.reserve(nt);
    try {
        for (int t = 0; t < nt; t++) pool.emplace_back(guarded, t);
    } catch (...) {
        record();
    }
    for (std::thread& th : pool) th.join();
    if (first) std::rethrow_exception(first);
}

}  // namespace sfm
