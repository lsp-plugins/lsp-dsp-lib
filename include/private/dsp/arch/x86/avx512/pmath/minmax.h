/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_MINMAX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t minmax_abs[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff)
            };
        )

        #define MINMAX_CORE(DST, A, B, OP) \
            __ASM_EMIT("xor         %[off], %[off]") \
            /* 64x blocks */ \
            __ASM_EMIT("sub         $64, %[count]") \
            __ASM_EMIT("jb          2f")    \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%zmm0") \
            __ASM_EMIT("vmovups     0x40(%[" A "], %[off]), %%zmm1") \
            __ASM_EMIT("vmovups     0x80(%[" A "], %[off]), %%zmm2") \
            __ASM_EMIT("vmovups     0xc0(%[" A "], %[off]), %%zmm3") \
            __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%zmm0, %%zmm0") \
            __ASM_EMIT(OP "         0x40(%[" B "], %[off]), %%zmm1, %%zmm1") \
            __ASM_EMIT(OP "         0x80(%[" B "], %[off]), %%zmm2, %%zmm2") \
            __ASM_EMIT(OP "         0xc0(%[" B "], %[off]), %%zmm3, %%zmm3") \
            __ASM_EMIT("vmovups     %%zmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm1, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm2, 0x80(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm3, 0xc0(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x100, %[off]") \
            __ASM_EMIT("sub         $64, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* 32x block */ \
            __ASM_EMIT("add         $32, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups     0x20(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x40(%[" A "], %[off]), %%ymm2") \
            __ASM_EMIT("vmovups     0x60(%[" A "], %[off]), %%ymm3") \
            __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%ymm0, %%ymm0") \
            __ASM_EMIT(OP "         0x20(%[" B "], %[off]), %%ymm1, %%ymm1") \
            __ASM_EMIT(OP "         0x40(%[" B "], %[off]), %%ymm2, %%ymm2") \
            __ASM_EMIT(OP "         0x60(%[" B "], %[off]), %%ymm3, %%ymm3") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm2, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm3, 0x60(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("4:") \
            /* 16x block */ \
            __ASM_EMIT("add         $16, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups     0x20(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%ymm0, %%ymm0") \
            __ASM_EMIT(OP "         0x20(%[" B "], %[off]), %%ymm1, %%ymm1") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("6:") \
            /* 8x block */ \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          8f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovups     0x10(%[" A "], %[off]), %%xmm1") \
            __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%xmm0, %%xmm0") \
            __ASM_EMIT(OP "         0x10(%[" B "], %[off]), %%xmm1, %%xmm1") \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%xmm1, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("8:") \
            /* 4x block */ \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          10f") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%xmm0, %%xmm0") \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("10:") \
            /* 1x blocks */ \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          12f")    \
            __ASM_EMIT("11:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm1") \
            __ASM_EMIT(OP "         %%xmm1, %%xmm0, %%xmm0") \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         11b") \
            __ASM_EMIT("12:")

        void pmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MINMAX_CORE("dst", "dst", "src", "vminps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MINMAX_CORE("dst", "a", "b", "vminps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MINMAX_CORE("dst", "dst", "src", "vmaxps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                MINMAX_CORE("dst", "a", "b", "vmaxps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef MINMAX_CORE

        #define SIGN_MINMAX_CORE(DST, A, B, CMP) \
            __ASM_EMIT("vmovaps     %[X_SIGN], %%zmm6") \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("vmovaps     %%zmm6, %%zmm7") \
            /* 32x blocks */ \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("jb          2f")    \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%zmm0") \
            __ASM_EMIT("vmovups     0x40(%[" A "], %[off]), %%zmm1") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%zmm2") \
            __ASM_EMIT("vmovups     0x40(%[" B "], %[off]), %%zmm3") \
            __ASM_EMIT("vandps      %%zmm6, %%zmm0, %%zmm4") \
            __ASM_EMIT("vandps      %%zmm7, %%zmm2, %%zmm5") \
            __ASM_EMIT("vcmpps      " CMP ", %%zmm5, %%zmm4, %%k4") \
            __ASM_EMIT("vandps      %%zmm6, %%zmm1, %%zmm4") \
            __ASM_EMIT("vandps      %%zmm7, %%zmm3, %%zmm5") \
            __ASM_EMIT("vcmpps      " CMP ", %%zmm5, %%zmm4, %%k5") \
            __ASM_EMIT("vmovaps     %%zmm0, %%zmm2 %{%%k4%}") \
            __ASM_EMIT("vmovaps     %%zmm1, %%zmm3 %{%%k5%}") \
            __ASM_EMIT("vmovups     %%zmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm3, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* 8x blocks */ \
            __ASM_EMIT("add         $24, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("3:") \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm1") \
            __ASM_EMIT("vandps      %%ymm6, %%ymm0, %%ymm2") \
            __ASM_EMIT("vandps      %%ymm7, %%ymm1, %%ymm3") \
            __ASM_EMIT("vcmpps      " CMP ", %%ymm3, %%ymm2, %%ymm2") \
            __ASM_EMIT("vblendvps   %%ymm2, %%ymm0, %%ymm1, %%ymm0") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jge         3b") \
            __ASM_EMIT("4:") \
            /* 4x block */ \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          6f")    \
            __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm1") \
            __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm2") \
            __ASM_EMIT("vandps      %%xmm7, %%xmm1, %%xmm3") \
            __ASM_EMIT("vcmpps      " CMP ", %%xmm3, %%xmm2, %%xmm2") \
            __ASM_EMIT("vblendvps   %%xmm2, %%xmm0, %%xmm1, %%xmm0") \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("6:") \
            /* 1x blocks */ \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          8f")    \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm1") \
            __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm2") \
            __ASM_EMIT("vandps      %%xmm7, %%xmm1, %%xmm3") \
            __ASM_EMIT("vcmpps      " CMP ", %%xmm3, %%xmm2, %%xmm2") \
            __ASM_EMIT("vblendvps   %%xmm2, %%xmm0, %%xmm1, %%xmm0") \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         7b") \
            __ASM_EMIT("8:")

        void psmin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                SIGN_MINMAX_CORE("dst", "dst", "src", "$1")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [X_SIGN] "m" (minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5"
            );
        }

        void psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                SIGN_MINMAX_CORE("dst", "a", "b", "$1")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [X_SIGN] "m" (minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5"
            );
        }

        void psmax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                SIGN_MINMAX_CORE("dst", "dst", "src", "$5")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [X_SIGN] "m" (minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5"
            );
        }

        void psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                SIGN_MINMAX_CORE("dst", "a", "b", "$5")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [X_SIGN] "m" (minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5"
            );
        }

        #undef SIGN_MINMAX_CORE

        #define ABS_MINMAX_CORE(DST, A, B, OP) \
            __ASM_EMIT("vmovaps     %[X_SIGN], %%zmm6") \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("vmovaps     %%zmm6, %%zmm7") \
            /* 48x blocks */ \
            __ASM_EMIT("sub         $48, %[count]") \
            __ASM_EMIT("jb          2f")    \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vandps      0x00(%[" A "], %[off]), %%zmm6, %%zmm0") \
            __ASM_EMIT("vandps      0x40(%[" A "], %[off]), %%zmm7, %%zmm1") \
            __ASM_EMIT("vandps      0x80(%[" A "], %[off]), %%zmm6, %%zmm2") \
            __ASM_EMIT("vandps      0x00(%[" B "], %[off]), %%zmm7, %%zmm3") \
            __ASM_EMIT("vandps      0x40(%[" B "], %[off]), %%zmm6, %%zmm4") \
            __ASM_EMIT("vandps      0x80(%[" B "], %[off]), %%zmm7, %%zmm5") \
            __ASM_EMIT(OP "         %%zmm3, %%zmm0, %%zmm0") \
            __ASM_EMIT(OP "         %%zmm4, %%zmm1, %%zmm1") \
            __ASM_EMIT(OP "         %%zmm5, %%zmm2, %%zmm2") \
            __ASM_EMIT("vmovups     %%zmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm1, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm2, 0x80(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0xc0, %[off]") \
            __ASM_EMIT("sub         $48, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* 16x blocks */ \
            __ASM_EMIT("add         $32, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("3:") \
            __ASM_EMIT("vandps      0x00(%[" A "], %[off]), %%ymm6, %%ymm0") \
            __ASM_EMIT("vandps      0x20(%[" A "], %[off]), %%ymm7, %%ymm1") \
            __ASM_EMIT("vandps      0x00(%[" B "], %[off]), %%ymm6, %%ymm3") \
            __ASM_EMIT("vandps      0x20(%[" B "], %[off]), %%ymm7, %%ymm4") \
            __ASM_EMIT(OP "         %%ymm3, %%ymm0, %%ymm0") \
            __ASM_EMIT(OP "         %%ymm4, %%ymm1, %%ymm1") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("jge         3b") \
            __ASM_EMIT("4:") \
            /* 8x block */ \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          6f")    \
            __ASM_EMIT("vandps      0x00(%[" A "], %[off]), %%xmm6, %%xmm0") \
            __ASM_EMIT("vandps      0x10(%[" A "], %[off]), %%xmm7, %%xmm1") \
            __ASM_EMIT("vandps      0x00(%[" B "], %[off]), %%xmm6, %%xmm3") \
            __ASM_EMIT("vandps      0x10(%[" B "], %[off]), %%xmm7, %%xmm4") \
            __ASM_EMIT(OP "         %%xmm3, %%xmm0, %%xmm0") \
            __ASM_EMIT(OP "         %%xmm4, %%xmm1, %%xmm1") \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%xmm1, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("6:") \
            /* 4x block */ \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          8f")    \
            __ASM_EMIT("vandps      0x00(%[" A "], %[off]), %%xmm6, %%xmm0") \
            __ASM_EMIT("vandps      0x00(%[" B "], %[off]), %%xmm7, %%xmm3") \
            __ASM_EMIT(OP "         %%xmm3, %%xmm0, %%xmm0") \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("8:") \
            /* 1x blocks */ \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          10f")    \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm3") \
            __ASM_EMIT("vandps      %%xmm0, %%xmm6, %%xmm0") \
            __ASM_EMIT("vandps      %%xmm3, %%xmm7, %%xmm3") \
            __ASM_EMIT(OP "         %%xmm3, %%xmm0, %%xmm0") \
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         9b") \
            __ASM_EMIT("10:")

        void pamin2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                ABS_MINMAX_CORE("dst", "dst", "src", "vminps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [X_SIGN] "m" (minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                ABS_MINMAX_CORE("dst", "a", "b", "vminps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [X_SIGN] "m" (minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pamax2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                ABS_MINMAX_CORE("dst", "dst", "src", "vmaxps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [X_SIGN] "m" (minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                ABS_MINMAX_CORE("dst", "a", "b", "vmaxps")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [X_SIGN] "m" (minmax_abs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef ABS_MINMAX_CORE
    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_MINMAX_H_ */
