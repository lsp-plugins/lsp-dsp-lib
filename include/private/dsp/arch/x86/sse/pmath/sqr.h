/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 сент. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PMATH_SQR_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PMATH_SQR_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
    #define SQR_CORE(DST, SRC)   \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* 32x blocks */ \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x30(%[" SRC "], %[off]), %%xmm3") \
        __ASM_EMIT("movups      0x40(%[" SRC "], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x50(%[" SRC "], %[off]), %%xmm5") \
        __ASM_EMIT("movups      0x60(%[" SRC "], %[off]), %%xmm6") \
        __ASM_EMIT("movups      0x70(%[" SRC "], %[off]), %%xmm7") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm0") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm1") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm3, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm4, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm5, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm6, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm7, %%xmm7") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm2, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm3, 0x30(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm4, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm5, 0x50(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm6, 0x60(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm7, 0x70(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 16x block */ \
        __ASM_EMIT("add         $16, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x30(%[" SRC "], %[off]), %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm0") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm1") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm3, %%xmm3") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm2, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm3, 0x30(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("4:") \
        /* 8x block */ \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm0") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm1") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("6:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm0") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("8:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("movss       0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("mulss       %%xmm0, %%xmm0") \
        __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

        void sqr1(float *dst, size_t count)
        {
            size_t off;
            ARCH_X86_ASM
            (
                SQR_CORE("dst", "dst")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sqr2(float *dst, const float *src, size_t count)
        {
            size_t off;
            ARCH_X86_ASM
            (
                SQR_CORE("dst", "src")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef SQR_CORE
    } /* namespace sse */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE_PMATH_SQR_H_ */
