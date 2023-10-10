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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_SQR_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_SQR_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
    #define SQR_CORE(DST, SRC)   \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* 128x blocks */ \
        __ASM_EMIT("sub         $128, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%zmm0") \
        __ASM_EMIT("vmovups     0x40(%[" SRC "], %[off]), %%zmm1") \
        __ASM_EMIT("vmovups     0x80(%[" SRC "], %[off]), %%zmm2") \
        __ASM_EMIT("vmovups     0xc0(%[" SRC "], %[off]), %%zmm3") \
        __ASM_EMIT("vmovups     0x100(%[" SRC "], %[off]), %%zmm4") \
        __ASM_EMIT("vmovups     0x140(%[" SRC "], %[off]), %%zmm5") \
        __ASM_EMIT("vmovups     0x180(%[" SRC "], %[off]), %%zmm6") \
        __ASM_EMIT("vmovups     0x1c0(%[" SRC "], %[off]), %%zmm7") \
        __ASM_EMIT("vmulps      %%zmm0, %%zmm0, %%zmm0") \
        __ASM_EMIT("vmulps      %%zmm1, %%zmm1, %%zmm1") \
        __ASM_EMIT("vmulps      %%zmm2, %%zmm2, %%zmm2") \
        __ASM_EMIT("vmulps      %%zmm3, %%zmm3, %%zmm3") \
        __ASM_EMIT("vmulps      %%zmm4, %%zmm4, %%zmm4") \
        __ASM_EMIT("vmulps      %%zmm5, %%zmm5, %%zmm5") \
        __ASM_EMIT("vmulps      %%zmm6, %%zmm6, %%zmm6") \
        __ASM_EMIT("vmulps      %%zmm7, %%zmm7, %%zmm7") \
        __ASM_EMIT("vmovups     %%zmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm2, 0x80(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm3, 0xc0(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm4, 0x100(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm5, 0x140(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm6, 0x180(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm7, 0x1c0(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x200, %[off]") \
        __ASM_EMIT("sub         $128, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 64x blocks */ \
        __ASM_EMIT("add         $64, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x20(%[" SRC "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x40(%[" SRC "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups     0x60(%[" SRC "], %[off]), %%ymm3") \
        __ASM_EMIT("vmovups     0x80(%[" SRC "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0xa0(%[" SRC "], %[off]), %%ymm5") \
        __ASM_EMIT("vmovups     0xc0(%[" SRC "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0xe0(%[" SRC "], %[off]), %%ymm7") \
        __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm0") \
        __ASM_EMIT("vmulps      %%ymm1, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmulps      %%ymm2, %%ymm2, %%ymm2") \
        __ASM_EMIT("vmulps      %%ymm3, %%ymm3, %%ymm3") \
        __ASM_EMIT("vmulps      %%ymm4, %%ymm4, %%ymm4") \
        __ASM_EMIT("vmulps      %%ymm5, %%ymm5, %%ymm5") \
        __ASM_EMIT("vmulps      %%ymm6, %%ymm6, %%ymm6") \
        __ASM_EMIT("vmulps      %%ymm7, %%ymm7, %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm3, 0x60(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm4, 0x80(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm5, 0xa0(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm6, 0xc0(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0xe0(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $64, %[count]") \
        __ASM_EMIT("add         $0x100, %[off]") \
        __ASM_EMIT("4:") \
        /* 32x block */ \
        __ASM_EMIT("add         $32, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x20(%[" SRC "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x40(%[" SRC "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups     0x60(%[" SRC "], %[off]), %%ymm3") \
        __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm0") \
        __ASM_EMIT("vmulps      %%ymm1, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmulps      %%ymm2, %%ymm2, %%ymm2") \
        __ASM_EMIT("vmulps      %%ymm3, %%ymm3, %%ymm3") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm3, 0x60(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("6:") \
        /* 16x block */ \
        __ASM_EMIT("add         $16, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x20(%[" SRC "], %[off]), %%ymm1") \
        __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm0") \
        __ASM_EMIT("vmulps      %%ymm1, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("8:") \
        /* 8x block */ \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovups     0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmulps      %%xmm1, %%xmm1, %%xmm1") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("10:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          12f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("12:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          14f") \
        __ASM_EMIT("13:") \
        __ASM_EMIT("vmovss      0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("vmulss      %%xmm0, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         13b") \
        __ASM_EMIT("14:")

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
    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX_PMATH_SQR_H_ */
