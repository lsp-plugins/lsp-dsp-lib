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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_PMATH_CLAMP_H_
#define PRIVATE_DSP_ARCH_X86_AVX_PMATH_CLAMP_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        void clamp_vv1(float *dst, const float *min, const float *max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                // 64x blocks
                __ASM_EMIT("sub                 $64, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[dst],%[off]), %%ymm1")
                __ASM_EMIT("vmovups             0x40(%[dst],%[off]), %%ymm2")
                __ASM_EMIT("vmovups             0x60(%[dst],%[off]), %%ymm3")
                __ASM_EMIT("vmovups             0x80(%[dst],%[off]), %%ymm4")
                __ASM_EMIT("vmovups             0xa0(%[dst],%[off]), %%ymm5")
                __ASM_EMIT("vmovups             0xc0(%[dst],%[off]), %%ymm6")
                __ASM_EMIT("vmovups             0xe0(%[dst],%[off]), %%ymm7")
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x20(%[min],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vmaxps              0x40(%[min],%[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vmaxps              0x60(%[min],%[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vmaxps              0x80(%[min],%[off]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmaxps              0xa0(%[min],%[off]), %%ymm5, %%ymm5")
                __ASM_EMIT("vmaxps              0xc0(%[min],%[off]), %%ymm6, %%ymm6")
                __ASM_EMIT("vmaxps              0xe0(%[min],%[off]), %%ymm7, %%ymm7")
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x20(%[max],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              0x40(%[max],%[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vminps              0x60(%[max],%[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vminps              0x80(%[max],%[off]), %%ymm4, %%ymm4")
                __ASM_EMIT("vminps              0xa0(%[max],%[off]), %%ymm5, %%ymm5")
                __ASM_EMIT("vminps              0xc0(%[max],%[off]), %%ymm6, %%ymm6")
                __ASM_EMIT("vminps              0xe0(%[max],%[off]), %%ymm7, %%ymm7")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm2, 0x40(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm3, 0x60(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm4, 0x80(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm5, 0xa0(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm6, 0xc0(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm7, 0xe0(%[dst],%[off])")
                __ASM_EMIT("add                 $0x100, %[off]")
                __ASM_EMIT("sub                 $64, %[count]")
                __ASM_EMIT("jae                 1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add                 $32, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[dst],%[off]), %%ymm1")
                __ASM_EMIT("vmovups             0x40(%[dst],%[off]), %%ymm2")
                __ASM_EMIT("vmovups             0x60(%[dst],%[off]), %%ymm3")
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x20(%[min],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vmaxps              0x40(%[min],%[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vmaxps              0x60(%[min],%[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x20(%[max],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              0x40(%[max],%[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vminps              0x60(%[max],%[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm2, 0x40(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm3, 0x60(%[dst],%[off])")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("add                 $0x80, %[off]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[dst],%[off]), %%ymm1")
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x20(%[min],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x20(%[max],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%xmm0, %%xmm0")       /* xmm0 = max(x, min) */
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%xmm0, %%xmm0")       /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss              0x00(%[dst],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxss              0x00(%[min],%[off]), %%xmm0, %%xmm0")       /* xmm0 = max(x, min) */
                __ASM_EMIT("vminss              0x00(%[max],%[off]), %%xmm0, %%xmm0")       /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 11b")
                // End
                __ASM_EMIT("12:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [min] "r" (min), [max] "r" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void clamp_vv2(float *dst, const float *src, const float *min, const float *max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                // 64x blocks
                __ASM_EMIT64("sub               $64, %[count]")
                __ASM_EMIT32("subl              $64, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[src],%[off]), %%ymm1")
                __ASM_EMIT("vmovups             0x40(%[src],%[off]), %%ymm2")
                __ASM_EMIT("vmovups             0x60(%[src],%[off]), %%ymm3")
                __ASM_EMIT("vmovups             0x80(%[src],%[off]), %%ymm4")
                __ASM_EMIT("vmovups             0xa0(%[src],%[off]), %%ymm5")
                __ASM_EMIT("vmovups             0xc0(%[src],%[off]), %%ymm6")
                __ASM_EMIT("vmovups             0xe0(%[src],%[off]), %%ymm7")
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x20(%[min],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vmaxps              0x40(%[min],%[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vmaxps              0x60(%[min],%[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vmaxps              0x80(%[min],%[off]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmaxps              0xa0(%[min],%[off]), %%ymm5, %%ymm5")
                __ASM_EMIT("vmaxps              0xc0(%[min],%[off]), %%ymm6, %%ymm6")
                __ASM_EMIT("vmaxps              0xe0(%[min],%[off]), %%ymm7, %%ymm7")
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x20(%[max],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              0x40(%[max],%[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vminps              0x60(%[max],%[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vminps              0x80(%[max],%[off]), %%ymm4, %%ymm4")
                __ASM_EMIT("vminps              0xa0(%[max],%[off]), %%ymm5, %%ymm5")
                __ASM_EMIT("vminps              0xc0(%[max],%[off]), %%ymm6, %%ymm6")
                __ASM_EMIT("vminps              0xe0(%[max],%[off]), %%ymm7, %%ymm7")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm2, 0x40(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm3, 0x60(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm4, 0x80(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm5, 0xa0(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm6, 0xc0(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm7, 0xe0(%[dst],%[off])")
                __ASM_EMIT("add                 $0x100, %[off]")
                __ASM_EMIT64("sub               $64, %[count]")
                __ASM_EMIT32("subl              $64, %[count]")
                __ASM_EMIT("jae                 1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT64("add               $32, %[count]")
                __ASM_EMIT32("addl              $32, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[src],%[off]), %%ymm1")
                __ASM_EMIT("vmovups             0x40(%[src],%[off]), %%ymm2")
                __ASM_EMIT("vmovups             0x60(%[src],%[off]), %%ymm3")
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x20(%[min],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vmaxps              0x40(%[min],%[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vmaxps              0x60(%[min],%[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x20(%[max],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              0x40(%[max],%[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vminps              0x60(%[max],%[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm2, 0x40(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm3, 0x60(%[dst],%[off])")
                __ASM_EMIT64("sub               $32, %[count]")
                __ASM_EMIT32("subl              $32, %[count]")
                __ASM_EMIT("add                 $0x80, %[off]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT64("add               $16, %[count]")
                __ASM_EMIT32("addl              $16, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[src],%[off]), %%ymm1")
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x20(%[min],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x20(%[max],%[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT64("sub               $16, %[count]")
                __ASM_EMIT32("subl              $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT64("add               $8, %[count]")
                __ASM_EMIT32("addl              $8, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT64("sub               $8, %[count]")
                __ASM_EMIT32("subl              $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT64("add               $4, %[count]")
                __ASM_EMIT32("addl              $4, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%xmm0, %%xmm0")       /* xmm0 = max(x, min) */
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%xmm0, %%xmm0")       /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT64("sub               $4, %[count]")
                __ASM_EMIT32("subl              $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT64("add               $3, %[count]")
                __ASM_EMIT32("addl              $3, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss              0x00(%[src],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxss              0x00(%[min],%[off]), %%xmm0, %%xmm0")       /* xmm0 = max(x, min) */
                __ASM_EMIT("vminss              0x00(%[max],%[off]), %%xmm0, %%xmm0")       /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT("jge                 11b")
                // End
                __ASM_EMIT("12:")
                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r" (dst), [src] "r" (src), [min] "r" (min), [max] "r" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void clamp_kk1(float *dst, float min, float max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                __ASM_EMIT("vbroadcastss        %[min], %%ymm6")                            /* ymm6 = min */
                __ASM_EMIT("vbroadcastss        %[max], %%ymm7")                            /* ymm7 = max */
                // 32x blocks
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[dst],%[off]), %%ymm1")
                __ASM_EMIT("vmovups             0x40(%[dst],%[off]), %%ymm2")
                __ASM_EMIT("vmovups             0x60(%[dst],%[off]), %%ymm3")
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm1, %%ymm1")
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm3, %%ymm3")
                __ASM_EMIT("vminps              %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              %%ymm7, %%ymm2, %%ymm2")
                __ASM_EMIT("vminps              %%ymm7, %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm2, 0x40(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm3, 0x60(%[dst],%[off])")
                __ASM_EMIT("add                 $0x80, %[off]")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jae                 1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[dst],%[off]), %%ymm1")
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = max(x, min) */
                __ASM_EMIT("vminps              %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxps              %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("vminps              %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss              0x00(%[dst],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxss              %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("vminss              %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 9b")
                // End
                __ASM_EMIT("10:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [min] "m" (min),
                  [max] "m" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

        void clamp_kk2(float *dst, const float *src, float min, float max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                __ASM_EMIT("vbroadcastss        %[min], %%ymm6")                            /* ymm6 = min */
                __ASM_EMIT("vbroadcastss        %[max], %%ymm7")                            /* ymm7 = max */
                // 32x blocks
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[src],%[off]), %%ymm1")
                __ASM_EMIT("vmovups             0x40(%[src],%[off]), %%ymm2")
                __ASM_EMIT("vmovups             0x60(%[src],%[off]), %%ymm3")
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm1, %%ymm1")
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm3, %%ymm3")
                __ASM_EMIT("vminps              %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              %%ymm7, %%ymm2, %%ymm2")
                __ASM_EMIT("vminps              %%ymm7, %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm2, 0x40(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm3, 0x60(%[dst],%[off])")
                __ASM_EMIT("add                 $0x80, %[off]")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jae                 1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmovups             0x20(%[src],%[off]), %%ymm1")
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = max(x, min) */
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm1, %%ymm1")
                __ASM_EMIT("vminps              %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = max(x, min) */
                __ASM_EMIT("vminps              %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxps              %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("vminps              %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss              0x00(%[src],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxss              %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("vminss              %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 9b")
                // End
                __ASM_EMIT("10:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [min] "m" (min),
                  [max] "m" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }
    } /* namespace avx */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX_PMATH_CLAMP_H_ */
