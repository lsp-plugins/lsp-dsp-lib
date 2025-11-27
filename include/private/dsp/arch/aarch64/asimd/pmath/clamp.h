/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 нояб. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_CLAMP_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_CLAMP_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <private/dsp/arch/aarch64/asimd/pmath/sin.h>

namespace lsp
{
    namespace asimd
    {
        void clamp_vv1(float *dst, const float *min, const float *max, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp         q8, q9, [%[min], #0x00]")
                __ASM_EMIT("ldp         q10, q11, [%[min], #0x20]")
                __ASM_EMIT("ldp         q12, q13, [%[min], #0x40]")
                __ASM_EMIT("ldp         q14, q15, [%[min], #0x60]")
                __ASM_EMIT("ldp         q16, q17, [%[max], #0x00]")
                __ASM_EMIT("ldp         q18, q19, [%[max], #0x20]")
                __ASM_EMIT("ldp         q20, q21, [%[max], #0x40]")
                __ASM_EMIT("ldp         q22, q23, [%[max], #0x60]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmax        v2.4s, v2.4s, v10.4s")
                __ASM_EMIT("fmax        v3.4s, v3.4s, v11.4s")
                __ASM_EMIT("fmax        v4.4s, v4.4s, v12.4s")
                __ASM_EMIT("fmax        v5.4s, v5.4s, v13.4s")
                __ASM_EMIT("fmax        v6.4s, v6.4s, v14.4s")
                __ASM_EMIT("fmax        v7.4s, v7.4s, v15.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v17.4s")
                __ASM_EMIT("fmin        v2.4s, v2.4s, v18.4s")
                __ASM_EMIT("fmin        v3.4s, v3.4s, v19.4s")
                __ASM_EMIT("fmin        v4.4s, v4.4s, v20.4s")
                __ASM_EMIT("fmin        v5.4s, v5.4s, v21.4s")
                __ASM_EMIT("fmin        v6.4s, v6.4s, v22.4s")
                __ASM_EMIT("fmin        v7.4s, v7.4s, v23.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[min], %[min], #0x80")
                __ASM_EMIT("add         %[max], %[max], #0x80")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp         q8, q9, [%[min], #0x00]")
                __ASM_EMIT("ldp         q10, q11, [%[min], #0x20]")
                __ASM_EMIT("ldp         q16, q17, [%[max], #0x00]")
                __ASM_EMIT("ldp         q18, q19, [%[max], #0x20]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmax        v2.4s, v2.4s, v10.4s")
                __ASM_EMIT("fmax        v3.4s, v3.4s, v11.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v17.4s")
                __ASM_EMIT("fmin        v2.4s, v2.4s, v18.4s")
                __ASM_EMIT("fmin        v3.4s, v3.4s, v19.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[min], %[min], #0x40")
                __ASM_EMIT("add         %[max], %[max], #0x40")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q8, q9, [%[min], #0x00]")
                __ASM_EMIT("ldp         q16, q17, [%[max], #0x00]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v17.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[min], %[min], #0x20")
                __ASM_EMIT("add         %[max], %[max], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[dst], #0x00]")       // v0   = x
                __ASM_EMIT("ldr         q8, [%[min], #0x00]")
                __ASM_EMIT("ldr         q16, [%[max], #0x00]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[min], %[min], #0x10")
                __ASM_EMIT("add         %[max], %[max], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[dst]]")         // v0   = x
                __ASM_EMIT("ld1r        {v8.4s}, [%[min]]")
                __ASM_EMIT("ld1r        {v16.4s}, [%[max]]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[min], %[min], #0x04")
                __ASM_EMIT("add         %[max], %[max], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [min] "+r" (min), [max] "+r" (max),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23"
            );
        }

        void clamp_vv2(float *dst, const float *src, const float *min, const float *max, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[src], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[src], #0x60]")
                __ASM_EMIT("ldp         q8, q9, [%[min], #0x00]")
                __ASM_EMIT("ldp         q10, q11, [%[min], #0x20]")
                __ASM_EMIT("ldp         q12, q13, [%[min], #0x40]")
                __ASM_EMIT("ldp         q14, q15, [%[min], #0x60]")
                __ASM_EMIT("ldp         q16, q17, [%[max], #0x00]")
                __ASM_EMIT("ldp         q18, q19, [%[max], #0x20]")
                __ASM_EMIT("ldp         q20, q21, [%[max], #0x40]")
                __ASM_EMIT("ldp         q22, q23, [%[max], #0x60]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmax        v2.4s, v2.4s, v10.4s")
                __ASM_EMIT("fmax        v3.4s, v3.4s, v11.4s")
                __ASM_EMIT("fmax        v4.4s, v4.4s, v12.4s")
                __ASM_EMIT("fmax        v5.4s, v5.4s, v13.4s")
                __ASM_EMIT("fmax        v6.4s, v6.4s, v14.4s")
                __ASM_EMIT("fmax        v7.4s, v7.4s, v15.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v17.4s")
                __ASM_EMIT("fmin        v2.4s, v2.4s, v18.4s")
                __ASM_EMIT("fmin        v3.4s, v3.4s, v19.4s")
                __ASM_EMIT("fmin        v4.4s, v4.4s, v20.4s")
                __ASM_EMIT("fmin        v5.4s, v5.4s, v21.4s")
                __ASM_EMIT("fmin        v6.4s, v6.4s, v22.4s")
                __ASM_EMIT("fmin        v7.4s, v7.4s, v23.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[src], %[src], #0x80")
                __ASM_EMIT("add         %[min], %[min], #0x80")
                __ASM_EMIT("add         %[max], %[max], #0x80")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
                __ASM_EMIT("ldp         q8, q9, [%[min], #0x00]")
                __ASM_EMIT("ldp         q10, q11, [%[min], #0x20]")
                __ASM_EMIT("ldp         q16, q17, [%[max], #0x00]")
                __ASM_EMIT("ldp         q18, q19, [%[max], #0x20]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmax        v2.4s, v2.4s, v10.4s")
                __ASM_EMIT("fmax        v3.4s, v3.4s, v11.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v17.4s")
                __ASM_EMIT("fmin        v2.4s, v2.4s, v18.4s")
                __ASM_EMIT("fmin        v3.4s, v3.4s, v19.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[src], %[src], #0x40")
                __ASM_EMIT("add         %[min], %[min], #0x40")
                __ASM_EMIT("add         %[max], %[max], #0x40")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q8, q9, [%[min], #0x00]")
                __ASM_EMIT("ldp         q16, q17, [%[max], #0x00]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v17.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[src], %[src], #0x20")
                __ASM_EMIT("add         %[min], %[min], #0x20")
                __ASM_EMIT("add         %[max], %[max], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[src], #0x00]")       // v0   = x
                __ASM_EMIT("ldr         q8, [%[min], #0x00]")
                __ASM_EMIT("ldr         q16, [%[max], #0x00]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[src], %[src], #0x10")
                __ASM_EMIT("add         %[min], %[min], #0x10")
                __ASM_EMIT("add         %[max], %[max], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[src]]")         // v0   = x
                __ASM_EMIT("ld1r        {v8.4s}, [%[min]]")
                __ASM_EMIT("ld1r        {v16.4s}, [%[max]]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s")      // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[src], %[src], #0x04")
                __ASM_EMIT("add         %[min], %[min], #0x04")
                __ASM_EMIT("add         %[max], %[max], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [min] "+r" (min), [max] "+r" (max),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23"
            );
        }

        void clamp_kk1(float *dst, float min, float max, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v8.4s}, [%[min]]")
                __ASM_EMIT("ld1r        {v9.4s}, [%[max]]")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v8.4s")
                __ASM_EMIT("fmax        v2.4s, v2.4s, v8.4s")
                __ASM_EMIT("fmax        v3.4s, v3.4s, v8.4s")
                __ASM_EMIT("fmax        v4.4s, v4.4s, v8.4s")
                __ASM_EMIT("fmax        v5.4s, v5.4s, v8.4s")
                __ASM_EMIT("fmax        v6.4s, v6.4s, v8.4s")
                __ASM_EMIT("fmax        v7.4s, v7.4s, v8.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmin        v2.4s, v2.4s, v9.4s")
                __ASM_EMIT("fmin        v3.4s, v3.4s, v9.4s")
                __ASM_EMIT("fmin        v4.4s, v4.4s, v9.4s")
                __ASM_EMIT("fmin        v5.4s, v5.4s, v9.4s")
                __ASM_EMIT("fmin        v6.4s, v6.4s, v9.4s")
                __ASM_EMIT("fmin        v7.4s, v7.4s, v9.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs        1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v8.4s")
                __ASM_EMIT("fmax        v2.4s, v2.4s, v8.4s")
                __ASM_EMIT("fmax        v3.4s, v3.4s, v8.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmin        v2.4s, v2.4s, v9.4s")
                __ASM_EMIT("fmin        v3.4s, v3.4s, v9.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = x
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v8.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[dst], #0x00]")       // v0   = x
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[dst]]")         // v0   = x
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst),
                  [count] "+r" (count)
                : [min] "r" (&min), [max] "r" (&max)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9"
            );
        }

        void clamp_kk2(float *dst, const float *src, float min, float max, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v8.4s}, [%[min]]")
                __ASM_EMIT("ld1r        {v9.4s}, [%[max]]")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[src], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[src], #0x60]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v8.4s")
                __ASM_EMIT("fmax        v2.4s, v2.4s, v8.4s")
                __ASM_EMIT("fmax        v3.4s, v3.4s, v8.4s")
                __ASM_EMIT("fmax        v4.4s, v4.4s, v8.4s")
                __ASM_EMIT("fmax        v5.4s, v5.4s, v8.4s")
                __ASM_EMIT("fmax        v6.4s, v6.4s, v8.4s")
                __ASM_EMIT("fmax        v7.4s, v7.4s, v8.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmin        v2.4s, v2.4s, v9.4s")
                __ASM_EMIT("fmin        v3.4s, v3.4s, v9.4s")
                __ASM_EMIT("fmin        v4.4s, v4.4s, v9.4s")
                __ASM_EMIT("fmin        v5.4s, v5.4s, v9.4s")
                __ASM_EMIT("fmin        v6.4s, v6.4s, v9.4s")
                __ASM_EMIT("fmin        v7.4s, v7.4s, v9.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[src], %[src], #0x80")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")   // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v8.4s")
                __ASM_EMIT("fmax        v2.4s, v2.4s, v8.4s")
                __ASM_EMIT("fmax        v3.4s, v3.4s, v8.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fmin        v2.4s, v2.4s, v9.4s")
                __ASM_EMIT("fmin        v3.4s, v3.4s, v9.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[src], %[src], #0x40")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")   // v0   = x
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmax        v1.4s, v1.4s, v8.4s")
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("fmin        v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[src], %[src], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[src], #0x00]")       // v0   = x
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[src], %[src], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[src]]")         // v0   = x
                __ASM_EMIT("fmax        v0.4s, v0.4s, v8.4s")       // v0   = max(x, min)
                __ASM_EMIT("fmin        v0.4s, v0.4s, v9.4s")       // v0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[src], %[src], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [min] "r" (&min), [max] "r" (&max)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9"
            );
        }
    } /* namespace asimd */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_CLAMP_H_ */
