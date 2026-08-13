/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 3 июл. 2026 г.
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

#ifndef INCLUDE_PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LERP_H_
#define INCLUDE_PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LERP_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <private/dsp/arch/aarch64/asimd/pmath/sin.h>

namespace lsp
{
    namespace asimd
    {
        void lerp_vvv(float *dst, const float *a, const float *b, const float *k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("ldp         q2, q3, [%[a]], #0x20")
                __ASM_EMIT("ldp         q18, q19, [%[b]], #0x20")
                __ASM_EMIT("ldp         q26, q27, [%[k]], #0x20")
                __ASM_EMIT("ldp         q4, q5, [%[a]], #0x20")
                __ASM_EMIT("ldp         q20, q21, [%[b]], #0x20")
                __ASM_EMIT("ldp         q28, q29, [%[k]], #0x20")
                __ASM_EMIT("ldp         q6, q7, [%[a]], #0x20")
                __ASM_EMIT("ldp         q22, q23, [%[b]], #0x20")
                __ASM_EMIT("ldp         q30, q31, [%[k]], #0x20")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fsub        v20.4s, v20.4s, v4.4s")
                __ASM_EMIT("fsub        v21.4s, v21.4s, v5.4s")
                __ASM_EMIT("fsub        v22.4s, v22.4s, v6.4s")
                __ASM_EMIT("fsub        v23.4s, v23.4s, v7.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v31.4s")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                __ASM_EMIT("stp         q4, q5, [%[dst]], #0x20")
                __ASM_EMIT("stp         q6, q7, [%[dst]], #0x20")
                __ASM_EMIT("b.hs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("ldp         q2, q3, [%[a]], #0x20")
                __ASM_EMIT("ldp         q18, q19, [%[b]], #0x20")
                __ASM_EMIT("ldp         q26, q27, [%[k]], #0x20")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[a]], #0x10")             // v0   = a
                __ASM_EMIT("ldr         q16, [%[b]], #0x10")            // v16  = b
                __ASM_EMIT("ldr         q24, [%[k]], #0x10")            // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("str         q0, [%[dst]], #0x10")
                __ASM_EMIT("sub         %[count], %[count], #4")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[a]], #0x04")        // v0   = a
                __ASM_EMIT("ld1r        {v16.4s}, [%[b]], #0x04")       // v16  = b
                __ASM_EMIT("ld1r        {v24.4s}, [%[k]], #0x04")       // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [k] "+r" (k),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        void lerp_vvk(float *dst, const float *a, const float *b, float k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v24.4s}, [%[k]]")              // v24  = k
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("ldp         q2, q3, [%[a]], #0x20")
                __ASM_EMIT("ldp         q18, q19, [%[b]], #0x20")
                __ASM_EMIT("ldp         q4, q5, [%[a]], #0x20")
                __ASM_EMIT("ldp         q20, q21, [%[b]], #0x20")
                __ASM_EMIT("ldp         q6, q7, [%[a]], #0x20")
                __ASM_EMIT("ldp         q22, q23, [%[b]], #0x20")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16 = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fsub        v20.4s, v20.4s, v4.4s")
                __ASM_EMIT("fsub        v21.4s, v21.4s, v5.4s")
                __ASM_EMIT("fsub        v22.4s, v22.4s, v6.4s")
                __ASM_EMIT("fsub        v23.4s, v23.4s, v7.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v24.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v24.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v24.4s")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                __ASM_EMIT("stp         q4, q5, [%[dst]], #0x20")
                __ASM_EMIT("stp         q6, q7, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("ldp         q2, q3, [%[a]], #0x20")
                __ASM_EMIT("ldp         q18, q19, [%[b]], #0x20")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[a]], #0x10")             // v0   = a
                __ASM_EMIT("ldr         q16, [%[b]], #0x10")            // v16  = b
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("str         q0, [%[dst]], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[a]], #0x04")        // v0   = a
                __ASM_EMIT("ld1r        {v16.4s}, [%[b]], #0x04")       // v16  = b
                __ASM_EMIT("fsub        v16.4s, v16.4s, v0.4s")         // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [k] "r" (&k)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24"
            );
        }

        void lerp_vkv(float *dst, const float *a, float b, const float *k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v8.4s}, [%[b]]")               // v8   = k
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("ldp         q2, q3, [%[a]], #0x20")
                __ASM_EMIT("ldp         q26, q27, [%[k]], #0x20")
                __ASM_EMIT("ldp         q4, q5, [%[a]], #0x20")
                __ASM_EMIT("ldp         q28, q29, [%[k]], #0x20")
                __ASM_EMIT("ldp         q6, q7, [%[a]], #0x20")
                __ASM_EMIT("ldp         q30, q31, [%[k]], #0x20")
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v8.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v8.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v8.4s, v3.4s")
                __ASM_EMIT("fsub        v20.4s, v8.4s, v4.4s")
                __ASM_EMIT("fsub        v21.4s, v8.4s, v5.4s")
                __ASM_EMIT("fsub        v22.4s, v8.4s, v6.4s")
                __ASM_EMIT("fsub        v23.4s, v8.4s, v7.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v31.4s")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                __ASM_EMIT("stp         q4, q5, [%[dst]], #0x20")
                __ASM_EMIT("stp         q6, q7, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("ldp         q2, q3, [%[a]], #0x20")
                __ASM_EMIT("ldp         q26, q27, [%[k]], #0x20")
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v8.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v8.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v8.4s, v3.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v8.4s, v1.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[a]], #0x10")             // v0   = a
                __ASM_EMIT("ldr         q24, [%[k]], #0x10")            // v24  = k
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("str         q0, [%[dst]], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[a]], #0x04")        // v0   = a
                __ASM_EMIT("ld1r        {v24.4s}, [%[k]], #0x04")       // v24  = k
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [a] "+r" (a), [k] "+r" (k),
                  [count] "+r" (count)
                : [b] "r" (&b)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        void lerp_vkk(float *dst, const float *a, float b, float k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v8.4s}, [%[b]]")               // v8   = b
                __ASM_EMIT("ld1r        {v9.4s}, [%[k]]")               // v9   = k
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q2, q3, [%[a]], #0x20")
                __ASM_EMIT("ldp         q4, q5, [%[a]], #0x20")
                __ASM_EMIT("ldp         q6, q7, [%[a]], #0x20")
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v8.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v8.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v8.4s, v3.4s")
                __ASM_EMIT("fsub        v20.4s, v8.4s, v4.4s")
                __ASM_EMIT("fsub        v21.4s, v8.4s, v5.4s")
                __ASM_EMIT("fsub        v22.4s, v8.4s, v6.4s")
                __ASM_EMIT("fsub        v23.4s, v8.4s, v7.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v9.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v9.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v9.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v9.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v9.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v9.4s")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                __ASM_EMIT("stp         q4, q5, [%[dst]], #0x20")
                __ASM_EMIT("stp         q6, q7, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("ldp         q2, q3, [%[a]], #0x20")
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v8.4s, v1.4s")
                __ASM_EMIT("fsub        v18.4s, v8.4s, v2.4s")
                __ASM_EMIT("fsub        v19.4s, v8.4s, v3.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v9.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v9.4s")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[a]], #0x20")         // v0   = a
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v8.4s, v1.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[a]], #0x10")             // v0   = a
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("str         q0, [%[dst]], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[a]], #0x04")        // v0   = a
                __ASM_EMIT("fsub        v16.4s, v8.4s, v0.4s")          // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [a] "+r" (a),
                  [count] "+r" (count)
                : [b] "r" (&b),
                  [k] "r" (&k)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v8.4s}, [%[a]]")               // v8   = a
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("ldp         q18, q19, [%[b]], #0x20")
                __ASM_EMIT("mov         v2.16b, v8.16b")
                __ASM_EMIT("ldp         q26, q27, [%[k]], #0x20")
                __ASM_EMIT("mov         v3.16b, v8.16b")
                __ASM_EMIT("ldp         q20, q21, [%[b]], #0x20")
                __ASM_EMIT("mov         v4.16b, v8.16b")
                __ASM_EMIT("ldp         q28, q29, [%[k]], #0x20")
                __ASM_EMIT("mov         v5.16b, v8.16b")
                __ASM_EMIT("ldp         q22, q23, [%[b]], #0x20")
                __ASM_EMIT("mov         v6.16b, v8.16b")
                __ASM_EMIT("ldp         q30, q31, [%[k]], #0x20")
                __ASM_EMIT("mov         v7.16b, v8.16b")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v8.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v8.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v8.4s")
                __ASM_EMIT("fsub        v20.4s, v20.4s, v8.4s")
                __ASM_EMIT("fsub        v21.4s, v21.4s, v8.4s")
                __ASM_EMIT("fsub        v22.4s, v22.4s, v8.4s")
                __ASM_EMIT("fsub        v23.4s, v23.4s, v8.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v31.4s")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                __ASM_EMIT("stp         q4, q5, [%[dst]], #0x20")
                __ASM_EMIT("stp         q6, q7, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("ldp         q18, q19, [%[b]], #0x20")
                __ASM_EMIT("mov         v2.16b, v8.16b")
                __ASM_EMIT("ldp         q26, q27, [%[k]], #0x20")
                __ASM_EMIT("mov         v3.16b, v8.16b")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v8.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v8.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v8.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v27.4s")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v8.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q16, [%[b]], #0x10")            // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("ldr         q24, [%[k]], #0x10")            // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("str         q0, [%[dst]], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v16.4s}, [%[b]], #0x04")       // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("ld1r        {v24.4s}, [%[k]], #0x04")       // v24  = k
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v24.4s")         // v0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [b] "+r" (b), [k] "+r" (k),
                  [count] "+r" (count)
                : [a] "r" (&a)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        void lerp_kvk(float *dst, float a, const float *b, float k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v8.4s}, [%[a]]")               // v8   = a
                __ASM_EMIT("ld1r        {v9.4s}, [%[k]]")               // v9   = k
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("ldp         q18, q19, [%[b]], #0x20")
                __ASM_EMIT("mov         v2.16b, v8.16b")
                __ASM_EMIT("mov         v3.16b, v8.16b")
                __ASM_EMIT("ldp         q20, q21, [%[b]], #0x20")
                __ASM_EMIT("mov         v4.16b, v8.16b")
                __ASM_EMIT("mov         v5.16b, v8.16b")
                __ASM_EMIT("ldp         q22, q23, [%[b]], #0x20")
                __ASM_EMIT("mov         v6.16b, v8.16b")
                __ASM_EMIT("mov         v7.16b, v8.16b")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v8.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v8.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v8.4s")
                __ASM_EMIT("fsub        v20.4s, v20.4s, v8.4s")
                __ASM_EMIT("fsub        v21.4s, v21.4s, v8.4s")
                __ASM_EMIT("fsub        v22.4s, v22.4s, v8.4s")
                __ASM_EMIT("fsub        v23.4s, v23.4s, v8.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v9.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v9.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v9.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v9.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v9.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v9.4s")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                __ASM_EMIT("stp         q4, q5, [%[dst]], #0x20")
                __ASM_EMIT("stp         q6, q7, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("ldp         q18, q19, [%[b]], #0x20")
                __ASM_EMIT("mov         v2.16b, v8.16b")
                __ASM_EMIT("mov         v3.16b, v8.16b")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v8.4s")
                __ASM_EMIT("fsub        v18.4s, v18.4s, v8.4s")
                __ASM_EMIT("fsub        v19.4s, v19.4s, v8.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v9.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v9.4s")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q16, q17, [%[b]], #0x20")       // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("ldp         q24, q25, [%[k]], #0x20")       // v24  = k
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fsub        v17.4s, v17.4s, v8.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q16, [%[b]], #0x10")            // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("str         q0, [%[dst]], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v16.4s}, [%[b]], #0x04")       // v16  = b
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("fsub        v16.4s, v16.4s, v8.4s")         // v16  = b-a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [b] "+r" (b),
                  [count] "+r" (count)
                : [a] "r" (&a),
                  [k] "r" (&k)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lerp_kkv(float *dst, float a, float b, const float *k, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x32 blocks
                __ASM_EMIT("ld1r        {v8.4s}, [%[a]]")               // v8   = a
                __ASM_EMIT("ld1r        {v9.4s}, [%[b]]")               // v9   = b
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("fsub        v9.4s, v9.4s, v8.4s")           // v9   = b-a
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q16, q17, [%[k]], #0x20")       // v16  = k
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("ldp         q18, q19, [%[k]], #0x20")
                __ASM_EMIT("mov         v2.16b, v8.16b")
                __ASM_EMIT("mov         v3.16b, v8.16b")
                __ASM_EMIT("ldp         q20, q21, [%[k]], #0x20")
                __ASM_EMIT("mov         v4.16b, v8.16b")
                __ASM_EMIT("mov         v5.16b, v8.16b")
                __ASM_EMIT("ldp         q22, q23, [%[k]], #0x20")
                __ASM_EMIT("mov         v6.16b, v8.16b")
                __ASM_EMIT("mov         v7.16b, v8.16b")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v9.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v9.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v9.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v9.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v9.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v9.4s")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                __ASM_EMIT("stp         q4, q5, [%[dst]], #0x20")
                __ASM_EMIT("stp         q6, q7, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q16, q17, [%[k]], #0x20")       // v16  = k
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("ldp         q18, q19, [%[k]], #0x20")
                __ASM_EMIT("mov         v2.16b, v8.16b")
                __ASM_EMIT("mov         v3.16b, v8.16b")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v9.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v9.4s")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                __ASM_EMIT("stp         q2, q3, [%[dst]], #0x20")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q16, q17, [%[k]], #0x20")       // v16  = k
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("mov         v1.16b, v8.16b")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("fmla        v1.4s, v17.4s, v9.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("stp         q0, q1, [%[dst]], #0x20")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q16, [%[k]], #0x10")            // v16  = k
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("str         q0, [%[dst]], #0x10")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v16.4s}, [%[k]], #0x04")       // v16  = k
                __ASM_EMIT("mov         v0.16b, v8.16b")                // v0   = a
                __ASM_EMIT("fmla        v0.4s, v16.4s, v9.4s")          // v0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge        9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [k] "+r" (k),
                  [count] "+r" (count)
                : [a] "r" (&a),
                  [b] "r" (&b)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

    } /* namespace asimd */
} /* namespace lsp */




#endif /* INCLUDE_PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LERP_H_ */
