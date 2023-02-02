/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 февр. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PMATH_LRAMP_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PMATH_LRAMP_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        IF_ARCH_X86(
            static const float lramp_const[] __lsp_aligned16 =
            {
                0.0f, 1.0f, 2.0f, 3.0f, // Initial values 0..3
                4.0f, 5.0f, 6.0f, 7.0f, // Initial values 4..7
                8.0f, 8.0f, 8.0f, 8.0f  // Step
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[v1], %%xmm6")
                __ASM_EMIT("movss       %[delta], %%xmm7")
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm3")               // xmm3     = i[0]
                __ASM_EMIT("movaps      0x10(%[CC]), %%xmm4")               // xmm4     = i[1]
                __ASM_EMIT("movaps      0x20(%[CC]), %%xmm5")               // xmm5     = step
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = v1
                __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = delta

                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                // 8x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("movaps      %%xmm3, %%xmm0")                    // xmm0     = i[0]
                __ASM_EMIT("movaps      %%xmm4, %%xmm1")                    // xmm1     = i[1]
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = i[0]*delta
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                    // xmm3     = x[0]' = x[0] + step
                __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    // xmm1     = i[1]*delta
                __ASM_EMIT("addps       %%xmm5, %%xmm4")                    // xmm4     = x[1]' = x[1] + step
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = v1 + i[0]*delta
                __ASM_EMIT("addps       %%xmm6, %%xmm1")                    // xmm1     = v1 + i[1]*delta
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")

                // 4x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movaps      %%xmm3, %%xmm0")                    // xmm0     = i[0]
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = x[0]*delta
                __ASM_EMIT("movaps      %%xmm4, %%xmm3")                    // xmm3     = x[0]' = x[1]
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = v1 + i[0]*delta
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")

                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")                    // xmm3     = x[0]*delta
                __ASM_EMIT("addps       %%xmm6, %%xmm3")                    // xmm3     = v1 + i[0]*delta
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       %%xmm3, 0x00(%[dst], %[off])")
                __ASM_EMIT("shufps      $0x39, %%xmm3, %%xmm3")             // shift xmm3
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")

                __ASM_EMIT("6:")
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        #define LRAMP_CORE(DST, SRC) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("movss       %[v1], %%xmm6") \
            __ASM_EMIT("movss       %[delta], %%xmm7") \
            __ASM_EMIT("movaps      0x00(%[CC]), %%xmm4")               /* xmm4     = i[0] */ \
            __ASM_EMIT("movaps      0x10(%[CC]), %%xmm5")               /* xmm5     = i[1] */ \
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             /* xmm6     = v1 */ \
            __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             /* xmm7     = delta */ \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jb          2f") \
            /* 8x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    /* xmm0     = i[0] */ \
            __ASM_EMIT("movaps      %%xmm5, %%xmm1")                    /* xmm1     = i[1] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    /* xmm0     = i[0]*delta */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    /* xmm1     = i[1]*delta */ \
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm4")               /* xmm4     = x[0]' = x[0] + step */ \
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")               /* xmm5     = x[1]' = x[1] + step */ \
            __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2")  /* xmm2     = src[0] */ \
            __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3")  /* xmm3     = src[1] */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    /* xmm0     = v1 + i[0]*delta */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                    /* xmm1     = v1 + i[1]*delta */ \
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    /* xmm0     = src[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm1")                    /* xmm1     = src[1] * (v1 + i[1]*delta) */ \
            __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jae         1b") \
            \
            /* 4x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    /* xmm0     = i[0] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    /* xmm0     = i[0]*delta */ \
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")                    /* xmm4     = x[0]' = x[0] + step */ \
            __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2")  /* xmm2     = src[0] */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    /* xmm0     = v1 + i[0]*delta */ \
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    /* xmm0     = src[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            \
            /* 1x blocks */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("5:") \
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    /* xmm0     = i[0] */ \
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    /* xmm0     = i[0]*delta */ \
            __ASM_EMIT("shufps      $0x39, %%xmm4, %%xmm4")             /* shift xmm4 */ \
            __ASM_EMIT("movss       0x00(%[" SRC "], %[off]), %%xmm2")  /* xmm2     = src[0] */ \
            __ASM_EMIT("addss       %%xmm6, %%xmm0")                    /* xmm0     = v1 + i[0]*delta */ \
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")                    /* xmm0     = src[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         5b") \
            \
            __ASM_EMIT("6:")


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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_CORE("dst", "dst")
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_CORE("dst", "src")
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        #undef LRAMP_CORE

        #define OP_DSEL(a, b)   a
        #define OP_RSEL(a, b)   b

        #define LRAMP_OP3_CORE(DST, A, B, OP, SEL) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("movss       %[v1], %%xmm6") \
            __ASM_EMIT("movss       %[delta], %%xmm7") \
            __ASM_EMIT("movaps      0x00(%[CC]), %%xmm4")               /* xmm4     = i[0] */ \
            __ASM_EMIT("movaps      0x10(%[CC]), %%xmm5")               /* xmm5     = i[1] */ \
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             /* xmm6     = v1 */ \
            __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             /* xmm7     = delta */ \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jb          2f") \
            /* 8x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    /* xmm0     = i[0] */ \
            __ASM_EMIT("movaps      %%xmm5, %%xmm1")                    /* xmm1     = i[1] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    /* xmm0     = i[0]*delta */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    /* xmm1     = i[1]*delta */ \
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm4")               /* xmm4     = x[0]' = x[0] + step */ \
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")               /* xmm5     = x[1]' = x[1] + step */ \
            __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm2")    /* xmm2     = b[0] */ \
            __ASM_EMIT("movups      0x10(%[" B "], %[off]), %%xmm3")    /* xmm3     = b[1] */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    /* xmm0     = v1 + i[0]*delta */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                    /* xmm1     = v1 + i[1]*delta */ \
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    /* xmm0     = b[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm1")                    /* xmm1     = b[1] * (v1 + i[1]*delta) */ \
            __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm2")    /* xmm2     = a[0] */ \
            __ASM_EMIT("movups      0x10(%[" A "], %[off]), %%xmm3")    /* xmm3     = b[1] */ \
            __ASM_EMIT(OP "ps       " SEL("%%xmm0, %%xmm2", "%%xmm2, %%xmm0")) /* xmm2     = a[0] OP b[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT(OP "ps       " SEL("%%xmm1, %%xmm3", "%%xmm3, %%xmm1")) /* xmm2     = a[0] OP b[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("movups      " SEL("%%xmm2", "%%xmm0") ", 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("movups      " SEL("%%xmm3", "%%xmm1") ", 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jae         1b") \
            \
            /* 4x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    /* xmm0     = i[0] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    /* xmm0     = i[0]*delta */ \
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")                    /* xmm4     = x[0]' = x[0] + step */ \
            __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm2")    /* xmm2     = b[0] */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    /* xmm0     = v1 + i[0]*delta */ \
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    /* xmm0     = b[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm2")    /* xmm2     = a[0] */ \
            __ASM_EMIT(OP "ps       " SEL("%%xmm0, %%xmm2", "%%xmm2, %%xmm0")) /* xmm2     = a[0] OP b[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("movups      " SEL("%%xmm2", "%%xmm0") ", 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            \
            /* 1x blocks */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("5:") \
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    /* xmm0     = i[0] */ \
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    /* xmm0     = i[0]*delta */ \
            __ASM_EMIT("shufps      $0x39, %%xmm4, %%xmm4")             /* shift xmm4 */ \
            __ASM_EMIT("movss       0x00(%[" B "], %[off]), %%xmm2")    /* xmm2     = b[0] */ \
            __ASM_EMIT("addss       %%xmm6, %%xmm0")                    /* xmm0     = v1 + i[0]*delta */ \
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")                    /* xmm0     = b[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("movss       0x00(%[" A "], %[off]), %%xmm2")    /* xmm2     = a[0] */ \
            __ASM_EMIT(OP "ss       " SEL("%%xmm0, %%xmm2", "%%xmm2, %%xmm0")) /* xmm2     = a[0] OP b[0] * (v1 + i[0]*delta) */ \
            __ASM_EMIT("movss       " SEL("%%xmm2", "%%xmm0") ", 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         5b") \
            \
            __ASM_EMIT("6:")

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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "add", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "sub", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "sub", OP_RSEL)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "mul", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "div", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "dst", "src", "div", OP_RSEL)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "add", OP_DSEL)
                : [off] "=&r" (off),
                  __IF_32([count] "+g" (count))
                  __IF_64([count] "+r" (count))
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "sub", OP_DSEL)
                : [off] "=&r" (off),
                  __IF_32([count] "+g" (count))
                  __IF_64([count] "+r" (count))
                  : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "sub", OP_RSEL)
                : [off] "=&r" (off),
                  __IF_32([count] "+g" (count))
                  __IF_64([count] "+r" (count))
                  : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "mul", OP_DSEL)
                : [off] "=&r" (off),
                  __IF_32([count] "+g" (count))
                  __IF_64([count] "+r" (count))
                  : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "div", OP_DSEL)
                : [off] "=&r" (off),
                  __IF_32([count] "+g" (count))
                  __IF_64([count] "+r" (count))
                  : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
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
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                LRAMP_OP3_CORE("dst", "a", "b", "div", OP_RSEL)
                : [off] "=&r" (off),
                  __IF_32([count] "+g" (count))
                  __IF_64([count] "+r" (count))
                  : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "r" (lramp_const),
                  [delta] "m" (delta),
                  [v1] "m" (v1)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        #undef LRAMP_OP3_CORE
        #undef OP_DSEL
        #undef OP_RSEL

    } /* namespace sse */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_SSE_PMATH_LRAMP_H_ */
