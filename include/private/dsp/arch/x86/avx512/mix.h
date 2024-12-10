/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 дек. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_MIX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_MIX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        void mix2(float *a, const float *b, float k1, float k2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                // 64x blocks
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("vbroadcastss    %[k1], %%zmm6")
                __ASM_EMIT("vbroadcastss    %[k2], %%zmm7")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%zmm6, %%zmm0")
                __ASM_EMIT("vmulps          0x040(%[a],%[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vmulps          0x080(%[a],%[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vmulps          0x0c0(%[a],%[off]), %%zmm6, %%zmm3")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%zmm7, %%zmm0")
                __ASM_EMIT("vfmadd231ps     0x040(%[b],%[off]), %%zmm7, %%zmm1")
                __ASM_EMIT("vfmadd231ps     0x080(%[b],%[off]), %%zmm7, %%zmm2")
                __ASM_EMIT("vfmadd231ps     0x0c0(%[b],%[off]), %%zmm7, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[a],%[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[a],%[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x080(%[a],%[off])")
                __ASM_EMIT("vmovups         %%zmm3, 0x0c0(%[a],%[off])")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%zmm6, %%zmm0")
                __ASM_EMIT("vmulps          0x040(%[a],%[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%zmm7, %%zmm0")
                __ASM_EMIT("vfmadd231ps     0x040(%[b],%[off]), %%zmm7, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[a],%[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[a],%[off])")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("add             $0x80, %[off]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%ymm6, %%ymm0")
                __ASM_EMIT("vmulps          0x020(%[a],%[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%ymm7, %%ymm0")
                __ASM_EMIT("vfmadd231ps     0x020(%[b],%[off]), %%ymm7, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x000(%[a],%[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x020(%[a],%[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x040, %[off]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vmulps          0x010(%[a],%[off]), %%xmm6, %%xmm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vfmadd231ps     0x010(%[b],%[off]), %%xmm7, %%xmm1")
                __ASM_EMIT("vmovups         %%xmm0, 0x000(%[a],%[off])")
                __ASM_EMIT("vmovups         %%xmm1, 0x010(%[a],%[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x020, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x000(%[a],%[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x010, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vfmadd231ss     0x00(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[a],%[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                // End
                __ASM_EMIT("12:")
                : [count] "+r" (count),
                  [off] "=&r" (off)
                : [a] "r" (a), [b] "r" (b),
                  [k1] "m" (k1), [k2] "m" (k2)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

        void mix_copy2(float *dst, const float *a, const float *b, float k1, float k2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                // 64x blocks
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("vbroadcastss    %[k1], %%zmm6")
                __ASM_EMIT("vbroadcastss    %[k2], %%zmm7")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%zmm6, %%zmm0")
                __ASM_EMIT("vmulps          0x040(%[a],%[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vmulps          0x080(%[a],%[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vmulps          0x0c0(%[a],%[off]), %%zmm6, %%zmm3")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%zmm7, %%zmm0")
                __ASM_EMIT("vfmadd231ps     0x040(%[b],%[off]), %%zmm7, %%zmm1")
                __ASM_EMIT("vfmadd231ps     0x080(%[b],%[off]), %%zmm7, %%zmm2")
                __ASM_EMIT("vfmadd231ps     0x0c0(%[b],%[off]), %%zmm7, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x080(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm3, 0x0c0(%[dst],%[off])")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%zmm6, %%zmm0")
                __ASM_EMIT("vmulps          0x040(%[a],%[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%zmm7, %%zmm0")
                __ASM_EMIT("vfmadd231ps     0x040(%[b],%[off]), %%zmm7, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("add             $0x80, %[off]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%ymm6, %%ymm0")
                __ASM_EMIT("vmulps          0x020(%[a],%[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%ymm7, %%ymm0")
                __ASM_EMIT("vfmadd231ps     0x020(%[b],%[off]), %%ymm7, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x020(%[dst],%[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x040, %[off]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vmulps          0x010(%[a],%[off]), %%xmm6, %%xmm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vfmadd231ps     0x010(%[b],%[off]), %%xmm7, %%xmm1")
                __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%xmm1, 0x010(%[dst],%[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x020, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x010, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vfmadd231ss     0x00(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                // End
                __ASM_EMIT("12:")
                : [count] "+r" (count),
                  [off] "=&r" (off)
                : [a] "r" (a), [b] "r" (b), [dst] "r" (dst),
                  [k1] "m" (k1), [k2] "m" (k2)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

        void mix_add2(float *dst, const float *a, const float *b, float k1, float k2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                // 64x blocks
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("vbroadcastss    %[k1], %%zmm6")
                __ASM_EMIT("vbroadcastss    %[k2], %%zmm7")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%zmm6, %%zmm0")
                __ASM_EMIT("vmulps          0x040(%[a],%[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vmulps          0x080(%[a],%[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vmulps          0x0c0(%[a],%[off]), %%zmm6, %%zmm3")
                __ASM_EMIT("vaddps          0x000(%[dst],%[off]), %%zmm0, %%zmm0")
                __ASM_EMIT("vaddps          0x040(%[dst],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vaddps          0x080(%[dst],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vaddps          0x0c0(%[dst],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%zmm7, %%zmm0")
                __ASM_EMIT("vfmadd231ps     0x040(%[b],%[off]), %%zmm7, %%zmm1")
                __ASM_EMIT("vfmadd231ps     0x080(%[b],%[off]), %%zmm7, %%zmm2")
                __ASM_EMIT("vfmadd231ps     0x0c0(%[b],%[off]), %%zmm7, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x080(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm3, 0x0c0(%[dst],%[off])")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%zmm6, %%zmm0")
                __ASM_EMIT("vmulps          0x040(%[a],%[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vaddps          0x000(%[dst],%[off]), %%zmm0, %%zmm0")
                __ASM_EMIT("vaddps          0x040(%[dst],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%zmm7, %%zmm0")
                __ASM_EMIT("vfmadd231ps     0x040(%[b],%[off]), %%zmm7, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("add             $0x80, %[off]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%ymm6, %%ymm0")
                __ASM_EMIT("vmulps          0x020(%[a],%[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vaddps          0x000(%[dst],%[off]), %%ymm0, %%ymm0")
                __ASM_EMIT("vaddps          0x020(%[dst],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%ymm7, %%ymm0")
                __ASM_EMIT("vfmadd231ps     0x020(%[b],%[off]), %%ymm7, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x020(%[dst],%[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x040, %[off]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vmulps          0x010(%[a],%[off]), %%xmm6, %%xmm1")
                __ASM_EMIT("vaddps          0x000(%[dst],%[off]), %%xmm0, %%xmm0")
                __ASM_EMIT("vaddps          0x010(%[dst],%[off]), %%xmm1, %%xmm1")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vfmadd231ps     0x010(%[b],%[off]), %%xmm7, %%xmm1")
                __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%xmm1, 0x010(%[dst],%[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x020, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmulps          0x000(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vaddps          0x000(%[dst],%[off]), %%xmm0, %%xmm0")
                __ASM_EMIT("vfmadd231ps     0x000(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x010, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm6, %%xmm0")
                __ASM_EMIT("vaddss          0x000(%[dst],%[off]), %%xmm0, %%xmm0")
                __ASM_EMIT("vfmadd231ss     0x00(%[b],%[off]), %%xmm7, %%xmm0")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                // End
                __ASM_EMIT("12:")
                : [count] "+r" (count),
                  [off] "=&r" (off)
                : [a] "r" (a), [b] "r" (b), [dst] "r" (dst),
                  [k1] "m" (k1), [k2] "m" (k2)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX512_MIX_H_ */
