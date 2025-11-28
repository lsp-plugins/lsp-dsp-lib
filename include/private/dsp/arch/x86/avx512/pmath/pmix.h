/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 нояб. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_PMIX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_PMIX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        void pmix_v1(float *dst, const float *src, const float *k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                // 64x blocks
                __ASM_EMIT("sub                 $64, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%zmm0")           /* zmm0 = s */
                __ASM_EMIT("vmovups             0x40(%[src],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x80(%[src],%[off]), %%zmm2")
                __ASM_EMIT("vmovups             0xc0(%[src],%[off]), %%zmm3")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%zmm4")           /* zmm4 = d */
                __ASM_EMIT("vmovups             0x40(%[dst],%[off]), %%zmm5")
                __ASM_EMIT("vmovups             0x80(%[dst],%[off]), %%zmm6")
                __ASM_EMIT("vmovups             0xc0(%[dst],%[off]), %%zmm7")
                __ASM_EMIT("vsubps              %%zmm4, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vsubps              %%zmm5, %%zmm1, %%zmm1")
                __ASM_EMIT("vsubps              %%zmm6, %%zmm2, %%zmm2")
                __ASM_EMIT("vsubps              %%zmm7, %%zmm3, %%zmm3")
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%zmm4, %%zmm0")     /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vfmadd132ps         0x40(%[k],%[off]), %%zmm5, %%zmm1")
                __ASM_EMIT("vfmadd132ps         0x80(%[k],%[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vfmadd132ps         0xc0(%[k],%[off]), %%zmm7, %%zmm3")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm2, 0x80(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm3, 0xc0(%[dst],%[off])")
                __ASM_EMIT("add                 $0x100, %[off]")
                __ASM_EMIT("sub                 $64, %[count]")
                __ASM_EMIT("jae                 1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add                 $32, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%zmm0")           /* zmm0 = s */
                __ASM_EMIT("vmovups             0x40(%[src],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%zmm4")           /* zmm4 = d */
                __ASM_EMIT("vmovups             0x40(%[dst],%[off]), %%zmm5")
                __ASM_EMIT("vsubps              %%zmm4, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vsubps              %%zmm5, %%zmm1, %%zmm1")
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%zmm4, %%zmm0")     /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vfmadd132ps         0x40(%[k],%[off]), %%zmm5, %%zmm1")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("add                 $0x80, %[off]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%zmm0")           /* zmm0 = s */
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%zmm4")           /* zmm4 = d */
                __ASM_EMIT("vsubps              %%zmm4, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%zmm4, %%zmm0")     /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")           /* ymm0 = s */
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm4")           /* ymm4 = d */
                __ASM_EMIT("vsubps              %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = s - d */
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%ymm4, %%ymm0")     /* ymm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%xmm0")           /* xmm0 = s */
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%xmm4")           /* xmm4 = d */
                __ASM_EMIT("vsubps              %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = s - d */
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%xmm4, %%xmm0")     /* xmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss              0x00(%[src],%[off]), %%xmm0")           /* xmm0 = s */
                __ASM_EMIT("vmovss              0x00(%[dst],%[off]), %%xmm4")           /* xmm4 = d */
                __ASM_EMIT("vsubss              %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = s - d */
                __ASM_EMIT("vfmadd132ss         0x00(%[k],%[off]), %%xmm4, %%xmm0")     /* xmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 11b")
                // End
                __ASM_EMIT("12:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src), [k] "r" (k)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                // 64x blocks
                __ASM_EMIT32("subl              $64, %[count]")
                __ASM_EMIT64("sub               $64, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%zmm0")          /* zmm0 = s */
                __ASM_EMIT("vmovups             0x40(%[src2],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x80(%[src2],%[off]), %%zmm2")
                __ASM_EMIT("vmovups             0xc0(%[src2],%[off]), %%zmm3")
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%zmm4")          /* zmm4 = d */
                __ASM_EMIT("vmovups             0x40(%[src1],%[off]), %%zmm5")
                __ASM_EMIT("vmovups             0x80(%[src1],%[off]), %%zmm6")
                __ASM_EMIT("vmovups             0xc0(%[src1],%[off]), %%zmm7")
                __ASM_EMIT("vsubps              %%zmm4, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vsubps              %%zmm5, %%zmm1, %%zmm1")
                __ASM_EMIT("vsubps              %%zmm6, %%zmm2, %%zmm2")
                __ASM_EMIT("vsubps              %%zmm7, %%zmm3, %%zmm3")
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%zmm4, %%zmm0")     /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vfmadd132ps         0x40(%[k],%[off]), %%zmm5, %%zmm1")
                __ASM_EMIT("vfmadd132ps         0x80(%[k],%[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vfmadd132ps         0xc0(%[k],%[off]), %%zmm7, %%zmm3")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm2, 0x80(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm3, 0xc0(%[dst],%[off])")
                __ASM_EMIT("add                 $0x100, %[off]")
                __ASM_EMIT32("subl              $64, %[count]")
                __ASM_EMIT64("sub               $64, %[count]")
                __ASM_EMIT("jae                 1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("addl              $32, %[count]")
                __ASM_EMIT64("add               $32, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%zmm0")          /* zmm0 = s */
                __ASM_EMIT("vmovups             0x40(%[src2],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%zmm4")          /* zmm4 = d */
                __ASM_EMIT("vmovups             0x40(%[src1],%[off]), %%zmm5")
                __ASM_EMIT("vsubps              %%zmm4, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vsubps              %%zmm5, %%zmm1, %%zmm1")
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%zmm4, %%zmm0")     /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vfmadd132ps         0x40(%[k],%[off]), %%zmm5, %%zmm1")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT32("subl              $32, %[count]")
                __ASM_EMIT64("sub               $32, %[count]")
                __ASM_EMIT("add                 $0x80, %[off]")
                // 16x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT32("addl              $16, %[count]")
                __ASM_EMIT64("add               $16, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%zmm0")          /* zmm0 = s */
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%zmm4")          /* zmm4 = d */
                __ASM_EMIT("vsubps              %%zmm4, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%zmm4, %%zmm0")     /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x40, %[off]")
                __ASM_EMIT32("subl              $16, %[count]")
                __ASM_EMIT64("sub               $16, %[count]")
                __ASM_EMIT("jae                 1b")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT32("addl              $8, %[count]")
                __ASM_EMIT64("add               $8, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%ymm0")          /* ymm0 = s */
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%ymm4")          /* ymm4 = d */
                __ASM_EMIT("vsubps              %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = s - d */
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%ymm4, %%ymm0")     /* ymm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT32("subl              $8, %[count]")
                __ASM_EMIT64("sub               $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT32("addl              $4, %[count]")
                __ASM_EMIT64("add               $4, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%xmm0")          /* xmm0 = s */
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%xmm4")          /* xmm4 = d */
                __ASM_EMIT("vsubps              %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = s - d */
                __ASM_EMIT("vfmadd132ps         0x00(%[k],%[off]), %%xmm4, %%xmm0")     /* xmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT32("subl              $4, %[count]")
                __ASM_EMIT64("sub               $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT32("addl              $3, %[count]")
                __ASM_EMIT64("add               $3, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss              0x00(%[src2],%[off]), %%xmm0")          /* xmm0 = s */
                __ASM_EMIT("vmovss              0x00(%[src1],%[off]), %%xmm4")          /* xmm4 = d */
                __ASM_EMIT("vsubss              %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = s - d */
                __ASM_EMIT("vfmadd132ss         0x00(%[k],%[off]), %%xmm4, %%xmm0")     /* xmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jge                 11b")
                // End
                __ASM_EMIT("12:")
                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2), [k] "r" (k)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pmix_k1(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                __ASM_EMIT("vbroadcastss        %[k], %%zmm4")
                // 32x blocks
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%zmm0")           /* zmm0 = s */
                __ASM_EMIT("vmovups             0x40(%[src],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%zmm2")           /* zmm2 = d */
                __ASM_EMIT("vmovups             0x40(%[dst],%[off]), %%zmm3")
                __ASM_EMIT("vsubps              %%zmm2, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vsubps              %%zmm3, %%zmm1, %%zmm1")
                __ASM_EMIT("vfmadd132ps         %%zmm4, %%zmm2, %%zmm0")                /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vfmadd132ps         %%zmm4, %%zmm3, %%zmm1")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("add                 $0x80, %[off]")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jae                 1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%zmm0")           /* zmm0 = s */
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%zmm2")           /* zmm2 = d */
                __ASM_EMIT("vsubps              %%zmm2, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vfmadd132ps         %%zmm4, %%zmm2, %%zmm0")                /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")           /* ymm0 = s */
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm2")           /* ymm2 = d */
                __ASM_EMIT("vsubps              %%ymm2, %%ymm0, %%ymm0")                /* ymm0 = s - d */
                __ASM_EMIT("vfmadd132ps         %%ymm4, %%ymm2, %%ymm0")                /* ymm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%xmm0")           /* xmm0 = s */
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%xmm2")           /* xmm2 = d */
                __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = s - d */
                __ASM_EMIT("vfmadd132ps         %%xmm4, %%xmm2, %%xmm0")                /* xmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss              0x00(%[src],%[off]), %%xmm0")           /* xmm0 = s */
                __ASM_EMIT("vmovss              0x00(%[dst],%[off]), %%xmm2")           /* xmm2 = d */
                __ASM_EMIT("vsubss              %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = s - d */
                __ASM_EMIT("vfmadd132ss         %%xmm4, %%xmm2, %%xmm0")                /* xmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 9b")
                // End
                __ASM_EMIT("10:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [k] "m" (k)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4"
            );
        }

        void pmix_k2(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                __ASM_EMIT("vbroadcastss        %[k], %%zmm4")
                // 32x blocks
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%zmm0")          /* zmm0 = s */
                __ASM_EMIT("vmovups             0x40(%[src2],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%zmm2")          /* zmm2 = d */
                __ASM_EMIT("vmovups             0x40(%[src1],%[off]), %%zmm3")
                __ASM_EMIT("vsubps              %%zmm2, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vsubps              %%zmm3, %%zmm1, %%zmm1")
                __ASM_EMIT("vfmadd132ps         %%zmm4, %%zmm2, %%zmm0")                /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vfmadd132ps         %%zmm4, %%zmm3, %%zmm1")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("add                 $0x80, %[off]")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jae                 1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%zmm0")          /* zmm0 = s */
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%zmm2")          /* zmm2 = d */
                __ASM_EMIT("vsubps              %%zmm2, %%zmm0, %%zmm0")                /* zmm0 = s - d */
                __ASM_EMIT("vfmadd132ps         %%zmm4, %%zmm2, %%zmm0")                /* zmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%ymm0")          /* ymm0 = s */
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%ymm2")          /* ymm2 = d */
                __ASM_EMIT("vsubps              %%ymm2, %%ymm0, %%ymm0")                /* ymm0 = s - d */
                __ASM_EMIT("vfmadd132ps         %%ymm4, %%ymm2, %%ymm0")                /* ymm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[src2],%[off]), %%xmm0")          /* xmm0 = s */
                __ASM_EMIT("vmovups             0x00(%[src1],%[off]), %%xmm2")          /* xmm2 = d */
                __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = s - d */
                __ASM_EMIT("vfmadd132ps         %%xmm4, %%xmm2, %%xmm0")                /* xmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss              0x00(%[src2],%[off]), %%xmm0")          /* xmm0 = s */
                __ASM_EMIT("vmovss              0x00(%[src1],%[off]), %%xmm2")          /* xmm2 = d */
                __ASM_EMIT("vsubss              %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = s - d */
                __ASM_EMIT("vfmadd132ss         %%xmm4, %%xmm2, %%xmm0")                /* xmm0 = d + (s-d)*k */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 9b")
                // End
                __ASM_EMIT("10:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2),
                  [k] "m" (k)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4"
            );
        }
    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_PMIX_H_ */
