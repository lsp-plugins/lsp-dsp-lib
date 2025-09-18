/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LOG_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LOG_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const uint32_t LOG2_CONST[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x007fffff), // MM = frac
                LSP_DSP_VEC4(0x0000007f), // ME = 127
                LSP_DSP_VEC4(0x3dba2e8c), // C0 = 1/11 = 0.0909090936183929
                LSP_DSP_VEC4(0x3de38e39), // C1 = 1/9 = 0.1111111119389534
                LSP_DSP_VEC4(0x3e124925), // C2 = 1/7 = 0.1428571492433548
                LSP_DSP_VEC4(0x3e4ccccd), // C3 = 1/5 = 0.2000000029802322
                LSP_DSP_VEC4(0x3eaaaaab), // C4 = 1/3 = 0.3333333432674408
                LSP_DSP_VEC4(0x3f800000), // C5 = 1.0f
            };

            static const float LOGB_C[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(2.0f * float(M_LOG2E)),
                LSP_DSP_VEC4(2.0f * float(M_LOG2E))
            };

            static const float LOGE_C[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1.0f / float(M_LOG2E)),
                LSP_DSP_VEC4(1.0f / float(M_LOG2E))
            };

            static const float LOGD_C[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(2.0f * float(M_LOG10E)),
                LSP_DSP_VEC4(0.301029995663981f) // 1/log2(10)
            };
        )

    #define LOGN_CORE_X8(MM, ME, C0, C1, C2, C3, C4, C5) \
        /* v0 = x */ \
        __ASM_EMIT("ushr            v2.4s, v0.4s, #23")             /* v2   = ilog2(x) + 127 */ \
        __ASM_EMIT("ushr            v3.4s, v1.4s, #23") \
        __ASM_EMIT("and             v0.16b, v0.16b, " MM ".16b")    /* v0   = x & MM */ \
        __ASM_EMIT("and             v1.16b, v1.16b, " MM ".16b") \
        __ASM_EMIT("sub             v2.4s, v2.4s, " ME ".4s")       /* v2   = r - ME = ilog2(x) */ \
        __ASM_EMIT("sub             v3.4s, v3.4s, " ME ".4s") \
        __ASM_EMIT("orr             v0.16b, v0.16b, " C5 ".16b")    /* v0   = X = (x & MM) | (C7 = 1.0f) */ \
        __ASM_EMIT("orr             v1.16b, v1.16b, " C5 ".16b") \
        __ASM_EMIT("scvtf           v2.4s, v2.4s")                  /* v2   = R = float(r) */ \
        __ASM_EMIT("scvtf           v3.4s, v3.4s") \
        __ASM_EMIT("fadd            v4.4s, v0.4s, " C5 ".4s")       /* v4   = XB = X + (C7 = 1) */ \
        __ASM_EMIT("fadd            v5.4s, v1.4s, " C5 ".4s") \
        __ASM_EMIT("fsub            v0.4s, v0.4s, " C5 ".4s")       /* v0   = XT = X - (C7 = 1) */ \
        __ASM_EMIT("fsub            v1.4s, v1.4s, " C5 ".4s") \
        __ASM_EMIT("frecpe          v6.4s, v4.4s")                  /* v6   = xb */ \
        __ASM_EMIT("frecpe          v7.4s, v5.4s") \
        __ASM_EMIT("frecps          v8.4s, v6.4s, v4.4s")           /* v8   = (2 - XB*xb) */ \
        __ASM_EMIT("frecps          v9.4s, v7.4s, v5.4s") \
        __ASM_EMIT("fmul            v6.4s, v8.4s, v6.4s")           /* v6   = xb' = xb * (2 - XB*xb) */ \
        __ASM_EMIT("fmul            v7.4s, v9.4s, v7.4s") \
        __ASM_EMIT("frecps          v8.4s, v6.4s, v4.4s")           /* v8   = (2 - XB*xb') */ \
        __ASM_EMIT("frecps          v9.4s, v7.4s, v5.4s") \
        __ASM_EMIT("fmul            v4.4s, v8.4s, v6.4s")           /* v4   = 1/(X+1) = xb" = xb' * (2 - XB*xb) */  \
        __ASM_EMIT("fmul            v5.4s, v9.4s, v7.4s") \
        __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = y = (X-1)/(X+1) */ \
        __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s") \
        __ASM_EMIT("fmul            v4.4s, v0.4s, v0.4s")           /* v4   = Y = y*y */ \
        __ASM_EMIT("fmul            v5.4s, v1.4s, v1.4s") \
        /* v0 = y, v2 = R, v4 = Y */ \
        __ASM_EMIT("fmul            v6.4s, v4.4s, " C0 ".4s")       /* v6   = C0*Y */ \
        __ASM_EMIT("fmul            v7.4s, v5.4s, " C0 ".4s") \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C1 ".4s")       /* v6   = C1+C0*Y */ \
        __ASM_EMIT("fadd            v7.4s, v7.4s, " C1 ".4s") \
        __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C1+C0*Y) */ \
        __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C2 ".4s")       /* v6   = C2+Y*(C1+C0*Y) */ \
        __ASM_EMIT("fadd            v7.4s, v7.4s, " C2 ".4s") \
        __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C2+Y*(C1+C0*Y)) */ \
        __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C3 ".4s")       /* v6   = C3+Y*(C2+Y*(C1+C0*Y)) */ \
        __ASM_EMIT("fadd            v7.4s, v7.4s, " C3 ".4s") \
        __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
        __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C4 ".4s")       /* v6   = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
        __ASM_EMIT("fadd            v7.4s, v7.4s, " C4 ".4s") \
        __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
        __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C5 ".4s")       /* v6   = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
        __ASM_EMIT("fadd            v7.4s, v7.4s, " C5 ".4s") \
        __ASM_EMIT("fmul            v0.4s, v0.4s, v6.4s")           /* v0   = y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
        __ASM_EMIT("fmul            v1.4s, v1.4s, v7.4s") \
        /* v0 = y*L, v2 = R */

    #define LOGN_CORE_X4(MM, ME, C0, C1, C2, C3, C4, C5) \
        /* v0 = x */ \
        __ASM_EMIT("ushr            v2.4s, v0.4s, #23")             /* v2   = ilog2(x) + 127 */ \
        __ASM_EMIT("and             v0.16b, v0.16b, " MM ".16b")    /* v0   = x & MM */ \
        __ASM_EMIT("sub             v2.4s, v2.4s, " ME ".4s")       /* v2   = r - ME = ilog2(x) */ \
        __ASM_EMIT("orr             v0.16b, v0.16b, " C5 ".16b")    /* v0   = X = (x & MM) | (C7 = 1.0f) */ \
        __ASM_EMIT("scvtf           v2.4s, v2.4s")                  /* v2   = R = float(r) */ \
        __ASM_EMIT("fadd            v4.4s, v0.4s, " C5 ".4s")       /* v4   = XB = X + (C7 = 1) */ \
        __ASM_EMIT("fsub            v0.4s, v0.4s, " C5 ".4s")       /* v0   = XT = X - (C7 = 1) */ \
        __ASM_EMIT("frecpe          v6.4s, v4.4s")                  /* v6   = xb */ \
        __ASM_EMIT("frecps          v8.4s, v6.4s, v4.4s")           /* v8   = (2 - XB*xb) */ \
        __ASM_EMIT("fmul            v6.4s, v8.4s, v6.4s")           /* v6   = xb' = xb * (2 - XB*xb) */ \
        __ASM_EMIT("frecps          v8.4s, v6.4s, v4.4s")           /* v8   = (2 - XB*xb') */ \
        __ASM_EMIT("fmul            v4.4s, v8.4s, v6.4s")           /* v4   = 1/(X+1) = xb" = xb' * (2 - XB*xb) */  \
        __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = y = (X-1)/(X+1) */ \
        __ASM_EMIT("fmul            v4.4s, v0.4s, v0.4s")           /* v4   = Y = y*y */ \
        /* v0 = y, v2 = R, v4 = Y */ \
        __ASM_EMIT("fmul            v6.4s, v4.4s, " C0 ".4s")       /* v6   = C0*Y */ \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C1 ".4s")       /* v6   = C1+C0*Y */ \
        __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C1+C0*Y) */ \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C2 ".4s")       /* v6   = C2+Y*(C1+C0*Y) */ \
        __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C2+Y*(C1+C0*Y)) */ \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C3 ".4s")       /* v6   = C3+Y*(C2+Y*(C1+C0*Y)) */ \
        __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C4 ".4s")       /* v6   = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
        __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
        __ASM_EMIT("fadd            v6.4s, v6.4s, " C5 ".4s")       /* v6   = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
        __ASM_EMIT("fmul            v0.4s, v0.4s, v6.4s")           /* v0   = y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
        /* v0 = y*L, v2 = R */

    #define LOGN_CORE_LOAD \
        __ASM_EMIT("ldp             q26, q27, [%[LOGC]]") \
        __ASM_EMIT("ldp             q16, q17, [%[L2C], #0x00]")     /* v16  = MM, v17 = ME */ \
        __ASM_EMIT("ldp             q18, q19, [%[L2C], #0x20]")     /* v18  = C0, v19 = C1 */ \
        __ASM_EMIT("ldp             q20, q21, [%[L2C], #0x40]")     /* v20  = C2, v21 = C3 */ \
        __ASM_EMIT("ldp             q22, q23, [%[L2C], #0x60]")     /* v22  = C4, v23 = C5 */

    #define LOGB_CORE_X8_NOLOAD \
        /* in: v0 = x1, v1 = x2 */ \
        LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23") \
        __ASM_EMIT("fmla            v2.4s, v0.4s, v26.4s")          /* v2   = R + 2*y*L*M_LOG2E */ \
        __ASM_EMIT("fmla            v3.4s, v1.4s, v27.4s") \
        /* out: v0 = logb(x0), v1 = logb(x1) */

    #define LOGB_CORE_X4_NOLOAD \
        /* in: v0 = x1 */ \
        LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23") \
        __ASM_EMIT("fmla            v2.4s, v0.4s, v26.4s")          /* v2   = R + 2*y*L*M_LOG2E */ \
        /* out: v0 = logb(x0) */

    #define LOGB_CORE_X8 \
        /* in: v0 = x1, v1 = x2 */ \
        LOGN_CORE_LOAD \
        LOGB_CORE_X8_NOLOAD \
        /* out: v0 = logb(x0), v1 = logb(x1) */

    #define LOGB_CORE_X4 \
        /* in: v0 = x1 */ \
        LOGN_CORE_LOAD \
        LOGB_CORE_X4_NOLOAD \
        /* out: v0 = logb(x0) */

    #define LOGE_CORE_X8_NOLOAD \
        /* in: v0 = x1, v1 = x2 */ \
        LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23") \
        __ASM_EMIT("fadd            v0.4s, v0.4s, v0.4s")           /* v0 = 2*y*L */ \
        __ASM_EMIT("fadd            v1.4s, v1.4s, v1.4s") \
        __ASM_EMIT("fmla            v0.4s, v2.4s, v26.4s")          /* v0 = 2*y*L + R/log2(E) */ \
        __ASM_EMIT("fmla            v1.4s, v3.4s, v27.4s") \
        /* out: v0 = loge(x0), v1 = loge(x1) */

    #define LOGE_CORE_X4_NOLOAD \
        /* in: v0 = x1 */ \
        LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23") \
        __ASM_EMIT("fadd            v0.4s, v0.4s, v0.4s")           /* v0 = 2*y*L */ \
        __ASM_EMIT("fmla            v0.4s, v2.4s, v26.4s")          /* v0 = 2*y*L + R/log2(E) */ \
        /* out: v0 = loge(x0) */

    #define LOGE_CORE_X8 \
        /* in: v0 = x1, v1 = x2 */ \
        LOGN_CORE_LOAD \
        LOGE_CORE_X8_NOLOAD \
        /* out: v0 = loge(x0), v1 = loge(x1) */

    #define LOGE_CORE_X4 \
        /* in: v0 = x1 */ \
        LOGN_CORE_LOAD \
        LOGE_CORE_X4_NOLOAD \
        /* out: v0 = loge(x0) */

    #define LOGD_CORE_X8_NOLOAD \
        /* in: v0 = x1, v1 = x2 */ \
        LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23") \
        __ASM_EMIT("fmul            v0.4s, v0.4s, v26.4s")          /* v0 = 2*y*L*log10(E) */ \
        __ASM_EMIT("fmul            v1.4s, v1.4s, v26.4s") \
        __ASM_EMIT("fmla            v0.4s, v2.4s, v27.4s")          /* v0 = 2*y*L*log10(E) + R/log2(10) */ \
        __ASM_EMIT("fmla            v1.4s, v3.4s, v27.4s") \
        /* out: v0 = logd(x0), v1 = logd(x1) */

    #define LOGD_CORE_X4_NOLOAD \
        /* in: v0 = x1 */ \
        LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23") \
        __ASM_EMIT("fmul            v0.4s, v0.4s, v26.4s")          /* v0 = 2*y*L*log10(E) */ \
        __ASM_EMIT("fmla            v0.4s, v2.4s, v27.4s")          /* v0 = 2*y*L*log10(E) + R/log2(10) */ \
        /* out: v0 = logd(x0) */

    #define LOGD_CORE_X8 \
        /* in: v0 = x1, v1 = x2 */ \
        LOGN_CORE_LOAD \
        LOGD_CORE_X8_NOLOAD \
        /* out: v0 = logd(x0), v1 = logd(x1) */

    #define LOGD_CORE_X4 \
        /* in: v0 = x1 */ \
        LOGN_CORE_LOAD \
        LOGD_CORE_X4_NOLOAD \
        /* out: v0 = logd(x0) */

        void logb2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // prepare
                LOGN_CORE_LOAD
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src]]")
                LOGB_CORE_X8_NOLOAD
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q2, q3, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count] , #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[src]]")
                LOGB_CORE_X4_NOLOAD
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q2, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
                __ASM_EMIT("8:")
                LOGB_CORE_X4_NOLOAD
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st1             {v2.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st1             {v2.d}[1], [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGB_C[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27"
            );
        }

        void logb1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM(
                // prepare
                LOGN_CORE_LOAD
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[dst]]")
                LOGB_CORE_X8_NOLOAD
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q2, q3, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count] , #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[dst]]")
                LOGB_CORE_X4_NOLOAD
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q2, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("8:")
                LOGB_CORE_X4_NOLOAD
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("sub             %[dst], %[dst], #0x04")
                __ASM_EMIT("st1             {v2.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st1             {v2.d}[1], [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGB_C[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27"
            );
        }

        void loge2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // prepare
                LOGN_CORE_LOAD
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src]]")
                LOGE_CORE_X8_NOLOAD
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count] , #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[src]]")
                LOGE_CORE_X4_NOLOAD
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
                __ASM_EMIT("8:")
                LOGE_CORE_X4_NOLOAD
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
                : [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27"
            );
        }

        void loge1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM(
                // prepare
                LOGN_CORE_LOAD
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[dst]]")
                LOGE_CORE_X8_NOLOAD
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count] , #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[dst]]")
                LOGE_CORE_X4_NOLOAD
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("8:")
                LOGE_CORE_X4_NOLOAD
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
                : [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27"
            );
        }

        void logd2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // prepare
                LOGN_CORE_LOAD
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src]]")
                LOGD_CORE_X8_NOLOAD
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count] , #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[src]]")
                LOGD_CORE_X4_NOLOAD
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
                __ASM_EMIT("8:")
                LOGD_CORE_X4_NOLOAD
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
                : [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGD_C[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27"
            );
        }

        void logd1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM(
                // prepare
                LOGN_CORE_LOAD
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[dst]]")
                LOGD_CORE_X8_NOLOAD
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count] , #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[dst]]")
                LOGD_CORE_X4_NOLOAD
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                // Tail: 1x-3x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("8:")
                LOGD_CORE_X4_NOLOAD
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
                : [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGD_C[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27"
            );
        }
    } /* namespace asimd */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LOG_H_ */
