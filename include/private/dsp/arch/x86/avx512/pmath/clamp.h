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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_CLAMP_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_CLAMP_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        void clamp_vv1(float *dst, const float *min, const float *max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                // 128x blocks
                __ASM_EMIT("sub                 $128, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x000(%[dst],%[off]), %%zmm0")               /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[dst],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x080(%[dst],%[off]), %%zmm2")
                __ASM_EMIT("vmovups             0x0c0(%[dst],%[off]), %%zmm3")
                __ASM_EMIT("vmovups             0x100(%[dst],%[off]), %%zmm4")
                __ASM_EMIT("vmovups             0x140(%[dst],%[off]), %%zmm5")
                __ASM_EMIT("vmovups             0x180(%[dst],%[off]), %%zmm6")
                __ASM_EMIT("vmovups             0x1c0(%[dst],%[off]), %%zmm7")
                __ASM_EMIT("vmaxps              0x000(%[min],%[off]), %%zmm0, %%zmm0")       /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x040(%[min],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vmaxps              0x080(%[min],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps              0x0c0(%[min],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vmaxps              0x100(%[min],%[off]), %%zmm4, %%zmm4")
                __ASM_EMIT("vmaxps              0x140(%[min],%[off]), %%zmm5, %%zmm5")
                __ASM_EMIT("vmaxps              0x180(%[min],%[off]), %%zmm6, %%zmm6")
                __ASM_EMIT("vmaxps              0x1c0(%[min],%[off]), %%zmm7, %%zmm7")
                __ASM_EMIT("vminps              0x000(%[max],%[off]), %%zmm0, %%zmm0")       /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x040(%[max],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              0x080(%[max],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vminps              0x0c0(%[max],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vminps              0x100(%[max],%[off]), %%zmm4, %%zmm4")
                __ASM_EMIT("vminps              0x140(%[max],%[off]), %%zmm5, %%zmm5")
                __ASM_EMIT("vminps              0x180(%[max],%[off]), %%zmm6, %%zmm6")
                __ASM_EMIT("vminps              0x1c0(%[max],%[off]), %%zmm7, %%zmm7")
                __ASM_EMIT("vmovups             %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm2, 0x080(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm3, 0x0c0(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm4, 0x100(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm5, 0x140(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm6, 0x180(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm7, 0x1c0(%[dst],%[off])")
                __ASM_EMIT("add                 $0x200, %[off]")
                __ASM_EMIT("sub                 $128, %[count]")
                __ASM_EMIT("jae                 1b")
                // 64x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add                 $64, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x000(%[dst],%[off]), %%zmm0")               /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[dst],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x080(%[dst],%[off]), %%zmm2")
                __ASM_EMIT("vmovups             0x0c0(%[dst],%[off]), %%zmm3")
                __ASM_EMIT("vmaxps              0x000(%[min],%[off]), %%zmm0, %%zmm0")       /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x040(%[min],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vmaxps              0x080(%[min],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps              0x0c0(%[min],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vminps              0x000(%[max],%[off]), %%zmm0, %%zmm0")       /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x040(%[max],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              0x080(%[max],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vminps              0x0c0(%[max],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vmovups             %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm2, 0x080(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm3, 0x0c0(%[dst],%[off])")
                __ASM_EMIT("sub                 $64, %[count]")
                __ASM_EMIT("add                 $0x100, %[off]")
                // 32x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $32, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x000(%[dst],%[off]), %%zmm0")               /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[dst],%[off]), %%zmm1")
                __ASM_EMIT("vmaxps              0x000(%[min],%[off]), %%zmm0, %%zmm0")       /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x040(%[min],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              0x000(%[max],%[off]), %%zmm0, %%zmm0")       /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x040(%[max],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vmovups             %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("add                 $0x80, %[off]")
                // 16x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x000(%[dst],%[off]), %%zmm0")               /* zmm0 = x */
                __ASM_EMIT("vmaxps              0x000(%[min],%[off]), %%zmm0, %%zmm0")       /* zmm0 = max(x, min) */
                __ASM_EMIT("vminps              0x000(%[max],%[off]), %%zmm0, %%zmm0")       /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("10:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%xmm0, %%xmm0")       /* xmm0 = max(x, min) */
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%xmm0, %%xmm0")       /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("12:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  14f")
                __ASM_EMIT("13:")
                __ASM_EMIT("vmovss              0x00(%[dst],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxss              0x00(%[min],%[off]), %%xmm0, %%xmm0")       /* xmm0 = max(x, min) */
                __ASM_EMIT("vminss              0x00(%[max],%[off]), %%xmm0, %%xmm0")       /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 13b")
                // End
                __ASM_EMIT("14:")
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
                // 128x blocks
                __ASM_EMIT64("sub               $128, %[count]")
                __ASM_EMIT32("subl              $128, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x000(%[src],%[off]), %%zmm0")               /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[src],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x080(%[src],%[off]), %%zmm2")
                __ASM_EMIT("vmovups             0x0c0(%[src],%[off]), %%zmm3")
                __ASM_EMIT("vmovups             0x100(%[src],%[off]), %%zmm4")
                __ASM_EMIT("vmovups             0x140(%[src],%[off]), %%zmm5")
                __ASM_EMIT("vmovups             0x180(%[src],%[off]), %%zmm6")
                __ASM_EMIT("vmovups             0x1c0(%[src],%[off]), %%zmm7")
                __ASM_EMIT("vmaxps              0x000(%[min],%[off]), %%zmm0, %%zmm0")       /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x040(%[min],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vmaxps              0x080(%[min],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps              0x0c0(%[min],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vmaxps              0x100(%[min],%[off]), %%zmm4, %%zmm4")
                __ASM_EMIT("vmaxps              0x140(%[min],%[off]), %%zmm5, %%zmm5")
                __ASM_EMIT("vmaxps              0x180(%[min],%[off]), %%zmm6, %%zmm6")
                __ASM_EMIT("vmaxps              0x1c0(%[min],%[off]), %%zmm7, %%zmm7")
                __ASM_EMIT("vminps              0x000(%[max],%[off]), %%zmm0, %%zmm0")       /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x040(%[max],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              0x080(%[max],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vminps              0x0c0(%[max],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vminps              0x100(%[max],%[off]), %%zmm4, %%zmm4")
                __ASM_EMIT("vminps              0x140(%[max],%[off]), %%zmm5, %%zmm5")
                __ASM_EMIT("vminps              0x180(%[max],%[off]), %%zmm6, %%zmm6")
                __ASM_EMIT("vminps              0x1c0(%[max],%[off]), %%zmm7, %%zmm7")
                __ASM_EMIT("vmovups             %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm2, 0x080(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm3, 0x0c0(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm4, 0x100(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm5, 0x140(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm6, 0x180(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm7, 0x1c0(%[dst],%[off])")
                __ASM_EMIT64("add               $0x200, %[off]")
                __ASM_EMIT32("addl              $0x200, %[off]")
                __ASM_EMIT("sub                 $128, %[count]")
                __ASM_EMIT("jae                 1b")
                // 64x block
                __ASM_EMIT("2:")
                __ASM_EMIT64("add               $64, %[count]")
                __ASM_EMIT32("addl              $64, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x000(%[src],%[off]), %%zmm0")               /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[src],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x080(%[src],%[off]), %%zmm2")
                __ASM_EMIT("vmovups             0x0c0(%[src],%[off]), %%zmm3")
                __ASM_EMIT("vmaxps              0x000(%[min],%[off]), %%zmm0, %%zmm0")       /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x040(%[min],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vmaxps              0x080(%[min],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps              0x0c0(%[min],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vminps              0x000(%[max],%[off]), %%zmm0, %%zmm0")       /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x040(%[max],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              0x080(%[max],%[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vminps              0x0c0(%[max],%[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vmovups             %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm2, 0x080(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm3, 0x0c0(%[dst],%[off])")
                __ASM_EMIT("add                 $0x100, %[off]")
                __ASM_EMIT64("sub               $64, %[count]")
                __ASM_EMIT32("subl              $64, %[count]")
                __ASM_EMIT("jae                 1b")
                // 32x block
                __ASM_EMIT("4:")
                __ASM_EMIT64("add               $32, %[count]")
                __ASM_EMIT32("addl              $32, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x000(%[src],%[off]), %%zmm0")               /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[src],%[off]), %%zmm1")
                __ASM_EMIT("vmaxps              0x000(%[min],%[off]), %%zmm0, %%zmm0")       /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              0x040(%[min],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              0x000(%[max],%[off]), %%zmm0, %%zmm0")       /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              0x040(%[max],%[off]), %%zmm1, %%zmm1")
                __ASM_EMIT("vmovups             %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x040(%[dst],%[off])")
                __ASM_EMIT64("sub               $32, %[count]")
                __ASM_EMIT32("subl              $32, %[count]")
                __ASM_EMIT("add                 $0x80, %[off]")
                // 16x block
                __ASM_EMIT("6:")
                __ASM_EMIT64("add               $16, %[count]")
                __ASM_EMIT32("addl              $16, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x000(%[src],%[off]), %%zmm0")               /* zmm0 = x */
                __ASM_EMIT("vmaxps              0x000(%[min],%[off]), %%zmm0, %%zmm0")       /* zmm0 = max(x, min) */
                __ASM_EMIT("vminps              0x000(%[max],%[off]), %%zmm0, %%zmm0")       /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%zmm0, 0x000(%[dst],%[off])")
                __ASM_EMIT64("sub               $16, %[count]")
                __ASM_EMIT32("subl              $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("8:")
                __ASM_EMIT64("add               $8, %[count]")
                __ASM_EMIT32("addl              $8, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%ymm0, %%ymm0")       /* ymm0 = max(x, min) */
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%ymm0, %%ymm0")       /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT64("sub               $8, %[count]")
                __ASM_EMIT32("subl              $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("10:")
                __ASM_EMIT64("add               $4, %[count]")
                __ASM_EMIT32("addl              $4, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxps              0x00(%[min],%[off]), %%xmm0, %%xmm0")       /* xmm0 = max(x, min) */
                __ASM_EMIT("vminps              0x00(%[max],%[off]), %%xmm0, %%xmm0")       /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT64("sub               $4, %[count]")
                __ASM_EMIT32("subl              $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("12:")
                __ASM_EMIT64("add               $3, %[count]")
                __ASM_EMIT32("addl              $3, %[count]")
                __ASM_EMIT("jl                  14f")
                __ASM_EMIT("13:")
                __ASM_EMIT("vmovss              0x00(%[src],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxss              0x00(%[min],%[off]), %%xmm0, %%xmm0")       /* xmm0 = max(x, min) */
                __ASM_EMIT("vminss              0x00(%[max],%[off]), %%xmm0, %%xmm0")       /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT("jge                 13b")
                // End
                __ASM_EMIT("14:")
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
                __ASM_EMIT("vbroadcastss        %[min], %%zmm6")                            /* zmm6 = min */
                __ASM_EMIT("vbroadcastss        %[max], %%zmm7")                            /* zmm7 = max */
                // 64x blocks
                __ASM_EMIT("sub                 $64, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x000(%[dst],%[off]), %%zmm0")              /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[dst],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x080(%[dst],%[off]), %%zmm2")
                __ASM_EMIT("vmovups             0x0c0(%[dst],%[off]), %%zmm3")
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm1, %%zmm1")
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm3, %%zmm3")
                __ASM_EMIT("vminps              %%zmm7, %%zmm0, %%zmm0")                    /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              %%zmm7, %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              %%zmm7, %%zmm2, %%zmm2")
                __ASM_EMIT("vminps              %%zmm7, %%zmm3, %%zmm3")
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
                __ASM_EMIT("vmovups             0x000(%[dst],%[off]), %%zmm0")              /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[dst],%[off]), %%zmm1")
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              %%zmm7, %%zmm0, %%zmm0")                    /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              %%zmm7, %%zmm1, %%zmm1")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("add                 $0x80, %[off]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x000(%[dst],%[off]), %%zmm0")              /* zmm0 = x */
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = max(x, min) */
                __ASM_EMIT("vminps              %%zmm7, %%zmm0, %%zmm0")                    /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = max(x, min) */
                __ASM_EMIT("vminps              %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x00(%[dst],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxps              %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("vminps              %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss              0x00(%[dst],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxss              %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("vminss              %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 11b")
                // End
                __ASM_EMIT("12:")
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
                __ASM_EMIT("vbroadcastss        %[min], %%zmm6")                            /* zmm6 = min */
                __ASM_EMIT("vbroadcastss        %[max], %%zmm7")                            /* zmm7 = max */
                // 64x blocks
                __ASM_EMIT("sub                 $64, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x000(%[src],%[off]), %%zmm0")              /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[src],%[off]), %%zmm1")
                __ASM_EMIT("vmovups             0x080(%[src],%[off]), %%zmm2")
                __ASM_EMIT("vmovups             0x0c0(%[src],%[off]), %%zmm3")
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm1, %%zmm1")
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm3, %%zmm3")
                __ASM_EMIT("vminps              %%zmm7, %%zmm0, %%zmm0")                    /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              %%zmm7, %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              %%zmm7, %%zmm2, %%zmm2")
                __ASM_EMIT("vminps              %%zmm7, %%zmm3, %%zmm3")
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
                __ASM_EMIT("vmovups             0x000(%[src],%[off]), %%zmm0")              /* zmm0 = x */
                __ASM_EMIT("vmovups             0x040(%[src],%[off]), %%zmm1")
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = max(x, min) */
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm1, %%zmm1")
                __ASM_EMIT("vminps              %%zmm7, %%zmm0, %%zmm0")                    /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vminps              %%zmm7, %%zmm1, %%zmm1")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups             %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("add                 $0x80, %[off]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x000(%[src],%[off]), %%zmm0")              /* zmm0 = x */
                __ASM_EMIT("vmaxps              %%zmm6, %%zmm0, %%zmm0")                    /* zmm0 = max(x, min) */
                __ASM_EMIT("vminps              %%zmm7, %%zmm0, %%zmm0")                    /* zmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%ymm0")               /* ymm0 = x */
                __ASM_EMIT("vmaxps              %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = max(x, min) */
                __ASM_EMIT("vminps              %%ymm7, %%ymm0, %%ymm0")                    /* ymm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x00(%[src],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxps              %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("vminps              %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss              0x00(%[src],%[off]), %%xmm0")               /* xmm0 = x */
                __ASM_EMIT("vmaxss              %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("vminss              %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jge                 11b")
                // End
                __ASM_EMIT("12:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [min] "m" (min),
                  [max] "m" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_CLAMP_H_ */
