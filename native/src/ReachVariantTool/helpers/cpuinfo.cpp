#include "cpuinfo.h"
#include <cstdint>
#ifdef _MSC_VER
	#include <intrin.h>
#endif

// Polyfills for MSVC intrinsics (CPUID, etc.):
#ifdef __GNUC__
namespace {
	void __cpuid(int* cpuinfo, int info) {
		__asm__ __volatile__(
R"--(
xchg %%ebx, %%edi;
cpuid;
xchg %%ebx, %%edi;
)--"
			: "=a" (cpuinfo[0]), "=D" (cpuinfo[1]), "=c" (cpuinfo[2]), "=d" (cpuinfo[3])
			:  "0" (info)
		);
	}
	uint64_t _xgetbv(unsigned int index) {
		unsigned int eax, edx;
		__asm__ __volatile__(
			"xgetbv;"
			: "=a" (eax), "=d"(edx)
			: "c" (index)
		);
		return ((uint64_t)edx << 32) | eax;
	}
}
#endif

namespace cobb {
	cpuinfo cpuinfo::instance = cpuinfo();

	cpuinfo::cpuinfo() {
		int info[4];
		//
		// Check SSE - SSE4.2 and AVX:
		//
		__cpuid(info, 1);
		this->extension_support.sse_1   = info[3] & (1 << 25);
		this->extension_support.sse_2   = info[3] & (1 << 26);
		this->extension_support.sse_3   = info[2] & 1;
		this->extension_support.ssse_3  = info[2] & (1 <<  9);
		this->extension_support.sse_4_1 = info[2] & (1 << 19);
		this->extension_support.sse_4_2 = info[2] & (1 << 20);
		this->extension_support.avx     = info[2] & (1 << 28);
		if (this->extension_support.avx) {
			bool osx = info[2] & (1 << 27);
			if (osx) {
				auto features = _xgetbv(0);
				this->extension_support.avx = (features & 0x6) == 0x6; // even with CPU support, AVX doesn't actually work unless the registers are saved by the OS on context switch
			}
		}
		//
		// Check SSE4a and SSE5:
		//
		__cpuid(info, 0x80000000);
		int extended_id_count = info[0];
		if (extended_id_count > 0x80000000) {
			__cpuid(info, 0x80000001);
			this->extension_support.sse_4a = info[2] & (1 <<  6);
			this->extension_support.sse_5  = info[2] & (1 << 11);
		}
	}
}