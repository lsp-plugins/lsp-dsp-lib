/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 14 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_COS_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_COS_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <private/dsp/arch/aarch64/asimd/pmath/sin.h>

namespace lsp
{
    namespace asimd
    {
        void cosf1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("ldp             q16, q17, [%[S2C], #0x00]") /* v16  = PI/2, v17 = PI */
                __ASM_EMIT("ldp             q18, q19, [%[S2C], #0x20]") /* v18  = 1/(2*PI), v19 = C0 */
                __ASM_EMIT("ldp             q20, q21, [%[S2C], #0x40]") /* v20  = C1, v21 = C2 */
                __ASM_EMIT("ldp             q22, q23, [%[S2C], #0x60]") /* v22  = C3, v23 = C4 */
                // x16 blocks
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")   // v0   = x0, v1 = x1
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")   // v2   = x2, v3 = x3
                __ASM_EMIT("fsub            v0.4s, v17.4s, v0.4s")      // v0   = PI/2 - x0 + PI/2
                __ASM_EMIT("fsub            v1.4s, v17.4s, v1.4s")      // v1   = PI/2 - x1 + PI/2
                __ASM_EMIT("fsub            v2.4s, v17.4s, v2.4s")      // v2   = PI/2 - x2 + PI/2
                __ASM_EMIT("fsub            v3.4s, v17.4s, v3.4s")      // v3   = PI/2 - x3 + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")   // v0   = x0, v1 = x1
                __ASM_EMIT("fsub            v0.4s, v17.4s, v0.4s")      // v0   = PI/2 - x0 + PI/2
                __ASM_EMIT("fsub            v1.4s, v17.4s, v1.4s")      // v1   = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q0, [%[dst], #0x00]")       // v0   = x0
                __ASM_EMIT("fsub            v0.4s, v17.4s, v0.4s")      // v0   = PI/2 - x0 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst], #0x00]")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            14f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("8:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("10:")
                __ASM_EMIT("fsub            v0.4s, v17.4s, v0.4s")      // v0   = PI/2 - x0 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("sub             %[dst], %[dst], #0x04")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("12:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            14f")
                __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
                // End
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "r" (&sinf_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void cosf2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("ldp             q16, q17, [%[S2C], #0x00]") /* v16  = PI/2, v17 = PI */
                __ASM_EMIT("ldp             q18, q19, [%[S2C], #0x20]") /* v18  = 1/(2*PI), v19 = C0 */
                __ASM_EMIT("ldp             q20, q21, [%[S2C], #0x40]") /* v20  = C1, v21 = C2 */
                __ASM_EMIT("ldp             q22, q23, [%[S2C], #0x60]") /* v22  = C3, v23 = C4 */
                // x16 blocks
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")   // v0   = x0, v1 = x1
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")   // v2   = x2, v3 = x3
                __ASM_EMIT("fsub            v0.4s, v17.4s, v0.4s")      // v0   = PI/2 - x0 + PI/2
                __ASM_EMIT("fsub            v1.4s, v17.4s, v1.4s")      // v1   = PI/2 - x1 + PI/2
                __ASM_EMIT("fsub            v2.4s, v17.4s, v2.4s")      // v2   = PI/2 - x2 + PI/2
                __ASM_EMIT("fsub            v3.4s, v17.4s, v3.4s")      // v3   = PI/2 - x3 + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("add             %[src], %[src], #0x40")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")   // v0   = x0, v1 = x1
                __ASM_EMIT("fsub            v0.4s, v17.4s, v0.4s")      // v0   = PI/2 - x0 + PI/2
                __ASM_EMIT("fsub            v1.4s, v17.4s, v1.4s")      // v1   = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q0, [%[src], #0x00]")       // v0   = x0
                __ASM_EMIT("fsub            v0.4s, v17.4s, v0.4s")      // v0   = PI/2 - x0 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst], #0x00]")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            14f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("8:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
                __ASM_EMIT("10:")
                __ASM_EMIT("fsub            v0.4s, v17.4s, v0.4s")      // v0   = PI/2 - x0 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("12:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            14f")
                __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
                // End
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [S2C] "r" (&sinf_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void cosf_kp1(float *dst, float k, float p, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("ldp             q16, q17, [%[S2C], #0x00]")     /* v16  = PI/2, v17 = PI */
                __ASM_EMIT("ldp             q18, q19, [%[S2C], #0x20]")     /* v18  = 1/(2*PI), v19 = C0 */
                __ASM_EMIT("ldp             q20, q21, [%[S2C], #0x40]")     /* v20  = C1, v21 = C2 */
                __ASM_EMIT("ldp             q22, q23, [%[S2C], #0x60]")     /* v22  = C3, v23 = C4 */
                __ASM_EMIT("ld1r            {v28.4s}, [%[k]]")              /* v28  = k k k k */
                __ASM_EMIT("ld1r            {v29.4s}, [%[p]]")              /* v29  = p p p p */
                __ASM_EMIT("ldp             q24, q25, [%[S2KP], #0x00]")    /* v24  = 0 1 2 3, v25 = 4 5 6 7 */
                __ASM_EMIT("ldp             q26, q27, [%[S2KP], #0x20]")    /* v26  = 8 8 8 8, v27 = 4 4 4 4 */
                __ASM_EMIT("fsub            v29.4s, v17.4s, v29.4s")        /* v29  = PI/2 - p + PI/2 ... */
                // x16 blocks
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("fmul            v0.4s, v28.4s, v24.4s")         // v0   = k*x0
                __ASM_EMIT("fmul            v1.4s, v28.4s, v25.4s")         // v1   = k*x1
                __ASM_EMIT("fadd            v24.4s, v24.4s, v26.4s")        // v24  = i0 + 8
                __ASM_EMIT("fadd            v25.4s, v25.4s, v26.4s")        // v25  = i1 + 8
                __ASM_EMIT("fsub            v0.4s, v29.4s, v0.4s")          // v0   = PI/2 - (k*x0 + p) + PI/2
                __ASM_EMIT("fsub            v1.4s, v29.4s, v1.4s")          // v1   = PI/2 - (k*x1 + p) + PI/2
                __ASM_EMIT("fmul            v2.4s, v28.4s, v24.4s")         // v2   = k*x2
                __ASM_EMIT("fmul            v3.4s, v28.4s, v25.4s")         // v3   = k*x3
                __ASM_EMIT("fadd            v24.4s, v24.4s, v26.4s")        // v24  = i0 + 8
                __ASM_EMIT("fadd            v25.4s, v25.4s, v26.4s")        // v25  = i1 + 8
                __ASM_EMIT("fsub            v2.4s, v29.4s, v2.4s")          // v2   = PI/2 - (k*x2 + p) + PI/2
                __ASM_EMIT("fsub            v3.4s, v29.4s, v3.4s")          // v3   = PI/2 - (k*x3 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("fmul            v0.4s, v28.4s, v24.4s")         // v0   = k*x0
                __ASM_EMIT("fmul            v1.4s, v28.4s, v25.4s")         // v1   = k*x1
                __ASM_EMIT("fadd            v24.4s, v24.4s, v26.4s")        // v24  = i0 + 8
                __ASM_EMIT("fadd            v25.4s, v25.4s, v26.4s")        // v25  = i1 + 8
                __ASM_EMIT("fsub            v0.4s, v29.4s, v0.4s")          // v0   = PI/2 - (k*x0 + p) + PI/2
                __ASM_EMIT("fsub            v1.4s, v29.4s, v1.4s")          // v1   = PI/2 - (k*x1 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("fmul            v0.4s, v28.4s, v24.4s")         // v0   = k*x0
                __ASM_EMIT("fadd            v24.4s, v24.4s, v27.4s")        // v24  = i0 + 4
                __ASM_EMIT("fsub            v0.4s, v29.4s, v0.4s")          // v0   = PI/2 - (k*x0 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst], #0x00]")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            10f")
                __ASM_EMIT("fmul            v0.4s, v28.4s, v24.4s")         // v0   = k*x0
                __ASM_EMIT("fsub            v0.4s, v29.4s, v0.4s")          // v0   = PI/2 - (k*x0 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #4")    // v0   = S1 S2 S3 S0
                __ASM_EMIT("8:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st1             {v0.d}[0], [%[dst]]")
                // End
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [k] "r" (&k),
                  [p] "r" (&p),
                  [S2C] "r" (&sinf_const[0]),
                  [S2KP] "r" (&kp_gen_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29"
            );
        }

    } /* namespace asimd */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_COS_H_ */
