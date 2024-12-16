/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 11 дек. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_HMATH_HDOTP_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_HMATH_HDOTP_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        float h_dotp(const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(
                float result;
                size_t off;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps          %%zmm0, %%zmm0, %%zmm0")
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("vxorps          %%zmm1, %%zmm1, %%zmm1")
                /* x64 blocks */
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm2")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm3")
                __ASM_EMIT("vmovups         0x80(%[a], %[off]), %%zmm4")
                __ASM_EMIT("vmovups         0xc0(%[a], %[off]), %%zmm5")
                __ASM_EMIT("vmulps          0x00(%[b], %[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          0x40(%[b], %[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vmulps          0x80(%[b], %[off]), %%zmm4, %%zmm4")
                __ASM_EMIT("vmulps          0xc0(%[b], %[off]), %%zmm5, %%zmm5")
                __ASM_EMIT("vaddps          %%zmm2, %%zmm0, %%zmm0")
                __ASM_EMIT("vaddps          %%zmm3, %%zmm1, %%zmm1")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm0, %%zmm0")
                __ASM_EMIT("vaddps          %%zmm5, %%zmm1, %%zmm1")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                /* x32 block */
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm2")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm3")
                __ASM_EMIT("vmulps          0x00(%[b], %[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          0x40(%[b], %[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vaddps          %%zmm2, %%zmm0, %%zmm0")
                __ASM_EMIT("vaddps          %%zmm3, %%zmm1, %%zmm1")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("4:")
                __ASM_EMIT("vaddps          %%zmm1, %%zmm0, %%zmm0")
                /* x16 block */
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm2")
                __ASM_EMIT("vmulps          0x00(%[b], %[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vaddps          %%zmm2, %%zmm0, %%zmm0")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("6:")
                __ASM_EMIT("vextractf64x4   $1, %%zmm0, %%ymm2")
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
                /* x8 block */
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm2")
                __ASM_EMIT("vmulps          0x00(%[b], %[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("8:")
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm2")
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
                /* x4 block */
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm2")
                __ASM_EMIT("vmulps          0x00(%[b], %[off]), %%xmm2, %%xmm2")
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("10:")
                /* x1 block */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm2")
                __ASM_EMIT("vmulss          0x00(%[b], %[off]), %%xmm2, %%xmm2")
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")
                /* end */
                : [count] "+r" (count), [off] "=&r" (off),
                  [res] "=Yz" (result)
                : [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );

            return result;
        }

        float h_sqr_dotp(const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(
                float result;
                size_t off;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps          %%zmm0, %%zmm0, %%zmm0")
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("vxorps          %%zmm1, %%zmm1, %%zmm1")
                /* x32 blocks */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm2")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm3")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm4")
                __ASM_EMIT("vmovups         0x40(%[b], %[off]), %%zmm5")
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          %%zmm3, %%zmm3, %%zmm3")
                __ASM_EMIT("vmulps          %%zmm4, %%zmm4, %%zmm4")
                __ASM_EMIT("vmulps          %%zmm5, %%zmm5, %%zmm5")
                __ASM_EMIT("vfmadd231ps     %%zmm4, %%zmm2, %%zmm0")
                __ASM_EMIT("vfmadd231ps     %%zmm5, %%zmm3, %%zmm1")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                __ASM_EMIT("vaddps          %%zmm1, %%zmm0, %%zmm0")
                /* x16 block */
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm2")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm4")
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          %%zmm4, %%zmm4, %%zmm4")
                __ASM_EMIT("vfmadd231ps     %%zmm4, %%zmm2, %%zmm0")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                __ASM_EMIT("vextractf64x4   $1, %%zmm0, %%ymm2")
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
                /* x8 block */
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm2")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm4")
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
                __ASM_EMIT("vfmadd231ps     %%ymm4, %%ymm2, %%ymm0")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm2")
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
                /* x4 block */
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm2")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm4")
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm4, %%xmm4")
                __ASM_EMIT("vfmadd231ps     %%xmm4, %%xmm2, %%xmm0")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("8:")
                /* x1 block */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm2")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm4")
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulss          %%xmm4, %%xmm4, %%xmm4")
                __ASM_EMIT("vfmadd231ss     %%xmm4, %%xmm2, %%xmm0")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")
                /* end */
                : [count] "+r" (count), [off] "=&r" (off),
                  [res] "=Yz" (result)
                : [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );

            return result;
        }

        IF_ARCH_X86(
            static const uint32_t h_abs_dotp_const[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff)
            };
        )

        float h_abs_dotp(const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(
                float result;
                size_t off;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps          %%zmm0, %%zmm0, %%zmm0")
                __ASM_EMIT("vmovaps         %[CC], %%zmm6")
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("vxorps          %%zmm1, %%zmm1, %%zmm1")
                __ASM_EMIT("vmovaps         %%zmm6, %%zmm7")
                /* x32 blocks */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vandps          0x00(%[a], %[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vandps          0x40(%[a], %[off]), %%zmm7, %%zmm3")
                __ASM_EMIT("vandps          0x00(%[b], %[off]), %%zmm6, %%zmm4")
                __ASM_EMIT("vandps          0x40(%[b], %[off]), %%zmm7, %%zmm5")
                __ASM_EMIT("vfmadd231ps     %%zmm4, %%zmm2, %%zmm0")
                __ASM_EMIT("vfmadd231ps     %%zmm5, %%zmm3, %%zmm1")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                __ASM_EMIT("vaddps          %%zmm1, %%zmm0, %%zmm0")
                /* x16 block */
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vandps          0x00(%[a], %[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vandps          0x00(%[b], %[off]), %%zmm6, %%zmm4")
                __ASM_EMIT("vfmadd231ps     %%zmm4, %%zmm2, %%zmm0")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                __ASM_EMIT("vextractf64x4   $1, %%zmm0, %%ymm2")
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
                /* x8 block */
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vandps          0x00(%[a], %[off]), %%ymm6, %%ymm2")
                __ASM_EMIT("vandps          0x00(%[b], %[off]), %%ymm6, %%ymm4")
                __ASM_EMIT("vfmadd231ps     %%ymm4, %%ymm2, %%ymm0")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm2")
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
                /* x4 block */
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vandps          0x00(%[a], %[off]), %%xmm6, %%xmm2")
                __ASM_EMIT("vandps          0x00(%[b], %[off]), %%xmm7, %%xmm4")
                __ASM_EMIT("vfmadd231ps     %%xmm4, %%xmm2, %%xmm0")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("8:")
                /* x1 block */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm2")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm4")
                __ASM_EMIT("vandps          %%xmm2, %%xmm6, %%xmm2")
                __ASM_EMIT("vandps          %%xmm4, %%xmm7, %%xmm4")
                __ASM_EMIT("vfmadd231ps     %%xmm4, %%xmm2, %%xmm0")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")
                /* end */
                : [count] "+r" (count), [off] "=&r" (off),
                  [res] "=Yz" (result)
                : [a] "r" (a), [b] "r" (b),
                  [CC] "m" (h_abs_dotp_const)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return result;
        }
    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_HMATH_HDOTP_H_ */
