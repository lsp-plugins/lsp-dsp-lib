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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_SSQRT_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_SSQRT_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
    #define SQRT_CORE(DST, SRC)   \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* 64x blocks */ \
        __ASM_EMIT("sub         $64, %[count]") \
        __ASM_EMIT("vxorps      %%zmm6, %%zmm6, %%zmm6") \
        __ASM_EMIT("vxorps      %%zmm7, %%zmm7, %%zmm7") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmaxps      0x00(%[" SRC "], %[off]), %%zmm6, %%zmm0") \
        __ASM_EMIT("vmaxps      0x40(%[" SRC "], %[off]), %%zmm7, %%zmm1") \
        __ASM_EMIT("vmaxps      0x80(%[" SRC "], %[off]), %%zmm6, %%zmm2") \
        __ASM_EMIT("vmaxps      0xc0(%[" SRC "], %[off]), %%zmm7, %%zmm3") \
        __ASM_EMIT("vsqrtps     %%zmm0, %%zmm0") \
        __ASM_EMIT("vsqrtps     %%zmm1, %%zmm1") \
        __ASM_EMIT("vsqrtps     %%zmm2, %%zmm2") \
        __ASM_EMIT("vsqrtps     %%zmm3, %%zmm3") \
        __ASM_EMIT("vmovups     %%zmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm2, 0x80(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm3, 0xc0(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x100, %[off]") \
        __ASM_EMIT("sub         $64, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 32x blocks */ \
        __ASM_EMIT("add         $32, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmaxps      0x00(%[" SRC "], %[off]), %%ymm6, %%ymm0") \
        __ASM_EMIT("vmaxps      0x20(%[" SRC "], %[off]), %%ymm7, %%ymm1") \
        __ASM_EMIT("vmaxps      0x40(%[" SRC "], %[off]), %%ymm6, %%ymm2") \
        __ASM_EMIT("vmaxps      0x60(%[" SRC "], %[off]), %%ymm7, %%ymm3") \
        __ASM_EMIT("vsqrtps     %%ymm0, %%ymm0") \
        __ASM_EMIT("vsqrtps     %%ymm1, %%ymm1") \
        __ASM_EMIT("vsqrtps     %%ymm2, %%ymm2") \
        __ASM_EMIT("vsqrtps     %%ymm3, %%ymm3") \
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
        __ASM_EMIT("vmaxps      0x00(%[" SRC "], %[off]), %%ymm6, %%ymm0") \
        __ASM_EMIT("vmaxps      0x20(%[" SRC "], %[off]), %%ymm7, %%ymm1") \
        __ASM_EMIT("vsqrtps     %%ymm0, %%ymm0") \
        __ASM_EMIT("vsqrtps     %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("6:") \
        /* 8x block */ \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmaxps      0x00(%[" SRC "], %[off]), %%xmm6, %%xmm0") \
        __ASM_EMIT("vmaxps      0x10(%[" SRC "], %[off]), %%xmm7, %%xmm1") \
        __ASM_EMIT("vcmpps      $2, %%xmm0, %%xmm4, %%xmm4") \
        __ASM_EMIT("vcmpps      $2, %%xmm1, %%xmm5, %%xmm5") \
        __ASM_EMIT("vsqrtps     %%xmm0, %%xmm0") \
        __ASM_EMIT("vsqrtps     %%xmm1, %%xmm1") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("8:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("vmaxps      0x00(%[" SRC "], %[off]), %%xmm6, %%xmm0") \
        __ASM_EMIT("vsqrtps     %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("10:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          12f") \
        __ASM_EMIT("11:") \
        __ASM_EMIT("vmaxss      0x00(%[" SRC "], %[off]), %%xmm6, %%xmm0") \
        __ASM_EMIT("vsqrtss     %%xmm0, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         11b") \
        __ASM_EMIT("12:")

        void ssqrt1(float *dst, size_t count)
        {
            size_t off;
            ARCH_X86_ASM
            (
                SQRT_CORE("dst", "dst")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void ssqrt2(float *dst, const float *src, size_t count)
        {
            size_t off;
            ARCH_X86_ASM
            (
                SQRT_CORE("dst", "src")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef SQRT_CORE
    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE_PMATH_SSQRT_H_ */
