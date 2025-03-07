/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-dsp-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PRIVATE_DSP_ARCH_X86_CPUID_H_
#define PRIVATE_DSP_ARCH_X86_CPUID_H_

#ifndef PRIVATE_DSP_ARCH_X86_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_IMPL */

//-------------------------------------------------------------------------
// CPUID Function 0
#define X86_CPUID0_INTEL_EBX                    0x756e6547
#define X86_CPUID0_INTEL_ECX                    0x6c65746e
#define X86_CPUID0_INTEL_EDX                    0x49656e69

#define X86_CPUID0_AMD_EBX                      0x68747541
#define X86_CPUID0_AMD_ECX                      0x444d4163
#define X86_CPUID0_AMD_EDX                      0x69746e65

//-------------------------------------------------------------------------
// Function 1
#define X86_CPUID1_INTEL_ECX_SSE3               (1 << 0)
#define X86_CPUID1_INTEL_ECX_SSSE3              (1 << 9)
#define X86_CPUID1_INTEL_ECX_FMA3               (1 << 12)
#define X86_CPUID1_INTEL_ECX_SSE4_1             (1 << 19)
#define X86_CPUID1_INTEL_ECX_SSE4_2             (1 << 20)
#define X86_CPUID1_INTEL_ECX_XSAVE              (1 << 26)
#define X86_CPUID1_INTEL_ECX_OSXSAVE            (1 << 27)
#define X86_CPUID1_INTEL_ECX_AVX                (1 << 28)

#define X86_CPUID1_INTEL_EDX_FPU                (1 << 0)
#define X86_CPUID1_INTEL_EDX_CMOV               (1 << 15)
#define X86_CPUID1_INTEL_EDX_MMX                (1 << 23)
#define X86_CPUID1_INTEL_EDX_SSE                (1 << 25)
#define X86_CPUID1_INTEL_EDX_SSE2               (1 << 26)

#define X86_CPUID1_AMD_ECX_SSE3                 (1 << 0)
#define X86_CPUID1_AMD_ECX_SSSE3                (1 << 9)
#define X86_CPUID1_AMD_ECX_FMA3                 (1 << 12)
#define X86_CPUID1_AMD_ECX_SSE4_1               (1 << 19)
#define X86_CPUID1_AMD_ECX_SSE4_2               (1 << 20)
#define X86_CPUID1_AMD_ECX_XSAVE                (1 << 26)
#define X86_CPUID1_AMD_ECX_OSXSAVE              (1 << 27)
#define X86_CPUID1_AMD_ECX_AVX                  (1 << 28)

#define X86_CPUID1_AMD_EDX_FPU                  (1 << 0)
#define X86_CPUID1_AMD_EDX_CMOV                 (1 << 15)
#define X86_CPUID1_AMD_EDX_MMX                  (1 << 23)
#define X86_CPUID1_AMD_EDX_SSE                  (1 << 25)
#define X86_CPUID1_AMD_EDX_SSE2                 (1 << 26)

//-------------------------------------------------------------------------
// Function 7
#define X86_CPUID7_INTEL_EBX_AVX2               (1 << 5)
#define X86_CPUID7_INTEL_EBX_AVX512F            (1 << 16)
#define X86_CPUID7_INTEL_EBX_AVX512DQ           (1 << 17)
#define X86_CPUID7_INTEL_EBX_AVX512IFMA         (1 << 21)
#define X86_CPUID7_INTEL_EBX_AVX512PF           (1 << 26)
#define X86_CPUID7_INTEL_EBX_AVX512ER           (1 << 27)
#define X86_CPUID7_INTEL_EBX_AVX512CD           (1 << 28)
#define X86_CPUID7_INTEL_EBX_AVX512BW           (1 << 30)
#define X86_CPUID7_INTEL_EBX_AVX512VL           (1 << 31)

#define X86_CPUID7_INTEL_ECX_AVX512VBMI         (1 << 1)

#define X86_CPUID7_AMD_EBX_AVX2                 (1 << 5)
#define X86_CPUID7_AMD_EBX_AVX512F              (1 << 16)
#define X86_CPUID7_AMD_EBX_AVX512DQ             (1 << 17)
#define X86_CPUID7_AMD_EBX_AVX512IFMA           (1 << 21)
#define X86_CPUID7_AMD_EBX_AVX512PF             (1 << 26)
#define X86_CPUID7_AMD_EBX_AVX512ER             (1 << 27)
#define X86_CPUID7_AMD_EBX_AVX512CD             (1 << 28)
#define X86_CPUID7_AMD_EBX_AVX512BW             (1 << 30)
#define X86_CPUID7_AMD_EBX_AVX512VL             (1 << 31)

