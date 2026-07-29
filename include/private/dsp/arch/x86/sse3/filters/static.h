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

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_STATIC_H_
#define PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_STATIC_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE3_IMPL */

#include <private/dsp/arch/x86/sse3/filters/common.h>

namespace lsp
{
    namespace sse3
    {
        void x64_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f)
        {
            IF_ARCH_X86_64(
                float MASK0[4] __lsp_aligned16;
                float MASK1[4] __lsp_aligned16;
                size_t mask;
            )

            ARCH_X86_64_ASM
            (
                // Check count
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          8f")

                // Initialize mask
                // xmm0=tmp,  xmm1 ={s,s2[4]}, xmm2 = p1[4], xmm3 = p2[4], xmm6 = d0[4], xmm7 = d2[4]
                // xmm8=mask, xmm9 ={r,r2[4]}, xmm10= q1[4], xmm11= q2[4], xmm14= d1[4], xmm15= d3[4]
                __ASM_EMIT("mov         $1, %[mask]")
                __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")
                __ASM_EMIT("movaps      %%xmm0, %[MASK0]")
                __ASM_EMIT("movaps      %%xmm1, %[MASK1]")

                // Load delay buffer
                __ASM_EMIT("movaps      0x00(%[d]), %%xmm6")                        // xmm6     = d0
                __ASM_EMIT("movaps      0x10(%[d]), %%xmm14")                       // xmm14    = d1
                __ASM_EMIT("movaps      0x20(%[d]), %%xmm7")                        // xmm7     = d2
                __ASM_EMIT("movaps      0x30(%[d]), %%xmm15")                       // xmm15    = d3

                //-------------------------------------------------------------
                // Process first steps
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
                __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
                __ASM_EMIT("add         $4, %[src]")                                // src      ++
                X64_FILTER_X8

                // Shift buffer and update delay
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
                __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Update delay only by mask for filters
                __ASM_EMIT("movaps      %[MASK0], %%xmm0")                          // xmm0     = MASK_LO
                __ASM_EMIT("movaps      %[MASK1], %%xmm8")                          // xmm8     = MASK_HI
                X64_FILTER_X8_MASK_MEMSYNC

                // Shift mask and repeat loop
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jz          4f")                                        // jump to completion
                __ASM_EMIT("lea         0x01(,%[mask],2), %[mask]")                 // mask     = (mask << 1) | 1
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                            // xmm2     = m[0] m[1] m[2] m[3]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("movaps      %%xmm0, %[MASK0]")                          // *MASK_LO = xmm0
                __ASM_EMIT("movaps      %%xmm8, %[MASK1]")                          // *MASK_HI = xmm8
                __ASM_EMIT("cmp         $0xff, %[mask]")
                __ASM_EMIT("jne         1b")

                //-------------------------------------------------------------
                // 8x filter processing without mask
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("3:")
                __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
                __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
                __ASM_EMIT("add         $4, %[src]")                                // src      ++
                X64_FILTER_X8

                // Shift buffer and update delay
                X64_FILTER_X8_NOMASK_MEMSYNC
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("movss       %%xmm9, (%[dst])")                          // *dst     = r2[3]
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Repeat loop
                __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         3b")

                //-------------------------------------------------------------
                // Prepare last loop
                __ASM_EMIT("4:")
                __ASM_EMIT("movaps      %[MASK0], %%xmm0")                          // xmm0     = m[0] m[1] m[2] m[3]
                __ASM_EMIT("movaps      %[MASK1], %%xmm8")                          // xmm8     = m[4] m[5] m[6] m[7]
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]

                //-------------------------------------------------------------
                // Process last steps
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("5:")
                X64_FILTER_X8

                // Shift buffer and update delay
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
                __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
                __ASM_EMIT("test        $0x80, %[mask]")
                __ASM_EMIT("jz          6f")
                __ASM_EMIT("movss       %%xmm9, (%[dst])")                          // *dst     = r2[3]
                __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
                __ASM_EMIT("6:")
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Update delay only by mask for filters
                X64_FILTER_X8_MASK_MEMSYNC

                // Shift mask and repeat loop
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("and         $0xff, %[mask]")                            // mask     = (mask << 1) & 0xff
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
                __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

                //-------------------------------------------------------------
                // Store delay buffers
                __ASM_EMIT("movaps      %%xmm6, 0x00(%[d])")                        // xmm6     = d0
                __ASM_EMIT("movaps      %%xmm14, 0x10(%[d])")                       // xmm14    = d1
                __ASM_EMIT("movaps      %%xmm7, 0x20(%[d])")                        // xmm7     = d2
                __ASM_EMIT("movaps      %%xmm15, 0x30(%[d])")                       // xmm15    = d3

                // Exit label
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [f] "r" (f),
                  [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const),
                  [MASK0] "m" (MASK0),
                  [MASK1] "m" (MASK1)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void x64_biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f)
        {
            IF_ARCH_X86_64(
                float MASK0[4] __lsp_aligned16;
                float MASK1[4] __lsp_aligned16;
                float *X_DST;
                size_t X_COUNT;
                size_t mask;
            )

            ARCH_X86_64_ASM
            (
                // Check count
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          10f")
                __ASM_EMIT("mov         %[dst], %[X_DST]")
                __ASM_EMIT("mov         %[count], %[X_COUNT]")

                //---------------------------------------------------------------------
                // Cycle 1
                // Initialize mask
                // xmm0=tmp,  xmm1 ={s,s2[4]}, xmm2 = p1[4], xmm3 = p2[4], xmm6 = d0[4], xmm7 = d2[4]
                // xmm8=mask, xmm9 ={r,r2[4]}, xmm10= q1[4], xmm11= q2[4], xmm14= d1[4], xmm15= d3[4]
                __ASM_EMIT("mov         $1, %[mask]")
                __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")
                __ASM_EMIT("movaps      %%xmm0, %[MASK0]")
                __ASM_EMIT("movaps      %%xmm1, %[MASK1]")

                // Load delay buffer
                __ASM_EMIT("movaps      0x00(%[d]), %%xmm6")                        // xmm6     = d0
                __ASM_EMIT("movaps      0x10(%[d]), %%xmm14")                       // xmm14    = d1
                __ASM_EMIT("movaps      0x40(%[d]), %%xmm7")                        // xmm7     = d2
                __ASM_EMIT("movaps      0x50(%[d]), %%xmm15")                       // xmm15    = d3

                // Process first steps
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
                __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
                __ASM_EMIT("add         $4, %[src]")                                // src      ++
                X64_FILTER_X16P1

                // Shift buffer and update delay
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
                __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Update delay only by mask for filters
                __ASM_EMIT("movaps      %[MASK0], %%xmm0")                          // xmm0     = MASK_LO
                __ASM_EMIT("movaps      %[MASK1], %%xmm8")                          // xmm8     = MASK_HI
                X64_FILTER_X8_MASK_MEMSYNC

                // Shift mask and repeat loop
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jz          4f")                                        // jump to completion
                __ASM_EMIT("lea         0x01(,%[mask],2), %[mask]")                 // mask     = (mask << 1) | 1
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                            // xmm2     = m[0] m[1] m[2] m[3]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("movaps      %%xmm0, %[MASK0]")                          // *MASK_LO = xmm0
                __ASM_EMIT("movaps      %%xmm8, %[MASK1]")                          // *MASK_HI = xmm8
                __ASM_EMIT("cmp         $0xff, %[mask]")
                __ASM_EMIT("jne         1b")

                // 8x filter processing without mask
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("3:")
                __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
                __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
                __ASM_EMIT("add         $4, %[src]")                                // src      ++
                X64_FILTER_X16P1

                // Shift buffer and update delay
                X64_FILTER_X8_NOMASK_MEMSYNC
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("movss       %%xmm9, (%[dst])")                          // *dst     = r2[3]
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Repeat loop
                __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         3b")

                // Prepare last loop
                __ASM_EMIT("4:")
                __ASM_EMIT("movaps      %[MASK0], %%xmm0")                          // xmm0     = m[0] m[1] m[2] m[3]
                __ASM_EMIT("movaps      %[MASK1], %%xmm8")                          // xmm8     = m[4] m[5] m[6] m[7]
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]

                // Process last steps
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("5:")
                X64_FILTER_X16P1

                // Shift buffer and update delay
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
                __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
                __ASM_EMIT("test        $0x80, %[mask]")
                __ASM_EMIT("jz          6f")
                __ASM_EMIT("movss       %%xmm9, (%[dst])")                          // *dst     = r2[3]
                __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
                __ASM_EMIT("6:")
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Update delay only by mask for filters
                X64_FILTER_X8_MASK_MEMSYNC

                // Shift mask and repeat loop
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("and         $0xff, %[mask]")                            // mask     = (mask << 1) & 0xff
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
                __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

                // Store delay buffers
                __ASM_EMIT("movaps      %%xmm6, 0x00(%[d])")                        // xmm6     = d0
                __ASM_EMIT("movaps      %%xmm14, 0x10(%[d])")                       // xmm14    = d1
                __ASM_EMIT("movaps      %%xmm7, 0x40(%[d])")                        // xmm7     = d2
                __ASM_EMIT("movaps      %%xmm15, 0x50(%[d])")                       // xmm15    = d3

                //---------------------------------------------------------------------
                // Cycle 2
                __ASM_EMIT("mov         %[X_DST], %[dst]")
                __ASM_EMIT("mov         %[X_COUNT], %[count]")
                __ASM_EMIT("mov         %[dst], %[src]")
                // Initialize mask
                // xmm0=tmp,  xmm1 ={s,s2[4]}, xmm2 = p1[4], xmm3 = p2[4], xmm6 = d0[4], xmm7 = d2[4]
                // xmm8=mask, xmm9 ={r,r2[4]}, xmm10= q1[4], xmm11= q2[4], xmm14= d1[4], xmm15= d3[4]
                __ASM_EMIT("mov         $1, %[mask]")
                __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")
                __ASM_EMIT("movaps      %%xmm0, %[MASK0]")
                __ASM_EMIT("movaps      %%xmm1, %[MASK1]")

                // Load delay buffer
                __ASM_EMIT("movaps      0x20(%[d]), %%xmm6")                        // xmm6     = d0
                __ASM_EMIT("movaps      0x30(%[d]), %%xmm14")                       // xmm14    = d1
                __ASM_EMIT("movaps      0x60(%[d]), %%xmm7")                        // xmm7     = d2
                __ASM_EMIT("movaps      0x70(%[d]), %%xmm15")                       // xmm15    = d3

                // Process first steps
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
                __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
                __ASM_EMIT("add         $4, %[src]")                                // src      ++
                X64_FILTER_X16P2

                // Shift buffer and update delay
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
                __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Update delay only by mask for filters
                __ASM_EMIT("movaps      %[MASK0], %%xmm0")                          // xmm0     = MASK_LO
                __ASM_EMIT("movaps      %[MASK1], %%xmm8")                          // xmm8     = MASK_HI
                X64_FILTER_X8_MASK_MEMSYNC

                // Shift mask and repeat loop
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jz          4f")                                        // jump to completion
                __ASM_EMIT("lea         0x01(,%[mask],2), %[mask]")                 // mask     = (mask << 1) | 1
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                            // xmm2     = m[0] m[1] m[2] m[3]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("movaps      %%xmm0, %[MASK0]")                          // *MASK_LO = xmm0
                __ASM_EMIT("movaps      %%xmm8, %[MASK1]")                          // *MASK_HI = xmm8
                __ASM_EMIT("cmp         $0xff, %[mask]")
                __ASM_EMIT("jne         1b")

                // 8x filter processing without mask
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("3:")
                __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
                __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
                __ASM_EMIT("add         $4, %[src]")                                // src      ++
                X64_FILTER_X16P2

                // Shift buffer and update delay
                X64_FILTER_X8_NOMASK_MEMSYNC
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("movss       %%xmm9, (%[dst])")                          // *dst     = r2[3]
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Repeat loop
                __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         3b")

                // Prepare last loop
                __ASM_EMIT("4:")
                __ASM_EMIT("movaps      %[MASK0], %%xmm0")                          // xmm0     = m[0] m[1] m[2] m[3]
                __ASM_EMIT("movaps      %[MASK1], %%xmm8")                          // xmm8     = m[4] m[5] m[6] m[7]
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]

