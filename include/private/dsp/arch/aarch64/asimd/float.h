/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_FLOAT_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_FLOAT_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
    #define SEL_DST(a, b)       a
    #define SEL_NODST(a, b)     b

        IF_ARCH_AARCH64(
            static uint32_t SAT_IARGS[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7f800000),                   // X_P_INF
                LSP_DSP_VEC4(0xff800000),                   // X_N_INF
                LSP_DSP_VEC4(LSP_DSP_FLOAT_SAT_P_NAN_I),    // SX_P_NAN
                LSP_DSP_VEC4(LSP_DSP_FLOAT_SAT_P_INF_I),    // SX_P_INF
                LSP_DSP_VEC4(LSP_DSP_FLOAT_SAT_N_NAN_I),    // SX_N_NAN
                LSP_DSP_VEC4(LSP_DSP_FLOAT_SAT_N_INF_I)     // SX_N_INF
            };
        )

    #define MULTIPLE_SAT_BODY(DST, SRC, SEL) \
        __ASM_EMIT("ldp             q16, q17, [%[IARGS], #0x00]")       /* v16 = +inf, v17 = -inf */ \
        __ASM_EMIT("ldp             q18, q19, [%[IARGS], #0x20]")       /* v18 = X_P_NAN, v19 = X_P_INF */ \
        __ASM_EMIT("ldp             q20, q21, [%[IARGS], #0x40]")       /* v20 = X_N_NAN, v21 = X_N_INF */ \
        /* x8 blocks */ \
        __ASM_EMIT("subs            %[count], %[count], #8") \
        __ASM_EMIT("b.lo             2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp             q0, q1, [%[" SRC "]]")              /* v0   = s0..s3, v1 = s4..s7 */ \
        __ASM_EMIT("cmgt            v2.4s, v0.4s, v16.4s")              /* v2   = [s > +inf] */ \
        __ASM_EMIT("cmgt            v3.4s, v1.4s, v16.4s") \
        __ASM_EMIT("cmeq            v4.4s, v0.4s, v16.4s")              /* v4   = [s == +inf] */ \
        __ASM_EMIT("cmeq            v5.4s, v1.4s, v16.4s") \
        __ASM_EMIT("bit             v0.16b, v18.16b, v2.16b")           /* v0   = s' = X_P_NAN * [s > +inf] | s * [s <= +inf] */ \
        __ASM_EMIT("bit             v1.16b, v18.16b, v3.16b") \
        __ASM_EMIT("bit             v0.16b, v19.16b, v4.16b")           /* v0   = X_P_INF * [s == +inf] | s' * [s != +inf] */ \
        __ASM_EMIT("bit             v1.16b, v19.16b, v5.16b") \
        __ASM_EMIT("cmhi            v2.4s, v0.4s, v17.4s")              /* v2   = [s > -inf] */ \
        __ASM_EMIT("cmhi            v3.4s, v1.4s, v17.4s") \
        __ASM_EMIT("cmeq            v4.4s, v0.4s, v17.4s")              /* v4   = [s == -inf] */ \
        __ASM_EMIT("cmeq            v5.4s, v1.4s, v17.4s") \
        __ASM_EMIT("bit             v0.16b, v20.16b, v2.16b")           /* v0   = s' = X_N_NAN * [s > -inf] | s * [s <= -inf] */ \
        __ASM_EMIT("bit             v1.16b, v20.16b, v3.16b") \
        __ASM_EMIT("bit             v0.16b, v21.16b, v4.16b")           /* v0   = X_N_INF * [s == +inf] | s' * [s != +inf] */ \
        __ASM_EMIT("bit             v1.16b, v21.16b, v5.16b") \
        __ASM_EMIT("stp             q0, q1, [%[" DST "]]") \
        __ASM_EMIT("subs            %[count], %[count], #8") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x20") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x20", "")) \
        __ASM_EMIT("b.hs            1b") \
        /* x4 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds            %[count], %[count], #4") \
        __ASM_EMIT("b.lt            4f") \
        __ASM_EMIT("ldr             q0, [%[" SRC "]]")                  /* v0   = s0..s3 */ \
        __ASM_EMIT("cmgt            v2.4s, v0.4s, v16.4s")              /* v2   = [s > +inf] */ \
        __ASM_EMIT("cmeq            v4.4s, v0.4s, v16.4s")              /* v4   = [s == +inf] */ \
        __ASM_EMIT("bit             v0.16b, v18.16b, v2.16b")           /* v0   = s' = X_P_NAN * [s > +inf] | s * [s <= +inf] */ \
        __ASM_EMIT("bit             v0.16b, v19.16b, v4.16b")           /* v0   = X_P_INF * [s == +inf] | s' * [s != +inf] */ \
        __ASM_EMIT("cmhi            v2.4s, v0.4s, v17.4s")              /* v2   = [s > -inf] */ \
        __ASM_EMIT("cmeq            v4.4s, v0.4s, v17.4s")              /* v4   = [s == -inf] */ \
        __ASM_EMIT("bit             v0.16b, v20.16b, v2.16b")           /* v0   = s' = X_N_NAN * [s > -inf] | s * [s <= -inf] */ \
        __ASM_EMIT("bit             v0.16b, v21.16b, v4.16b")           /* v0   = X_N_INF * [s == +inf] | s' * [s != +inf] */ \
        __ASM_EMIT("str             q0, [%[" DST "]]") \
        __ASM_EMIT("sub             %[count], %[count], #4") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x10") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x10", "")) \
        __ASM_EMIT("4:") \
        /* x1 blocks */ \
        __ASM_EMIT("adds            %[count], %[count], #3") \
        __ASM_EMIT("b.lt            6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("ld1r            {v0.4s}, [%[" SRC "]]")             /* v0   = s */ \
        __ASM_EMIT("cmgt            v2.4s, v0.4s, v16.4s")              /* v2   = [s > +inf] */ \
        __ASM_EMIT("cmeq            v4.4s, v0.4s, v16.4s")              /* v4   = [s == +inf] */ \
        __ASM_EMIT("bit             v0.16b, v18.16b, v2.16b")           /* v0   = s' = X_P_NAN * [s > +inf] | s * [s <= +inf] */ \
        __ASM_EMIT("bit             v0.16b, v19.16b, v4.16b")           /* v0   = X_P_INF * [s == +inf] | s' * [s != +inf] */ \
        __ASM_EMIT("cmhi            v2.4s, v0.4s, v17.4s")              /* v2   = [s > -inf] */ \
        __ASM_EMIT("cmeq            v4.4s, v0.4s, v17.4s")              /* v4   = [s == -inf] */ \
        __ASM_EMIT("bit             v0.16b, v20.16b, v2.16b")           /* v0   = s' = X_N_NAN * [s > -inf] | s * [s <= -inf] */ \
        __ASM_EMIT("bit             v0.16b, v21.16b, v4.16b")           /* v0   = X_N_INF * [s == +inf] | s' * [s != +inf] */ \
        __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
        __ASM_EMIT("subs            %[count], %[count], #1") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x04") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x04", "")) \
        __ASM_EMIT("b.ge            5b") \
        __ASM_EMIT("6:")

        void copy_saturated(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM (
                MULTIPLE_SAT_BODY("dst", "src", SEL_DST)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [IARGS] "r" (&SAT_IARGS[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21"
            );
        }

        void saturate(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM(
                MULTIPLE_SAT_BODY("dst", "dst", SEL_NODST)
                : [dst] "+r" (dst), [count] "+r" (count)
                : [IARGS] "r" (&SAT_IARGS[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21"
            );
        }

    #undef MULTIPLE_SAT_BODY

    #define LIMIT_SAT_BODY(DST, SRC, SEL) \
        __ASM_EMIT("ldp             q16, q17, [%[XC], #0x00]")      /* v16 = abs, v17 = +1 */ \
        __ASM_EMIT("ldp             q18, q19, [%[XC], #0x20]")      /* v18 = sign, v19 = +Inf */ \
        /* x8 blocks */ \
        __ASM_EMIT("subs            %[count], %[count], #8") \
        __ASM_EMIT("b.lo            2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp             q0, q1, [%[" SRC "]]")          /* v0 = s0, v1 = s1 */ \
        __ASM_EMIT("and             v6.16b, v0.16b, v16.16b")       /* v6 = abs(s) */ \
        __ASM_EMIT("and             v7.16b, v1.16b, v16.16b") \
        __ASM_EMIT("fcmge           v2.4s, v17.4s, v6.4s")          /* v2 = [ abs(s) <= +1 ] */ \
        __ASM_EMIT("fcmge           v3.4s, v17.4s, v7.4s") \
        __ASM_EMIT("and             v4.16b, v0.16b, v18.16b")       /* v4 = sign(s) */ \
        __ASM_EMIT("and             v5.16b, v1.16b, v18.16b") \
        __ASM_EMIT("cmge            v6.4s, v19.4s, v6.4s")          /* v6 = [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("cmge            v7.4s, v19.4s, v7.4s") \
        __ASM_EMIT("orr             v4.16b, v4.16b, v17.16b")       /* v4 = +1 * sign(s) */ \
        __ASM_EMIT("orr             v5.16b, v5.16b, v17.16b") \
        __ASM_EMIT("and             v4.16b, v4.16b, v6.16b")        /* v4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("and             v5.16b, v5.16b, v7.16b") \
        __ASM_EMIT("bif             v0.16b, v4.16b, v2.16b")        /* v0 = ( s & [ abs(s) <= +1 ]) | ( r & [ abs(s) > +1 ]) */ \
        __ASM_EMIT("bif             v1.16b, v5.16b, v3.16b") \
        __ASM_EMIT("stp             q0, q1, [%[" DST "]]") \
        __ASM_EMIT("subs            %[count], %[count], #8") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x20") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x20", "")) \
        __ASM_EMIT("b.hs            1b") \
        __ASM_EMIT("2:") \
        /* x4 block */ \
        __ASM_EMIT("adds            %[count], %[count], #4") \
        __ASM_EMIT("b.lt            4f") \
        __ASM_EMIT("ldr             q0, [%[" SRC "]]")              /* v0 = s0, v1 = s1 */ \
        __ASM_EMIT("and             v6.16b, v0.16b, v16.16b")       /* v6 = abs(s) */ \
        __ASM_EMIT("fcmge           v2.4s, v17.4s, v6.4s")          /* v2 = [ abs(s) <= +1 ] */ \
        __ASM_EMIT("and             v4.16b, v0.16b, v18.16b")       /* v4 = sign(s) */ \
        __ASM_EMIT("cmge            v6.4s, v19.4s, v6.4s")          /* v6 = [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("orr             v4.16b, v4.16b, v17.16b")       /* v4 = +1 * sign(s) */ \
        __ASM_EMIT("and             v4.16b, v4.16b, v6.16b")        /* v4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("bif             v0.16b, v4.16b, v2.16b")        /* v0 = ( s & [ abs(s) <= +1 ]) | ( r & [ abs(s) > +1 ]) */ \
        __ASM_EMIT("str             q0, [%[" DST "]]") \
        __ASM_EMIT("sub             %[count], %[count], #4") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x10") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x10", "")) \
        __ASM_EMIT("4:") \
        /* 1x blocks */ \
        __ASM_EMIT("adds            %[count], %[count], #3") \
        __ASM_EMIT("b.lt            6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("ld1r            {v0.4s}, [%[" SRC "]]")         /* v0 = s0, v1 = s1 */ \
        __ASM_EMIT("and             v6.16b, v0.16b, v16.16b")       /* v6 = abs(s) */ \
        __ASM_EMIT("fcmge           v2.4s, v17.4s, v6.4s")          /* v2 = [ abs(s) <= +1 ] */ \
        __ASM_EMIT("and             v4.16b, v0.16b, v18.16b")       /* v4 = sign(s) */ \
        __ASM_EMIT("cmge            v6.4s, v19.4s, v6.4s")          /* v6 = [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("orr             v4.16b, v4.16b, v17.16b")       /* v4 = +1 * sign(s) */ \
        __ASM_EMIT("and             v4.16b, v4.16b, v6.16b")        /* v4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("bif             v0.16b, v4.16b, v2.16b")        /* v0 = ( s & [ abs(s) <= +1 ]) | ( r & [ abs(s) > +1 ]) */ \
        __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
        __ASM_EMIT("subs            %[count], %[count], #1") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x04") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x04", "")) \
        __ASM_EMIT("b.ge            5b") \
        __ASM_EMIT("6:")

        IF_ARCH_AARCH64(
            static uint32_t XLIM_SAT[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7fffffff),      // abs
                LSP_DSP_VEC4(0x3f800000),      // +1
                LSP_DSP_VEC4(0x80000000),      // sign
                LSP_DSP_VEC4(0x7f800000)       // +Inf
            };
        )

        void limit_saturate1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM(
                LIMIT_SAT_BODY("dst", "dst", SEL_NODST)
                : [dst] "+r" (dst), [count] "+r" (count)
                : [XC] "r" (&XLIM_SAT[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

        void limit_saturate2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                LIMIT_SAT_BODY("dst", "src", SEL_DST)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [XC] "r" (&XLIM_SAT[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

    #undef LIMIT_SAT_BODY

    #define LIMIT_BODY(DST, SRC, SEL) \
        __ASM_EMIT("dup             v16.4s, %[min].s[0]")           /* v16  = min */ \
        __ASM_EMIT("dup             v17.4s, %[max].s[0]")           /* v17  = max */ \
        /* x16 blocks */ \
        __ASM_EMIT("subs            %[count], %[count], #16") \
        __ASM_EMIT("b.lo            2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp             q0, q1, [%[" SRC "], #0x00]")   /* v0 = s0, v1 = s1 */ \
        __ASM_EMIT("ldp             q2, q3, [%[" SRC "], #0x20]")   /* v0 = s0, v1 = s1 */ \
        __ASM_EMIT("fcmge           v4.4s, v0.4s, v16.4s")          /* v4 = c = [ (s >= min) && (!isnan(s)) ] */ \
        __ASM_EMIT("fcmge           v5.4s, v1.4s, v16.4s") \
        __ASM_EMIT("fcmge           v6.4s, v2.4s, v16.4s") \
        __ASM_EMIT("fcmge           v7.4s, v3.4s, v16.4s") \
        __ASM_EMIT("bif             v0.16b, v16.16b, v4.16b")       /* v0 = s*c | min*(!c) */ \
        __ASM_EMIT("bif             v1.16b, v16.16b, v5.16b") \
        __ASM_EMIT("bif             v2.16b, v16.16b, v6.16b") \
        __ASM_EMIT("bif             v3.16b, v16.16b, v7.16b") \
        __ASM_EMIT("fcmge           v4.4s, v17.4s, v0.4s")          /* v4 = c = [ (s <= max) && (!isnan(s)) ] */ \
        __ASM_EMIT("fcmge           v5.4s, v17.4s, v1.4s") \
        __ASM_EMIT("fcmge           v6.4s, v17.4s, v2.4s") \
        __ASM_EMIT("fcmge           v7.4s, v17.4s, v3.4s") \
        __ASM_EMIT("bif             v0.16b, v17.16b, v4.16b")       /* v0 = s*c | min*(!c) */ \
        __ASM_EMIT("bif             v1.16b, v17.16b, v5.16b") \
        __ASM_EMIT("bif             v2.16b, v17.16b, v6.16b") \
        __ASM_EMIT("bif             v3.16b, v17.16b, v7.16b") \
        __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT("stp             q2, q3, [%[" DST "], #0x20]") \
        __ASM_EMIT("subs            %[count], %[count], #16") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x40") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x40", "")) \
        __ASM_EMIT("b.hs            1b") \
        __ASM_EMIT("2:") \
        /* x8 block */ \
        __ASM_EMIT("adds            %[count], %[count], #8") \
        __ASM_EMIT("b.lt            4f") \
        __ASM_EMIT("ldp             q0, q1, [%[" SRC "], #0x00]")   /* v0 = s0, v1 = s1 */ \
        __ASM_EMIT("fcmge           v4.4s, v0.4s, v16.4s")          /* v4 = c = [ (s >= min) && (!isnan(s)) ] */ \
        __ASM_EMIT("fcmge           v5.4s, v1.4s, v16.4s") \
        __ASM_EMIT("bif             v0.16b, v16.16b, v4.16b")       /* v0 = s*c | min*(!c) */ \
        __ASM_EMIT("bif             v1.16b, v16.16b, v5.16b") \
        __ASM_EMIT("fcmge           v4.4s, v17.4s, v0.4s")          /* v4 = c = [ (s <= max) && (!isnan(s)) ] */ \
        __ASM_EMIT("fcmge           v5.4s, v17.4s, v1.4s") \
        __ASM_EMIT("bif             v0.16b, v17.16b, v4.16b")       /* v0 = s*c | min*(!c) */ \
        __ASM_EMIT("bif             v1.16b, v17.16b, v5.16b") \
        __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT("sub             %[count], %[count], #8") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x20") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x20", "")) \
        __ASM_EMIT("4:") \
        /* x4 block */ \
        __ASM_EMIT("adds            %[count], %[count], #4") \
        __ASM_EMIT("b.lt            6f") \
        __ASM_EMIT("ldr             q0, [%[" SRC "]]")              /* v0 = s0 */ \
        __ASM_EMIT("fcmge           v4.4s, v0.4s, v16.4s")          /* v4 = c = [ (s >= min) && (!isnan(s)) ] */ \
        __ASM_EMIT("bif             v0.16b, v16.16b, v4.16b")       /* v0 = s*c | min*(!c) */ \
        __ASM_EMIT("fcmge           v4.4s, v17.4s, v0.4s")          /* v4 = c = [ (s <= max) && (!isnan(s)) ] */ \
        __ASM_EMIT("bif             v0.16b, v17.16b, v4.16b")       /* v0 = s*c | min*(!c) */ \
        __ASM_EMIT("str             q0, [%[" DST "]]") \
        __ASM_EMIT("sub             %[count], %[count], #4") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x10") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x10", "")) \
        __ASM_EMIT("6:") \
        /* 1x blocks */ \
        __ASM_EMIT("adds            %[count], %[count], #3") \
        __ASM_EMIT("b.lt            8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("ld1r            {v0.4s}, [%[" SRC "]]")         /* v0 = s0 */ \
        __ASM_EMIT("fcmge           v4.4s, v0.4s, v16.4s")          /* v4 = c = [ (s >= min) && (!isnan(s)) ] */ \
        __ASM_EMIT("bif             v0.16b, v16.16b, v4.16b")       /* v0 = s*c | min*(!c) */ \
        __ASM_EMIT("fcmge           v4.4s, v17.4s, v0.4s")          /* v4 = c = [ (s <= max) && (!isnan(s)) ] */ \
        __ASM_EMIT("bif             v0.16b, v17.16b, v4.16b")       /* v0 = s*c | min*(!c) */ \
        __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
        __ASM_EMIT("subs            %[count], %[count], #1") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x04") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x04", "")) \
        __ASM_EMIT("b.ge            7b") \
        __ASM_EMIT("8:")

        void limit1(float *dst, float min, float max, size_t count)
        {
            ARCH_AARCH64_ASM(
                LIMIT_BODY("dst", "dst", SEL_NODST)
                : [dst] "+r" (dst), [count] "+r" (count),
                  [min] "+w" (min), [max] "+w" (max)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17"
            );
        }

        void limit2(float *dst, const float *src, float min, float max, size_t count)
        {
            ARCH_AARCH64_ASM(
                LIMIT_BODY("dst", "src", SEL_DST)
                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
                  [min] "+w" (min), [max] "+w" (max)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17"
            );
        }

    #undef LIMIT_BODY

    #define SANITIZE_BODY(DST, SRC, SEL) \
        __ASM_EMIT("ldp             q16, q17, [%[CVAL], #0x00]") \
        __ASM_EMIT("ldp             q18, q19, [%[CVAL], #0x20]") \
        __ASM_EMIT("subs            %[count], %[count], #8") \
        __ASM_EMIT("b.lo            2f") \
        /* 8x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp             q0, q1, [%[" SRC "]]")                  /* v0 = s */ \
        __ASM_EMIT("and             v2.16b, v0.16b, v16.16b")               /* v2 = abs(s) */ \
        __ASM_EMIT("and             v3.16b, v1.16b, v16.16b") \
        __ASM_EMIT("and             v4.16b, v0.16b, v17.16b")               /* v4 = sign(s) */ \
        __ASM_EMIT("and             v5.16b, v1.16b, v17.16b") \
        __ASM_EMIT("cmge            v6.4s, v18.4s, v2.4s")                  /* v6 = abs(s) <= X_MAX */ \
        __ASM_EMIT("cmge            v7.4s, v18.4s, v3.4s") \
        __ASM_EMIT("cmgt            v2.4s, v2.4s, v19.4s")                  /* v2 = abs(s) > X_MIN */ \
        __ASM_EMIT("cmgt            v3.4s, v3.4s, v19.4s") \
        __ASM_EMIT("and             v2.16b, v2.16b, v6.16b")                /* v2 = (abs(s) > X_MIN) & (abs(s) <= X_MIN) */ \
        __ASM_EMIT("and             v3.16b, v3.16b, v7.16b") \
        __ASM_EMIT("bif             v0.16b, v4.16b, v2.16b")                /* v0 = ((abs(s) > X_MIN) & (abs(s) <= X_MIN)) ? s : sign(s) */ \
        __ASM_EMIT("bif             v1.16b, v5.16b, v3.16b") \
        __ASM_EMIT("stp             q0, q1, [%[" DST "]]")                  /* q0 = s */ \
        __ASM_EMIT("subs            %[count], %[count], #8") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x20") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x20", "")) \
        __ASM_EMIT("b.hs            1b") \
        /* 4x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds            %[count], %[count], #4") \
        __ASM_EMIT("b.lt            4f") \
        __ASM_EMIT("ldr             q0, [%[" SRC "]]")                      /* v0 = s */ \
        __ASM_EMIT("and             v2.16b, v0.16b, v16.16b")               /* v2 = abs(s) */ \
        __ASM_EMIT("and             v4.16b, v0.16b, v17.16b")               /* v4 = sign(s) */ \
        __ASM_EMIT("cmge            v6.4s, v18.4s, v2.4s")                  /* v6 = abs(s) <= X_MAX */ \
        __ASM_EMIT("cmgt            v2.4s, v2.4s, v19.4s")                  /* v2 = abs(s) > X_MIN */ \
        __ASM_EMIT("and             v2.16b, v2.16b, v6.16b")                /* v2 = (abs(s) > X_MIN) & (abs(s) <= X_MIN) */ \
        __ASM_EMIT("bif             v0.16b, v4.16b, v2.16b")                /* v0 = ((abs(s) > X_MIN) & (abs(s) <= X_MIN)) ? s : sign(s) */ \
        __ASM_EMIT("str             q0, [%[" DST "]]")                      /* q0 = s */ \
        __ASM_EMIT("sub             %[count], %[count], #4") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x10") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x10", "")) \
        /* 1x blocks */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds            %[count], %[count], #3") \
        __ASM_EMIT("b.lt            6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("ld1r            {v0.4s}, [%[" SRC "]]")                 /* v0 = s */ \
        __ASM_EMIT("and             v2.16b, v0.16b, v16.16b")               /* v2 = abs(s) */ \
        __ASM_EMIT("and             v4.16b, v0.16b, v17.16b")               /* v4 = sign(s) */ \
        __ASM_EMIT("cmge            v6.4s, v18.4s, v2.4s")                  /* v6 = abs(s) <= X_MAX */ \
        __ASM_EMIT("cmgt            v2.4s, v2.4s, v19.4s")                  /* v2 = abs(s) > X_MIN */ \
        __ASM_EMIT("and             v2.16b, v2.16b, v6.16b")                /* v2 = (abs(s) > X_MIN) & (abs(s) <= X_MIN) */ \
        __ASM_EMIT("bif             v0.16b, v4.16b, v2.16b")                /* v0 = ((abs(s) > X_MIN) & (abs(s) <= X_MIN)) ? s : sign(s) */ \
        __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]")               /* q0 = s */ \
        __ASM_EMIT("subs            %[count], %[count], #1") \
        __ASM_EMIT("add             %[" SRC "], %[" SRC "], #0x04") \
        __ASM_EMIT(SEL("add         %[" DST "], %[" DST "], #0x04", "")) \
        __ASM_EMIT("b.ge            5b") \
        /* end */ \
        __ASM_EMIT("6:")

        IF_ARCH_AARCH64(
            static uint32_t SANITIZE_CVAL[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7fffffff),            // X_ABS
                LSP_DSP_VEC4(0x80000000),            // X_SIGN
                LSP_DSP_VEC4(0x7f7fffff),            // X_MAX
                LSP_DSP_VEC4(0x007fffff)             // X_MIN
            };
        )

        void sanitize1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                SANITIZE_BODY("dst", "dst", SEL_NODST)
                : [dst] "+r" (dst),
                  [count] "+r" (count)
                : [CVAL] "r" (&SANITIZE_CVAL[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

        void sanitize2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                SANITIZE_BODY("dst", "src", SEL_DST)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CVAL] "r" (&SANITIZE_CVAL[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

    #undef SANITIZE_BODY

    #undef SEL_DST
    #undef SEL_NODST
    }
}

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_FLOAT_H_ */
