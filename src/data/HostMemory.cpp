#include "data/HostMemory.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <cstdio>
#include <unistd.h>
#endif

namespace spirula {

uint64_t available_ram_bytes() {
#if defined(_WIN32)
    MEMORYSTATUSEX st{};
    st.dwLength = sizeof(st);
    if (GlobalMemoryStatusEx(&st)) return st.ullAvailPhys;
#elif defined(__linux__)
    // MemAvailable counts reclaimable page cache; _SC_AVPHYS_PAGES does not,
    // and reads near zero on a machine that has just read a large dataset.
    if (FILE* f = std::fopen("/proc/meminfo", "r")) {
        char line[256];
        unsigned long long kib = 0;
        while (std::fgets(line, sizeof line, f))
            if (std::sscanf(line, "MemAvailable: %llu kB", &kib) == 1) break;
        std::fclose(f);
        if (kib) return (uint64_t)kib * 1024;
    }
    const long pages = sysconf(_SC_AVPHYS_PAGES), page = sysconf(_SC_PAGESIZE);
    if (pages > 0 && page > 0) return (uint64_t)pages * (uint64_t)page;
#elif defined(_SC_AVPHYS_PAGES) && defined(_SC_PAGESIZE)
    const long pages = sysconf(_SC_AVPHYS_PAGES), page = sysconf(_SC_PAGESIZE);
    if (pages > 0 && page > 0) return (uint64_t)pages * (uint64_t)page;
#endif
    return 0;
}

}  // namespace spirula
