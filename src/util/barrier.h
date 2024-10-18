#pragma once

namespace erpc {

#if defined(__x86_64__) || defined(__i386__)

// ------------------------
// x86/x86_64 Implementations
// ------------------------

static inline void memory_barrier() { 
    asm volatile("" ::: "memory"); 
}

static inline void lfence() { 
    asm volatile("lfence" ::: "memory"); 
}

static inline void sfence() { 
    asm volatile("sfence" ::: "memory"); 
}

static inline void mfence() { 
    asm volatile("mfence" ::: "memory"); 
}

static inline void pause_() { 
    asm volatile("pause"); 
}

static inline void clflush(volatile void* p) { 
    asm volatile("clflush (%0)" :: "r"(p)); 
}

static inline void cpuid(unsigned int* eax, unsigned int* ebx, unsigned int* ecx,
                        unsigned int* edx) {
    asm volatile("cpuid"
                 : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
                 : "0"(*eax), "2"(*ecx));
}

#elif defined(__aarch64__)

// ------------------------
// AArch64 Implementations
// ------------------------

#include <atomic>

// Memory barrier using C++11 atomic_thread_fence for portability
static inline void memory_barrier() { 
    std::atomic_thread_fence(std::memory_order_seq_cst); 
}

// Data Memory Barrier (equivalent to lfence/sfence/mfence)
static inline void dmb_sy() { 
    asm volatile("dmb ish" ::: "memory"); 
}

// Instruction Synchronization Barrier
static inline void isb() { 
    asm volatile("isb" ::: "memory"); 
}

// AArch64 does not have a direct equivalent to 'pause'. 
// 'yield' can be used to hint to the processor that it's in a spin-wait loop.
static inline void pause_() { 
    asm volatile("yield"); 
}

// Cache Line Flush in AArch64
static inline void clflush(volatile void* p) { 
    // Cast away volatile safely using const_cast and reinterpret_cast
    __builtin___clear_cache(
        reinterpret_cast<char*>(const_cast<void*>(static_cast<const volatile void*>(p))),
        reinterpret_cast<char*>(const_cast<void*>(static_cast<const volatile void*>(p))) + 64
    );
}

// cpuid is not available on AArch64. If you need CPU feature detection,
// you'll have to use AArch64-specific methods or libraries.
// For now, we'll provide a stub that sets all outputs to zero.
static inline void cpuid(unsigned int* eax, unsigned int* ebx, unsigned int* ecx,
                        unsigned int* edx) {
    *eax = *ebx = *ecx = *edx = 0;
}

#else
#error "Unsupported architecture. Please define implementations for your architecture."
#endif

}  // namespace erpc
