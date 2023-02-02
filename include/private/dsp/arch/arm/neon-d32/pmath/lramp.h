/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 февр. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LRAMP_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LRAMP_H_

namespace lsp
{
    namespace neon_d32
    {

        IF_ARCH_ARM(
            static const float lramp_const[] __lsp_aligned16 =
            {
                0.0f,  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  /* Initial values 0..7 */
                8.0f,  9.0f,  10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, /* Initial values 8..15 */
                16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f  /* Step */
            };
        )

        void lramp_set1(float *dst, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fill(dst, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[v1]]")
                __ASM_EMIT("vld1.32         {d30[], d31[]}, [%[delta]]")
                __ASM_EMIT("vldm            %[CC], {q8-q13}")
                /* q8 = i[0], q9 = i[1], q10 = i[2], q11 = i[3] */
                /* q12 = step, q13 = step */
                /* q14 = v1, q15 = delta */
                /* 16x blocks */
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */
                __ASM_EMIT("vmul.f32        q1, q9, q15")           /* q1       = i[1] * delta */
                __ASM_EMIT("vmul.f32        q2, q10, q15")          /* q2       = i[2] * delta */
                __ASM_EMIT("vmul.f32        q3, q11, q15")          /* q3       = i[3] * delta */
                __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */
                __ASM_EMIT("vadd.f32        q1, q1, q14")           /* q1       = v1 + i[0] * delta */
                __ASM_EMIT("vadd.f32        q2, q2, q14")           /* q2       = v1 + i[0] * delta */
                __ASM_EMIT("vadd.f32        q3, q3, q14")           /* q3       = v1 + i[0] * delta */
                __ASM_EMIT("vadd.f32        q8, q8, q12")           /* q8       = i[0]' = i[0] + step */
                __ASM_EMIT("vadd.f32        q9, q9, q13")           /* q9       = i[1]' = i[1] + step */
                __ASM_EMIT("vadd.f32        q10, q10, q12")         /* q10      = i[2]' = i[2] + step */
                __ASM_EMIT("vadd.f32        q11, q11, q13")         /* q11      = i[3]' = i[3] + step */
                __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                /* 8x blocks */
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */
                __ASM_EMIT("vmul.f32        q1, q9, q15")           /* q1       = i[1] * delta */
                __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */
                __ASM_EMIT("vadd.f32        q1, q1, q14")           /* q1       = v1 + i[0] * delta */
                __ASM_EMIT("vmov            q8, q10")               /* q8       = i[0]' = i[0] + step */
                __ASM_EMIT("vmov            q9, q11")               /* q9       = i[1]' = i[1] + step */
                __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */
                __ASM_EMIT("vmov            q8, q9")                /* q8       = i[0]' = i[0] + step */
                __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */
                __ASM_EMIT("vstm            %[dst]!, {q0}")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("6:")
                /* 1x blocks */
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */
                __ASM_EMIT("vext.32         q8, q8, #1")            /* q8       = i[0]' = i[0] + step */
                __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             7b")
                __ASM_EMIT("8:")

                : [count] "+r" (count), [dst] "+r" (dst)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        #define LRAMP_CORE(DST, SRC, SINC) \
            __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[v1]]") \
            __ASM_EMIT("vld1.32         {d30[], d31[]}, [%[delta]]") \
            __ASM_EMIT("vldm            %[CC], {q8-q13}") \
            /* q8 = i[0], q9 = i[1], q10 = i[2], q11 = i[3] */ \
            /* q12 = step, q13 = step */ \
            /* q14 = v1, q15 = delta */ \
            /* 16x blocks */ \
            __ASM_EMIT("subs            %[count], #16") \
            __ASM_EMIT("blo             2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q1, q9, q15")           /* q1       = i[1] * delta */ \
            __ASM_EMIT("vmul.f32        q2, q10, q15")          /* q2       = i[2] * delta */ \
            __ASM_EMIT("vmul.f32        q3, q11, q15")          /* q3       = i[3] * delta */ \
            __ASM_EMIT("vldm            %[" SRC "] " SINC ", {q4-q7}") \
            __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q1, q1, q14")           /* q1       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q2, q2, q14")           /* q2       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q3, q3, q14")           /* q3       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q8, q8, q12")           /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vadd.f32        q9, q9, q13")           /* q9       = i[1]' = i[1] + step */ \
            __ASM_EMIT("vadd.f32        q10, q10, q12")         /* q10      = i[2]' = i[2] + step */ \
            __ASM_EMIT("vadd.f32        q11, q11, q13")         /* q11      = i[3]' = i[3] + step */ \
            __ASM_EMIT("vmul.f32        q0, q0, q4")            /* q0       = src[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vmul.f32        q1, q1, q5")            /* q1       = src[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("vmul.f32        q2, q2, q6")            /* q2       = src[2] * (v1 + i[2] * delta) */ \
            __ASM_EMIT("vmul.f32        q3, q3, q7")            /* q3       = src[3] * (v1 + i[3] * delta) */ \
            __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
            __ASM_EMIT("subs            %[count], #16") \
            __ASM_EMIT("bhs             1b") \
            __ASM_EMIT("2:") \
            /* 8x blocks */ \
            __ASM_EMIT("adds            %[count], #8") \
            __ASM_EMIT("blt             4f") \
            __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q1, q9, q15")           /* q1       = i[1] * delta */ \
            __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q4-q5}") \
            __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q1, q1, q14")           /* q1       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmov            q8, q10")               /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vmov            q9, q11")               /* q9       = i[1]' = i[1] + step */ \
            __ASM_EMIT("vmul.f32        q0, q0, q4")            /* q0       = src[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vmul.f32        q1, q1, q5")            /* q1       = src[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
            __ASM_EMIT("sub             %[count], #8") \
            __ASM_EMIT("4:") \
            /* 4x block */ \
            __ASM_EMIT("adds            %[count], #4") \
            __ASM_EMIT("blt             6f") \
            __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */ \
            __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q4}") \
            __ASM_EMIT("vmov            q8, q9")                /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q0, q0, q4")            /* q0       = src[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
            __ASM_EMIT("sub             %[count], #4") \
            __ASM_EMIT("6:") \
            /* 1x blocks */ \
            __ASM_EMIT("adds            %[count], #3") \
            __ASM_EMIT("blt             8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */ \
            __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" SRC "]]" SINC) \
            __ASM_EMIT("vext.32         q8, q8, #1")            /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q0, q0, q4")            /* q0       = src[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
            __ASM_EMIT("subs            %[count], #1") \
            __ASM_EMIT("bge             7b") \
            __ASM_EMIT("8:")

        void lramp1(float *dst, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::mul_k2(dst, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_CORE("dst", "dst", "")
                : [count] "+r" (count), [dst] "+r" (dst)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::mul_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_CORE("dst", "src", "!")
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        #undef LRAMP_CORE

        #define OP_DSEL(a, b)   a
        #define OP_RSEL(a, b)   b

        #define LRAMP_OP3_CORE(DST, A, B, OP, SEL, AINC) \
            __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[v1]]") \
            __ASM_EMIT("vld1.32         {d30[], d31[]}, [%[delta]]") \
            __ASM_EMIT("vldm            %[CC], {q8-q13}") \
            /* q8 = i[0], q9 = i[1], q10 = i[2], q11 = i[3] */ \
            /* q12 = step, q13 = step */ \
            /* q14 = v1, q15 = delta */ \
            /* 16x blocks */ \
            __ASM_EMIT("subs            %[count], #16") \
            __ASM_EMIT("blo             2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q1, q9, q15")           /* q1       = i[1] * delta */ \
            __ASM_EMIT("vmul.f32        q2, q10, q15")          /* q2       = i[2] * delta */ \
            __ASM_EMIT("vmul.f32        q3, q11, q15")          /* q3       = i[3] * delta */ \
            __ASM_EMIT("vldm            %[" B "]!, {q4-q7}") \
            __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q1, q1, q14")           /* q1       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q2, q2, q14")           /* q2       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q3, q3, q14")           /* q3       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q0, q0, q4")            /* q0       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vmul.f32        q1, q1, q5")            /* q1       = b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("vmul.f32        q2, q2, q6")            /* q2       = b[2] * (v1 + i[2] * delta) */ \
            __ASM_EMIT("vmul.f32        q3, q3, q7")            /* q3       = b[3] * (v1 + i[3] * delta) */ \
            __ASM_EMIT("vldm            %[" A "]" AINC ", {q4-q7}") \
            __ASM_EMIT("vadd.f32        q8, q8, q12")           /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vadd.f32        q9, q9, q13")           /* q9       = i[1]' = i[1] + step */ \
            __ASM_EMIT("vadd.f32        q10, q10, q12")         /* q10      = i[2]' = i[2] + step */ \
            __ASM_EMIT("vadd.f32        q11, q11, q13")         /* q11      = i[3]' = i[3] + step */ \
            __ASM_EMIT(OP ".f32         q0, " SEL("q4, q0", "q0, q4"))  /* q0   = a[0] OP b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT(OP ".f32         q1, " SEL("q5, q1", "q1, q5"))  /* q1   = a[1] OP b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT(OP ".f32         q2, " SEL("q6, q2", "q2, q6"))  /* q2   = a[2] OP b[2] * (v1 + i[2] * delta) */ \
            __ASM_EMIT(OP ".f32         q3, " SEL("q7, q3", "q3, q7"))  /* q3   = a[3] OP b[3] * (v1 + i[3] * delta) */ \
            __ASM_EMIT("subs            %[count], #16") \
            __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
            __ASM_EMIT("bhs             1b") \
            __ASM_EMIT("2:") \
            /* 8x blocks */ \
            __ASM_EMIT("adds            %[count], #8") \
            __ASM_EMIT("blt             4f") \
            __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q1, q9, q15")           /* q1       = i[1] * delta */ \
            __ASM_EMIT("vldm            %[" B "]!, {q4-q5}") \
            __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q1, q1, q14")           /* q1       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q0, q0, q4")            /* q0       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vmul.f32        q1, q1, q5")            /* q1       = b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("vldm            %[" A "]" AINC ", {q4-q5}") \
            __ASM_EMIT("vmov            q8, q10")               /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vmov            q9, q11")               /* q9       = i[1]' = i[1] + step */ \
            __ASM_EMIT(OP ".f32         q0, " SEL("q4, q0", "q0, q4"))  /* q0   = a[0] OP b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT(OP ".f32         q1, " SEL("q5, q1", "q1, q5"))  /* q1   = a[1] OP b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("sub             %[count], #8") \
            __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
            __ASM_EMIT("4:") \
            /* 4x block */ \
            __ASM_EMIT("adds            %[count], #4") \
            __ASM_EMIT("blt             6f") \
            __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */ \
            __ASM_EMIT("vldm            %[" B "]!, {q4}") \
            __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q0, q0, q4")            /* q0       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vldm            %[" A "]" AINC ", {q4}") \
            __ASM_EMIT("vmov            q8, q9")                /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT(OP ".f32         q0, " SEL("q4, q0", "q0, q4"))  /* q0   = a[0] OP b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("sub             %[count], #4") \
            __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
            __ASM_EMIT("6:") \
            /* 1x blocks */ \
            __ASM_EMIT("adds            %[count], #3") \
            __ASM_EMIT("blt             8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vmul.f32        q0, q8, q15")           /* q0       = i[0] * delta */ \
            __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" B "]]!") /* q4 = b[0] */ \
            __ASM_EMIT("vadd.f32        q0, q0, q14")           /* q0       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q0, q0, q4")            /* q0       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" A "]]" AINC) /* q4 = a[0] */ \
            __ASM_EMIT("vext.32         q8, q8, #1")            /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT(OP ".f32         q0, " SEL("q4, q0", "q0, q4"))  /* q0   = a[0] OP b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("subs            %[count], #1") \
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!") \
            __ASM_EMIT("bge             7b") \
            __ASM_EMIT("8:")

        void lramp_add2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmadd_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "vadd", OP_DSEL, "")
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_sub2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmsub_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "vsub", OP_DSEL, "")
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_rsub2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmrsub_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "vsub", OP_RSEL, "")
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_mul2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmmul_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "vmul", OP_DSEL, "")
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_add3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmadd_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "vadd", OP_DSEL, "!")
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_sub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmsub_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "vsub", OP_DSEL, "!")
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_rsub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmrsub_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "vsub", OP_RSEL, "!")
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_mul3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmmul_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "vmul", OP_DSEL, "!")
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        #undef LRAMP_OP3_CORE

        IF_ARCH_ARM(
            static const float lramp_div_const[] __lsp_aligned16 =
            {
                0.0f,  1.0f,  2.0f,  3.0f,
                4.0f,  5.0f,  6.0f,  7.0f,  /* Initial values 0..7 */
                8.0f,  8.0f,  8.0f,  8.0f   /* Step */
            };
        )

        #define LRAMP_DIV3_CORE(DST, A, B, SEL, AINC) \
            __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[v1]]") \
            __ASM_EMIT("vld1.32         {d26[], d27[]}, [%[delta]]") \
            __ASM_EMIT("vldm            %[CC], {q8-q10}") \
            /* q8 = i[0], q9 = i[1] */ \
            /* q10 = step */ \
            /* q12 = v1, q13 = delta */ \
            /* 8x blocks */ \
            __ASM_EMIT("subs            %[count], #8") \
            __ASM_EMIT("blo             2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmul.f32        q4, q8, q13")           /* q4       = i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q5, q9, q13")           /* q5       = i[1] * delta */ \
            __ASM_EMIT("vldm            %[" A "]" AINC ", {q0-q1}")     /* q0 = a[0], q1 = a[1] */ \
            __ASM_EMIT("vldm            %[" B "]!, {q2-q3}")    /* q2       = b[0], q3 = b[1] */ \
            __ASM_EMIT("vadd.f32        q4, q4, q12")           /* q4       = v1 + i[0] * delta */ \
            __ASM_EMIT("vadd.f32        q5, q5, q12")           /* q5       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q2, q2, q4")            /* q2       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vmul.f32        q3, q3, q5")            /* q3       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vadd.f32        q8, q8, q10")           /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vadd.f32        q9, q9, q10")           /* q9       = i[1]' = i[1] + step */ \
            __ASM_EMIT("vrecpe.f32      q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vrecpe.f32      q5, " SEL("q3", "q1")) \
            __ASM_EMIT("vrecps.f32      q6, q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vrecps.f32      q7, q5, " SEL("q3", "q1")) \
            __ASM_EMIT("vmul.f32        q4, q6, q4") \
            __ASM_EMIT("vmul.f32        q5, q7, q5") \
            __ASM_EMIT("vrecps.f32      q6, q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vrecps.f32      q7, q5, " SEL("q3", "q1")) \
            __ASM_EMIT("vmul.f32        " SEL("q2", "q0") ", q6, q4")   /* q2   = 1/(b[0] * (v1 + i[0] * delta)) */ \
            __ASM_EMIT("vmul.f32        " SEL("q3", "q1") ", q7, q5")   /* q3   = 1/(b[1] * (v1 + i[1] * delta)) */ \
            __ASM_EMIT("vmul.f32        q0, q0, q2")                    /* q0   = a[0] / b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vmul.f32        q1, q1, q3")                    /* q1   = a[1] / b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("subs            %[count], #8") \
            __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
            __ASM_EMIT("bhs             1b") \
            __ASM_EMIT("2:") \
            /* 4x blocks */ \
            __ASM_EMIT("adds            %[count], #4") \
            __ASM_EMIT("blt             4f") \
            __ASM_EMIT("vmul.f32        q4, q8, q13")           /* q4       = i[0] * delta */ \
            __ASM_EMIT("vldm            %[" A "]" AINC ", {q0}")/* q0       = a[0], q1 = a[1] */ \
            __ASM_EMIT("vldm            %[" B "]!, {q2}")       /* q2       = b[0], q3 = b[1] */ \
            __ASM_EMIT("vadd.f32        q4, q4, q12")           /* q4       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q2, q2, q4")            /* q2       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vmov            q8, q9")                /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vrecpe.f32      q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vrecps.f32      q6, q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vmul.f32        q4, q6, q4") \
            __ASM_EMIT("vrecps.f32      q6, q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vmul.f32        " SEL("q2", "q0") ", q6, q4")   /* q2   = 1/(b[0] * (v1 + i[0] * delta)) */ \
            __ASM_EMIT("vmul.f32        q0, q0, q2")                    /* q0   = a[0] / b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("sub             %[count], #4") \
            __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
            __ASM_EMIT("4:") \
            /* 1x blocks */ \
            __ASM_EMIT("adds            %[count], #3") \
            __ASM_EMIT("blt             6f") \
            __ASM_EMIT("5:") \
            __ASM_EMIT("vmul.f32        q4, q8, q13")           /* q4       = i[0] * delta */ \
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" A "]]" AINC) \
            __ASM_EMIT("vld1.32         {d4[], d5[]}, [%[" B "]]!") \
            __ASM_EMIT("vadd.f32        q4, q4, q12")           /* q4       = v1 + i[0] * delta */ \
            __ASM_EMIT("vmul.f32        q2, q2, q4")            /* q2       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("vext.32         q8, q8, #1")            /* q8       = i[0]' = i[0] + step */ \
            __ASM_EMIT("vrecpe.f32      q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vrecps.f32      q6, q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vmul.f32        q4, q6, q4") \
            __ASM_EMIT("vrecps.f32      q6, q4, " SEL("q2", "q0")) \
            __ASM_EMIT("vmul.f32        " SEL("q2", "q0") ", q6, q4")   /* q2   = 1/(b[0] * (v1 + i[0] * delta)) */ \
            __ASM_EMIT("vmul.f32        q0, q0, q2")                    /* q0   = a[0] / b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("subs            %[count], #1") \
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!") \
            __ASM_EMIT("bge             5b") \
            __ASM_EMIT("6:")

        void lramp_div2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmdiv_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_DIV3_CORE("dst", "dst", "src", OP_DSEL, "")
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_div_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_rdiv2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmrdiv_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_DIV3_CORE("dst", "dst", "src", OP_RSEL, "")
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_div_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_div3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmdiv_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_DIV3_CORE("dst", "a", "b", OP_DSEL, "!")
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_div_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lramp_rdiv3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmrdiv_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            ARCH_ARM_ASM(
                LRAMP_DIV3_CORE("dst", "a", "b", OP_RSEL, "!")
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_div_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        #undef LRAMP_DIV3_CORE

        #undef OP_DSEL
        #undef OP_RSEL

    } /* namespace neon_d32 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LRAMP_H_ */