#define X86_CPUID7_AMD_ECX_AVX512VBMI           (1 << 1)

//-------------------------------------------------------------------------
// Function 80000001
#define X86_XCPUID1_AMD_ECX_FMA4                (1 << 16)
#define X86_XCPUID1_AMD_ECX_SSE4A               (1 << 6)

#define X86_XCPUID1_AMD_EDX_FPU                 (1 << 0)
#define X86_XCPUID1_AMD_EDX_CMOV                (1 << 15)
#define X86_XCPUID1_AMD_EDX_MMX                 (1 << 23)

//-------------------------------------------------------------------------
// Processor families
#define X86_FAMILY_PENTIUM                      5
#define X86_FAMILY_PENTIUM_PRO                  6
#define X86_FAMILY_PENTIUM_PRO_PLUS             15

//-------------------------------------------------------------------------
// XCR0 register flags
#define XCR0_FLAG_FPU                           (1 << 0)
#define XCR0_FLAG_SSE                           (1 << 1)
#define XCR0_FLAG_AVX                           (1 << 2)
#define XCR0_FLAG_BNDREG                        (1 << 3)
#define XCR0_FLAG_BNDCSR                        (1 << 4)
#define XCR0_FLAG_OPMASK                        (1 << 5)
#define XCR0_FLAG_ZMM_HI256                     (1 << 6)
#define XCR0_FLAG_HI16_ZMM                      (1 << 7)
#define XCR0_FLAG_PKRU                          (1 << 8)

#define XCR_FLAGS_AVX                           (XCR0_FLAG_SSE | XCR0_FLAG_AVX)
#define XCR_FLAGS_AVX512                        (XCR_FLAGS_AVX | XCR0_FLAG_OPMASK | XCR0_FLAG_ZMM_HI256 | XCR0_FLAG_HI16_ZMM)

//-------------------------------------------------------------------------
// Different processor families

#define INTEL_FAMILY_686_CORE                   0x06

#define AMD_FAMILY_K8_HAMMER                    0x0f
#define AMD_FAMILY_K10                          0x10
#define AMD_FAMILY_BOBCAT                       0x14
#define AMD_FAMILY_BULLDOZER                    0x15
#define AMD_FAMILY_JAGUAR                       0x16
#define AMD_FAMILY_ZEN_1_2                      0x17
#define AMD_FAMILY_DHYANA                       0x18
#define AMD_FAMILY_ZEN_3_4                      0x19
#define AMD_FAMILY_ZEN_5                        0x1a

#define AMD_MODEL_ZEN_2                         0x31

namespace lsp
{
    namespace x86
    {
    #pragma pack(push, 1)
        typedef struct cpuid_info_t
        {
            uint32_t        eax;
            uint32_t        ebx;
            uint32_t        ecx;
            uint32_t        edx;
        } cpuid_info_t;
        #pragma pack(pop)

        #if defined(ARCH_I386)
            inline bool cpuid_supported()
            {
                bool result;
                ARCH_X86_ASM
                (
                    __ASM_EMIT("pushfl")
                    __ASM_EMIT("pop         %%eax")
                    __ASM_EMIT("mov         %%eax, %%edx")
                    __ASM_EMIT("xor         $0x200000, %%eax")
                    __ASM_EMIT("push        %%eax")
                    __ASM_EMIT("popfl")
                    __ASM_EMIT("pushfl")
                    __ASM_EMIT("pop         %%eax")
                    __ASM_EMIT("xor         %%edx, %%eax")
                    __ASM_EMIT("shr         $21, %%eax")
                    __ASM_EMIT("and         $1, %%eax")
                    : "=a"(result)
                    : : "cc", "%edx"
                );

                return result;
            }
        #elif defined(ARCH_X86_64)
            inline bool cpuid_supported()
            {
                return true;
            }
        #endif /* __i386__ */

        inline bool cpuid(cpuid_info_t *info, uint32_t leaf, uint32_t subleaf)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT32("push      %%ebx")
                __ASM_EMIT("cpuid")
                __ASM_EMIT("mov         %%eax, 0x0(%[info])")
                __ASM_EMIT("mov         %%ebx, 0x4(%[info])")
                __ASM_EMIT("mov         %%ecx, 0x8(%[info])")
                __ASM_EMIT("mov         %%edx, 0xc(%[info])")
                __ASM_EMIT32("pop       %%ebx")

                : "+a"(leaf), "+c"(subleaf)
                : [info] "D" (info)
                : "cc", "memory",
                  __IF_64("%ebx", ) "%edx"
            );

            return true;
        }

        uint64_t read_xcr(umword_t xcr_id);

    } /* namespace x86 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_CPUID_H_ */