                // Process last steps
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("5:")
                X64_FILTER_X16P2

                // Shift buffer and update delay
                __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
                __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
                __ASM_EMIT("test        $0x80, %[mask]")
                __ASM_EMIT("jz          6f")
                __ASM_EMIT("movss       %%xmm9, (%[dst])")                          // *dst     = r2[3]
                __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
                __ASM_EMIT("6:")
                __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

                // Update delay only by mask for filters
                X64_FILTER_X8_MASK_MEMSYNC

                // Shift mask and repeat loop
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
                __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
                __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
                __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
                __ASM_EMIT("and         $0xff, %[mask]")                            // mask     = (mask << 1) & 0xff
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
                __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

                // Store delay buffers
                __ASM_EMIT("movaps      %%xmm6, 0x20(%[d])")                        // xmm6     = d0
                __ASM_EMIT("movaps      %%xmm14, 0x30(%[d])")                       // xmm14    = d1
                __ASM_EMIT("movaps      %%xmm7, 0x60(%[d])")                        // xmm7     = d2
                __ASM_EMIT("movaps      %%xmm15, 0x70(%[d])")                       // xmm15    = d3

                // Exit label
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [f] "r" (f),
                  [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const),
                  [MASK0] "m" (MASK0),
                  [MASK1] "m" (MASK1),
                  [X_DST] "m" (X_DST),
                  [X_COUNT] "m" (X_COUNT)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    } /* namespace sse3 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_STATIC_H_ */
