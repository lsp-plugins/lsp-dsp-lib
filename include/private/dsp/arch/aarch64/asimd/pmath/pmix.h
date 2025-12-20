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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_PMIX_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_PMIX_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <private/dsp/arch/aarch64/asimd/pmath/sin.h>

namespace lsp
{
    namespace asimd
    {
        void pmix_v1(float *dst, const float *src, const float *k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = d
                __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp         q16, q17, [%[src], #0x00]") // v16  = s
                __ASM_EMIT("ldp         q18, q19, [%[src], #0x20]")
                __ASM_EMIT("ldp         q20, q21, [%[src], #0x40]")
                __ASM_EMIT("ldp         q22, q23, [%[src], #0x60]")
                __ASM_EMIT("ldp         q24, q25, [%[k], #0x00]")   // v24  = k
                __ASM_EMIT("ldp         q26, q27, [%[k], #0x20]")
                __ASM_EMIT("ldp         q28, q29, [%[k], #0x40]")
                __ASM_EMIT("ldp         q30, q31, [%[k], #0x60]")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fsub        v20.4s, v20.4s, v4.4s")
                __ASM_EMIT("fsub        v21.4s, v21.4s, v5.4s")
                __ASM_EMIT("fsub        v22.4s, v22.4s, v6.4s")
                __ASM_EMIT("fsub        v23.4s, v23.4s, v7.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v31.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[src], %[src], #0x80")
                __ASM_EMIT("add         %[k], %[k], #0x80")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = d
                __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp         q16, q17, [%[src], #0x00]") // v16  = s
                __ASM_EMIT("ldp         q18, q19, [%[src], #0x20]")
                __ASM_EMIT("ldp         q24, q25, [%[k], #0x00]")   // v24  = k
                __ASM_EMIT("ldp         q26, q27, [%[k], #0x20]")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[src], %[src], #0x40")
                __ASM_EMIT("add         %[k], %[k], #0x40")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = d
                __ASM_EMIT("ldp         q16, q17, [%[src], #0x00]") // v16  = s
                __ASM_EMIT("ldp         q24, q25, [%[k], #0x00]")   // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[src], %[src], #0x20")
                __ASM_EMIT("add         %[k], %[k], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[dst], #0x00]")       // v0   = d
                __ASM_EMIT("ldr         q16, [%[src], #0x00]")      // v16  = s
                __ASM_EMIT("ldr         q24, [%[k], #0x00]")        // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[src], %[src], #0x10")
                __ASM_EMIT("add         %[k], %[k], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[dst]]")         // v0   = d
                __ASM_EMIT("ld1r        {v16.4s}, [%[src]]")        // v16  = s
                __ASM_EMIT("ld1r        {v24.4s}, [%[k]]")          // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[src], %[src], #0x04")
                __ASM_EMIT("add         %[k], %[k], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [k] "+r" (k),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[src1], #0x00]")  // v0   = d
                __ASM_EMIT("ldp         q2, q3, [%[src1], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[src1], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[src1], #0x60]")
                __ASM_EMIT("ldp         q16, q17, [%[src2], #0x00]")// v16  = s
                __ASM_EMIT("ldp         q18, q19, [%[src2], #0x20]")
                __ASM_EMIT("ldp         q20, q21, [%[src2], #0x40]")
                __ASM_EMIT("ldp         q22, q23, [%[src2], #0x60]")
                __ASM_EMIT("ldp         q24, q25, [%[k], #0x00]")   // v24  = k
                __ASM_EMIT("ldp         q26, q27, [%[k], #0x20]")
                __ASM_EMIT("ldp         q28, q29, [%[k], #0x40]")
                __ASM_EMIT("ldp         q30, q31, [%[k], #0x60]")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fsub        v20.4s, v20.4s, v4.4s")
                __ASM_EMIT("fsub        v21.4s, v21.4s, v5.4s")
                __ASM_EMIT("fsub        v22.4s, v22.4s, v6.4s")
                __ASM_EMIT("fsub        v23.4s, v23.4s, v7.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v31.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[src1], %[src1], #0x80")
                __ASM_EMIT("add         %[src2], %[src2], #0x80")
                __ASM_EMIT("add         %[k], %[k], #0x80")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[src1], #0x00]")  // v0   = d
                __ASM_EMIT("ldp         q2, q3, [%[src1], #0x20]")
                __ASM_EMIT("ldp         q16, q17, [%[src2], #0x00]")// v16  = s
                __ASM_EMIT("ldp         q18, q19, [%[src2], #0x20]")
                __ASM_EMIT("ldp         q24, q25, [%[k], #0x00]")   // v24  = k
                __ASM_EMIT("ldp         q26, q27, [%[k], #0x20]")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[src1], %[src1], #0x40")
                __ASM_EMIT("add         %[src2], %[src2], #0x40")
                __ASM_EMIT("add         %[k], %[k], #0x40")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[src1], #0x00]")  // v0   = d
                __ASM_EMIT("ldp         q16, q17, [%[src2], #0x00]")// v16  = s
                __ASM_EMIT("ldp         q24, q25, [%[k], #0x00]")   // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[src1], %[src1], #0x20")
                __ASM_EMIT("add         %[src2], %[src2], #0x20")
                __ASM_EMIT("add         %[k], %[k], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[src1], #0x00]")      // v0   = d
                __ASM_EMIT("ldr         q16, [%[src2], #0x00]")     // v16  = s
                __ASM_EMIT("ldr         q24, [%[k], #0x00]")        // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[src1], %[src1], #0x10")
                __ASM_EMIT("add         %[src2], %[src2], #0x10")
                __ASM_EMIT("add         %[k], %[k], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[src1]]")        // v0   = d
                __ASM_EMIT("ld1r        {v16.4s}, [%[src2]]")       // v16  = s
                __ASM_EMIT("ld1r        {v24.4s}, [%[k]]")          // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[src1], %[src1], #0x04")
                __ASM_EMIT("add         %[src2], %[src2], #0x04")
                __ASM_EMIT("add         %[k], %[k], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [k] "+r" (k),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pmix_k1(float *dst, const float *src, float k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v24.4s}, [%[k]]")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = d
                __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp         q16, q17, [%[src], #0x00]") // v16  = s
                __ASM_EMIT("ldp         q18, q19, [%[src], #0x20]")
                __ASM_EMIT("ldp         q20, q21, [%[src], #0x40]")
                __ASM_EMIT("ldp         q22, q23, [%[src], #0x60]")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fsub        v20.4s, v20.4s, v4.4s")
                __ASM_EMIT("fsub        v21.4s, v21.4s, v5.4s")
                __ASM_EMIT("fsub        v22.4s, v22.4s, v6.4s")
                __ASM_EMIT("fsub        v23.4s, v23.4s, v7.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v24.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v24.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v24.4s")
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
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = d
                __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp         q16, q17, [%[src], #0x00]") // v16  = s
                __ASM_EMIT("ldp         q18, q19, [%[src], #0x20]")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[src], %[src], #0x40")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")   // v0   = d
                __ASM_EMIT("ldp         q16, q17, [%[src], #0x00]") // v16  = s
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[src], %[src], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[dst], #0x00]")       // v0   = d
                __ASM_EMIT("ldr         q16, [%[src], #0x00]")      // v16  = s
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[src], %[src], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[dst]]")         // v0   = d
                __ASM_EMIT("ld1r        {v16.4s}, [%[src]]")        // v16  = s
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[src], %[src], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [k] "r" (&k)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24"
            );
        }

        void pmix_k2(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v24.4s}, [%[k]]")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[src1], #0x00]")  // v0   = d
                __ASM_EMIT("ldp         q2, q3, [%[src1], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[src1], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[src1], #0x60]")
                __ASM_EMIT("ldp         q16, q17, [%[src2], #0x00]")// v16  = s
                __ASM_EMIT("ldp         q18, q19, [%[src2], #0x20]")
                __ASM_EMIT("ldp         q20, q21, [%[src2], #0x40]")
                __ASM_EMIT("ldp         q22, q23, [%[src2], #0x60]")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fsub        v20.4s, v20.4s, v4.4s")
                __ASM_EMIT("fsub        v21.4s, v21.4s, v5.4s")
                __ASM_EMIT("fsub        v22.4s, v22.4s, v6.4s")
                __ASM_EMIT("fsub        v23.4s, v23.4s, v7.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v24.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v24.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v24.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[src1], %[src1], #0x80")
                __ASM_EMIT("add         %[src2], %[src2], #0x80")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[src1], #0x00]")  // v0   = d
                __ASM_EMIT("ldp         q2, q3, [%[src1], #0x20]")
                __ASM_EMIT("ldp         q16, q17, [%[src2], #0x00]")// v16  = s
                __ASM_EMIT("ldp         q18, q19, [%[src2], #0x20]")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[src1], %[src1], #0x40")
                __ASM_EMIT("add         %[src2], %[src2], #0x40")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[src1], #0x00]")  // v0   = d
                __ASM_EMIT("ldp         q16, q17, [%[src2], #0x00]")// v16  = s
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[src1], %[src1], #0x20")
                __ASM_EMIT("add         %[src2], %[src2], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[src1], #0x00]")      // v0   = d
                __ASM_EMIT("ldr         q16, [%[src2], #0x00]")     // v16  = s
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[src1], %[src1], #0x10")
                __ASM_EMIT("add         %[src2], %[src2], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[src1]]")        // v0   = d
                __ASM_EMIT("ld1r        {v16.4s}, [%[src2]]")       // v16  = s
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")     // v16 = s - d
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")     // v0   = d + (s-d)*k
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[src1], %[src1], #0x04")
                __ASM_EMIT("add         %[src2], %[src2], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
                  [count] "+r" (count)
                : [k] "r" (&k)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24"
            );
        }
    } /* namespace asimd */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_PMIX_H_ */
