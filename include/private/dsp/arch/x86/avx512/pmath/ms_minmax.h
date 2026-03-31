/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 29 мар. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_MS_MINMAX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_MS_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t ms_minmax_abs[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff),
                LSP_DSP_VEC16(0x3f000000)           // 0.5f
            };
        )

        #define MS_MUL(x)       __ASM_EMIT(x)
        #define MS_NOMUL(x)

        #define MS_MINMAX_CORE(DST, A, B, OP, IF_MUL) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $32, %[count]") \
            IF_MUL("vmovaps         0x40 + %[CC], %%zmm6")                      /* zmm6 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 32x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%zmm0")            /* zmm0 = l    */ \
            __ASM_EMIT("vmovups     0x40(%[" A "], %[off]), %%zmm1") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%zmm4")            /* zmm4 = r    */ \
            __ASM_EMIT("vmovups     0x40(%[" B "], %[off]), %%zmm5") \
            __ASM_EMIT("vsubps      %%zmm4, %%zmm0, %%zmm2")                    /* zmm2 = s = l - r */ \
            __ASM_EMIT("vsubps      %%zmm5, %%zmm1, %%zmm3") \
            __ASM_EMIT("vaddps      %%zmm4, %%zmm0, %%zmm0")                    /* zmm0 = m = l + r */ \
            __ASM_EMIT("vaddps      %%zmm5, %%zmm1, %%zmm1") \
            __ASM_EMIT(OP "         %%zmm2, %%zmm0, %%zmm0")                    /* zmm0 = O = OP(m, s) */ \
            __ASM_EMIT(OP "         %%zmm3, %%zmm1, %%zmm1") \
            IF_MUL("vmulps          %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = O * 0.5f */ \
            IF_MUL("vmulps          %%zmm6, %%zmm1, %%zmm1") \
            __ASM_EMIT("vmovups     %%zmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm1, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 16x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $16, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%zmm0")            /* zmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%zmm4")            /* zmm4 = r    */ \
            __ASM_EMIT("vsubps      %%zmm4, %%zmm0, %%zmm2")                    /* zmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%zmm4, %%zmm0, %%zmm0")                    /* zmm0 = m = l + r */ \
            __ASM_EMIT(OP "         %%zmm2, %%zmm0, %%zmm0")                    /* zmm0 = O = OP(m, s) */ \
            IF_MUL("vmulps          %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%zmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("add         $0x40, %[off]") \
            /* 8x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT(OP "         %%ymm2, %%ymm0, %%ymm0")                    /* ymm0 = O = OP(m, s) */ \
            IF_MUL("vmulps          %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("add         $0x20, %[off]") \
            /* 4x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          8f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubps      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0, %%xmm0")                    /* xmm0 = O = OP(m, s) */ \
            IF_MUL("vmulps          %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* 1x blocks */ \
            __ASM_EMIT("8:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          10f")    \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubss      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddss      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0, %%xmm0")                    /* xmm0 = O = OP(m, s) */ \
            IF_MUL("vmulss          %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         9b") \
            __ASM_EMIT("10:")

        void ms_pmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "vminps", MS_MUL)
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
                MS_MINMAX_CORE("dst", "a", "b", "vminps", MS_MUL)
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
                MS_MINMAX_CORE("dst", "dst", "src", "vmaxps", MS_MUL)
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
                MS_MINMAX_CORE("dst", "a", "b", "vmaxps", MS_MUL)
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
                MS_MINMAX_CORE("dst", "dst", "src", "vminps", MS_NOMUL)
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
                MS_MINMAX_CORE("dst", "a", "b", "vminps", MS_NOMUL)
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
                MS_MINMAX_CORE("dst", "dst", "src", "vmaxps", MS_NOMUL)
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
                MS_MINMAX_CORE("dst", "a", "b", "vmaxps", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        #define MS_SIGN_MINMAX_CORE(DST, A, B, CMP, IF_MUL) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("vmovaps     0x00 + %[CC], %%zmm6")                      /* zmm6 = MASK */ \
            IF_MUL("vmovaps         0x40 + %[CC], %%zmm7")                      /* zmm7 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 32x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%zmm0")            /* zmm0 = l    */ \
            __ASM_EMIT("vmovups     0x40(%[" A "], %[off]), %%zmm1") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%zmm4")            /* zmm4 = r    */ \
            __ASM_EMIT("vmovups     0x40(%[" B "], %[off]), %%zmm5") \
            __ASM_EMIT("vsubps      %%zmm4, %%zmm0, %%zmm2")                    /* zmm2 = s = l - r */ \
            __ASM_EMIT("vsubps      %%zmm5, %%zmm1, %%zmm3") \
            __ASM_EMIT("vaddps      %%zmm4, %%zmm0, %%zmm0")                    /* zmm0 = m = l + r */ \
            __ASM_EMIT("vaddps      %%zmm5, %%zmm1, %%zmm1") \
            __ASM_EMIT("vandps      %%zmm6, %%zmm2, %%zmm5")                    /* zmm5 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%zmm6, %%zmm0, %%zmm4")                    /* zmm4 = fabsf(m) */ \
            __ASM_EMIT("vcmpps      " CMP ", %%zmm5, %%zmm4, %%k4")             /* k4   = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vandps      %%zmm6, %%zmm3, %%zmm5") \
            __ASM_EMIT("vandps      %%zmm6, %%zmm1, %%zmm4") \
            __ASM_EMIT("vcmpps      " CMP ", %%zmm5, %%zmm4, %%k5") \
            __ASM_EMIT("vmovaps     %%zmm0, %%zmm2 %{%%k4%}")                   /* zmm2 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            __ASM_EMIT("vmovaps     %%zmm1, %%zmm3 %{%%k5%}") \
            IF_MUL("vmulps          %%zmm7, %%zmm2, %%zmm2")                    /* zmm0 = O * 0.5f */ \
            IF_MUL("vmulps          %%zmm7, %%zmm3, %%zmm3") \
            __ASM_EMIT("vmovups     %%zmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm3, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 16x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $16, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%zmm0")            /* zmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%zmm4")            /* zmm4 = r    */ \
            __ASM_EMIT("vsubps      %%zmm4, %%zmm0, %%zmm2")                    /* zmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%zmm4, %%zmm0, %%zmm0")                    /* zmm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%zmm6, %%zmm2, %%zmm5")                    /* zmm5 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%zmm6, %%zmm0, %%zmm4")                    /* zmm4 = fabsf(m) */ \
            __ASM_EMIT("vcmpps      " CMP ", %%zmm5, %%zmm4, %%k4")             /* k4   = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vmovaps     %%zmm0, %%zmm2 %{%%k4%}")                   /* zmm2 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            IF_MUL("vmulps          %%zmm7, %%zmm2, %%zmm2")                    /* zmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%zmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("add         $0x40, %[off]") \
            /* 8x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          6f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%ymm6, %%ymm2, %%ymm1")                    /* ymm1 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%ymm6, %%ymm0, %%ymm3")                    /* ymm3 = fabsf(m) */ \
            __ASM_EMIT("vcmpps      " CMP ", %%ymm1, %%ymm3, %%ymm3")           /* ymm4 = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vblendvps   %%ymm3, %%ymm0, %%ymm2, %%ymm0")            /* ymm0 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            IF_MUL("vmulps          %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("add         $0x20, %[off]") \
            /* 4x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          8f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubps      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm2, %%xmm1")                    /* xmm1 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm3")                    /* xmm3 = fabsf(m) */ \
            __ASM_EMIT("vcmpps      " CMP ", %%xmm1, %%xmm3, %%xmm3")           /* xmm4 = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vblendvps   %%xmm3, %%xmm0, %%xmm2, %%xmm0")            /* xmm0 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            IF_MUL("vmulps          %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* 1x blocks */ \
            __ASM_EMIT("8:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          10f")    \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubss      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddss      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm2, %%xmm1")                    /* xmm1 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm3")                    /* xmm3 = fabsf(m) */ \
            __ASM_EMIT("vcmpps      " CMP ", %%xmm1, %%xmm3, %%xmm3")           /* xmm4 = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vblendvps   %%xmm3, %%xmm0, %%xmm2, %%xmm0")            /* xmm0 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            IF_MUL("vmulss          %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         9b") \
            __ASM_EMIT("10:")

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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5"
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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5"
            );
        }

        void ms_psmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "$5", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5"
            );
        }

        void ms_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "$5", MS_MUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5"
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
                  "%xmm4", "%xmm5", "%xmm6",
                  "%k4", "%k5"
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
                  "%xmm4", "%xmm5", "%xmm6",
                  "%k4", "%k5"
            );
        }

        void lr_psmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "$5", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6",
                  "%k4", "%k5"
            );
        }

        void lr_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "$5", MS_NOMUL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6",
                  "%k4", "%k5"
            );
        }

        #undef MS_SIGN_MINMAX_CORE

        #define MS_ABS_MINMAX_CORE(DST, A, B, OP, IF_MUL) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("vmovaps     0x00 + %[CC], %%zmm6")                      /* ymm6 = MASK */ \
            IF_MUL("vmovaps         0x40 + %[CC], %%zmm7")                      /* ymm7 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 32x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%zmm0")            /* zmm0 = l    */ \
            __ASM_EMIT("vmovups     0x40(%[" A "], %[off]), %%zmm1") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%zmm4")            /* zmm4 = r    */ \
            __ASM_EMIT("vmovups     0x40(%[" B "], %[off]), %%zmm5") \
            __ASM_EMIT("vsubps      %%zmm4, %%zmm0, %%zmm2")                    /* zmm2 = s = l - r */ \
            __ASM_EMIT("vsubps      %%zmm5, %%zmm1, %%zmm3") \
            __ASM_EMIT("vaddps      %%zmm4, %%zmm0, %%zmm0")                    /* zmm0 = m = l + r */ \
            __ASM_EMIT("vaddps      %%zmm5, %%zmm1, %%zmm1") \
            __ASM_EMIT("vandps      %%zmm6, %%zmm2, %%zmm2")                    /* zmm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%zmm6, %%zmm3, %%zmm3") \
            __ASM_EMIT("vandps      %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = fabsf(m) */ \
            __ASM_EMIT("vandps      %%zmm6, %%zmm1, %%zmm1") \
            __ASM_EMIT(OP "         %%zmm2, %%zmm0, %%zmm0")                    /* zmm0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT(OP "         %%zmm3, %%zmm1, %%zmm1") \
            IF_MUL("vmulps          %%zmm7, %%zmm0, %%zmm0")                    /* zmm0 = O * 0.5f */ \
            IF_MUL("vmulps          %%zmm7, %%zmm1, %%zmm1") \
            __ASM_EMIT("vmovups     %%zmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm1, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 16x blocks */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $16, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%zmm0")            /* zmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%zmm4")            /* zmm4 = r    */ \
            __ASM_EMIT("vsubps      %%zmm4, %%zmm0, %%zmm2")                    /* zmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%zmm4, %%zmm0, %%zmm0")                    /* zmm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%zmm6, %%zmm2, %%zmm2")                    /* zmm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = fabsf(m) */ \
            __ASM_EMIT(OP "         %%zmm2, %%zmm0, %%zmm0")                    /* zmm0 = O = OP(fabsf(m), fabsf(s)) */ \
            IF_MUL("vmulps          %%zmm7, %%zmm0, %%zmm0")                    /* zmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%zmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 8x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          6f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%ymm6, %%ymm2, %%ymm2")                    /* ymm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = fabsf(m) */ \
            __ASM_EMIT(OP "         %%ymm2, %%ymm0, %%ymm0")                    /* ymm0 = O = OP(fabsf(m), fabsf(s)) */ \
            IF_MUL("vmulps          %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("add         $0x20, %[off]") \
            /* 4x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          8f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubps      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm2, %%xmm2")                    /* xmm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = fabsf(m) */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0, %%xmm0")                    /* xmm0 = O = OP(fabsf(m), fabsf(s)) */ \
            IF_MUL("vmulps          %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* 1x blocks */ \
            __ASM_EMIT("8:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          10f")    \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubss      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddss      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm2, %%xmm2")                    /* xmm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = fabsf(m) */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0, %%xmm0")                    /* xmm0 = O = OP(fabsf(m), fabsf(s)) */ \
            IF_MUL("vmulss          %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         9b") \
            __ASM_EMIT("10:")

        void ms_pamin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vminps", MS_MUL)
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
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vminps", MS_MUL)
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
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vmaxps", MS_MUL)
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
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vmaxps", MS_MUL)
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
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vminps", MS_NOMUL)
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
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vminps", MS_NOMUL)
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
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vmaxps", MS_NOMUL)
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
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vmaxps", MS_NOMUL)
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

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_MS_MINMAX_H_ */
