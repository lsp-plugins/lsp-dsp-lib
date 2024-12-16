/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_SEARCH_MINMAX_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_SEARCH_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
    #define SEL_NONE(a, b)              a
    #define SEL_ABS(a, b)               b

    #define MINMAX_SEARCH_CORE(OP, SEL) \
        __ASM_EMIT("subs        %[count], #1") \
        __ASM_EMIT("blo         12f") \
        /* x32 blocks */ \
        __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!") \
        __ASM_EMIT(SEL("", "vabs.f32    q0, q0")) \
        __ASM_EMIT("subs        %[count], #32") \
        __ASM_EMIT("vmov        q1, q0") \
        __ASM_EMIT("blt         2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(SEL("", "vabs.f32    q10, q10")) \
        __ASM_EMIT(SEL("", "vabs.f32    q11, q11")) \
        __ASM_EMIT(SEL("", "vabs.f32    q12, q12")) \
        __ASM_EMIT(SEL("", "vabs.f32    q13, q13")) \
        __ASM_EMIT(SEL("", "vabs.f32    q14, q14")) \
        __ASM_EMIT(SEL("", "vabs.f32    q15, q15")) \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT(OP "         q1, q1, q9") \
        __ASM_EMIT(OP "         q0, q0, q10") \
        __ASM_EMIT(OP "         q1, q1, q11") \
        __ASM_EMIT(OP "         q0, q0, q12") \
        __ASM_EMIT(OP "         q1, q1, q13") \
        __ASM_EMIT(OP "         q0, q0, q14") \
        __ASM_EMIT(OP "         q1, q1, q15") \
        __ASM_EMIT("subs        %[count], #32") \
        __ASM_EMIT("bhs         1b") \
        __ASM_EMIT("2:") \
        /* x16 block */ \
        __ASM_EMIT("adds        %[count], #16") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q11}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(SEL("", "vabs.f32    q10, q10")) \
        __ASM_EMIT(SEL("", "vabs.f32    q11, q11")) \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT(OP "         q1, q1, q9") \
        __ASM_EMIT(OP "         q0, q0, q10") \
        __ASM_EMIT(OP "         q1, q1, q11") \
        __ASM_EMIT("sub         %[count], #16") \
        __ASM_EMIT("4:") \
        /* x8 block */ \
        __ASM_EMIT("adds        %[count], #8") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q9}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT(OP "         q1, q1, q9") \
        __ASM_EMIT("sub         %[count], #8") \
        __ASM_EMIT("6:") \
        __ASM_EMIT(OP "         q0, q0, q1") \
        /* x4 block */ \
        __ASM_EMIT("adds        %[count], #4") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vldm        %[src]!, {q8}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT("sub         %[count], #4") \
        __ASM_EMIT("8:") \
        /* x1 block */ \
        __ASM_EMIT("adds        %[count], #3") \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[src]]!") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(OP "         q0, q0, q8")  \
        __ASM_EMIT("subs        %[count], #1") \
        __ASM_EMIT("bge         9b") \
        __ASM_EMIT("10:") \
        /* end */ \
        __ASM_EMIT("vext.32     q8, q0, q0, #2") \
        __ASM_EMIT(OP "         q0, q0, q8")  \
        __ASM_EMIT("vext.32     q8, q0, q0, #1") \
        __ASM_EMIT(OP "         q0, q0, q8") \
        __ASM_EMIT("vmov        %[res], s0") \
        __ASM_EMIT("12:")

        float min(const float *src, size_t count)
        {
            float res = 0.0f;
            ARCH_ARM_ASM(
                MINMAX_SEARCH_CORE("vmin.f32", SEL_NONE)
                : [count] "+r" (count), [src] "+r" (src),
                  [res] "+t" (res)
                :
                : "cc",
                  "q1",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
            return res;
        }

        float max(const float *src, size_t count)
        {
            float res = 0.0f;
            ARCH_ARM_ASM(
                MINMAX_SEARCH_CORE("vmax.f32", SEL_NONE)
                : [count] "+r" (count), [src] "+r" (src),
                  [res] "+t" (res)
                :
                : "cc",
                  "q1",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
            return res;
        }

        float abs_min(const float *src, size_t count)
        {
            float res = 0.0f;
            ARCH_ARM_ASM(
                MINMAX_SEARCH_CORE("vmin.f32", SEL_ABS)
                : [count] "+r" (count), [src] "+r" (src),
                  [res] "+t" (res)
                :
                : "cc",
                  "q1",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
            return res;
        }

        float abs_max(const float *src, size_t count)
        {
            float res = 0.0f;
            ARCH_ARM_ASM(
                MINMAX_SEARCH_CORE("vmax.f32", SEL_ABS)
                : [count] "+r" (count), [src] "+r" (src),
                  [res] "+t" (res)
                :
                : "cc",
                  "q1",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
            return res;
        }

    #undef MINMAX_SEARCH_CORE

    #define SIGN_SEARCH_CORE(OP) \
        __ASM_EMIT("cmp         %[count], #0") \
        __ASM_EMIT("bls         8f") \
        __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]")    /* q0 = res0 */ \
        __ASM_EMIT("vabs.f32    q2, q0")                    /* q2 = abs(res0) */ \
        __ASM_EMIT("subs        %[count], #16") \
        __ASM_EMIT("vmov        q1, q0")                    /* q1 = res1 */ \
        __ASM_EMIT("vmov        q3, q2")                    /* q3 = abs(res1) */ \
        __ASM_EMIT("blt         2f") \
        /* x16 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q8-q11}")         /* q8 = x0, q9 = x1, q10 = x2, q11 = x3 */ \
        __ASM_EMIT("vabs.f32    q12, q8")                   /* q12 = abs(x0) */ \
        __ASM_EMIT("vabs.f32    q13, q9")                   /* q13 = abs(x1) */ \
        __ASM_EMIT("vabs.f32    q14, q10")                  /* q14 = abs(x2) */ \
        __ASM_EMIT("vabs.f32    q15, q11")                  /* q15 = abs(x3) */ \
        __ASM_EMIT(OP "         q4, q2, q12")               /* q4 = [abs(res0) <=> abs(x0) ] */ \
        __ASM_EMIT(OP "         q5, q3, q13")               /* q5 = [abs(res1) <=> abs(x1) ] */ \
        __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(res0) <=> abs(x0) ] ? res0 : x0 */ \
        __ASM_EMIT("vbif        q1, q9, q5")                /* q1 = [abs(res1) <=> abs(x1) ] ? res0 : x0 */ \
        __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(res0) <=> abs(x0) ] ? abs(res0) : abs(x0) */ \
        __ASM_EMIT("vbif        q3, q13, q5")               /* q13 = [abs(res1) <=> abs(x1) ] ? abs(res1) : abs(x1) */ \
        __ASM_EMIT(OP "         q4, q2, q14")               /* q4 = [abs(res0) <=> abs(x2) ] */ \
        __ASM_EMIT(OP "         q5, q3, q15")               /* q5 = [abs(res1) <=> abs(x3) ] */ \
        __ASM_EMIT("vbif        q0, q10, q4")               /* q0 = [abs(res0) <=> abs(x2) ] ? res0 : x2 */ \
        __ASM_EMIT("vbif        q1, q11, q5")               /* q1 = [abs(res1) <=> abs(x3) ] ? res0 : x3 */ \
        __ASM_EMIT("vbif        q2, q14, q4")               /* q12 = [abs(res0) <=> abs(x2) ] ? abs(res0) : abs(x2) */ \
        __ASM_EMIT("vbif        q3, q15, q5")               /* q13 = [abs(res1) <=> abs(x3) ] ? abs(res1) : abs(x3) */ \
        __ASM_EMIT("subs        %[count], #16") \
        __ASM_EMIT("bhs         1b") \
        __ASM_EMIT("2:") \
        /* x8 block */ \
        __ASM_EMIT("adds        %[count], #8") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q9}")          /* q8 = x0, q9 = x1 */ \
        __ASM_EMIT("vabs.f32    q12, q8")                   /* q12 = abs(x0) */ \
        __ASM_EMIT("vabs.f32    q13, q9")                   /* q13 = abs(x1) */ \
        __ASM_EMIT(OP "         q4, q2, q12")               /* q4 = [abs(res0) <=> abs(x0) ] */ \
        __ASM_EMIT(OP "         q5, q3, q13")               /* q5 = [abs(res1) <=> abs(x1) ] */ \
        __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(res0) <=> abs(x0) ] ? res0 : x0 */ \
        __ASM_EMIT("vbif        q1, q9, q5")                /* q1 = [abs(res1) <=> abs(x1) ] ? res0 : x0 */ \
        __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(res0) <=> abs(x0) ] ? abs(res0) : abs(x0) */ \
        __ASM_EMIT("vbif        q3, q13, q5")               /* q13 = [abs(res1) <=> abs(x1) ] ? abs(res1) : abs(x1) */ \
        __ASM_EMIT("sub         %[count], #8") \
        __ASM_EMIT("4:") \
        __ASM_EMIT(OP "         q4, q2, q3")                /* q3 = [abs(res0) <=> abs(res1) ] */ \
        __ASM_EMIT("vbif        q0, q1, q4")                /* q0 = [abs(res0) <=> abs(res1) ] ? res0 : res1 */ \
        __ASM_EMIT("vbif        q2, q3, q4")                /* q1 = [abs(res1) <=> abs(res1) ] ? abs(res0) : abs(res1) */ \
        /* x4 block */ \
        __ASM_EMIT("adds        %[count], #4") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src]!, {q8}")             /* q8 = x0 */ \
        __ASM_EMIT("vabs.f32    q12, q8")                   /* q12 = abs(x0) */ \
        __ASM_EMIT(OP "         q4, q2, q12")               /* q4 = [abs(res0) <=> abs(x0) ] */ \
        __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(res0) <=> abs(x0) ] ? res0 : x0 */ \
        __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(res0) <=> abs(x0) ] ? abs(res0) : abs(x0) */ \
        __ASM_EMIT("sub         %[count], #4") \
        __ASM_EMIT("6:") \
        __ASM_EMIT("vext.32     q8, q0, q0, #2")            /* q8 = x0 */ \
        __ASM_EMIT("vext.32     q12, q2, q2, #2")           /* q12 = abs(x0) */  \
        __ASM_EMIT(OP "         q4, q2, q12")               /* q4 = [abs(res0) <=> abs(x0) ] */ \
        __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(res0) <=> abs(x0) ] ? res0 : x0 */ \
        __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(res0) <=> abs(x0) ] ? abs(res0) : abs(x0) */ \
        __ASM_EMIT("vext.32     q8, q0, q0, #1")            /* q8 = x0 */ \
        __ASM_EMIT("vext.32     q12, q2, q2, #1")           /* q12 = abs(x0) */  \
        __ASM_EMIT(OP "         q4, q2, q12")               /* q4 = [abs(res0) <=> abs(x0) ] */ \
        __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(res0) <=> abs(x0) ] ? res0 : x0 */ \
        __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(res0) <=> abs(x0) ] ? abs(res0) : abs(x0) */ \
        /* x1 block */ \
        __ASM_EMIT("adds        %[count], #3") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[src]]!") \
        __ASM_EMIT("vabs.f32    q12, q8")                   /* q12 = abs(x0) */ \
        __ASM_EMIT(OP "         q4, q2, q12")               /* q4 = [abs(res0) <=> abs(x0) ] */ \
        __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(res0) <=> abs(x0) ] ? res0 : x0 */ \
        __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(res0) <=> abs(x0) ] ? abs(res0) : abs(x0) */ \
        __ASM_EMIT("subs        %[count], #1") \
        __ASM_EMIT("bge         7b") \
        __ASM_EMIT("8:") \
        /* end */ \
        __ASM_EMIT("vmov        %[res], s0")

        float sign_min(const float *src, size_t count)
        {
            float res = 0.0f;
            ARCH_ARM_ASM(
                SIGN_SEARCH_CORE("vclt.f32")
                : [count] "+r" (count), [src] "+r" (src),
                  [res] "+t" (res)
                :
                : "cc",
                  "q1", "q2", "q3", "q4", "q5",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
            return res;
        }

        float sign_max(const float *src, size_t count)
        {
            float res = 0.0f;
            ARCH_ARM_ASM(
                SIGN_SEARCH_CORE("vcgt.f32")
                : [count] "+r" (count), [src] "+r" (src),
                  [res] "+t" (res)
                :
                : "cc",
                  "q1", "q2", "q3", "q4", "q5",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
            return res;
        }

    #undef SIGN_SEARCH_CORE

    #define MINMAX2_SEARCH_CORE(SEL) \
        __ASM_EMIT("veor        q0, q0") \
        __ASM_EMIT("veor        q1, q0") \
        __ASM_EMIT("subs        %[count], #1") \
        __ASM_EMIT("blo         12f") \
        /* x32 blocks */ \
        __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!") \
        __ASM_EMIT(SEL("", "vabs.f32    q0, q0")) \
        __ASM_EMIT("subs        %[count], #32") \
        __ASM_EMIT("vmov        q1, q0") \
        __ASM_EMIT("blt         2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src]!, {q8-q15}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(SEL("", "vabs.f32    q10, q10")) \
        __ASM_EMIT(SEL("", "vabs.f32    q11, q11")) \
        __ASM_EMIT(SEL("", "vabs.f32    q12, q12")) \
        __ASM_EMIT(SEL("", "vabs.f32    q13, q13")) \
        __ASM_EMIT(SEL("", "vabs.f32    q14, q14")) \
        __ASM_EMIT(SEL("", "vabs.f32    q15, q15")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q12") \
        __ASM_EMIT("vmin.f32    q0, q0, q9") \
        __ASM_EMIT("vmax.f32    q1, q1, q13") \
        __ASM_EMIT("vmin.f32    q0, q0, q10") \
        __ASM_EMIT("vmax.f32    q1, q1, q14") \
        __ASM_EMIT("vmin.f32    q0, q0, q11") \
        __ASM_EMIT("vmax.f32    q1, q1, q15") \
        __ASM_EMIT("vmin.f32    q0, q0, q12") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("vmin.f32    q0, q0, q13") \
        __ASM_EMIT("vmax.f32    q1, q1, q9") \
        __ASM_EMIT("vmin.f32    q0, q0, q14") \
        __ASM_EMIT("vmax.f32    q1, q1, q10") \
        __ASM_EMIT("vmin.f32    q0, q0, q15") \
        __ASM_EMIT("vmax.f32    q1, q1, q11") \
        __ASM_EMIT("subs        %[count], #32") \
        __ASM_EMIT("bhs         1b") \
        __ASM_EMIT("2:") \
        /* x16 block */ \
        __ASM_EMIT("adds        %[count], #16") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q11}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT(SEL("", "vabs.f32    q10, q10")) \
        __ASM_EMIT(SEL("", "vabs.f32    q11, q11")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q10") \
        __ASM_EMIT("vmin.f32    q0, q0, q9") \
        __ASM_EMIT("vmax.f32    q1, q1, q11") \
        __ASM_EMIT("vmin.f32    q0, q0, q10") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("vmin.f32    q0, q0, q11") \
        __ASM_EMIT("vmax.f32    q1, q1, q9") \
        __ASM_EMIT("sub         %[count], #16") \
        __ASM_EMIT("4:") \
        /* x8 block */ \
        __ASM_EMIT("adds        %[count], #8") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src]!, {q8-q9}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT(SEL("", "vabs.f32    q9, q9")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q9") \
        __ASM_EMIT("vmin.f32    q0, q0, q9") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("sub         %[count], #8") \
        __ASM_EMIT("6:") \
        /* x4 block */ \
        __ASM_EMIT("adds        %[count], #4") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vldm        %[src]!, {q8}") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("sub         %[count], #4") \
        __ASM_EMIT("8:") \
        /* x1 block */ \
        __ASM_EMIT("adds        %[count], #3") \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[src]]!") \
        __ASM_EMIT(SEL("", "vabs.f32    q8, q8")) \
        __ASM_EMIT("vmin.f32    q0, q0, q8") \
        __ASM_EMIT("vmax.f32    q1, q1, q8") \
        __ASM_EMIT("subs        %[count], #1") \
        __ASM_EMIT("bge         9b") \
        __ASM_EMIT("10:") \
        /* end */ \
        __ASM_EMIT("vext.32     q8, q0, q0, #2") \
        __ASM_EMIT("vext.32     q9, q1, q1, #2") \
        __ASM_EMIT("vmin.f32    q0, q0, q8")  \
        __ASM_EMIT("vmax.f32    q1, q1, q9")  \
        __ASM_EMIT("vext.32     q8, q0, q0, #1") \
        __ASM_EMIT("vext.32     q9, q1, q1, #1") \
        __ASM_EMIT("vmin.f32    q0, q0, q8")  \
        __ASM_EMIT("vmax.f32    q1, q1, q9")  \
        __ASM_EMIT("12:") \
        __ASM_EMIT("vst1.32     {d0[0]}, [%[min]]") \
        __ASM_EMIT("vst1.32     {d2[0]}, [%[max]]")

        void minmax(const float *src, size_t count, float *min, float *max)
        {
            ARCH_ARM_ASM(
                MINMAX2_SEARCH_CORE(SEL_NONE)
                : [count] "+r" (count), [src] "+r" (src)
                : [min] "r" (min), [max] "r" (max)
                : "cc",
                  "q0", "q1",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        void abs_minmax(const float *src, size_t count, float *min, float *max)
        {
            ARCH_ARM_ASM(
                MINMAX2_SEARCH_CORE(SEL_ABS)
                : [count] "+r" (count), [src] "+r" (src)
                : [min] "r" (min), [max] "r" (max)
                : "cc",
                  "q0", "q1",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

    #undef SEL_NONE
    #undef SEL_ABS
    #undef MINMAX2_SEARCH_CORE

        void sign_minmax(const float *src, size_t count, float *min, float *max)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("veor        q0, q0") \
                __ASM_EMIT("veor        q1, q0") \
                __ASM_EMIT("cmp         %[count], #0") \
                __ASM_EMIT("bls         8f") \
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]")    /* q0 = min */ \
                __ASM_EMIT("vabs.f32    q2, q0")                    /* q2 = abs(min) */ \
                __ASM_EMIT("vmov        q1, q0")                    /* q1 = max */ \
                __ASM_EMIT("vmov        q3, q2")                    /* q3 = abs(max) */ \
                __ASM_EMIT("subs        %[count], #16") \
                __ASM_EMIT("blt         2f") \
                /* x16 blocks */ \
                __ASM_EMIT("1:") \
                __ASM_EMIT("vldm        %[src]!, {q8-q11}")         /* q8 = x0, q9 = x1, q10 = x2, q11 = x3 */ \
                __ASM_EMIT("vabs.f32    q12, q8")                   /* q12 = abs(x0) */ \
                __ASM_EMIT("vabs.f32    q13, q9")                   /* q13 = abs(x1) */ \
                __ASM_EMIT("vabs.f32    q14, q10")                  /* q14 = abs(x2) */ \
                __ASM_EMIT("vabs.f32    q15, q11")                  /* q15 = abs(x3) */ \
                __ASM_EMIT("vclt.f32    q4, q2, q12")               /* q4 = [abs(min) < abs(x0) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q12")               /* q4 = [abs(max) > abs(x0) ] */ \
                __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(min) < abs(x0) ] ? min : x0 */ \
                __ASM_EMIT("vbif        q1, q8, q5")                /* q1 = [abs(max) > abs(x0) ] ? res : x0 */ \
                __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(min) < abs(x0) ] ? abs(min) : abs(x0) */ \
                __ASM_EMIT("vbif        q3, q12, q5")               /* q13 = [abs(max) > abs(x0) ] ? abs(max) : abs(x0) */ \
                __ASM_EMIT("vclt.f32    q4, q2, q13")               /* q4 = [abs(min) < abs(x1) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q13")               /* q4 = [abs(max) > abs(x1) ] */ \
                __ASM_EMIT("vbif        q0, q9, q4")                /* q0 = [abs(min) < abs(x1) ] ? min : x1 */ \
                __ASM_EMIT("vbif        q1, q9, q5")                /* q1 = [abs(max) > abs(x1) ] ? res : x1 */ \
                __ASM_EMIT("vbif        q2, q13, q4")               /* q12 = [abs(min) < abs(x1) ] ? abs(min) : abs(x1) */ \
                __ASM_EMIT("vbif        q3, q13, q5")               /* q13 = [abs(max) > abs(x1) ] ? abs(max) : abs(x1) */ \
                __ASM_EMIT("vclt.f32    q4, q2, q14")               /* q4 = [abs(min) < abs(x1) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q14")               /* q4 = [abs(max) > abs(x1) ] */ \
                __ASM_EMIT("vbif        q0, q10, q4")               /* q0 = [abs(min) < abs(x2) ] ? min : x2 */ \
                __ASM_EMIT("vbif        q1, q10, q5")               /* q1 = [abs(max) > abs(x2) ] ? res : x2 */ \
                __ASM_EMIT("vbif        q2, q14, q4")               /* q12 = [abs(min) < abs(x2) ] ? abs(min) : abs(x2) */ \
                __ASM_EMIT("vbif        q3, q14, q5")               /* q13 = [abs(max) > abs(x2) ] ? abs(max) : abs(x2) */ \
                __ASM_EMIT("vclt.f32    q4, q2, q15")               /* q4 = [abs(min) < abs(x1) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q15")               /* q4 = [abs(max) > abs(x1) ] */ \
                __ASM_EMIT("vbif        q0, q11, q4")               /* q0 = [abs(min) < abs(x3) ] ? min : x3 */ \
                __ASM_EMIT("vbif        q1, q11, q5")               /* q1 = [abs(max) > abs(x3) ] ? res : x3 */ \
                __ASM_EMIT("vbif        q2, q15, q4")               /* q12 = [abs(min) < abs(x3) ] ? abs(min) : abs(x3) */ \
                __ASM_EMIT("vbif        q3, q15, q5")               /* q13 = [abs(max) > abs(x3) ] ? abs(max) : abs(x3) */ \
                __ASM_EMIT("subs        %[count], #16") \
                __ASM_EMIT("bhs         1b") \
                __ASM_EMIT("2:") \
                /* x8 block */ \
                __ASM_EMIT("adds        %[count], #8") \
                __ASM_EMIT("blt         4f") \
                __ASM_EMIT("vldm        %[src]!, {q8-q9}")          /* q8 = x0, q9 = x1 */ \
                __ASM_EMIT("vabs.f32    q12, q8")                   /* q12 = abs(x0) */ \
                __ASM_EMIT("vabs.f32    q13, q9")                   /* q13 = abs(x1) */ \
                __ASM_EMIT("vclt.f32    q4, q2, q12")               /* q4 = [abs(min) < abs(x0) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q12")               /* q4 = [abs(max) > abs(x0) ] */ \
                __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(min) < abs(x0) ] ? min : x0 */ \
                __ASM_EMIT("vbif        q1, q8, q5")                /* q1 = [abs(max) > abs(x0) ] ? res : x0 */ \
                __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(min) < abs(x0) ] ? abs(min) : abs(x0) */ \
                __ASM_EMIT("vbif        q3, q12, q5")               /* q13 = [abs(max) > abs(x0) ] ? abs(max) : abs(x0) */ \
                __ASM_EMIT("vclt.f32    q4, q2, q13")               /* q4 = [abs(min) < abs(x1) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q13")               /* q4 = [abs(max) > abs(x1) ] */ \
                __ASM_EMIT("vbif        q0, q9, q4")                /* q0 = [abs(min) < abs(x1) ] ? min : x1 */ \
                __ASM_EMIT("vbif        q1, q9, q5")                /* q1 = [abs(max) > abs(x1) ] ? res : x1 */ \
                __ASM_EMIT("vbif        q2, q13, q4")               /* q12 = [abs(min) < abs(x1) ] ? abs(min) : abs(x1) */ \
                __ASM_EMIT("vbif        q3, q13, q5")               /* q13 = [abs(max) > abs(x1) ] ? abs(max) : abs(x1) */ \
                __ASM_EMIT("sub         %[count], #8") \
                __ASM_EMIT("4:") \
                /* x4 block */ \
                __ASM_EMIT("adds        %[count], #4") \
                __ASM_EMIT("blt         6f") \
                __ASM_EMIT("vldm        %[src]!, {q8}")             /* q8 = x0, q9 = x1 */ \
                __ASM_EMIT("vabs.f32    q12, q8")                   /* q12 = abs(x0) */ \
                __ASM_EMIT("vclt.f32    q4, q2, q12")               /* q4 = [abs(min) < abs(x0) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q12")               /* q4 = [abs(max) > abs(x0) ] */ \
                __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(min) < abs(x0) ] ? min : x0 */ \
                __ASM_EMIT("vbif        q1, q8, q5")                /* q1 = [abs(max) > abs(x0) ] ? res : x0 */ \
                __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(min) < abs(x0) ] ? abs(min) : abs(x0) */ \
                __ASM_EMIT("vbif        q3, q12, q5")               /* q13 = [abs(max) > abs(x0) ] ? abs(max) : abs(x0) */ \
                __ASM_EMIT("sub         %[count], #4") \
                __ASM_EMIT("6:") \
                __ASM_EMIT("vext.32     q8, q0, q0, #2")            /* q8 = x0 */ \
                __ASM_EMIT("vext.32     q12, q2, q2, #2")           /* q12 = abs(x0) */  \
                __ASM_EMIT("vclt.f32    q4, q2, q12")               /* q4 = [abs(min) < abs(x0) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q12")               /* q4 = [abs(max) > abs(x0) ] */ \
                __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(min) < abs(x0) ] ? min : x0 */ \
                __ASM_EMIT("vbif        q1, q8, q5")                /* q1 = [abs(max) > abs(x0) ] ? res : x0 */ \
                __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(min) < abs(x0) ] ? abs(min) : abs(x0) */ \
                __ASM_EMIT("vbif        q3, q12, q5")               /* q13 = [abs(max) > abs(x0) ] ? abs(max) : abs(x0) */ \
                __ASM_EMIT("vext.32     q8, q0, q0, #1")            /* q8 = x0 */ \
                __ASM_EMIT("vext.32     q12, q2, q2, #1")           /* q12 = abs(x0) */  \
                __ASM_EMIT("vclt.f32    q4, q2, q12")               /* q4 = [abs(min) < abs(x0) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q12")               /* q4 = [abs(max) > abs(x0) ] */ \
                __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(min) < abs(x0) ] ? min : x0 */ \
                __ASM_EMIT("vbif        q1, q8, q5")                /* q1 = [abs(max) > abs(x0) ] ? res : x0 */ \
                __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(min) < abs(x0) ] ? abs(min) : abs(x0) */ \
                __ASM_EMIT("vbif        q3, q12, q5")               /* q13 = [abs(max) > abs(x0) ] ? abs(max) : abs(x0) */ \
                /* x1 block */ \
                __ASM_EMIT("adds        %[count], #3") \
                __ASM_EMIT("blt         8f") \
                __ASM_EMIT("7:") \
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[src]]!") \
                __ASM_EMIT("vabs.f32    q12, q8")                   /* q12 = abs(x0) */ \
                __ASM_EMIT("vclt.f32    q4, q2, q12")               /* q4 = [abs(min) < abs(x0) ] */ \
                __ASM_EMIT("vcgt.f32    q5, q3, q12")               /* q4 = [abs(max) > abs(x0) ] */ \
                __ASM_EMIT("vbif        q0, q8, q4")                /* q0 = [abs(min) < abs(x0) ] ? min : x0 */ \
                __ASM_EMIT("vbif        q1, q8, q5")                /* q1 = [abs(max) > abs(x0) ] ? res : x0 */ \
                __ASM_EMIT("vbif        q2, q12, q4")               /* q12 = [abs(min) < abs(x0) ] ? abs(min) : abs(x0) */ \
                __ASM_EMIT("vbif        q3, q12, q5")               /* q13 = [abs(max) > abs(x0) ] ? abs(max) : abs(x0) */ \
                __ASM_EMIT("subs        %[count], #1") \
                __ASM_EMIT("bge         7b") \
                __ASM_EMIT("8:") \
                /* end */ \
                __ASM_EMIT("vst1.32     {d0[0]}, [%[min]]") \
                __ASM_EMIT("vst1.32     {d2[0]}, [%[max]]")
                : [count] "+r" (count), [src] "+r" (src)
                : [min] "r" (min), [max] "r" (max)
                : "cc",
                  "q1", "q2", "q3", "q4", "q5",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }
    } /* namespace neon_d32 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_SEARCH_MINMAX_H_ */
