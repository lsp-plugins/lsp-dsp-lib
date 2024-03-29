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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_EXP_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_EXP_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const uint32_t EXP2_CONST[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7f),       // ME = 0x7f
                LSP_DSP_VEC4(0x3f317218), // L2 = ln(2)
                LSP_DSP_VEC4(0x40e00000), // C5 = 7!/6! = 7.0000000000000000
                LSP_DSP_VEC4(0x42280000), // C4 = 7!/5! = 42.0000000000000000
                LSP_DSP_VEC4(0x43520000), // C3 = 7!/4! = 210.0000000000000000
                LSP_DSP_VEC4(0x44520000), // C2 = 7!/3! = 840.0000000000000000
                LSP_DSP_VEC4(0x451d8000), // C1 = 7!/2! = 2520.0000000000000000
                LSP_DSP_VEC4(0x459d8000), // C0 = 7!/1! = 5040.0000000000000000
                LSP_DSP_VEC4(0x39500d01), // C6 = 1/7! = 0.0001984127011383
                LSP_DSP_VEC4(0x3f800000), // C7 = 1.0
            };

            static const float EXP_LOG2E[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(M_LOG2E),
                LSP_DSP_VEC4(M_LOG2E)
            };
        )

    #define POW2_CORE_X8(ME, L2, C5, C4, C3, C2, C1, C0, C6, C7) \
        /* v0 = x */ \
        __ASM_EMIT("sshr            v2.4s, v0.4s, #31")             /* v2   = [ x < 0 ] */ \
        __ASM_EMIT("sshr            v3.4s, v1.4s, #31") \
        __ASM_EMIT("fabs            v0.4s, v0.4s")                  /* v0   = XP = fabs(x) */ \
        __ASM_EMIT("fabs            v1.4s, v1.4s") \
        __ASM_EMIT("fcvtzs          v4.4s, v0.4s")                  /* v4   = R = int(x) */ \
        __ASM_EMIT("fcvtzs          v5.4s, v1.4s") \
        __ASM_EMIT("add             v6.4s, v4.4s, " ME ".4s")       /* v6   = R + (ME = 127) */ \
        __ASM_EMIT("add             v7.4s, v5.4s, " ME ".4s") \
        __ASM_EMIT("scvtf           v4.4s, v4.4s")                  /* v4   = float(R) */ \
        __ASM_EMIT("scvtf           v5.4s, v5.4s") \
        __ASM_EMIT("shl             v6.4s, v6.4s, #23")             /* v6   = 1 << (R + 127) */ \
        __ASM_EMIT("shl             v7.4s, v7.4s, #23") \
        __ASM_EMIT("fsub            v0.4s, v0.4s, v4.4s")           /* v0   = XP - float(R) */ \
        __ASM_EMIT("fsub            v1.4s, v1.4s, v5.4s") \
        __ASM_EMIT("fmul            v0.4s, v0.4s, " L2 ".4s")       /* v0   = X = ln(2) * (XP - float(R)) */ \
        __ASM_EMIT("fmul            v1.4s, v1.4s, " L2 ".4s") \
        /* v0 = X, v2 = [ x < 0 ], v6 = 1 << R */ \
        __ASM_EMIT("fadd            v4.4s, v0.4s, " C5 ".4s")       /* v4   = C5+X */ \
        __ASM_EMIT("fadd            v5.4s, v1.4s, " C5 ".4s") \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C5+X) */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, v1.4s") \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C4 ".4s")       /* v4   = C4+X*(C5+X) */ \
        __ASM_EMIT("fadd            v5.4s, v5.4s, " C4 ".4s") \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, v1.4s") \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C3 ".4s")       /* v4   = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("fadd            v5.4s, v5.4s, " C3 ".4s") \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, v1.4s") \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C2 ".4s")       /* v4   = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("fadd            v5.4s, v5.4s, " C2 ".4s") \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, v1.4s") \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C1 ".4s")       /* v4   = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("fadd            v5.4s, v5.4s, " C1 ".4s") \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, v1.4s") \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C0 ".4s")       /* v4   = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("fadd            v5.4s, v5.4s, " C0 ".4s") \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, v1.4s") \
        __ASM_EMIT("fmul            v4.4s, v4.4s, " C6 ".4s")       /* v4   = 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, " C6 ".4s") \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C7 ".4s")       /* v4   = 1 + 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("fadd            v5.4s, v5.4s, " C7 ".4s") \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v6.4s")           /* v4   = E = (1 << R) * (1 + 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, v7.4s") \
        /* Calc reciprocals into v0 */ \
        __ASM_EMIT("frecpe          v8.4s, v4.4s")                  /* v8   = e2 */ \
        __ASM_EMIT("frecpe          v9.4s, v5.4s") \
        __ASM_EMIT("frecps          v10.4s, v8.4s, v4.4s")          /* v10  = (2 - E*e2) */ \
        __ASM_EMIT("frecps          v11.4s, v9.4s, v5.4s") \
        __ASM_EMIT("fmul            v8.4s, v10.4s, v8.4s")          /* v8   = e2' = e2 * (2 - E*e2) */ \
        __ASM_EMIT("fmul            v9.4s, v11.4s, v9.4s") \
        __ASM_EMIT("frecps          v10.4s, v8.4s, v4.4s")          /* v10  = (2 - E*e2') */ \
        __ASM_EMIT("frecps          v11.4s, v9.4s, v5.4s") \
        __ASM_EMIT("fmul            v0.4s, v10.4s, v8.4s")          /* v0   = 1/E = e2" = e2' * (2 - E*e2) */  \
        __ASM_EMIT("fmul            v1.4s, v11.4s, v9.4s") \
        /* Perform conditional output */ \
        __ASM_EMIT("bif             v0.16b, v4.16b, v2.16b")        /* v0   = ((1/E) & [ x < 0 ]) | (E & [x >= 0]) */ \
        __ASM_EMIT("bif             v1.16b, v5.16b, v3.16b")

    #define POW2_CORE_X4(ME, L2, C5, C4, C3, C2, C1, C0, C6, C7) \
        /* v0 = x */ \
        __ASM_EMIT("sshr            v2.4s, v0.4s, #31")             /* v2   = [ x < 0 ] */ \
        __ASM_EMIT("fabs            v0.4s, v0.4s")                  /* v0   = XP = fabs(x) */ \
        __ASM_EMIT("fcvtzs          v4.4s, v0.4s")                  /* v4   = R = int(x) */ \
        __ASM_EMIT("add             v6.4s, v4.4s, " ME ".4s")       /* v6   = R + (ME = 127) */ \
        __ASM_EMIT("scvtf           v4.4s, v4.4s")                  /* v4   = float(R) */ \
        __ASM_EMIT("shl             v6.4s, v6.4s, #23")             /* v6   = 1 << (R + 127) */ \
        __ASM_EMIT("fsub            v0.4s, v0.4s, v4.4s")           /* v0   = XP - float(R) */ \
        __ASM_EMIT("fmul            v0.4s, v0.4s, " L2 ".4s")       /* v0   = X = ln(2) * (XP - float(R)) */ \
        /* v0 = X, v2 = [ x < 0 ], v6 = 1 << R */ \
        __ASM_EMIT("fadd            v4.4s, v0.4s, " C5 ".4s")       /* v4   = C5+X */ \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C5+X) */ \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C4 ".4s")       /* v4   = C4+X*(C5+X) */ \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C3 ".4s")       /* v4   = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C2 ".4s")       /* v4   = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C1 ".4s")       /* v4   = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C0 ".4s")       /* v4   = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v0.4s")           /* v4   = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("fmul            v4.4s, v4.4s, " C6 ".4s")       /* v4   = 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("fadd            v4.4s, v4.4s, " C7 ".4s")       /* v4   = 1 + 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("fmul            v4.4s, v4.4s, v6.4s")           /* v4   = E = (1 << R) * (1 + 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        /* Calc reciprocals into v0 */ \
        __ASM_EMIT("frecpe          v8.4s, v4.4s")                  /* v8   = e2 */ \
        __ASM_EMIT("frecps          v10.4s, v8.4s, v4.4s")          /* v10  = (2 - E*e2) */ \
        __ASM_EMIT("fmul            v8.4s, v10.4s, v8.4s")          /* v8   = e2' = e2 * (2 - E*e2) */ \
        __ASM_EMIT("frecps          v10.4s, v8.4s, v4.4s")          /* v10  = (2 - E*e2') */ \
        __ASM_EMIT("fmul            v0.4s, v10.4s, v8.4s")          /* v0   = 1/E = e2" = e2' * (2 - E*e2) */  \
        /* Perform conditional output */ \
        __ASM_EMIT("bif             v0.16b, v4.16b, v2.16b")        /* v0   = ((1/E) & [ x < 0 ]) | (E & [x >= 0]) */

    #define EXP_CORE_LOAD \
        __ASM_EMIT("ldp             q26, q27, [%[LOG2E]]") \
        __ASM_EMIT("ldp             q16, q17, [%[E2C], #0x00]")     /* v16  = ME, v17 = L2 */ \
        __ASM_EMIT("ldp             q18, q19, [%[E2C], #0x20]")     /* v18  = C5, v19 = C4 */ \
        __ASM_EMIT("ldp             q20, q21, [%[E2C], #0x40]")     /* v20  = C3, v21 = C2 */ \
        __ASM_EMIT("ldp             q22, q23, [%[E2C], #0x60]")     /* v22  = C1, v23 = C0 */ \
        __ASM_EMIT("ldp             q24, q25, [%[E2C], #0x80]")     /* v22  = C6, v23 = C7 */

    #define EXP_CORE_X8_NOLOAD \
        /* in: v0 = x0, v1 = x1 */ \
        __ASM_EMIT("fmul            v0.4s, v0.4s, v26.4s") \
        __ASM_EMIT("fmul            v1.4s, v1.4s, v27.4s") \
        POW2_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25") \
        /* out: v0 = exp(x0), v1 = exp(x1) */

    #define EXP_CORE_X4_NOLOAD \
        /* in: v0 = x0 */ \
        __ASM_EMIT("fmul            v0.4s, v0.4s, v26.4s") \
        POW2_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25") \
        /* out: v0 = exp(x0) */

    #define EXP_CORE_X8 \
        /* in: v0 = x0, v1 = x1 */ \
        EXP_CORE_LOAD \
        EXP_CORE_X8_NOLOAD \
        /* out: v0 = exp(x0), v1 = exp(x1) */

    #define EXP_CORE_X4 \
        /* in: v0 = x0 */ \
        EXP_CORE_LOAD \
        EXP_CORE_X4_NOLOAD \
        /* out: v0 = exp(x0) */

        void exp2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                EXP_CORE_LOAD
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src]]")
                EXP_CORE_X8_NOLOAD
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("ldr             q0, [%[src]]")
                EXP_CORE_X4_NOLOAD
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("bls             12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
                __ASM_EMIT("8:")
                EXP_CORE_X4_NOLOAD
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27"
            );
        }

        void exp1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM(
                EXP_CORE_LOAD
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[dst]]")
                EXP_CORE_X8
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("ldr             q0, [%[dst]]")
                EXP_CORE_X4
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("bls             12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("8:")
                EXP_CORE_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("sub             %[dst], %[dst], #0x04")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27"
            );
        }
    } /* namespace asimd */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_EXP_H_ */
