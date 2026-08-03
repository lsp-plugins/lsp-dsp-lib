/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_STATIC_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_STATIC_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

#include <private/dsp/arch/x86/avx512/filters/common.h>

namespace lsp
{
    namespace avx512
    {

        void biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f)
        {
            IF_ARCH_X86(size_t mask);
            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT32("cmpl              $0, %[count]")
                __ASM_EMIT64("test              %[count], %[count]")
                __ASM_EMIT("jz                  8f")

                // Initialize mask
                __ASM_EMIT("mov                 $1, %[mask]")
                __ASM_EMIT("vxorps              %%zmm1, %%zmm1, %%zmm1")                            // zmm1     = 0
                __ASM_EMIT("kmovw               %k[mask], %%k1")
                __ASM_EMIT("vmovaps             %[X_MASK], %%zmm0")                                 // zmm0     = shuffle mask
                __ASM_EMIT("kmovw               %k[mask], %%k2")

                // Load delay buffer
                __ASM_EMIT("vmovaps             0x00(%[d]), %%zmm6")                                // zmm6     = d0
                __ASM_EMIT("vmovaps             0x40(%[d]), %%zmm7")                                // zmm7     = d1

                // Process first steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vbroadcastss        (%[src]), %%zmm1 %{%%k1%}")                         // zmm1     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                FILTER_X16
                __ASM_EMIT("vaddps              %%zmm7, %%zmm2, %%zmm6 %{%%k2%}")                   // zmm2     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vpermps             %%zmm1, %%zmm0, %%zmm1")                            // zmm1     = s2[15] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6] s2[7] s2[8] s2[9] s2[10] s2[11] s2[12] s2[13] s2[14]
                __ASM_EMIT("vmovaps             %%zmm3, %%zmm7 %{%%k2%}")                           // zmm7     = (p2 & MASK) | (d1 & ~MASK)

                // Repeat loop
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jz                  4f")                                                // jump to completion
                __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
                __ASM_EMIT("kmovw               %k[mask], %%k2")
                __ASM_EMIT("cmp                 $0xffff, %[mask]")
                __ASM_EMIT("jne                 1b")

                // 8x filter processing without mask
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("3:")
                __ASM_EMIT("vbroadcastss        (%[src]), %%zmm1 %{%%k1%}")                         // zmm1     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                FILTER_X16
                __ASM_EMIT("vaddps              %%zmm7, %%zmm2, %%zmm6")                            // zmm6     = p1 + d1
                __ASM_EMIT("vpermps             %%zmm1, %%zmm0, %%zmm1")                            // zmm1     = s2[15] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6] s2[7] s2[8] s2[9] s2[10] s2[11] s2[12] s2[13] s2[14]
                __ASM_EMIT("vmovaps             %%zmm3, %%zmm7")                                    // zmm7     = s*b2 + s2*a2 = p2
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[15]

                // Repeat loop
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jnz                 3b")

                // Prepare last loop, shift mask
                __ASM_EMIT("4:")
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("kmovw               %k[mask], %%k2")

                // Process steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("5:")
                FILTER_X16
                __ASM_EMIT("vaddps              %%zmm7, %%zmm2, %%zmm6 %{%%k2%}")                   // zmm2     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vpermps             %%zmm1, %%zmm0, %%zmm1")                            // zmm1     = s2[15] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6] s2[7] s2[8] s2[9] s2[10] s2[11] s2[12] s2[13] s2[14]
                __ASM_EMIT("vmovaps             %%zmm3, %%zmm7 %{%%k2%}")                           // zmm7     = (p2 & MASK) | (d1 & ~MASK)
                __ASM_EMIT("test                $0x8000, %[mask]")
                __ASM_EMIT("jz                  6f")
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("6:")

                // Repeat loop
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("and                 $0xffff, %[mask]")                                  // mask     = (mask << 1) & 0xff
                __ASM_EMIT("kmovw               %k[mask], %%k2")
                __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("vmovaps             %%zmm6, 0x00(%[d])")                                // *d0      = zmm6
                __ASM_EMIT("vmovaps             %%zmm7, 0x40(%[d])")                                // *d1      = zmm7

                // Exit label
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [mask] "=&r"(mask), [count] X86_PGREG (count)
                : [f] "r" (f), [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k1", "%k2"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_STATIC_H_ */
