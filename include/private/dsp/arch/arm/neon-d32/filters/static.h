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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        void biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f)
        {
            ARCH_ARM_ASM
            (
                // Check count
                __ASM_EMIT("vldm            %[d],  {s14-s15}")
                __ASM_EMIT("vldm            %[f], {s8-s12}")
                // s8   = b0
                // s9   = b1
                // s10  = b2
                // s11  = a1
                // s12  = a2
                // s14  = d0
                // s15  = d1
                // x2 blocks
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[src]!, {s0-s1}")                      // s0   = s0, s1 = s1
                __ASM_EMIT("vmul.f32        s2, s0, s8")                            // s2   = b0*s0
                __ASM_EMIT("vmul.f32        s4, s0, s9")                            // s4   = b1*s0
                __ASM_EMIT("vmul.f32        s6, s0, s10")                           // s6   = b2*s0
                __ASM_EMIT("vmul.f32        s3, s1, s8")                            // s3   = b0*s1
                __ASM_EMIT("vadd.f32        s0, s2, s14")                           // s0   = s0' = d0 + b0*s0
                __ASM_EMIT("vmul.f32        s5, s1, s9")                            // s5   = b1*s1
                __ASM_EMIT("vmla.f32        s4, s0, s11")                           // s4   = b1*s0 + a1*s0'
                __ASM_EMIT("vmul.f32        s7, s1, s10")                           // s7   = b2*s1
                __ASM_EMIT("vmla.f32        s6, s0, s12")                           // s6   = d1' = b2*s0 + a2*s0'
                __ASM_EMIT("vadd.f32        s4, s15, s4")                           // s4   = d0' = d1 + b1*s0 + a1*s0'
                __ASM_EMIT("vadd.f32        s1, s3, s4")                            // s1   = s1' = d0' + b0*s1
                __ASM_EMIT("vmla.f32        s5, s1, s11")                           // s5   = b1*s1 + a1*s1'
                __ASM_EMIT("vmla.f32        s7, s1, s12")                           // s7   = d1" = b2*s1 + a2*s1'
                __ASM_EMIT("vadd.f32        s14, s6, s5")                           // s14  = d0" = d1' + b1*s1 + a1*s1'
                __ASM_EMIT("vmov            s15, s7")
                __ASM_EMIT("vstm            %[dst]!, {s0-s1}")
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x1 block:
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldr            s0, [%[src]]")                          // v0   = s0
                __ASM_EMIT("vmul.f32        s2, s0, s8")                            // s2   = b0*s0
                __ASM_EMIT("vmul.f32        s4, s0, s9")                            // s4   = b1*s0
                __ASM_EMIT("vmul.f32        s6, s0, s10")                           // s6   = b2*s0
                __ASM_EMIT("vadd.f32        s0, s2, s14")                           // s0   = s0' = d0 + b0*s0
                __ASM_EMIT("vmla.f32        s4, s0, s11")                           // s4   = b1*s0 + a1*s0'
                __ASM_EMIT("vmla.f32        s6, s0, s12")                           // s6   = d1' = b2*s0 + a2*s0'
                __ASM_EMIT("vadd.f32        s14, s15, s4")                          // s14  = d0' = d1 + b1*s0 + a1*s0'
                __ASM_EMIT("vmov            s15, s6")
                __ASM_EMIT("vstr            s0, [%[dst]]")
                __ASM_EMIT("4:")
                // Store the updated buffer state
                __ASM_EMIT("vstm            %[d],  {s14-s15}")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [d] "r" (d), [f] "r" (f)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3"
            );
        }

        void biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f)
        {
            ARCH_ARM_ASM
            (
                // Check count
                __ASM_EMIT("tst             %[count], %[count]")
                __ASM_EMIT("beq             6f")
                __ASM_EMIT("vldm            %[d], {d14-d15}")                       // d14  = d0 e0, d15 = d1 e1
                __ASM_EMIT("vldm            %[f], {d8-d12}")                        // d8   = b0, d9 = b1, d10 = b2, d11 = a1, d12 = a2
                // x1 head block
                __ASM_EMIT("vld1.32         {d0[0]}, [%[src]]!")                    // d0   = s0
                __ASM_EMIT("vmul.f32        d1, d8, d0")                            // d1   = b0*s0
                __ASM_EMIT("vadd.f32        d4, d14, d1")                           // d4   = s' = d0+b0*s0
                __ASM_EMIT("vmul.f32        d2, d9, d0")                            // d2   = b1*s0
                __ASM_EMIT("vadd.f32        d5, d15, d2")                           // d5   = d1+b1*s0
                __ASM_EMIT("vmul.f32        d6, d10, d0")                           // d6   = b2*s0
                __ASM_EMIT("vmla.f32        d5, d11, d4")                           // d5   = d0' = d1+b1*s0+a1*s'
                __ASM_EMIT("vmla.f32        d6, d12, d4")                           // d6   = d1' = b2*s0+a2*s'
                __ASM_EMIT("vext.32         d0, d0, d4, #1")                        // shift
                __ASM_EMIT("vmov            s28, s10")                              // update d0
                __ASM_EMIT("vmov            s30, s12")                              // update d1
                // x2 blocks
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("bls             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {d0[0]}, [%[src]]!")                    // d0   = s0 j0
                __ASM_EMIT("vmul.f32        d1, d8, d0")                            // d1   = b0*s0 b0*j0
                __ASM_EMIT("vadd.f32        d4, d14, d1")                           // d4   = s' j' = d0+b0*s0 e0+b0*j0
                __ASM_EMIT("vmul.f32        d2, d9, d0")                            // d2   = b1*s0 b1*j0
                __ASM_EMIT("vst1.32         {d4[1]}, [%[dst]]!")
                __ASM_EMIT("vadd.f32        d14, d15, d2")                          // d15  = d1+b1*s0 e1+b1*j0
                __ASM_EMIT("vmul.f32        d15, d10, d0")                          // d23  = b2*s0 b2*j0
                __ASM_EMIT("vmla.f32        d14, d11, d4")                          // d22  = d0' e0' = d1+b1*s0+a1*s' e1+b1*j0+a1*j'
                __ASM_EMIT("vmla.f32        d15, d12, d4")                          // d23  = d1' e1' = b2*s0+a2*s' b2*j0 a2*j'
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("vext.32         d0, d0, d4, #1")                        // shift
                __ASM_EMIT("bhi             1b")
                __ASM_EMIT("2:")
                // x1 tail block
                __ASM_EMIT("vmul.f32        d1, d8, d0")                            // v1   = b0*j0
                __ASM_EMIT("vadd.f32        d4, d14, d1")                           // v4   = s' = e0 + b0*j0
                __ASM_EMIT("vmul.f32        d2, d9, d0")                            // v2   = b1*j0
                __ASM_EMIT("vadd.f32        d5, d15, d2")                           // v5   = e1 + b1*j0
                __ASM_EMIT("vst1.32         {d4[1]}, [%[dst]]")
                __ASM_EMIT("vmul.f32        d6, d10, d0")                           // v6   = b2*j0
                __ASM_EMIT("vmla.f32        d5, d11, d4")                           // v5   = d0' = e1 + b1*j0 + a1*j'
                __ASM_EMIT("vmla.f32        d6, d12, d4")                           // v6   = d1' = b2*j0 + a2*j'
                __ASM_EMIT("vmov            s29, s11")                              // update d0
                __ASM_EMIT("vmov            s31, s13")                              // update d1
                // Store the updated buffer state
                __ASM_EMIT("vstm            %[d], {d14-d15}")
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [d] "r" (d), [f] "r" (f)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7"
            );
        }

        IF_ARCH_ARM(
            static const uint32_t biquad_x4_mask[8] __lsp_aligned16 =
            {
                0xffffffff, 0x00000000, 0x00000000, 0x00000000,
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
            };
        )

        void biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f)
        {
            IF_ARCH_ARM(
                size_t mask;
            );

            ARCH_ARM_ASM
            (
                __ASM_EMIT("tst         %[count], %[count]")
                __ASM_EMIT("beq         10f")

                // Prepare
                __ASM_EMIT("vldm        %[d], {q8-q9}")                         // q8-q9 = { d0, d1 }
                __ASM_EMIT("vldm        %[f], {q3-q7}")                         // q3-q7 = { b0, b1, b2, a1, a2 }
                __ASM_EMIT("vldm        %[X_MASK], {q10-q11}")                  // q10-q11 = { vmask, 1 }
                __ASM_EMIT("mov         %[mask], #1")                           // mask  = 1

                // Do pre-loop
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
                __ASM_EMIT("vmul.f32    q13, q3, q0")                           // q13   = b0*s
                __ASM_EMIT("vmul.f32    q14, q4, q0")                           // q14   = b1*s
                __ASM_EMIT("vmul.f32    q15, q5, q0")                           // q15   = b2*s
                __ASM_EMIT("vadd.f32    q0, q13, q8")                           // q0    = b0*s + d0 = s2
                __ASM_EMIT("vmla.f32    q14, q6, q0")                           // q14   = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q15, q7, q0")                           // q15   = d1' = b2*s + a2*s2
                __ASM_EMIT("vadd.f32    q14, q9")                               // q14   = d0' = d1 + b1*s + a1*s2
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vbit        q9, q15, q10")                          // q9    = (d1 & ~vmask) | (d1' & vmask)
                __ASM_EMIT("vbit        q8, q14, q10")                          // q8    = (d0 & ~vmask) | (d0' & vmask)
                __ASM_EMIT("vext.32     q0, q0, q0, #3")                        // q0    = s' = s[3] s[0] s[1] s[2]
                __ASM_EMIT("beq         6f")

                __ASM_EMIT("orr         %[mask], %[mask], LSL #1")              // mask  = (mask << 1) | 1
                __ASM_EMIT("vext.32     q10, q11, q10, #3")                     // q10   = (vmask << 1) | 1
                __ASM_EMIT("cmp         %[mask], #0x0f")
                __ASM_EMIT("bne         1b")

                // Do main loop (mask == 0x0f)
                __ASM_EMIT("5:")
                __ASM_EMIT("vld1.32     d0[0], [%[src]]!")
                __ASM_EMIT("vmul.f32    q13, q3, q0")                           // q13   = b0*s
                __ASM_EMIT("vmul.f32    q14, q4, q0")                           // q14   = b1*s
                __ASM_EMIT("vmul.f32    q15, q5, q0")                           // q15   = b2*s
                __ASM_EMIT("vadd.f32    q0, q13, q8")                           // q0    = b0*s + d0 = s2
                __ASM_EMIT("vmla.f32    q14, q6, q0")                           // q14   = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q15, q7, q0")                           // q15   = d1' = b2*s + a2*s2
                __ASM_EMIT("vadd.f32    q8, q14, q9")                           // q8    = d0' = d1 + a1*s + b1*s2
                __ASM_EMIT("vext.32     q0, q0, q0, #3")                        // q0    = s' = s[3] s[0] s[1] s[2]
                __ASM_EMIT("vmov        q9, q15")                               // q9    = d1'
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     d0[0], [%[dst]]!")
                __ASM_EMIT("bne         5b")

                // Do post-loop
                __ASM_EMIT("6:")
                __ASM_EMIT("veor        q11, q11")                              // q11    = 0
                __ASM_EMIT("lsl         %[mask], #1")                           // mask   = mask << 1
                __ASM_EMIT("vext.32     q10, q11, q10, #3")                     // q10    = (vmask << 1) | 0

                __ASM_EMIT("7:")
                __ASM_EMIT("vmul.f32    q13, q3, q0")                           // q13   = b0*s
                __ASM_EMIT("vmul.f32    q14, q4, q0")                           // q14   = b1*s
                __ASM_EMIT("vmul.f32    q15, q5, q0")                           // q15   = b2*s
                __ASM_EMIT("vadd.f32    q0, q13, q8")                           // q0    = b0*s + d0 = s2
                __ASM_EMIT("vmla.f32    q14, q6, q0")                           // q14   = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q15, q7, q0")                           // q15   = d1' = b2*s + a2*s2
                __ASM_EMIT("vadd.f32    q14, q9")                               // q14   = d0' = d1 + b1*s + a1*s2
                __ASM_EMIT("tst         %[mask], #0x08")
                __ASM_EMIT("beq         8f")
                __ASM_EMIT("vst1.32     d1[1], [%[dst]]!")
                __ASM_EMIT("8:")
                __ASM_EMIT("lsl         %[mask], #1")                           // mask  = mask << 1
                __ASM_EMIT("vbit        q9, q15, q10")                          // q9    = (d1 & ~vmask) | (d1' & vmask)
                __ASM_EMIT("vbit        q8, q14, q10")                          // q8    = (d0 & ~vmask) | (d0' & vmask)
                __ASM_EMIT("vext.32     q0, q0, q0, #3")                        // q0    = s' = s[3] s[0] s[1] s[2]
                __ASM_EMIT("tst         %[mask], #0x0f")
                __ASM_EMIT("vext.32     q10, q11, q10, #3")                     // q10   = (vmask << 1) | 0
                __ASM_EMIT("bne         7b")

                // Store memory
                __ASM_EMIT("vstm        %[d], {q8-q9}")
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [d] "r" (d), [f] "r" (f),
                  [X_MASK] "r" (&biquad_x4_mask[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q13", "q14", "q15"
            );
        }

        IF_ARCH_ARM(
            static const uint32_t biquad_x8_mask[16] __lsp_aligned16 =
            {
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                0xffffffff, 0x00000000, 0x00000000, 0x00000000,
                0x00000000, 0x00000000, 0x00000000, 0x00000000,
            };
        )

        void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f)
        {
            IF_ARCH_ARM(
                float vmask[16] __lsp_aligned16;
                size_t mask;
            );

            ARCH_ARM_ASM
            (
                __ASM_EMIT("tst         %[count], %[count]")
                __ASM_EMIT("beq         8f")

                // Prepare
                __ASM_EMIT("vldm        %[d], {q2-q5}")                         // q2-q3 = d0, q4-q5 = d1
                __ASM_EMIT("vldm        %[X_MASK], {q12-q15}")                  // q12-q15 = vmask
                __ASM_EMIT("mov         %[mask], #1")                           // mask  = 1
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")

                // Do pre-loop
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]]!")                     // q6-q7 = b0
                __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]]!")                     // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = b0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]]!")                   // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = b1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]]!")                   // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = b0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = b2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x80")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = b2*s + a2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q8, q8, q4")                            // q8    = b1*s + a1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q9, q9, q5")

                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vbit        q2, q8, q14")                           // q2    = (d0 & ~vmask) | (d0' & vmask)
                __ASM_EMIT("vbit        q3, q9, q15")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vbit        q4, q10, q14")                          // q4    = (d1 & ~vmask) | (d1' & vmask)
                __ASM_EMIT("vbit        q5, q11, q15")
                __ASM_EMIT("beq         4f")
                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("orr         %[mask], %[mask], LSL #1")              // mask  = (mask << 1) | 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = (vmask << 1) | 1
                __ASM_EMIT("cmp         %[mask], #0xff")
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")
                __ASM_EMIT("bne         1b")

                // Do main loop
                __ASM_EMIT("3:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]]!")                     // q6-q7 = b0
                __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]]!")                     // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = b0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]]!")                   // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = b1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]]!")                   // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = b0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = b2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x80")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = b2*s + a2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q2, q8, q4")                            // q2    = b1*s + a1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q3, q9, q5")
                __ASM_EMIT("vmov        q4, q10")                               // q4    = d1'
                __ASM_EMIT("vmov        q5, q11")

                __ASM_EMIT("vst1.32     d15[1], [%[dst]]!")                     // *dst++= s2[7]
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("bne         3b")

                // Do post-loop
                __ASM_EMIT("4:")
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("veor        q13, q13")                              // q13   = 0
                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("lsl         %[mask], #1")                           // mask  = mask << 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = vmask << 1
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")

                __ASM_EMIT("5:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]]!")                     // q6-q7 = b0
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]]!")                     // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = a0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]]!")                   // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = a1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]]!")                   // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = a0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = a2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x80")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = a1*s + b1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = a2*s + b2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q8, q8, q4")                            // q8    = a1*s + b1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q9, q9, q5")

                __ASM_EMIT("tst         %[mask], #0x80")
                __ASM_EMIT("beq         6f")
                __ASM_EMIT("vst1.32     d15[1], [%[dst]]!")
                __ASM_EMIT("6:")
                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vbit        q2, q8, q14")                           // q2    = (d0 & ~vmask) | (d0' & vmask)
                __ASM_EMIT("vbit        q3, q9, q15")
                __ASM_EMIT("vbit        q4, q10, q14")                          // q4    = (d1 & ~vmask) | (d1' & vmask)
                __ASM_EMIT("vbit        q5, q11, q15")

                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("lsl         %[mask], #1")                           // mask  = mask << 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = vmask << 1
                __ASM_EMIT("tst         %[mask], #0xff")                        // mask  == 0 ?
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")
                __ASM_EMIT("bne         5b")

                // Store memory
                __ASM_EMIT("6:")
                __ASM_EMIT("vstm        %[d], {q2-q5}")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [d] "r" (d), [f] "r" (f),
                  [vmask] "r" (&vmask[0]),
                  [X_MASK] "r" (&biquad_x8_mask[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f)
        {
            IF_ARCH_ARM(
                struct
                {
                    float vmask[16];
                    float *dst;
                    size_t count;
                } mem __lsp_aligned16;

                size_t mask, stride;
            );

            ARCH_ARM_ASM
            (
                __ASM_EMIT("tst         %[count], %[count]")
                __ASM_EMIT("beq         10f")

                //-------------------------------------------------------------
                // Cycle 1
                // Prepare
                __ASM_EMIT("mov         %[stride], #0x40")                      // stride = 0x40
                __ASM_EMIT("str         %[dst], [%[vmask], #0x40]")             // store dst
                __ASM_EMIT("str         %[count], [%[vmask], #0x44]")           // store counter
                __ASM_EMIT("vld1.32     {q2-q3}, [%[d]], %[stride]")            // q2-q3 = d0
                __ASM_EMIT("vld1.32     {q4-q5}, [%[d]]")                       // q4-q5 = d1
                __ASM_EMIT("vldm        %[X_MASK], {q12-q15}")                  // q12-q15 = vmask
                __ASM_EMIT("mov         %[mask], #1")                           // mask  = 1
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")
                __ASM_EMIT("sub         %[d], %[d], #0x40")

                // Do pre-loop
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]], %[stride]")           // q6-q7 = b0
                __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]], %[stride]")           // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = b0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]], %[stride]")         // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = b1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]], %[stride]")         // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = b0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = b2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x100")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = b2*s + a2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q8, q8, q4")                            // q8    = b1*s + a1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q9, q9, q5")

                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vbit        q2, q8, q14")                           // q2    = (d0 & ~vmask) | (d0' & vmask)
                __ASM_EMIT("vbit        q3, q9, q15")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vbit        q4, q10, q14")                          // q4    = (d1 & ~vmask) | (d1' & vmask)
                __ASM_EMIT("vbit        q5, q11, q15")
                __ASM_EMIT("beq         4f")
                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("orr         %[mask], %[mask], LSL #1")              // mask  = (mask << 1) | 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = (vmask << 1) | 1
                __ASM_EMIT("cmp         %[mask], #0xff")
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")
                __ASM_EMIT("bne         1b")

                // Do main loop
                __ASM_EMIT("3:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]], %[stride]")           // q6-q7 = b0
                __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]], %[stride]")           // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = b0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]], %[stride]")         // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = b1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]], %[stride]")         // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = b0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = b2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x100")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = b2*s + a2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q2, q8, q4")                            // q2    = b1*s + a1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q3, q9, q5")
                __ASM_EMIT("vmov        q4, q10")                               // q4    = d1'
                __ASM_EMIT("vmov        q5, q11")

                __ASM_EMIT("vst1.32     d15[1], [%[dst]]!")                     // *dst++= s2[7]
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("bne         3b")

                // Do post-loop
                __ASM_EMIT("4:")
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("veor        q13, q13")                              // q13   = 0
                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("lsl         %[mask], #1")                           // mask  = mask << 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = vmask << 1
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")

                __ASM_EMIT("5:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]], %[stride]")           // q6-q7 = b0
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]], %[stride]")           // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = a0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]], %[stride]")         // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = a1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]], %[stride]")         // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = a0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = a2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x100")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = a1*s + b1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = a2*s + b2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q8, q8, q4")                            // q8    = a1*s + b1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q9, q9, q5")

                __ASM_EMIT("tst         %[mask], #0x80")
                __ASM_EMIT("beq         6f")
                __ASM_EMIT("vst1.32     d15[1], [%[dst]]!")
                __ASM_EMIT("6:")
                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vbit        q2, q8, q14")                           // q2    = (d0 & ~vmask) | (d0' & vmask)
                __ASM_EMIT("vbit        q3, q9, q15")
                __ASM_EMIT("vbit        q4, q10, q14")                          // q4    = (d1 & ~vmask) | (d1' & vmask)
                __ASM_EMIT("vbit        q5, q11, q15")

                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("lsl         %[mask], #1")                           // mask  = mask << 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = vmask << 1
                __ASM_EMIT("tst         %[mask], #0xff")                        // mask  == 0 ?
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")
                __ASM_EMIT("bne         5b")

                // Store memory
                __ASM_EMIT("6:")
                __ASM_EMIT("vst1.32     {q2-q3}, [%[d]], %[stride]")
                __ASM_EMIT("vst1.32     {q4-q5}, [%[d]]")

                //-------------------------------------------------------------
                // Cycle 2
                // Prepare
                __ASM_EMIT("add         %[f], %[f], #0x20")
                __ASM_EMIT("sub         %[d], %[d], #0x20")
                __ASM_EMIT("ldr         %[dst], [%[vmask], #0x40]")             // load dst
                __ASM_EMIT("ldr         %[count], [%[vmask], #0x44]")           // load counter
                __ASM_EMIT("vld1.32     {q2-q3}, [%[d]], %[stride]")            // q2-q3 = d0
                __ASM_EMIT("vld1.32     {q4-q5}, [%[d]]")                       // q4-q5 = d1
                __ASM_EMIT("mov         %[src], %[dst]")                        // src  = dst
                __ASM_EMIT("vldm        %[X_MASK], {q12-q15}")                  // q12-q15 = vmask
                __ASM_EMIT("mov         %[mask], #1")                           // mask  = 1
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")
                __ASM_EMIT("sub         %[d], %[d], #0x40")

                // Do pre-loop
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]], %[stride]")           // q6-q7 = b0
                __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]], %[stride]")           // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = b0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]], %[stride]")         // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = b1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]], %[stride]")         // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = b0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = b2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x100")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = b2*s + a2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q8, q8, q4")                            // q8    = b1*s + a1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q9, q9, q5")

                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vbit        q2, q8, q14")                           // q2    = (d0 & ~vmask) | (d0' & vmask)
                __ASM_EMIT("vbit        q3, q9, q15")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vbit        q4, q10, q14")                          // q4    = (d1 & ~vmask) | (d1' & vmask)
                __ASM_EMIT("vbit        q5, q11, q15")
                __ASM_EMIT("beq         4f")
                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("orr         %[mask], %[mask], LSL #1")              // mask  = (mask << 1) | 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = (vmask << 1) | 1
                __ASM_EMIT("cmp         %[mask], #0xff")
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")
                __ASM_EMIT("bne         1b")

                // Do main loop
                __ASM_EMIT("3:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]], %[stride]")           // q6-q7 = b0
                __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]], %[stride]")           // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = b0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]], %[stride]")         // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = b1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]], %[stride]")         // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = b0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = b2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x100")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = b1*s + a1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = b2*s + a2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q2, q8, q4")                            // q2    = b1*s + a1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q3, q9, q5")
                __ASM_EMIT("vmov        q4, q10")                               // q4    = d1'
                __ASM_EMIT("vmov        q5, q11")

                __ASM_EMIT("vst1.32     d15[1], [%[dst]]!")                     // *dst++= s2[7]
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("bne         3b")

                // Do post-loop
                __ASM_EMIT("4:")
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("veor        q13, q13")                              // q13   = 0
                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("lsl         %[mask], #1")                           // mask  = mask << 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = vmask << 1
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")

                __ASM_EMIT("5:")
                __ASM_EMIT("vld1.32     {q6, q7}, [%[f]], %[stride]")           // q6-q7 = b0
                __ASM_EMIT("vld1.32     {q8, q9}, [%[f]], %[stride]")           // q8-q9 = b1
                __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = a0*s
                __ASM_EMIT("vmul.f32    q7, q7, q1")
                __ASM_EMIT("vld1.32     {q10, q11}, [%[f]], %[stride]")         // q10-q11 = b2
                __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = a1*s
                __ASM_EMIT("vmul.f32    q9, q9, q1")
                __ASM_EMIT("vld1.32     {q12, q13}, [%[f]], %[stride]")         // q12-q13 = a1
                __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = a0*s + d0 = s2
                __ASM_EMIT("vadd.f32    q7, q7, q3")
                __ASM_EMIT("vld1.32     {q14, q15}, [%[f]]")                    // q14-q15 = a2
                __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = a2*s
                __ASM_EMIT("vmul.f32    q11, q11, q1")
                __ASM_EMIT("sub         %[f], %[f], #0x100")
                __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = a1*s + b1*s2
                __ASM_EMIT("vmla.f32    q9, q13, q7")
                __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = a2*s + b2*s2 = d1'
                __ASM_EMIT("vmla.f32    q11, q15, q7")
                __ASM_EMIT("vadd.f32    q8, q8, q4")                            // q8    = a1*s + b1*s2 + d1 = d0'
                __ASM_EMIT("vadd.f32    q9, q9, q5")

                __ASM_EMIT("tst         %[mask], #0x80")
                __ASM_EMIT("beq         6f")
                __ASM_EMIT("vst1.32     d15[1], [%[dst]]!")
                __ASM_EMIT("6:")
                __ASM_EMIT("vext.32     q1, q6, q7, #3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
                __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
                __ASM_EMIT("vext.32     q0, q6, q6, #3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("vbit        q2, q8, q14")                           // q2    = (d0 & ~vmask) | (d0' & vmask)
                __ASM_EMIT("vbit        q3, q9, q15")
                __ASM_EMIT("vbit        q4, q10, q14")                          // q4    = (d1 & ~vmask) | (d1' & vmask)
                __ASM_EMIT("vbit        q5, q11, q15")

                __ASM_EMIT("vext.32     q15, q14, q15, #3")                     // vmask = vmask << 1
                __ASM_EMIT("lsl         %[mask], #1")                           // mask  = mask << 1
                __ASM_EMIT("vext.32     q14, q13, q14, #3")                     // vmask = vmask << 1
                __ASM_EMIT("tst         %[mask], #0xff")                        // mask  == 0 ?
                __ASM_EMIT("vstm        %[vmask], {q12-q15}")
                __ASM_EMIT("bne         5b")

                // Store memory
                __ASM_EMIT("6:")
                __ASM_EMIT("vst1.32     {q2-q3}, [%[d]], %[stride]")
                __ASM_EMIT("vst1.32     {q4-q5}, [%[d]]")

                // End
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
                  [mask] "=&r" (mask), [stride] "=&r" (stride),
                  [f] "+r" (f), [d] "+r" (d)
                : [vmask] "r" (&mem),
                  [X_MASK] "r" (&biquad_x8_mask[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }
    } /* namespace neon_d32 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_ */
