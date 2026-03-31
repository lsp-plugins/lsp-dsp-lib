/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 мар. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PMATH_MS_MINMAX_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PMATH_MS_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        IF_ARCH_X86(
            static const uint32_t ms_minmax_abs[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7fffffff),
                LSP_DSP_VEC4(0x3f000000)        // 0.5f
            };
        )

        #define MS_MUL(x)       __ASM_EMIT(x)
        #define MS_NOMUL(x)

        #define MS_MINMAX_CORE(DST, A, B, OP, IF_MUL) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            IF_MUL("movaps          0x10 + %[CC], %%xmm6")              /* xmm6 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 8x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0")    /* xmm0 = l */ \
            __ASM_EMIT("movups      0x10(%[" A "], %[off]), %%xmm1") \
            __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm4")    /* xmm4 = r */ \
            __ASM_EMIT("movups      0x10(%[" B "], %[off]), %%xmm5") \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = l */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm3") \
            __ASM_EMIT("addps       %%xmm4, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm1") \
            __ASM_EMIT("subps       %%xmm4, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("subps       %%xmm5, %%xmm3") \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0")                    /* xmm0 = OP(m, s) */ \
            __ASM_EMIT(OP "         %%xmm3, %%xmm1") \
            IF_MUL("mulps           %%xmm6, %%xmm0")                    /* xmm0 = OP(m, s) * 0.5f */ \
            IF_MUL("mulps           %%xmm6, %%xmm1") \
            __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 4x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0")    /* xmm0 = l */ \
            __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm4")    /* xmm4 = r */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = l */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("subps       %%xmm4, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0")                    /* xmm0 = OP(m, s) */ \
            IF_MUL("mulps           %%xmm6, %%xmm0")                    /* xmm0 = OP(m, s) * 0.5f */ \
            __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* 1x blocks */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          6f")    \
            __ASM_EMIT("5:") \
            __ASM_EMIT("movss       0x00(%[" A "], %[off]), %%xmm0")    /* xmm0 = l */ \
            __ASM_EMIT("movss       0x00(%[" B "], %[off]), %%xmm4")    /* xmm4 = r */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = l */ \
            __ASM_EMIT("addss       %%xmm4, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("subss       %%xmm4, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0")                    /* xmm0 = OP(m, s) */ \
            IF_MUL("mulss           %%xmm6, %%xmm0")                    /* xmm0 = OP(m, s) * 0.5f */ \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         5b") \
            __ASM_EMIT("6:")

        void ms_pmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "minps", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void ms_pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "minps", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void ms_pmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "maxps", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void ms_pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "maxps", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void lr_pmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "minps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void lr_pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "minps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void lr_pmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "maxps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void lr_pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "maxps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        #undef MS_MINMAX_CORE


        #define MS_SIGN_MINMAX_CORE(DST, A, B, CMP, IF_MUL) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("movaps      0x00 + %[CC], %%xmm6")              /* xmm6 = MASK */ \
            IF_MUL("movaps          0x10 + %[CC], %%xmm7")              /* xmm7 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 4x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0")    /* xmm0 = l */ \
            __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm2")    /* xmm2 = r */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    /* xmm1 = l */ \
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("subps       %%xmm2, %%xmm1")                    /* xmm1 = s = l - r */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = m */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                    /* xmm3 = s */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm2")                    /* xmm2 = fabsf(m) */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm3")                    /* xmm3 = fabsf(s) */ \
            __ASM_EMIT("cmpps       " CMP ", %%xmm3, %%xmm2")           /* xmm2 = fabsf(m) <=> fabsf(s) */ \
            __ASM_EMIT("andps       %%xmm2, %%xmm0")                    /* xmm0 = m & (abs(m) <=> abs(s)) */ \
            __ASM_EMIT("andnps      %%xmm1, %%xmm2")                    /* xmm2 = s & ~(abs(m) <=> abs(s)) */ \
            __ASM_EMIT("orps        %%xmm2, %%xmm0")                    /* xmm0 = O = (m & (abs(m) <=> abs(s))) | (s & ~(abs(m) <=> abs(s))) */ \
            IF_MUL("mulps           %%xmm7, %%xmm0")                    /* xmm0 = O + 0.5f */ \
            __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 1x blocks */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("3:") \
            __ASM_EMIT("movss       0x00(%[" A "], %[off]), %%xmm0")    /* xmm0 = l */ \
            __ASM_EMIT("movss       0x00(%[" B "], %[off]), %%xmm2")    /* xmm2 = r */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    /* xmm1 = l */ \
            __ASM_EMIT("addss       %%xmm2, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("subss       %%xmm2, %%xmm1")                    /* xmm1 = s = l - r */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = m */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                    /* xmm3 = s */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm2")                    /* xmm2 = fabsf(m) */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm3")                    /* xmm3 = fabsf(s) */ \
            __ASM_EMIT("cmpps       " CMP ", %%xmm3, %%xmm2")           /* xmm2 = fabsf(m) <=> fabsf(s) */ \
            __ASM_EMIT("andps       %%xmm2, %%xmm0")                    /* xmm0 = m & (abs(m) <=> abs(s)) */ \
            __ASM_EMIT("andnps      %%xmm1, %%xmm2")                    /* xmm2 = s & ~(abs(m) <=> abs(s)) */ \
            __ASM_EMIT("orps        %%xmm2, %%xmm0")                    /* xmm0 = O = (m & (abs(m) <=> abs(s))) | (s & ~(abs(m) <=> abs(s))) */ \
            IF_MUL("mulss           %%xmm7, %%xmm0")                    /* xmm0 = O + 0.5f */ \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         3b") \
            __ASM_EMIT("4:")

        void ms_psmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "$1", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

        void ms_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "$1", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

        void ms_psmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "$6", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

        void ms_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "$6", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

        void lr_psmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "$1", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6"
            );
        }

        void lr_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "$1", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6"
            );
        }

        void lr_psmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "$6", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6"
            );
        }

        void lr_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "$6", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6"
            );
        }
        #undef MS_SIGN_MINMAX_CORE

        #define MS_ABS_MINMAX_CORE(DST, A, B, OP, IF_MUL) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("movaps      0x00 + %[CC], %%xmm6")              /* xmm6 = MASK */ \
            IF_MUL("movaps          0x10 + %[CC], %%xmm7")              /* xmm7 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 8x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0")    /* xmm0 = l */ \
            __ASM_EMIT("movups      0x10(%[" A "], %[off]), %%xmm1") \
            __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm4")    /* xmm4 = r */ \
            __ASM_EMIT("movups      0x10(%[" B "], %[off]), %%xmm5") \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = l */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm3") \
            __ASM_EMIT("addps       %%xmm4, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm1") \
            __ASM_EMIT("subps       %%xmm4, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("subps       %%xmm5, %%xmm3") \
            __ASM_EMIT("andps       %%xmm6, %%xmm0")                    /* xmm0 = fabsf(m) */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm1") \
            __ASM_EMIT("andps       %%xmm6, %%xmm2")                    /* xmm2 = fabsf(s) */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm3") \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0")                    /* xmm0 = OP(m, s) */ \
            __ASM_EMIT(OP "         %%xmm3, %%xmm1") \
            IF_MUL("mulps           %%xmm7, %%xmm0")                    /* xmm0 = OP(m, s) * 0.5f */ \
            IF_MUL("mulps           %%xmm7, %%xmm1") \
            __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 4x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0")    /* xmm0 = l */ \
            __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm4")    /* xmm4 = r */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = l */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("subps       %%xmm4, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm0")                    /* xmm0 = fabsf(m) */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm2")                    /* xmm2 = fabsf(s) */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0")                    /* xmm0 = OP(m, s) */ \
            IF_MUL("mulps           %%xmm7, %%xmm0")                    /* xmm0 = OP(m, s) * 0.5f */ \
            __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* 1x blocks */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          6f")    \
            __ASM_EMIT("5:") \
            __ASM_EMIT("movss       0x00(%[" A "], %[off]), %%xmm0")    /* xmm0 = l */ \
            __ASM_EMIT("movss       0x00(%[" B "], %[off]), %%xmm4")    /* xmm4 = r */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = l */ \
            __ASM_EMIT("addss       %%xmm4, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("subss       %%xmm4, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm0")                    /* xmm0 = fabsf(m) */ \
            __ASM_EMIT("andps       %%xmm6, %%xmm2")                    /* xmm2 = fabsf(s) */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0")                    /* xmm0 = OP(m, s) */ \
            IF_MUL("mulss           %%xmm7, %%xmm0")                    /* xmm0 = OP(m, s) * 0.5f */ \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         5b") \
            __ASM_EMIT("6:")

        void ms_pamin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "minps", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void ms_pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "a", "b", "minps", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void ms_pamax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "maxps", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void ms_pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "a", "b", "maxps", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }


        void lr_pamin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "minps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void lr_pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "a", "b", "minps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void lr_pamax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "maxps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void lr_pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "a", "b", "maxps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        #undef MS_ABS_MINMAX_CORE

        #undef MS_MUL
        #undef MS_NOMUL

    } /* namespace sse */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE_PMATH_MS_MINMAX_H_ */
