/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_FILTERS_DYNAMIC_H_
#define PRIVATE_DSP_ARCH_X86_AVX_FILTERS_DYNAMIC_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

#include <private/dsp/arch/x86/avx/filters/common.h>

namespace lsp
{
    namespace avx
    {

        // This function is tested, works and delivers high performance
        void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f)
        {
            IF_ARCH_X86_64(size_t mask);
            ARCH_X86_64_ASM
            (
                // Check count
                __ASM_EMIT("test                %[count], %[count]")
                __ASM_EMIT("jz                  8f")

                // Initialize mask
                // ymm0=tmp, ymm1={s,s2[8]}, ymm2=p1[8], ymm3=p2[8], ymm6=d0[8], ymm7=d1[8], ymm8=mask[8]
                __ASM_EMIT("mov                 $1, %[mask]")
                __ASM_EMIT("vmovaps             %[X_MASK], %%ymm8")                                 // ymm8     = m
                __ASM_EMIT("vxorps              %%ymm1, %%ymm1, %%ymm1")                            // ymm1     = 0

                // Load delay buffer
                __ASM_EMIT("vmovaps             0x00(%[d]), %%ymm6")                                // ymm6     = d0
                __ASM_EMIT("vmovaps             0x20(%[d]), %%ymm7")                                // ymm7     = d1

                // Process first 3 steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                __ASM_EMIT("vmulps              0x20(%[f]), %%ymm1, %%ymm2")                        // ymm2     = s*b1
                __ASM_EMIT("vmulps              0x40(%[f]), %%ymm1, %%ymm3")                        // ymm3     = s*b2
                __ASM_EMIT("vmulps              0x00(%[f]), %%ymm1, %%ymm1")                        // ymm1     = s*b0
                __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                            // ymm1     = s*b0+d0 = s2
                __ASM_EMIT("vmulps              0x60(%[f]), %%ymm1, %%ymm4")                        // ymm4     = s2*a1
                __ASM_EMIT("vmulps              0x80(%[f]), %%ymm1, %%ymm5")                        // ymm5     = s2*a2
                __ASM_EMIT("vaddps              %%ymm4, %%ymm2, %%ymm2")                            // ymm2     = s*b1 + s2*a1 = p1
                __ASM_EMIT("vaddps              %%ymm5, %%ymm3, %%ymm3")                            // ymm3     = s*b2 + s2*a2 = p2
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vblendvps           %%ymm8, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vblendvps           %%ymm8, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]

                // Repeat loop
                __ASM_EMIT("add                 $0xa0, %[f]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jz                  4f")                                                // jump to completion
                __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
                __ASM_EMIT("vpermilps           $0x93, %%ymm8, %%ymm8")                             // ymm8     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x02, %[X_MASK], %%ymm8, %%ymm3")                  // ymm3     =  0xff  0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm3, %%ymm8, %%ymm8")                     // ymm8     =  0xff  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("cmp                 $0xff, %[mask]")
                __ASM_EMIT("jne                 1b")

                // 8x filter processing without mask
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("3:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                __ASM_EMIT("vmulps              0x20(%[f]), %%ymm1, %%ymm2")                        // ymm2     = s*b1
                __ASM_EMIT("vmulps              0x40(%[f]), %%ymm1, %%ymm3")                        // ymm3     = s*b2
                __ASM_EMIT("vmulps              0x00(%[f]), %%ymm1, %%ymm1")                        // ymm1     = s*b0
                __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                            // ymm1     = s*b0+d0 = s2
                __ASM_EMIT("vmulps              0x60(%[f]), %%ymm1, %%ymm4")                        // ymm4     = s2*a1
                __ASM_EMIT("vmulps              0x80(%[f]), %%ymm1, %%ymm5")                        // ymm5     = s2*a2
                __ASM_EMIT("vaddps              %%ymm4, %%ymm2, %%ymm2")                            // ymm2     = s*b1 + s2*a1 = p1
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm6")                            // ymm6     = p1 + d1
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vaddps              %%ymm5, %%ymm3, %%ymm7")                            // ymm7     = s*b2 + s2*a2 = p2
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("add                 $0xa0, %[f]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jnz                 3b")

                // Prepare last loop, shift mask
                __ASM_EMIT("4:")
                __ASM_EMIT("vpermilps           $0x93, %%ymm8, %%ymm8")                             // ymm8     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm8, %%ymm8, %%ymm2")                     // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm2, %%ymm8, %%ymm8")                     // ymm8     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1

                // Process steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("5:")
                __ASM_EMIT("vmulps              0x20(%[f]), %%ymm1, %%ymm2")                        // ymm2     = s*b1
                __ASM_EMIT("vmulps              0x40(%[f]), %%ymm1, %%ymm3")                        // ymm3     = s*b2
                __ASM_EMIT("vmulps              0x00(%[f]), %%ymm1, %%ymm1")                        // ymm1     = s*b0
                __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                            // ymm1     = s*b0+d0 = s2
                __ASM_EMIT("vmulps              0x60(%[f]), %%ymm1, %%ymm4")                        // ymm4     = s2*a1
                __ASM_EMIT("vmulps              0x80(%[f]), %%ymm1, %%ymm5")                        // ymm5     = s2*a2
                __ASM_EMIT("vaddps              %%ymm4, %%ymm2, %%ymm2")                            // ymm2     = s*b1 + s2*a1 = p1
                __ASM_EMIT("vaddps              %%ymm5, %%ymm3, %%ymm3")                            // ymm3     = s*b2 + s2*a2 = p2
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1

                __ASM_EMIT("vblendvps           %%ymm8, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm8, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm8, %%ymm8")                             // ymm8     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm8, %%ymm8, %%ymm2")                     // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vblendps            $0x11, %%ymm2, %%ymm8, %%ymm8")                     // ymm1     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("test                $0x80, %[mask]")
                __ASM_EMIT("jz                  6f")
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("6:")

                // Repeat loop
                __ASM_EMIT("add                 $0xa0, %[f]")
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("and                 $0xff, %[mask]")                                    // mask     = (mask << 1) & 0xff
                __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("vmovaps             %%ymm6, 0x00(%[d])")                                // *d0      = ymm6
                __ASM_EMIT("vmovaps             %%ymm7, 0x20(%[d])")                                // *d1      = ymm7

                // Exit label
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [f] "+r" (f),
                  [mask] "=&r"(mask), [count] "+r" (count)
                : [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8"
            );
        }

        // This function is FMA3 implementation of biquad_process_x8
        void dyn_biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f)
        {
            IF_ARCH_X86(size_t mask);
            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT32("cmpl              $0, %[count]")
                __ASM_EMIT64("test              %[count], %[count]")
                __ASM_EMIT("jz                  8f")

                // Initialize mask
                // ymm0=tmp, ymm1={s,s2[8]}, ymm2=p1[8], ymm3=p2[8], ymm6=d0[8], ymm7=d1[8], ymm5=mask[8]
                __ASM_EMIT("mov                 $1, %[mask]")
                __ASM_EMIT("vmovaps             %[X_MASK], %%ymm5")                                 // ymm5     = m
                __ASM_EMIT("vxorps              %%ymm1, %%ymm1, %%ymm1")                            // ymm1     = 0

                // Load delay buffer
                __ASM_EMIT("vmovaps             0x00(%[d]), %%ymm6")                                // ymm6     = d0
                __ASM_EMIT("vmovaps             0x20(%[d]), %%ymm7")                                // ymm7     = d1

                // Process first 3 steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                FILTER_X8_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1

                // Update delay only by mask
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)

