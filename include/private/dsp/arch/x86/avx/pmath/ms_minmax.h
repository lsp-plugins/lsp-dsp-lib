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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_PMATH_MS_MINMAX_H_
#define PRIVATE_DSP_ARCH_X86_AVX_PMATH_MS_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        IF_ARCH_X86(
            static const uint32_t ms_minmax_abs[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0x7fffffff),
                LSP_DSP_VEC8(0x3f000000)            // 0.5f
            };
        )

        #define MS_MINMAX_CORE(DST, A, B, OP) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("vmovaps     0x20 + %[CC], %%ymm6")                      /* ymm6 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 16x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x20(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vmovups     0x20(%[" B "], %[off]), %%ymm5") \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vsubps      %%ymm5, %%ymm1, %%ymm3") \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT("vaddps      %%ymm5, %%ymm1, %%ymm1") \
            __ASM_EMIT(OP "         %%ymm2, %%ymm0, %%ymm0")                    /* ymm0 = O = OP(m, s) */ \
            __ASM_EMIT(OP "         %%ymm3, %%ymm1, %%ymm1") \
            __ASM_EMIT("vmulps      %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT("vmulps      %%ymm6, %%ymm1, %%ymm1") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 8x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT(OP "         %%ymm2, %%ymm0, %%ymm0")                    /* ymm0 = O = OP(m, s) */ \
            __ASM_EMIT("vmulps      %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("add         $0x20, %[off]") \
            /* 4x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubps      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0, %%xmm0")                    /* xmm0 = O = OP(m, s) */ \
            __ASM_EMIT("vmulps      %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* 1x blocks */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          8f")    \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubss      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddss      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0, %%xmm0")                    /* xmm0 = O = OP(m, s) */ \
            __ASM_EMIT("vmulss      %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         7b") \
            __ASM_EMIT("8:")

        void ms_pmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "vminps")
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
                MS_MINMAX_CORE("dst", "a", "b", "vminps")
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
                MS_MINMAX_CORE("dst", "dst", "src", "vmaxps")
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
                MS_MINMAX_CORE("dst", "a", "b", "vmaxps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

        #undef MS_MINMAX_CORE

        #define MS_SIGN_MINMAX_CORE(DST, A, B, CMP) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT64("vmovaps   0x00 + %[CC], %%ymm8")                      /* ymm8 = MASK */ \
            __ASM_EMIT64("vmovaps   0x20 + %[CC], %%ymm9")                      /* ymm9 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 16x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x20(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vmovups     0x20(%[" B "], %[off]), %%ymm5") \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vsubps      %%ymm5, %%ymm1, %%ymm3") \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT("vaddps      %%ymm5, %%ymm1, %%ymm1") \
            __ASM_EMIT64("vandps    %%ymm8, %%ymm2, %%ymm6")                    /* ymm6 = fabsf(s) */ \
            __ASM_EMIT64("vandps    %%ymm8, %%ymm3, %%ymm7") \
            __ASM_EMIT64("vandps    %%ymm8, %%ymm0, %%ymm4")                    /* ymm4 = fabsf(m) */ \
            __ASM_EMIT64("vandps    %%ymm8, %%ymm1, %%ymm5") \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%ymm2, %%ymm6")              /* ymm6 = fabsf(s) */ \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%ymm3, %%ymm7") \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%ymm0, %%ymm4")              /* ymm4 = fabsf(m) */ \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%ymm1, %%ymm5") \
            __ASM_EMIT("vcmpps      " CMP ", %%ymm6, %%ymm4, %%ymm4")           /* ymm4 = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vcmpps      " CMP ", %%ymm7, %%ymm5, %%ymm5") \
            __ASM_EMIT("vblendvps   %%ymm4, %%ymm0, %%ymm2, %%ymm0")            /* ymm0 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            __ASM_EMIT("vblendvps   %%ymm5, %%ymm1, %%ymm3, %%ymm1") \
            __ASM_EMIT64("vmulps    %%ymm9, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT64("vmulps    %%ymm9, %%ymm1, %%ymm1") \
            __ASM_EMIT32("vmulps    0x20 + %[CC], %%ymm0, %%ymm0")              /* ymm0 = O * 0.5f */ \
            __ASM_EMIT32("vmulps    0x20 + %[CC], %%ymm1, %%ymm1") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 8x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT64("vandps    %%ymm8, %%ymm2, %%ymm6")                    /* ymm6 = fabsf(s) */ \
            __ASM_EMIT64("vandps    %%ymm8, %%ymm0, %%ymm4")                    /* ymm4 = fabsf(m) */ \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%ymm2, %%ymm6")              /* ymm6 = fabsf(s) */ \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%ymm0, %%ymm4")              /* ymm4 = fabsf(m) */ \
            __ASM_EMIT("vcmpps      " CMP ", %%ymm6, %%ymm4, %%ymm4")           /* ymm4 = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vblendvps   %%ymm4, %%ymm0, %%ymm2, %%ymm0")            /* ymm0 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            __ASM_EMIT64("vmulps    %%ymm9, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT32("vmulps    0x20 + %[CC], %%ymm0, %%ymm0")              /* ymm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("add         $0x20, %[off]") \
            /* 4x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          6f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubps      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT64("vandps    %%xmm8, %%xmm2, %%xmm6")                    /* xmm6 = fabsf(s) */ \
            __ASM_EMIT64("vandps    %%xmm8, %%xmm0, %%xmm4")                    /* xmm4 = fabsf(m) */ \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%xmm2, %%xmm6")              /* xmm6 = fabsf(s) */ \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%xmm0, %%xmm4")              /* xmm4 = fabsf(m) */ \
            __ASM_EMIT("vcmpps      " CMP ", %%xmm6, %%xmm4, %%xmm4")           /* xmm4 = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vblendvps   %%xmm4, %%xmm0, %%xmm2, %%xmm0")            /* xmm0 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            __ASM_EMIT64("vmulps    %%xmm9, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT32("vmulps    0x20 + %[CC], %%xmm0, %%xmm0")              /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* 1x blocks */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          8f")    \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubss      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddss      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT64("vandps    %%xmm8, %%xmm2, %%xmm6")                    /* xmm6 = fabsf(s) */ \
            __ASM_EMIT64("vandps    %%xmm8, %%xmm0, %%xmm4")                    /* xmm4 = fabsf(m) */ \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%xmm2, %%xmm6")              /* xmm6 = fabsf(s) */ \
            __ASM_EMIT32("vandps    0x00 + %[CC], %%xmm0, %%xmm4")              /* xmm4 = fabsf(m) */ \
            __ASM_EMIT("vcmpss      " CMP ", %%xmm6, %%xmm4, %%xmm4")           /* xmm4 = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vblendvps   %%xmm4, %%xmm0, %%xmm2, %%xmm0")            /* xmm0 = O = OP(fabsf(m), fabsf(s)) ? m : s */ \
            __ASM_EMIT64("vmulss    %%xmm9, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT32("vmulss    0x20 + %[CC], %%xmm0, %%xmm0")              /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         7b") \
            __ASM_EMIT("8:")

        void ms_psmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "$1")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
            );
        }

        void ms_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "$1")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
            );
        }

        void ms_psmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "$5")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
            );
        }

        void ms_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "$5")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
            );
        }

        #undef MS_SIGN_MINMAX_CORE

        #define MS_ABS_MINMAX_CORE(DST, A, B, OP) \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("vmovaps     0x00 + %[CC], %%ymm6")                      /* ymm6 = MASK */ \
            __ASM_EMIT("vmovaps     0x20 + %[CC], %%ymm7")                      /* ymm7 = 0.5f */ \
            __ASM_EMIT("jb          2f")    \
            /* 16x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x20(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vmovups     0x20(%[" B "], %[off]), %%ymm5") \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vsubps      %%ymm5, %%ymm1, %%ymm3") \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT("vaddps      %%ymm5, %%ymm1, %%ymm1") \
            __ASM_EMIT("vandps      %%ymm6, %%ymm2, %%ymm2")                    /* ymm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%ymm6, %%ymm3, %%ymm3") \
            __ASM_EMIT("vandps      %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = fabsf(m) */ \
            __ASM_EMIT("vandps      %%ymm6, %%ymm1, %%ymm1") \
            __ASM_EMIT(OP "         %%ymm2, %%ymm0, %%ymm0")                    /* ymm0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT(OP "         %%ymm3, %%ymm1, %%ymm1") \
            __ASM_EMIT("vmulps      %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT("vmulps      %%ymm7, %%ymm1, %%ymm1") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 8x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0")            /* ymm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm4")            /* ymm4 = r    */ \
            __ASM_EMIT("vsubps      %%ymm4, %%ymm0, %%ymm2")                    /* ymm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")                    /* ymm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%ymm6, %%ymm2, %%ymm2")                    /* ymm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = fabsf(m) */ \
            __ASM_EMIT(OP "         %%ymm2, %%ymm0, %%ymm0")                    /* ymm0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vmulps      %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("add         $0x20, %[off]") \
            /* 4x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          6f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubps      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddps      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm2, %%xmm2")                    /* xmm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = fabsf(m) */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0, %%xmm0")                    /* xmm0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vmulps      %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* 1x blocks */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          8f")    \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0")            /* xmm0 = l    */ \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm4")            /* xmm4 = r    */ \
            __ASM_EMIT("vsubss      %%xmm4, %%xmm0, %%xmm2")                    /* xmm2 = s = l - r */ \
            __ASM_EMIT("vaddss      %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = m = l + r */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm2, %%xmm2")                    /* xmm2 = fabsf(s) */ \
            __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = fabsf(m) */ \
            __ASM_EMIT(OP "         %%xmm2, %%xmm0, %%xmm0")                    /* xmm0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT("vmulss      %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = O * 0.5f */ \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         7b") \
            __ASM_EMIT("8:")

        void ms_pamin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vminps")
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
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vminps")
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
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vmaxps")
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
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vmaxps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [CC] "o" (ms_minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef MS_ABS_MINMAX_CORE

    } /* namespace avx */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX_PMATH_MS_MINMAX_H_ */
