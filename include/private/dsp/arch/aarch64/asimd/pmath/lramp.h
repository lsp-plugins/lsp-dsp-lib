/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 3 февр. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LRAMP_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LRAMP_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
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
            ARCH_AARCH64_ASM(
                __ASM_EMIT("ld1r            {v14.4s}, [%[v1]]")
                __ASM_EMIT("ld1r            {v15.4s}, [%[delta]]")
                __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]")
                __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]")
                __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]")
                /* v8.4s = i[0], v9.4s = i[1], v10.4s = i[2], v11.4s = i[3] */
                /* v12.4s = step, v13.4s = step */
                /* v14.4s = v1, v15.4s = delta */
                /* 16x blocks */
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */
                __ASM_EMIT("fmul            v1.4s, v9.4s, v15.4s")           /* v1.4s       = i[1] * delta */
                __ASM_EMIT("fmul            v2.4s, v10.4s, v15.4s")          /* v2.4s       = i[2] * delta */
                __ASM_EMIT("fmul            v3.4s, v11.4s, v15.4s")          /* v3.4s       = i[3] * delta */
                __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */
                __ASM_EMIT("fadd            v1.4s, v1.4s, v14.4s")           /* v1.4s       = v1 + i[0] * delta */
                __ASM_EMIT("fadd            v2.4s, v2.4s, v14.4s")           /* v2.4s       = v1 + i[0] * delta */
                __ASM_EMIT("fadd            v3.4s, v3.4s, v14.4s")           /* v3.4s       = v1 + i[0] * delta */
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")           /* v8.4s       = i[0]' = i[0] + step */
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")           /* v9.4s       = i[1]' = i[1] + step */
                __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")         /* v10.4s      = i[2]' = i[2] + step */
                __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")         /* v11.4s      = i[3]' = i[3] + step */
                __ASM_EMIT("stp             q0, q1, [%[dst], 0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], 0x20]")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                /* 8x blocks */
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */
                __ASM_EMIT("fmul            v1.4s, v9.4s, v15.4s")           /* v1.4s       = i[1] * delta */
                __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */
                __ASM_EMIT("fadd            v1.4s, v1.4s, v14.4s")           /* v1.4s       = v1 + i[0] * delta */
                __ASM_EMIT("mov             v8.16b, v10.16b")                /* v8.4s       = i[0]' = i[0] + step */
                __ASM_EMIT("mov             v9.16b, v11.16b")                /* v9.4s       = i[1]' = i[1] + step */
                __ASM_EMIT("stp             q0, q1, [%[dst], 0x00]")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */
                __ASM_EMIT("mov             v8.16b, v9.16b")                 /* v8.4s       = i[0]' = i[0] + step */
                __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */
                __ASM_EMIT("str             q0, [%[dst], 0x00]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("6:")
                /* 1x blocks */
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */
                __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")     /* v8.4s       = i[0]' = i[0] + step */
                __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge            7b")
                __ASM_EMIT("8:")

                : [count] "+r" (count), [dst] "+r" (dst)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
            );
        }

        #define OP_SINC(a) a
        #define OP_SNOINC(a)

        #define LRAMP_CORE(DST, SRC, SINC) \
            __ASM_EMIT("ld1r            {v14.4s}, [%[v1]]") \
            __ASM_EMIT("ld1r            {v15.4s}, [%[delta]]") \
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]") \
            __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]") \
            __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]") \
            /* v8.4s = i[0], v9.4s = i[1], v10.4s = i[2], v11.4s = i[3] */ \
            /* v12.4s = step, v13.4s = step */ \
            /* v14.4s = v1, v15.4s = delta */ \
            /* 16x blocks */ \
            __ASM_EMIT("subs            %[count], %[count], #16") \
            __ASM_EMIT("b.lo            2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */ \
            __ASM_EMIT("fmul            v1.4s, v9.4s, v15.4s")           /* v1.4s       = i[1] * delta */ \
            __ASM_EMIT("fmul            v2.4s, v10.4s, v15.4s")          /* v2.4s       = i[2] * delta */ \
            __ASM_EMIT("fmul            v3.4s, v11.4s, v15.4s")          /* v3.4s       = i[3] * delta */ \
            __ASM_EMIT("ldp             q4, q5, [%[" SRC "], 0x00]") \
            __ASM_EMIT("ldp             q6, q7, [%[" SRC "], 0x20]") \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v1.4s, v1.4s, v14.4s")           /* v1.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v2.4s, v2.4s, v14.4s")           /* v2.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v3.4s, v3.4s, v14.4s")           /* v3.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")           /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")           /* v9.4s       = i[1]' = i[1] + step */ \
            __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")         /* v10.4s      = i[2]' = i[2] + step */ \
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")         /* v11.4s      = i[3]' = i[3] + step */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")            /* v0.4s       = src[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")            /* v1.4s       = src[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")            /* v2.4s       = src[2] * (v1 + i[2] * delta) */ \
            __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")            /* v3.4s       = src[3] * (v1 + i[3] * delta) */ \
            __ASM_EMIT("stp             q0, q1, [%[" DST "], 0x00]") \
            __ASM_EMIT("stp             q2, q3, [%[" DST "], 0x20]") \
            __ASM_EMIT("subs            %[count], %[count], #16") \
            __ASM_EMIT(SINC("add        %[" SRC "], %[" SRC "], #0x40")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x40") \
            __ASM_EMIT("b.hs            1b") \
            __ASM_EMIT("2:") \
            /* 8x blocks */ \
            __ASM_EMIT("adds            %[count], %[count], #8") \
            __ASM_EMIT("b.lt            4f") \
            __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */ \
            __ASM_EMIT("fmul            v1.4s, v9.4s, v15.4s")           /* v1.4s       = i[1] * delta */ \
            __ASM_EMIT("ldp             q4, q5, [%[" SRC "], 0x00]") \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v1.4s, v1.4s, v14.4s")           /* v1.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("mov             v8.16b, v10.16b")                /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("mov             v9.16b, v11.16b")                /* v9.4s       = i[1]' = i[1] + step */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")            /* v0.4s       = src[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")            /* v1.4s       = src[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("stp             q0, q1, [%[" DST "], 0x00]") \
            __ASM_EMIT("sub             %[count], %[count], #8") \
            __ASM_EMIT(SINC("add        %[" SRC "], %[" SRC "], #0x20")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x20") \
            __ASM_EMIT("4:") \
            /* 4x block */ \
            __ASM_EMIT("adds            %[count], %[count], #4") \
            __ASM_EMIT("b.lt            6f") \
            __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */ \
            __ASM_EMIT("ldr             q4, [%[" SRC "], 0x00]") \
            __ASM_EMIT("mov             v8.16b, v9.16b")                 /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")            /* v0.4s       = src[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("str             q0, [%[" DST "], 0x00]") \
            __ASM_EMIT("sub             %[count], %[count], #4") \
            __ASM_EMIT(SINC("add        %[" SRC "], %[" SRC "], #0x10")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x10") \
            __ASM_EMIT("6:") \
            /* 1x blocks */ \
            __ASM_EMIT("adds            %[count], %[count], #3") \
            __ASM_EMIT("b.lt            8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */ \
            __ASM_EMIT("ld1r            {v4.4s}, [%[" SRC "]]") \
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")     /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")            /* v0.4s       = src[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
            __ASM_EMIT("subs            %[count], %[count], #1") \
            __ASM_EMIT(SINC("add        %[" SRC "], %[" SRC "], #0x04")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x04") \
            __ASM_EMIT("b.ge            7b") \
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
            ARCH_AARCH64_ASM(
                LRAMP_CORE("dst", "dst", OP_SNOINC)
                : [count] "+r" (count), [dst] "+r" (dst)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_CORE("dst", "src", OP_SINC)
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
            );
        }

        #undef LRAMP_CORE

        #define OP_DSEL(a, b)   a
        #define OP_RSEL(a, b)   b

        #define LRAMP_OP3_CORE(DST, A, B, OP, SEL, AINC) \
            __ASM_EMIT("ld1r            {v14.4s}, [%[v1]]") \
            __ASM_EMIT("ld1r            {v15.4s}, [%[delta]]") \
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]") \
            __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]") \
            __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]") \
            /* v8.4s = i[0], v9.4s = i[1], v10.4s = i[2], v11.4s = i[3] */ \
            /* v12.4s = step, v13.4s = step */ \
            /* v14.4s = v1, v15.4s = delta */ \
            /* 16x blocks */ \
            __ASM_EMIT("subs            %[count], %[count], #16") \
            __ASM_EMIT("b.lo            2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */ \
            __ASM_EMIT("fmul            v1.4s, v9.4s, v15.4s")           /* v1.4s       = i[1] * delta */ \
            __ASM_EMIT("fmul            v2.4s, v10.4s, v15.4s")          /* v2.4s       = i[2] * delta */ \
            __ASM_EMIT("fmul            v3.4s, v11.4s, v15.4s")          /* v3.4s       = i[3] * delta */ \
            __ASM_EMIT("ldp             q4, q5, [%[" B "], 0x00]") \
            __ASM_EMIT("ldp             q6, q7, [%[" B "], 0x20]") \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v1.4s, v1.4s, v14.4s")           /* v1.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v2.4s, v2.4s, v14.4s")           /* v2.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v3.4s, v3.4s, v14.4s")           /* v3.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")            /* v0.4s       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")            /* v1.4s       = b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")            /* v2.4s       = b[2] * (v1 + i[2] * delta) */ \
            __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")            /* v3.4s       = b[3] * (v1 + i[3] * delta) */ \
            __ASM_EMIT("ldp             q4, q5, [%[" A "], 0x00]") \
            __ASM_EMIT("ldp             q6, q7, [%[" A "], 0x20]") \
            __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")           /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")           /* v9.4s       = i[1]' = i[1] + step */ \
            __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")         /* v10.4s      = i[2]' = i[2] + step */ \
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")         /* v11.4s      = i[3]' = i[3] + step */ \
            __ASM_EMIT(OP "             v0.4s, " SEL("v4.4s, v0.4s", "v0.4s, v4.4s"))  /* v0.4s   = a[0] OP b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT(OP "             v1.4s, " SEL("v5.4s, v1.4s", "v1.4s, v5.4s"))  /* v1.4s   = a[1] OP b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT(OP "             v2.4s, " SEL("v6.4s, v2.4s", "v2.4s, v6.4s"))  /* v2.4s   = a[2] OP b[2] * (v1 + i[2] * delta) */ \
            __ASM_EMIT(OP "             v3.4s, " SEL("v7.4s, v3.4s", "v3.4s, v7.4s"))  /* v3.4s   = a[3] OP b[3] * (v1 + i[3] * delta) */ \
            __ASM_EMIT("subs            %[count], %[count], #16") \
            __ASM_EMIT("stp             q0, q1, [%[" DST "], 0x00]") \
            __ASM_EMIT("stp             q2, q3, [%[" DST "], 0x20]") \
            __ASM_EMIT("add             %[" B "], %[" B "], #0x40") \
            __ASM_EMIT(AINC("add        %[" A "], %[" A "], #0x40")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x40") \
            __ASM_EMIT("b.hs            1b") \
            __ASM_EMIT("2:") \
            /* 8x blocks */ \
            __ASM_EMIT("adds            %[count], %[count], #8") \
            __ASM_EMIT("b.lt            4f") \
            __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */ \
            __ASM_EMIT("fmul            v1.4s, v9.4s, v15.4s")           /* v1.4s       = i[1] * delta */ \
            __ASM_EMIT("ldp             q4, q5, [%[" B "], 0x00]") \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v1.4s, v1.4s, v14.4s")           /* v1.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")            /* v0.4s       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")            /* v1.4s       = b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("ldp             q4, q5, [%[" A "], 0x00]") \
            __ASM_EMIT("mov             v8.16b, v10.16b")                /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("mov             v9.16b, v11.16b")                /* v9.4s       = i[1]' = i[1] + step */ \
            __ASM_EMIT(OP "             v0.4s, " SEL("v4.4s, v0.4s", "v0.4s, v4.4s"))  /* v0.4s   = a[0] OP b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT(OP "             v1.4s, " SEL("v5.4s, v1.4s", "v1.4s, v5.4s"))  /* v1.4s   = a[1] OP b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("sub             %[count], %[count], #8") \
            __ASM_EMIT("stp             q0, q1, [%[" DST "], 0x00]") \
            __ASM_EMIT("add             %[" B "], %[" B "], #0x20") \
            __ASM_EMIT(AINC("add        %[" A "], %[" A "], #0x20")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x20") \
            __ASM_EMIT("4:") \
            /* 4x block */ \
            __ASM_EMIT("adds            %[count], %[count], #4") \
            __ASM_EMIT("b.lt            6f") \
            __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */ \
            __ASM_EMIT("ldr             q4, [%[" B "], 0x00]") \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")            /* v0.4s       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("ldr             q4, [%[" A "], 0x00]") \
            __ASM_EMIT("mov             v8.16b, v9.16b")                 /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT(OP "             v0.4s, " SEL("v4.4s, v0.4s", "v0.4s, v4.4s"))  /* v0.4s   = a[0] OP b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("sub             %[count], %[count], #4") \
            __ASM_EMIT("str             q0, [%[" DST "], 0x00]") \
            __ASM_EMIT("add             %[" B "], %[" B "], #0x10") \
            __ASM_EMIT(AINC("add        %[" A "], %[" A "], #0x10")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x10") \
            __ASM_EMIT("6:") \
            /* 1x blocks */ \
            __ASM_EMIT("adds            %[count], %[count], #3") \
            __ASM_EMIT("b.lt            8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("fmul            v0.4s, v8.4s, v15.4s")           /* v0.4s       = i[0] * delta */ \
            __ASM_EMIT("ld1r            {v4.4s}, [%[" B "]]") \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v14.4s")           /* v0.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")            /* v0.4s       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("ld1r            {v4.4s}, [%[" A "]]") \
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")    /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT(OP "             v0.4s, " SEL("v4.4s, v0.4s", "v0.4s, v4.4s"))  /* v0.4s   = a[0] OP b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("subs            %[count], %[count], #1") \
            __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
            __ASM_EMIT("add             %[" B "], %[" B "], #0x04") \
            __ASM_EMIT(AINC("add        %[" A "], %[" A "], #0x04")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x04") \
            __ASM_EMIT("b.ge            7b") \
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
            ARCH_AARCH64_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "fadd", OP_DSEL, OP_SNOINC)
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "fsub", OP_DSEL, OP_SNOINC)
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "fsub", OP_RSEL, OP_SNOINC)
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "fmul", OP_DSEL, OP_SNOINC)
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "fadd", OP_DSEL, OP_SINC)
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "fsub", OP_DSEL, OP_SINC)
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "fsub", OP_RSEL, OP_SINC)
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "fmul", OP_DSEL, OP_SINC)
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
            );
        }

        #undef LRAMP_OP3_CORE

        IF_ARCH_AARCH64(
            static const float lramp_div_const[] __lsp_aligned16 =
            {
                0.0f,  1.0f,  2.0f,  3.0f,
                4.0f,  5.0f,  6.0f,  7.0f,  /* Initial values 0..7 */
                8.0f,  8.0f,  8.0f,  8.0f   /* Step */
            };
        )

        #define LRAMP_DIV3_CORE(DST, A, B, SEL, AINC) \
            __ASM_EMIT("ld1r            {v12.4s}, [%[v1]]") \
            __ASM_EMIT("ld1r            {v13.4s}, [%[delta]]") \
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]") \
            __ASM_EMIT("ldr             q10, [%[CC], #0x20]") \
            /* v8.4s = i[0], v9.4s = i[1] */ \
            /* v10.4s = step */ \
            /* v12.4s = v1, v13.4s = delta */ \
            /* 8x blocks */ \
            __ASM_EMIT("subs            %[count], %[count], #8") \
            __ASM_EMIT("b.lo            2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("fmul            v4.4s, v8.4s, v13.4s")           /* v4.4s       = i[0] * delta */ \
            __ASM_EMIT("fmul            v5.4s, v9.4s, v13.4s")           /* v5.4s       = i[1] * delta */ \
            __ASM_EMIT("ldp             q0, q1, [%[" A "], #0x00]")      /* v0.4s       = a[0], v1.4s = a[1] */ \
            __ASM_EMIT("ldp             q2, q3, [%[" B "], #0x00]")      /* v2.4s       = b[0], v3.4s = b[1] */ \
            __ASM_EMIT("fadd            v4.4s, v4.4s, v12.4s")           /* v4.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fadd            v5.4s, v5.4s, v12.4s")           /* v5.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v2.4s, v2.4s, v4.4s")            /* v2.4s       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("fmul            v3.4s, v3.4s, v5.4s")            /* v3.4s       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("fadd            v8.4s, v8.4s, v10.4s")           /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("fadd            v9.4s, v9.4s, v10.4s")           /* v9.4s       = i[1]' = i[1] + step */ \
            __ASM_EMIT("frecpe          v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("frecpe          v5.4s, " SEL("v3.4s", "v1.4s")) \
            __ASM_EMIT("frecps          v6.4s, v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("frecps          v7.4s, v5.4s, " SEL("v3.4s", "v1.4s")) \
            __ASM_EMIT("fmul            v4.4s, v6.4s, v4.4s") \
            __ASM_EMIT("fmul            v5.4s, v7.4s, v5.4s") \
            __ASM_EMIT("frecps          v6.4s, v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("frecps          v7.4s, v5.4s, " SEL("v3.4s", "v1.4s")) \
            __ASM_EMIT("fmul            " SEL("v2.4s", "v0.4s") ", v6.4s, v4.4s")   /* v2.4s   = 1/(b[0] * (v1 + i[0] * delta)) */ \
            __ASM_EMIT("fmul            " SEL("v3.4s", "v1.4s") ", v7.4s, v5.4s")   /* v3.4s   = 1/(b[1] * (v1 + i[1] * delta)) */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v2.4s")                    /* v0.4s   = a[0] / b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("fmul            v1.4s, v1.4s, v3.4s")                    /* v1.4s   = a[1] / b[1] * (v1 + i[1] * delta) */ \
            __ASM_EMIT("subs            %[count], %[count], #8") \
            __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("add             %[" B "], %[" B "], #0x20") \
            __ASM_EMIT(AINC("add        %[" A "], %[" A "], #0x20")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x20") \
            __ASM_EMIT("b.hs            1b") \
            __ASM_EMIT("2:") \
            /* 4x blocks */ \
            __ASM_EMIT("adds            %[count], %[count], #4") \
            __ASM_EMIT("b.lt            4f") \
            __ASM_EMIT("fmul            v4.4s, v8.4s, v13.4s")           /* v4.4s       = i[0] * delta */ \
            __ASM_EMIT("ldr             q0, [%[" A "], #0x00]")          /* v0.4s       = a[0] */ \
            __ASM_EMIT("ldr             q2, [%[" B "], #0x00]")          /* v2.4s       = b[0] */ \
            __ASM_EMIT("fadd            v4.4s, v4.4s, v12.4s")           /* v4.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v2.4s, v2.4s, v4.4s")            /* v2.4s       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("mov             v8.16b, v9.16b")                 /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("frecpe          v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("frecps          v6.4s, v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("fmul            v4.4s, v6.4s, v4.4s") \
            __ASM_EMIT("frecps          v6.4s, v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("fmul            " SEL("v2.4s", "v0.4s") ", v6.4s, v4.4s")   /* v2.4s   = 1/(b[0] * (v1 + i[0] * delta)) */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v2.4s")                    /* v0.4s   = a[0] / b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("sub             %[count], %[count], #4") \
            __ASM_EMIT("str             q0, [%[" DST "], #0x00]") \
            __ASM_EMIT("add             %[" B "], %[" B "], #0x10") \
            __ASM_EMIT(AINC("add        %[" A "], %[" A "], #0x10")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x10") \
            __ASM_EMIT("4:") \
            /* 1x blocks */ \
            __ASM_EMIT("adds            %[count], %[count], #3") \
            __ASM_EMIT("b.lt            6f") \
            __ASM_EMIT("5:") \
            __ASM_EMIT("fmul            v4.4s, v8.4s, v13.4s")           /* v4.4s       = i[0] * delta */ \
            __ASM_EMIT("ld1r            {v0.4s}, [%[" A "]]") \
            __ASM_EMIT("ld1r            {v2.4s}, [%[" B "]]") \
            __ASM_EMIT("fadd            v4.4s, v4.4s, v12.4s")           /* v4.4s       = v1 + i[0] * delta */ \
            __ASM_EMIT("fmul            v2.4s, v2.4s, v4.4s")            /* v2.4s       = b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")     /* v8.4s       = i[0]' = i[0] + step */ \
            __ASM_EMIT("frecpe          v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("frecps          v6.4s, v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("fmul            v4.4s, v6.4s, v4.4s") \
            __ASM_EMIT("frecps          v6.4s, v4.4s, " SEL("v2.4s", "v0.4s")) \
            __ASM_EMIT("fmul            " SEL("v2.4s", "v0.4s") ", v6.4s, v4.4s")   /* v2.4s   = 1/(b[0] * (v1 + i[0] * delta)) */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v2.4s")                    /* v0.4s   = a[0] / b[0] * (v1 + i[0] * delta) */ \
            __ASM_EMIT("subs            %[count], %[count], #1") \
            __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
            __ASM_EMIT("add             %[" B "], %[" B "], #0x04") \
            __ASM_EMIT(AINC("add        %[" A "], %[" A "], #0x04")) \
            __ASM_EMIT("add             %[" DST "], %[" DST "], #0x04") \
            __ASM_EMIT("b.ge            5b") \
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
            ARCH_AARCH64_ASM(
                LRAMP_DIV3_CORE("dst", "dst", "src", OP_DSEL, OP_SNOINC)
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_div_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_DIV3_CORE("dst", "dst", "src", OP_RSEL, OP_SNOINC)
                : [count] "+r" (count), [dst] "+r" (dst), [src] "+r" (src)
                : [CC] "r" (&lramp_div_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_DIV3_CORE("dst", "a", "b", OP_DSEL, OP_SINC)
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_div_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                LRAMP_DIV3_CORE("dst", "a", "b", OP_RSEL, OP_SINC)
                : [count] "+r" (count), [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b)
                : [CC] "r" (&lramp_div_const[0]),
                  [delta] "r" (&delta),
                  [v1] "r" (&v1)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
            );
        }

        #undef LRAMP_DIV3_CORE

        #undef OP_DSEL
        #undef OP_RSEL

        #undef OP_SINC
        #undef OP_SNOINC

    } /* namespace asimd */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LRAMP_H_ */
