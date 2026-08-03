/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 авг. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_FILTERS_DYNAMIC_NZ_H_
#define PRIVATE_DSP_ARCH_X86_AVX_FILTERS_DYNAMIC_NZ_H_


#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

#include <private/dsp/arch/x86/avx/filters/common.h>

namespace lsp
{
    namespace avx
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT32("cmpl              $0, %[count]")
                __ASM_EMIT64("test              %[count], %[count]")
                __ASM_EMIT("jz                  2f")

                // Load permanent data
                __ASM_EMIT("vmovss              0x00(%[d]), %%xmm6")                                // xmm6 = d0
                __ASM_EMIT("xor                 %[off], %[off]")
                __ASM_EMIT("vmovss              0x04(%[d]), %%xmm7")                                // xmm7 = d1

                // Start loop
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovss              (%[src], %[off], 4), %%xmm0")                       // xmm0 = s ? ? ?
                __ASM_EMIT("vmulss              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1 = b0*s
                __ASM_EMIT("vmulss              0x04(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s
                __ASM_EMIT("vmulss              0x08(%[f]), %%xmm0, %%xmm3")                        // xmm3 = b2*s
                __ASM_EMIT("vaddss              %%xmm6, %%xmm1, %%xmm0")                            // xmm0 = s' = d0 + b0*s
                __ASM_EMIT("vaddss              %%xmm7, %%xmm2, %%xmm2")                            // xmm2 = d1 + b1*s
                __ASM_EMIT("vmulss              0x0c(%[f]), %%xmm0, %%xmm4")                        // xmm4 = a1*s'
                __ASM_EMIT("vmulss              0x10(%[f]), %%xmm0, %%xmm5")                        // xmm5 = a2*s'
                __ASM_EMIT("vmovss              %%xmm0, (%[dst], %[off], 4)")                       // *dst = s'
                __ASM_EMIT("vaddss              %%xmm4, %%xmm2, %%xmm6")                            // xmm6 = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("add                 $1, %[off]")
                __ASM_EMIT("add                 $0x20, %[f]")
                __ASM_EMIT("cmp                 %[count], %[off]")
                __ASM_EMIT("vaddss              %%xmm5, %%xmm3, %%xmm7")                            // xmm7 = d1' = b2*s + a2*s'
                __ASM_EMIT("jb                  1b")

                // Store the updated buffer state
                __ASM_EMIT("vmovss              %%xmm6, 0x00(%[d])")
                __ASM_EMIT("vmovss              %%xmm7, 0x04(%[d])")

                // Exit label
                __ASM_EMIT("2:")

                : [off] "=&r"(off), [f] "+r" (f)
                : [dst] "r" (dst), [src] "r" (src),
                  [count] __ASM_ARG_RO (count),
                  [d] "r" (d)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void dyn_biquad_process_x1_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT32("cmpl              $0, %[count]")
                __ASM_EMIT64("test              %[count], %[count]")
                __ASM_EMIT("jz                  2f")

                // Load permanent data
                __ASM_EMIT("vmovss              0x00(%[d]), %%xmm6")                                // xmm6 = d0
                __ASM_EMIT("xor                 %[off], %[off]")
                __ASM_EMIT("vmovss              0x04(%[d]), %%xmm7")                                // xmm7 = d1

                // Start loop
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovss              (%[src], %[off], 4), %%xmm0")                       // xmm0 = s ? ? ?
                __ASM_EMIT("vmovaps             %%xmm7, %%xmm5")                                    // xmm5 = d1
                __ASM_EMIT("vmulss              0x04(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s
                __ASM_EMIT("vmulss              0x08(%[f]), %%xmm0, %%xmm7")                        // xmm7 = b2*s
                __ASM_EMIT("vfmadd132ss         0x00(%[f]), %%xmm6, %%xmm0")                        // xmm0 = s' = d0 + b0*s
                __ASM_EMIT("vfmadd231ss         0x0c(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s + a1*s'
                __ASM_EMIT("vmovss              %%xmm0, (%[dst], %[off], 4)")                       // *dst = s'
                __ASM_EMIT("add                 $1, %[off]")
                __ASM_EMIT("vfmadd231ss         0x10(%[f]), %%xmm0, %%xmm7")                        // xmm7 = d1' = b2*s + a2*s'
                __ASM_EMIT("add                 $0x20, %[f]")
                __ASM_EMIT("cmp                 %[count], %[off]")
                __ASM_EMIT("vaddss              %%xmm5, %%xmm2, %%xmm6")                            // xmm6 = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("jb                  1b")

                // Store the updated buffer state
                __ASM_EMIT("vmovss              %%xmm6, 0x00(%[d])")
                __ASM_EMIT("vmovss              %%xmm7, 0x04(%[d])")

                // Exit label
                __ASM_EMIT("2:")

                : [off] "=&r"(off), [f] "+r" (f)
                : [dst] "r" (dst), [src] "r" (src),
                  [count] __ASM_ARG_RO (count),
                  [d] "r" (d)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f)
        {
            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT("test                %[count], %[count]")
                __ASM_EMIT("jz                  4f")

                // Start loop
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0 = s ? ? ?
                __ASM_EMIT("vmulss              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1 = b0*s
                __ASM_EMIT("vmulss              0x08(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s
                __ASM_EMIT("vmulss              0x10(%[f]), %%xmm0, %%xmm3")                        // xmm3 = b2*s
                __ASM_EMIT("vaddss              0x00(%[d]), %%xmm1, %%xmm0")                        // xmm0 = s' = b0*s + d0
                __ASM_EMIT("vaddss              0x08(%[d]), %%xmm2, %%xmm2")                        // xmm2 = d1 + b1*s
                __ASM_EMIT("vmulss              0x18(%[f]), %%xmm0, %%xmm4")                        // xmm4 = a1*s'
                __ASM_EMIT("vmulss              0x20(%[f]), %%xmm0, %%xmm5")                        // xmm5 = a2*s'
                __ASM_EMIT("vaddss              %%xmm4, %%xmm2, %%xmm6")                            // xmm6 = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vaddss              %%xmm5, %%xmm3, %%xmm7")                            // xmm7 = d1' = b2*s + a2*s'
                __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
                __ASM_EMIT("add                 $0x30, %[f]")                                       // ++f
                __ASM_EMIT("add                 $0x04, %[src]")                                     // src++
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("vmovss              %%xmm6, 0x00(%[d])")
                __ASM_EMIT("vmovss              %%xmm7, 0x08(%[d])")
                __ASM_EMIT("jz                  2f")
                // x2 loop
                __ASM_EMIT("vmovaps             0x00(%[d]), %%xmm6")                                // xmm6 = d0 e0 d1 e1
                __ASM_EMIT("vxorps              %%xmm7, %%xmm7, %%xmm7")                            // xmm7 = 0 0 0 0
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vinsertps           $0xc0, (%[src]), %%xmm0, %%xmm0")                   // xmm0 = s0 s1
                __ASM_EMIT("vmovlhps            %%xmm0, %%xmm0, %%xmm0")                            // xmm0 = s0 s1 s0 s1
                __ASM_EMIT("vmulps              0x08(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s0 j1*s1 b2*s0 j2*s1
                __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm0")                        // xmm0 = b0*s0 j0*s1
                __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                            // xmm0 = s0' s1' = d0+b0*s0 e0+j0*s1
                __ASM_EMIT("vshufps             $0x0e, %%xmm7, %%xmm6, %%xmm6")                     // xmm6 = d1 e1 0 0
                __ASM_EMIT("vmovlhps            %%xmm0, %%xmm0, %%xmm0")                            // xmm0 = s0' s1' s0' s1'
                __ASM_EMIT("vaddps              %%xmm2, %%xmm6, %%xmm6")                            // xmm6 = d1+b1*s0 e1+j1*s1 b2*s0 j2*s1
                __ASM_EMIT("vmulps              0x18(%[f]), %%xmm0, %%xmm3")                        // xmm3 = a1*s0' i1*s1' a2*s0' i2*s1'
                __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
                __ASM_EMIT("vaddps              %%xmm3, %%xmm6, %%xmm6")                            // xmm6 = d0' e0' d1' e1' = d1+b1*s0+a1*s0' e1+j1*s1+i1*s1' b2*s0+a2*s0' j2*s1+i2*s1'
                __ASM_EMIT("vmovss              %%xmm0, (%[dst])")
                __ASM_EMIT("add                 $0x30, %[f]")                                       // ++f
                __ASM_EMIT("add                 $0x04, %[src]")
                __ASM_EMIT("add                 $0x04, %[dst]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jnz                 1b")
                __ASM_EMIT("vmovaps             %%xmm6, 0x00(%[d])")
                // Last step
                __ASM_EMIT("2:")
                __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
                __ASM_EMIT("vmulss              0x04(%[f]), %%xmm0, %%xmm1")                        // xmm1 = b0*s
                __ASM_EMIT("vmulss              0x0c(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s
                __ASM_EMIT("vmulss              0x14(%[f]), %%xmm0, %%xmm3")                        // xmm3 = b2*s
                __ASM_EMIT("vaddss              0x04(%[d]), %%xmm1, %%xmm0")                        // xmm0 = s' = b0*s + d0
                __ASM_EMIT("vaddss              0x0c(%[d]), %%xmm2, %%xmm2")                        // xmm2 = d1 + b1*s
                __ASM_EMIT("vmulss              0x1c(%[f]), %%xmm0, %%xmm4")                        // xmm4 = a1*s'
                __ASM_EMIT("vmulss              0x24(%[f]), %%xmm0, %%xmm5")                        // xmm5 = a2*s'
                __ASM_EMIT("vaddss              %%xmm4, %%xmm2, %%xmm6")                            // xmm6 = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vaddss              %%xmm5, %%xmm3, %%xmm7")                            // xmm7 = d1' = b2*s + a2*s'
                __ASM_EMIT("vmovss              %%xmm0, (%[dst])")
                __ASM_EMIT("vmovss              %%xmm6, 0x04(%[d])")
                __ASM_EMIT("vmovss              %%xmm7, 0x0c(%[d])")
                // Exit label
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src), [f] "+r" (f)
                : [count] "r" (count), [d] "r" (d)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void dyn_biquad_process_x2_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f)
        {
            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT("test                %[count], %[count]")
                __ASM_EMIT("jz                  4f")

                // Start loop
                __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0 = s ? ? ?
                __ASM_EMIT("vmulss              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1 = b0*s
                __ASM_EMIT("vmulss              0x08(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s
                __ASM_EMIT("vmulss              0x10(%[f]), %%xmm0, %%xmm3")                        // xmm3 = b2*s
                __ASM_EMIT("vaddss              0x00(%[d]), %%xmm1, %%xmm0")                        // xmm0 = s' = b0*s + d0
                __ASM_EMIT("vaddss              0x08(%[d]), %%xmm2, %%xmm2")                        // xmm2 = d1 + b1*s
                __ASM_EMIT("vfmadd231ss         0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3 = d1' = b2*s + a2*s'
                __ASM_EMIT("vfmadd231ss         0x18(%[f]), %%xmm0, %%xmm2")                        // xmm2 = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
                __ASM_EMIT("add                 $0x30, %[f]")                                       // ++f
                __ASM_EMIT("add                 $0x04, %[src]")                                     // src++
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("vmovss              %%xmm2, 0x00(%[d])")
                __ASM_EMIT("vmovss              %%xmm3, 0x08(%[d])")
                __ASM_EMIT("jz                  2f")
                // x2 loop
                __ASM_EMIT("vmovaps             0x00(%[d]), %%xmm6")                                // xmm6 = d0 e0 d1 e1
                __ASM_EMIT("vxorps              %%xmm7, %%xmm7, %%xmm7")                            // xmm7 = 0 0 0 0
                __ASM_EMIT(".align              16")
                __ASM_EMIT("1:")
                __ASM_EMIT("vinsertps           $0xc0, (%[src]), %%xmm0, %%xmm0")                   // xmm0 = s0 s1
                __ASM_EMIT("vmovlhps            %%xmm0, %%xmm0, %%xmm0")                            // xmm0 = s0 s1 s0 s1
                __ASM_EMIT("vmulps              0x08(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s0 j1*s1 b2*s0 j2*s1
                __ASM_EMIT("vfmadd132ps         0x00(%[f]), %%xmm6, %%xmm0")                        // xmm0 = s0' s1' = d0+b0*s0 e0+j0*s1
                __ASM_EMIT("vshufps             $0x0e, %%xmm7, %%xmm6, %%xmm6")                     // xmm6 = d1 e1 0 0
                __ASM_EMIT("vmovlhps            %%xmm0, %%xmm0, %%xmm0")                            // xmm0 = s0' s1' s0' s1'
                __ASM_EMIT("vaddps              %%xmm2, %%xmm6, %%xmm6")                            // xmm6 = d1+b1*s0 e1+j1*s1 b2*s0 j2*s1
                __ASM_EMIT("vmulps              0x18(%[f]), %%xmm0, %%xmm3")                        // xmm3 = a1*s0' i1*s1' a2*s0' i2*s1'
                __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
                __ASM_EMIT("vaddps              %%xmm3, %%xmm6, %%xmm6")                            // xmm6 = d0' e0' d1' e1' = d1+b1*s0+a1*s0' e1+j1*s1+i1*s1' b2*s0+a2*s0' j2*s1+i2*s1'
                __ASM_EMIT("vmovss              %%xmm0, (%[dst])")
                __ASM_EMIT("add                 $0x30, %[f]")                                       // ++f
                __ASM_EMIT("add                 $0x04, %[src]")
                __ASM_EMIT("add                 $0x04, %[dst]")
                __ASM_EMIT("dec                 %[count]")
                __ASM_EMIT("jnz                 1b")
                __ASM_EMIT("vmovaps             %%xmm6, 0x00(%[d])")
                // Last step
                __ASM_EMIT("2:")
                __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
                __ASM_EMIT("vmulss              0x04(%[f]), %%xmm0, %%xmm1")                        // xmm1 = b0*s
                __ASM_EMIT("vmulss              0x0c(%[f]), %%xmm0, %%xmm2")                        // xmm2 = b1*s
                __ASM_EMIT("vmulss              0x14(%[f]), %%xmm0, %%xmm3")                        // xmm3 = b2*s
                __ASM_EMIT("vaddss              0x04(%[d]), %%xmm1, %%xmm0")                        // xmm0 = s' = b0*s + d0
                __ASM_EMIT("vaddss              0x0c(%[d]), %%xmm2, %%xmm2")                        // xmm2 = d1 + b1*s
                __ASM_EMIT("vfmadd231ss         0x24(%[f]), %%xmm0, %%xmm3")                        // xmm3 = d1' = b2*s + a2*s'
                __ASM_EMIT("vfmadd231ss         0x1c(%[f]), %%xmm0, %%xmm2")                        // xmm2 = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vmovss              %%xmm0, (%[dst])")
                __ASM_EMIT("vmovss              %%xmm2, 0x04(%[d])")
                __ASM_EMIT("vmovss              %%xmm3, 0x0c(%[d])")
                // Exit label
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src), [f] "+r" (f)
                : [count] "r" (count), [d] "r" (d)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        IF_ARCH_X86(
            static const uint32_t dyn_biquad_x4_mask[4] __lsp_aligned16 =
            {
                0xffffffff, 0, 0, 0
            };
        )

        void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f)
        {
            IF_ARCH_X86(
                float   MASK[4] __lsp_aligned16;
                size_t  mask;
            )

            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT32("cmpl              $0, %[count]")
                __ASM_EMIT64("test              %[count], %[count]")
                __ASM_EMIT("jz                  8f")

                // Initialize mask
                // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
                __ASM_EMIT("mov                 $1, %[mask]")
                __ASM_EMIT("vmovaps             %[X_MASK], %%xmm5")
                __ASM_EMIT("xorps               %%xmm1, %%xmm1")
                __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")

                // Load delay buffer
                __ASM_EMIT("vmovaps             0x00(%[d]), %%xmm6")                                // xmm6     = d0
                __ASM_EMIT("vmovaps             0x10(%[d]), %%xmm7")                                // xmm7     = d1

                // Process first 3 steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vinsertps           $0x00, (%[src]), %%xmm0, %%xmm0")                   // xmm0     = s = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = b0*s
                __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = b1*s
                __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = b2*s
                __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = b0*s + d0
                __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + b1*s
                __ASM_EMIT("vmulps              0x30(%[f]), %%xmm0, %%xmm4")                        // xmm4     = a1*s'
                __ASM_EMIT("vmulps              0x40(%[f]), %%xmm0, %%xmm5")                        // xmm5     = a2*s'
                __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm2")                            // xmm2     = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vaddps              %%xmm5, %%xmm3, %%xmm3")                            // xmm3     = d1' = b2*s + a2*s'
                __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = mask
                __ASM_EMIT("vshufps             $0x90, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[0] s2[0] s2[1] s2[2]
                __ASM_EMIT("add                 $0x50, %[f]")
                __ASM_EMIT("vblendvps           %%xmm5, %%xmm2, %%xmm6, %%xmm6")                    // xmm6     = (d0') & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%xmm5, %%xmm3, %%xmm7, %%xmm7")                    // xmm7     = (d1') & MASK | (d0 & ~MASK)
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jz                  4f")                                                // jump to completion
                __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm5     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
                __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")                                   // store mask
                __ASM_EMIT("cmp                 $0x0f, %[mask]")
                __ASM_EMIT("jne                 1b")

                // 4x filter processing without mask
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("3:")
                __ASM_EMIT("vinsertps           $0x00, (%[src]), %%xmm0, %%xmm0")                   // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = b0*s
                __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = b1*s
                __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = b2*s
                __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = b0*s + d0
                __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + b1*s
                __ASM_EMIT("vmulps              0x30(%[f]), %%xmm0, %%xmm4")                        // xmm4     = a1*s'
                __ASM_EMIT("vmulps              0x40(%[f]), %%xmm0, %%xmm5")                        // xmm5     = a2*s'
                __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm6")                            // xmm6     = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vshufps             $0x93, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[0] s2[0] s2[1] s2[2]
                __ASM_EMIT("vaddps              %%xmm5, %%xmm3, %%xmm7")                            // xmm7     = d1' = b2*s + a2*s'
                __ASM_EMIT("vmovss              %%xmm0, (%[dst])")                                  // *dst     = s2[3]
                __ASM_EMIT("add                 $0x50, %[f]")
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jnz                 3b")
                __ASM_EMIT("4:")
                // Prepare last loop
                __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = m[0] m[1] m[2] m[3]
                __ASM_EMIT("vxorps              %%xmm2, %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm5     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("vmovss              %%xmm2, %%xmm5, %%xmm5")                            // xmm0     = 0 m[0] m[1] m[2]
                __ASM_EMIT("and                 $0x0f, %[mask]")                                    // mask     = (mask << 1) & 0x0f
                __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")

                // Process steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("5:")
                __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = b0*s
                __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = b1*s
                __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = b2*s
                __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = b0*s + d0
                __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + b1*s
                __ASM_EMIT("vmulps              0x30(%[f]), %%xmm0, %%xmm4")                        // xmm4     = a1*s'
                __ASM_EMIT("vmulps              0x40(%[f]), %%xmm0, %%xmm5")                        // xmm5     = a2*s'
                __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm2")                            // xmm2     = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vaddps              %%xmm5, %%xmm3, %%xmm3")                            // xmm3     = d1' = b2*s + a2*s'
                __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = mask
                __ASM_EMIT("vshufps             $0x93, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("test                $0x8, %[mask]")
                __ASM_EMIT("jz                  7f")
                __ASM_EMIT("vmovss              %%xmm0, (%[dst])")                                  // *dst     = s2[3]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("7:")
                __ASM_EMIT("add                 $0x50, %[f]")
                __ASM_EMIT("vblendvps           %%xmm5, %%xmm2, %%xmm6, %%xmm6")                    // xmm6     = (d0') & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%xmm5, %%xmm3, %%xmm7, %%xmm7")                    // xmm7     = (d1') & MASK | (d0 & ~MASK)
                // Repeat loop
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm0     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("and                 $0x0f, %[mask]")                                    // mask     = (mask << 1) & 0x0f
                __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")
                __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("vmovaps             %%xmm6, 0x00(%[d])")                                // xmm6     = d0
                __ASM_EMIT("vmovaps             %%xmm7, 0x10(%[d])")                                // xmm7     = d1
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [f] "+r" (f),
                  [mask] "=&r"(mask), [count] X86_PGREG (count)
                : [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const),
                  [MASK] "m" (MASK)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void dyn_biquad_process_x4_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f)
        {
            IF_ARCH_X86(
                size_t  mask;
            )

            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT32("cmpl              $0, %[count]")
                __ASM_EMIT64("test              %[count], %[count]")
                __ASM_EMIT("jz                  8f")

                // Initialize mask
                // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm5=mask[4], xmm6=d0[4], xmm7=d1[4]
                __ASM_EMIT("mov                 $1, %[mask]")
                __ASM_EMIT("vmovaps             %[X_MASK], %%xmm5")
                __ASM_EMIT("xorps               %%xmm1, %%xmm1")

                // Load delay buffer
                __ASM_EMIT("vmovaps             0x00(%[d]), %%xmm6")                                // xmm6     = d0
                __ASM_EMIT("vmovaps             0x10(%[d]), %%xmm7")                                // xmm7     = d1

                // Process first 3 steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vinsertps           $0x00, (%[src]), %%xmm0, %%xmm0")                   // xmm0     = s = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = b0*s
                __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = b1*s
                __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = b2*s
                __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = b0*s + d0
                __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + b1*s
                __ASM_EMIT("vfmadd231ps         0x30(%[f]), %%xmm0, %%xmm2")                        // xmm2     = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vfmadd231ps         0x40(%[f]), %%xmm0, %%xmm3")                        // xmm3     = d1' = b2*s + a2*s'
                __ASM_EMIT("vshufps             $0x90, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[0] s2[0] s2[1] s2[2]
                __ASM_EMIT("vblendvps           %%xmm5, %%xmm2, %%xmm6, %%xmm6")                    // xmm6     = (d0') & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%xmm5, %%xmm3, %%xmm7, %%xmm7")                    // xmm7     = (d1') & MASK | (d0 & ~MASK)
                __ASM_EMIT("add                 $0x50, %[f]")
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jz                  4f")                                                // jump to completion
                __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm5     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
                __ASM_EMIT("cmp                 $0x0f, %[mask]")
                __ASM_EMIT("jne                 1b")

                // 4x filter processing without mask
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("3:")
                __ASM_EMIT("vinsertps           $0x00, (%[src]), %%xmm0, %%xmm0")                   // xmm0     = *src
                __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
                __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = b1*s
                __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = b2*s
                __ASM_EMIT("vfmadd132ps         0x00(%[f]), %%xmm6, %%xmm0")                        // xmm0     = s' = b0*s + d0
                __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm6")                            // xmm6     = d1 + b1*s
                __ASM_EMIT("vfmadd231ps         0x40(%[f]), %%xmm0, %%xmm3")                        // xmm3     = d1' = b2*s + a2*s'
                __ASM_EMIT("vfmadd231ps         0x30(%[f]), %%xmm0, %%xmm6")                        // xmm6     = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vmovaps             %%xmm3, %%xmm7")                                    // xmm7     = d1' = b2*s + a2*s'
                __ASM_EMIT("vshufps             $0x93, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[0] s2[0] s2[1] s2[2]
                __ASM_EMIT("vmovss              %%xmm0, (%[dst])")                                  // *dst     = s2[3]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("add                 $0x50, %[f]")
                __ASM_EMIT32("decl              %[count]")
                __ASM_EMIT64("dec               %[count]")
                __ASM_EMIT("jnz                 3b")
                __ASM_EMIT("4:")
                // Prepare last loop
                __ASM_EMIT("vxorps              %%xmm2, %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm5     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("vmovss              %%xmm2, %%xmm5, %%xmm5")                            // xmm0     = 0 m[0] m[1] m[2]
                __ASM_EMIT("and                 $0x0f, %[mask]")                                    // mask     = (mask << 1) & 0x0f

                // Process steps
                __ASM_EMIT(".p2align            4")
                __ASM_EMIT("5:")
                __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = b0*s
                __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = b1*s
                __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = b2*s
                __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = b0*s + d0
                __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + b1*s
                __ASM_EMIT("vfmadd231ps         0x30(%[f]), %%xmm0, %%xmm2")                        // xmm2     = d0' = d1 + b1*s + a1*s'
                __ASM_EMIT("vfmadd231ps         0x40(%[f]), %%xmm0, %%xmm3")                        // xmm3     = d1' = b2*s + a2*s'
                __ASM_EMIT("vshufps             $0x93, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("test                $0x8, %[mask]")
                __ASM_EMIT("jz                  7f")
                __ASM_EMIT("vmovss              %%xmm0, (%[dst])")                                  // *dst     = s2[3]
                __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
                __ASM_EMIT("7:")
                __ASM_EMIT("add                 $0x50, %[f]")
                __ASM_EMIT("vblendvps           %%xmm5, %%xmm2, %%xmm6, %%xmm6")                    // xmm6     = (d0') & MASK | (d0 & ~MASK)
                __ASM_EMIT("vblendvps           %%xmm5, %%xmm3, %%xmm7, %%xmm7")                    // xmm7     = (d1') & MASK | (d0 & ~MASK)
                // Repeat loop
                __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
                __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm0     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("and                 $0x0f, %[mask]")                                    // mask     = (mask << 1) & 0x0f
                __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("vmovaps             %%xmm6, 0x00(%[d])")                                // xmm6     = d0
                __ASM_EMIT("vmovaps             %%xmm7, 0x10(%[d])")                                // xmm7     = d1
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [mask] "=&r"(mask), [count] X86_PGREG (count)
                : [f] "r" (f), [d] "r" (d),
                  [X_MASK] "m" (biquad_mask_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX_FILTERS_DYNAMIC_NZ_H_ */