                // Rotate buffer, AVX has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]

                // Repeat loop
                __ASM_EMIT("add                 $0xa0, %[f]")
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jz                  4f")                                                // jump to completion
                __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x02, %[X_MASK], %%ymm5, %%ymm3")                  // ymm3     =  0xff  0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm3, %%ymm5, %%ymm5")                     // ymm5     =  0xff  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("cmp                 $0xff, %[mask]")
                __ASM_EMIT("jne                 1b")

                // 8x filter processing without mask
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("3:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                FILTER_X8_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm6")                            // ymm6     = p1 + d1

                // Rotate buffer, AVX2 has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vmovaps             %%ymm3, %%ymm7")                                    // ymm7     = s*b2 + s2*a2 = p2
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]

                // Repeat loop
                __ASM_EMIT("add                 $0xa0, %[f]")
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jnz                 3b")

                // Prepare last loop, shift mask
                __ASM_EMIT("4:")
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm5, %%ymm5, %%ymm2")                     // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm2, %%ymm5, %%ymm5")                     // ymm5     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1

                // Process steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("5:")
                FILTER_X8_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1

                // Update delay only by mask
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)

                // Rotate buffer and mask, AVX2 has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm5, %%ymm5, %%ymm2")                     // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vblendps            $0x11, %%ymm2, %%ymm5, %%ymm5")                     // ymm1     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("test                $0x80, %[mask]")
                __ASM_EMIT("jz                  6f")
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("6:")

                // Repeat loop
                __ASM_EMIT("add                 $0xa0, %[f]")
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("and                 $0xff, %[mask]")                                    // mask     = (mask << 1) & 0xff
                __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("vmovaps             %%ymm6, 0x00(%[d])")                                // *d0      = ymm6
                __ASM_EMIT("vmovaps             %%ymm7, 0x20(%[d])")                                // *d1      = ymm7

                // Exit label
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [f] "+r" (f),
                  [mask] "=&r"(mask), [count] X86_PGREG (count)
                : [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void dyn_biquad_process_x16_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f)
        {
            IF_ARCH_X86(
                size_t mask, X_COUNT;
                float *X_F, *X_DST;
            );
            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT32("cmpl              $0, %[count]")
                __ASM_EMIT64("test              %[count], %[count]")
                __ASM_EMIT("jz                  8f")

                //---------------------------------------------------------------------
                // Cycle 1
                __ASM_EMIT("mov                 %[f], %[X_F]")
                __ASM_EMIT("mov                 %[dst], %[X_DST]")
                __ASM_EMIT32("mov               %[count], %[mask]")
                __ASM_EMIT32("mov               %[mask], %[X_COUNT]")
                __ASM_EMIT64("mov               %[count], %[X_COUNT]")
                // Initialize mask
                // ymm0=tmp, ymm1={s,s2[8]}, ymm2=p1[8], ymm3=p2[8], ymm6=d0[8], ymm7=d1[8], ymm5=mask[8]
                __ASM_EMIT("mov                 $1, %[mask]")
                __ASM_EMIT("vmovaps             %[X_MASK], %%ymm5")                                 // ymm5     = m
                __ASM_EMIT("vxorps              %%ymm1, %%ymm1, %%ymm1")                            // ymm1     = 0

                // Load delay buffer
                __ASM_EMIT("vmovaps             0x00(%[d]), %%ymm6")                                // ymm6     = d0
                __ASM_EMIT("vmovaps             0x40(%[d]), %%ymm7")                                // ymm7     = d1

                // Process first 3 steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                FILTER_X16P1_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Update delay only by mask
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)

                // Rotate buffer, AVX has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]

                // Repeat loop
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jz                  4f")                                                // jump to completion
                __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x01, %%ymm5, %%ymm5, %%ymm3")                     // ymm3     =  m[7]  m[4]  m[5]  m[6]  m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm3, %%ymm5, %%ymm5")                     // ymm5     =  m[7]  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("vorps               %[X_MASK], %%ymm5, %%ymm5")                         // ymm5     =  m[0]  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("cmp                 $0xff, %[mask]")
                __ASM_EMIT("jne                 1b")

                // 8x filter processing without mask
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("3:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                FILTER_X16P1_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm6")                            // ymm6     = p1 + d1
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Rotate buffer, AVX2 has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vmovaps             %%ymm3, %%ymm7")                                    // ymm7     = s*b2 + s2*a2 = p2
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]

                // Repeat loop
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jnz                 3b")

                // Prepare last loop, shift mask
                __ASM_EMIT("4:")
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm5, %%ymm5, %%ymm2")                     // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm2, %%ymm5, %%ymm5")                     // ymm5     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1

                // Process steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("5:")
                FILTER_X16P1_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Update delay only by mask
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)

                // Rotate buffer and mask, AVX2 has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vxorps              %%ymm2, %%ymm2, %%ymm2")                            // ymm2     =  0
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vinsertf128         $0x01, %%xmm5, %%ymm2, %%ymm2")                     // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm2, %%ymm5, %%ymm5")                     // ymm1     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("test                $0x80, %[mask]")
                __ASM_EMIT("jz                  6f")
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("6:")

                // Repeat loop
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("and                 $0xff, %[mask]")                                    // mask     = (mask << 1) & 0xff
                __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("vmovaps             %%ymm6, 0x00(%[d])")                                // *d0      = ymm6
                __ASM_EMIT("vmovaps             %%ymm7, 0x40(%[d])")                                // *d1      = ymm7

                //---------------------------------------------------------------------
                // Cycle 2
                __ASM_EMIT("mov                 %[X_F], %[f]")
                __ASM_EMIT("mov                 %[X_DST], %[dst]")
                __ASM_EMIT32("mov               %[X_COUNT], %[mask]")
                __ASM_EMIT32("mov               %[mask], %[count]")
                __ASM_EMIT64("mov               %[X_COUNT], %[count]")
                __ASM_EMIT("mov                 %[dst], %[src]")
                // Initialize mask
                // ymm0=tmp, ymm1={s,s2[8]}, ymm2=p1[8], ymm3=p2[8], ymm6=d0[8], ymm7=d1[8], ymm5=mask[8]
                __ASM_EMIT("add                 $0xa00, %[f]")                                      // f       += 8 * sizeof(*f)
                __ASM_EMIT("mov                 $1, %[mask]")
                __ASM_EMIT("vmovaps             %[X_MASK], %%ymm5")                                 // ymm5     = m
                __ASM_EMIT("vxorps              %%ymm1, %%ymm1, %%ymm1")                            // ymm1     = 0

                // Load delay buffer
                __ASM_EMIT("vmovaps             0x20(%[d]), %%ymm6")                                // ymm6     = d0
                __ASM_EMIT("vmovaps             0x60(%[d]), %%ymm7")                                // ymm7     = d1

                // Process first 3 steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                FILTER_X16P2_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Update delay only by mask
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)

                // Rotate buffer, AVX has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]

                // Repeat loop
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jz                  4f")                                                // jump to completion
                __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x02, %[X_MASK], %%ymm5, %%ymm3")                  // ymm3     =  m[7]  m[4]  m[5]  m[6]  m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm3, %%ymm5, %%ymm5")                     // ymm5     =  m[7]  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("cmp                 $0xff, %[mask]")
                __ASM_EMIT("jne                 1b")

                // 8x filter processing without mask
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("3:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                FILTER_X16P2_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm6")                            // ymm6     = p1 + d1
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Rotate buffer, AVX2 has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vmovaps             %%ymm3, %%ymm7")                                    // ymm7     = s*b2 + s2*a2 = p2
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]

                // Repeat loop
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jnz                 3b")

                // Prepare last loop, shift mask
                __ASM_EMIT("4:")
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm5, %%ymm5, %%ymm2")                     // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm2, %%ymm5, %%ymm5")                     // ymm5     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1

                // Process steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("5:")
                FILTER_X16P2_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Update delay only by mask
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)

                // Rotate buffer and mask, AVX2 has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vperm2f128          $0x01, %%ymm1, %%ymm1, %%ymm0")                     // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm5, %%ymm5, %%ymm2")                     // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vblendps            $0x11, %%ymm2, %%ymm5, %%ymm5")                     // ymm1     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("test                $0x80, %[mask]")
                __ASM_EMIT("jz                  6f")
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("6:")

                // Repeat loop
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("and                 $0xff, %[mask]")                                    // mask     = (mask << 1) & 0xff
                __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("vmovaps             %%ymm6, 0x20(%[d])")                                // *d0      = ymm6
                __ASM_EMIT("vmovaps             %%ymm7, 0x60(%[d])")                                // *d1      = ymm7
                __ASM_EMIT("mov                 %[X_F], %[f]")

                // Exit label
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [mask] "=&r"(mask), [count] X86_PGREG (count)
                : [f] "r" (f), [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const),
                  [X_COUNT] "m" (X_COUNT),
                  [X_DST] "m" (X_DST),
                  [X_F] "m" (X_F)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_dyn_biquad_process_x16_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f)
        {
            IF_ARCH_X86_64(
                size_t mask;
            );
            ARCH_X86_64_ASM
            (
                // Check count
                __ASM_EMIT("test                %[count], %[count]")
                __ASM_EMIT("jz                  8f")

                //---------------------------------------------------------------------
                // Cycle 1
                // Initialize mask
                // ymm0=tmp, ymm1={s,s2[8]}, ymm2=p1[8], ymm3=p2[8], ymm6=d0[8], ymm7=d1[8], ymm5=mask[8]
                __ASM_EMIT("mov                 $1, %[mask]")
                __ASM_EMIT("vmovaps             %[X_MASK], %%ymm5")                                 // ymm5     = m[0..7]
                __ASM_EMIT("vxorps              %%ymm13, %%ymm13, %%ymm13")                         // ymm13    = m[8..15]
                __ASM_EMIT("vxorps              %%ymm1, %%ymm1, %%ymm1")                            // ymm1     = 0
                __ASM_EMIT("vxorps              %%ymm9, %%ymm9, %%ymm9")                            // ymm9     = 0

                // Load delay buffer
                __ASM_EMIT("vmovaps             0x00(%[d]), %%ymm6")                                // ymm6     = d0[0..7]
                __ASM_EMIT("vmovaps             0x20(%[d]), %%ymm14")                               // ymm14    = d0[8..15]
                __ASM_EMIT("vmovaps             0x40(%[d]), %%ymm7")                                // ymm7     = d1[0..7]
                __ASM_EMIT("vmovaps             0x60(%[d]), %%ymm15")                               // ymm15    = d1[8..15]

                // Process first steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                X64_FILTER_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1
                __ASM_EMIT("vaddps              %%ymm15, %%ymm10, %%ymm10")
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Update delay only by mask
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm13, %%ymm10, %%ymm14, %%ymm14")
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm13, %%ymm11, %%ymm15, %%ymm15")

                // Rotate buffer
                // ymm1  = s2[0]  s2[1]  s2[2]  s2[3]  s2[4]  s2[5]  s2[6]  s2[7]
                // ymm9  = s2[8]  s2[9]  s2[10] s2[11] s2[12] s2[13] s2[14] s2[15]
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3]  s2[0]  s2[1]  s2[2]  s2[7]  s2[4]  s2[5]  s2[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm9, %%ymm9")                             // ymm9     = s2[11] s2[8]  s2[9]  s2[10] s2[15] s2[12] s2[13] s2[14]
                __ASM_EMIT("vperm2f128          $0x21, %%ymm9, %%ymm1, %%ymm0")                     // ymm0     = s2[7]  s2[4]  s2[5]  s2[6]  s2[11] s2[8]  s2[9]  s2[10]
                __ASM_EMIT("vperm2f128          $0x03, %%ymm9, %%ymm1, %%ymm8")                     // ymm8     = s2[15] s2[12] s2[13] s2[14] s2[3]  s2[0]  s2[1]  s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm8, %%ymm1, %%ymm1")                     // ymm1     = s2[15] s2[0]  s2[1]  s2[2]  s2[3]  s2[4]  s2[5]  s2[6]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm9, %%ymm9")                     // ymm9     = s2[7]  s2[8]  s2[9]  s2[10] s2[11] s2[12] s2[13] s2[14]

                // Repeat loop
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jz                  4f")                                                // jump to completion
                __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     = m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm13, %%ymm13")                           // ymm13    = m[11] m[8]  m[9]  m[10] m[15] m[12] m[13] m[14]
                __ASM_EMIT("vperm2f128          $0x21, %%ymm13, %%ymm5, %%ymm3")                    // ymm3     = m[7]  m[4]  m[5]  m[6]  m[11] m[8]  m[9]  m[10]
                __ASM_EMIT("vperm2f128          $0x02, %[X_MASK], %%ymm5, %%ymm11")                 // ymm11    = 0xff  0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm3, %%ymm13, %%ymm13")                   // ymm13    = m[7]  m[8]  m[9]  m[10] m[11] m[12] m[13] m[14]
                __ASM_EMIT("vblendps            $0x11, %%ymm11, %%ymm5, %%ymm5")                    // ymm5     = 0xff  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("cmp                 $0xffff, %[mask]")
                __ASM_EMIT("jne                 1b")

                // 8x filter processing without mask
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("3:")
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vblendps            $0x01, %%ymm0, %%ymm1, %%ymm1")                     // ymm1     = s
                X64_FILTER_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm6")                            // ymm6     = p1 + d1
                __ASM_EMIT("vaddps              %%ymm15, %%ymm10, %%ymm14")
                __ASM_EMIT("vmovaps             %%ymm3, %%ymm7")                                    // ymm7     = s*b2 + s2*a2 = p2
                __ASM_EMIT("vmovaps             %%ymm11, %%ymm15")
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Rotate buffer, AVX has better option for it
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3]  s2[0]  s2[1]  s2[2]  s2[7]  s2[4]  s2[5]  s2[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm9, %%ymm9")                             // ymm9     = s2[11] s2[8]  s2[9]  s2[10] s2[15] s2[12] s2[13] s2[14]
                __ASM_EMIT("vperm2f128          $0x21, %%ymm9, %%ymm1, %%ymm0")                     // ymm0     = s2[7]  s2[4]  s2[5]  s2[6]  s2[11] s2[8]  s2[9]  s2[10]
                __ASM_EMIT("vperm2f128          $0x03, %%ymm9, %%ymm1, %%ymm8")                     // ymm8     = s2[15] s2[12] s2[13] s2[14] s2[3]  s2[0]  s2[1]  s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm8, %%ymm1, %%ymm1")                     // ymm1     = s2[15] s2[0]  s2[1]  s2[2]  s2[3]  s2[4]  s2[5]  s2[6]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm9, %%ymm9")                     // ymm9     = s2[7]  s2[8]  s2[9]  s2[10] s2[11] s2[12] s2[13] s2[14]
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[15]

                // Repeat loop
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jnz                 3b")

                // Prepare last loop, shift mask
                __ASM_EMIT("4:")
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     = m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm13, %%ymm13")                           // ymm13    = m[11] m[8]  m[9]  m[10] m[15] m[12] m[13] m[14]
                __ASM_EMIT("vperm2f128          $0x21, %%ymm13, %%ymm5, %%ymm3")                    // ymm3     = m[7]  m[4]  m[5]  m[6]  m[11] m[8]  m[9]  m[10]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm13, %%ymm5, %%ymm11")                   // ymm11    = 0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm11, %%ymm5, %%ymm5")                    // ymm5     = 0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("vblendps            $0x11, %%ymm3, %%ymm13, %%ymm13")                   // ymm13    = m[7]  m[8]  m[9]  m[10] m[11] m[12] m[13] m[14]
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1

                // Process steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("5:")
                X64_FILTER_FMA3
                __ASM_EMIT("vaddps              %%ymm7, %%ymm2, %%ymm2")                            // ymm2     = p1 + d1
                __ASM_EMIT("vaddps              %%ymm15, %%ymm10, %%ymm10")
                __ASM_EMIT("add                 $0x140, %[f]")                                      // ++f

                // Update delay only by mask
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm2, %%ymm6, %%ymm6")                    // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm13, %%ymm10, %%ymm14, %%ymm14")
                __ASM_EMIT("vblendvps           %%ymm5, %%ymm3, %%ymm7, %%ymm7")                    // ymm7     = (p2 & MASK) | (d1 & ~MASK)
                __ASM_EMIT("vblendvps           %%ymm13, %%ymm11, %%ymm15, %%ymm15")

                // Rotate buffer and mask
                __ASM_EMIT("vpermilps           $0x93, %%ymm1, %%ymm1")                             // ymm1     = s2[3]  s2[0]  s2[1]  s2[2]  s2[7]  s2[4]  s2[5]  s2[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm9, %%ymm9")                             // ymm9     = s2[11] s2[8]  s2[9]  s2[10] s2[15] s2[12] s2[13] s2[14]
                __ASM_EMIT("vperm2f128          $0x21, %%ymm9, %%ymm1, %%ymm0")                     // ymm0     = s2[7]  s2[4]  s2[5]  s2[6]  s2[11] s2[8]  s2[9]  s2[10]
                __ASM_EMIT("vperm2f128          $0x03, %%ymm9, %%ymm1, %%ymm8")                     // ymm8     = s2[15] s2[12] s2[13] s2[14] s2[3]  s2[0]  s2[1]  s2[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm8, %%ymm1, %%ymm1")                     // ymm1     = s2[15] s2[0]  s2[1]  s2[2]  s2[3]  s2[4]  s2[5]  s2[6]
                __ASM_EMIT("vblendps            $0x11, %%ymm0, %%ymm9, %%ymm9")                     // ymm9     = s2[7]  s2[8]  s2[9]  s2[10] s2[11] s2[12] s2[13] s2[14]
                __ASM_EMIT("vpermilps           $0x93, %%ymm5, %%ymm5")                             // ymm5     = m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
                __ASM_EMIT("vpermilps           $0x93, %%ymm13, %%ymm13")                           // ymm13    = m[11] m[8]  m[9]  m[10] m[15] m[12] m[13] m[14]
                __ASM_EMIT("vperm2f128          $0x21, %%ymm13, %%ymm5, %%ymm3")                    // ymm3     = m[7]  m[4]  m[5]  m[6]  m[11] m[8]  m[9]  m[10]
                __ASM_EMIT("vperm2f128          $0x08, %%ymm13, %%ymm5, %%ymm11")                   // ymm11    = 0     0     0     0     m[3]  m[0]  m[1]  m[2]
                __ASM_EMIT("vblendps            $0x11, %%ymm3, %%ymm13, %%ymm13")                   // ymm13    = m[7]  m[8]  m[9]  m[10] m[11] m[12] m[13] m[14]
                __ASM_EMIT("vblendps            $0x11, %%ymm11, %%ymm5, %%ymm5")                    // ymm5     = 0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
                __ASM_EMIT("test                $0x8000, %[mask]")
                __ASM_EMIT("jz                  6f")
                __ASM_EMIT("vmovss              %%xmm1, (%[dst])")                                  // *dst     = s2[7]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("6:")

                // Repeat loop
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("and                 $0xffff, %[mask]")                                  // mask     = (mask << 1) & 0xffff
                __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("vmovaps             %%ymm6, 0x00(%[d])")                                // *d0      = ymm6
                __ASM_EMIT("vmovaps             %%ymm14, 0x20(%[d])")
                __ASM_EMIT("vmovaps             %%ymm7, 0x40(%[d])")                                // *d1      = ymm7
                __ASM_EMIT("vmovaps             %%ymm15, 0x60(%[d])")

                // Exit label
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [f] "+r" (f),
                  [mask] "=&r"(mask), [count] X86_PGREG (count)
                : [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    } /* namespace avx */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX_FILTERS_DYNAMIC_H_ */
