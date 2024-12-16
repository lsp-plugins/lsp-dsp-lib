/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PCOMPLEX_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PCOMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        void pcomplex_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs        %[count], #8")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("mov         %[src2], %[dst]")

                // x8 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32     {q0-q1}, [%[src1]]!")        // q0 = sr, q1 = si
                __ASM_EMIT("vld2.32     {q2-q3}, [%[src2]]!")        // q2 = dr, q3 = di
                __ASM_EMIT("vld2.32     {q4-q5}, [%[src1]]!")
                __ASM_EMIT("vld2.32     {q6-q7}, [%[src2]]!")
                __ASM_EMIT("vmul.f32    q8, q0, q2")                 // q8 = sr*dr
                __ASM_EMIT("vmul.f32    q9, q1, q2")                 // q9 = si*dr
                __ASM_EMIT("vmul.f32    q10, q4, q6")
                __ASM_EMIT("vmls.f32    q8, q1, q3")                 // q8 = sr*dr - si*di
                __ASM_EMIT("vmul.f32    q11, q5, q6")
                __ASM_EMIT("vmla.f32    q9, q0, q3")                 // q9 = si*dr + sr*di
                __ASM_EMIT("vmls.f32    q10, q5, q7")
                __ASM_EMIT("vmla.f32    q11, q4, q7")
                __ASM_EMIT("vst2.32     {q8-q9}, [%[dst]]!")
                __ASM_EMIT("subs        %[count], #8")
                __ASM_EMIT("vst2.32     {q10-q11}, [%[dst]]!")
                __ASM_EMIT("bhs         1b")

                // x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vld2.32     {q0-q1}, [%[src1]]!")        // q0 = sr, q1 = si
                __ASM_EMIT("vld2.32     {q2-q3}, [%[dst]]")          // q2 = dr, q3 = di
                __ASM_EMIT("vmul.f32    q8, q0, q2")                 // q8 = sr*dr
                __ASM_EMIT("vmul.f32    q9"
                        ", q1, q2")                 // q9 = si*dr
                __ASM_EMIT("vmls.f32    q8, q1, q3")                 // q8 = sr*dr - si*di
                __ASM_EMIT("vmla.f32    q9, q0, q3")                 // q9 = si*dr + sr*di
                __ASM_EMIT("vst2.32     {q8-q9}, [%[dst]]!")
                __ASM_EMIT("sub         %[count], #4")

                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vldm.32     %[src1]!, {s0-s1}")          // s0 = sr, q1 = si
                __ASM_EMIT("vldm.32     %[dst], {s2-s3}")            // s2 = dr, q3 = di
                __ASM_EMIT("vmul.f32    s8, s0, s2")                 // s8 = sr*dr
                __ASM_EMIT("vmul.f32    s9, s1, s2")                 // s9 = si*dr
                __ASM_EMIT("vmls.f32    s8, s1, s3")                 // s8 = sr*dr - si*di
                __ASM_EMIT("vmla.f32    s9, s0, s3")                 // s9 = si*dr + sr*di
                __ASM_EMIT("vstm.32     %[dst]!, {s8-s9}")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         5b")

                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src1] "+r" (src), [src2] "=&r" (src2),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs        %[count], #8")
                __ASM_EMIT("blo         2f")

                // x8 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32     {q0-q1}, [%[src1]]!")        // q0 = sr, q1 = si
                __ASM_EMIT("vld2.32     {q2-q3}, [%[src2]]!")        // q2 = dr, q3 = di
                __ASM_EMIT("vld2.32     {q4-q5}, [%[src1]]!")
                __ASM_EMIT("vld2.32     {q6-q7}, [%[src2]]!")
                __ASM_EMIT("vmul.f32    q8, q0, q2")                 // q8 = sr*dr
                __ASM_EMIT("vmul.f32    q9, q1, q2")                 // q9 = si*dr
                __ASM_EMIT("vmul.f32    q10, q4, q6")
                __ASM_EMIT("vmls.f32    q8, q1, q3")                 // q8 = sr*dr - si*di
                __ASM_EMIT("vmul.f32    q11, q5, q6")
                __ASM_EMIT("vmla.f32    q9, q0, q3")                 // q9 = si*dr + sr*di
                __ASM_EMIT("vmls.f32    q10, q5, q7")
                __ASM_EMIT("vmla.f32    q11, q4, q7")
                __ASM_EMIT("vst2.32     {q8-q9}, [%[dst]]!")
                __ASM_EMIT("subs        %[count], #8")
                __ASM_EMIT("vst2.32     {q10-q11}, [%[dst]]!")
                __ASM_EMIT("bhs         1b")

                // x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vld2.32     {q0-q1}, [%[src1]]!")        // q0 = sr, q1 = si
                __ASM_EMIT("vld2.32     {q2-q3}, [%[src2]]!")        // q2 = dr, q3 = di
                __ASM_EMIT("vmul.f32    q8, q0, q2")                 // q8 = sr*dr
                __ASM_EMIT("vmul.f32    q9, q1, q2")                 // q9 = si*dr
                __ASM_EMIT("vmls.f32    q8, q1, q3")                 // q8 = sr*dr - si*di
                __ASM_EMIT("vmla.f32    q9, q0, q3")                 // q9 = si*dr + sr*di
                __ASM_EMIT("vst2.32     {q8-q9}, [%[dst]]!")
                __ASM_EMIT("sub         %[count], #4")

                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vldm.32     %[src1]!, {s0-s1}")          // s0 = sr, q1 = si
                __ASM_EMIT("vldm.32     %[src2]!, {s2-s3}")          // s2 = dr, q3 = di
                __ASM_EMIT("vmul.f32    s8, s0, s2")                 // s8 = sr*dr
                __ASM_EMIT("vmul.f32    s9, s1, s2")                 // s9 = si*dr
                __ASM_EMIT("vmls.f32    s8, s1, s3")                 // s8 = sr*dr - si*di
                __ASM_EMIT("vmla.f32    s9, s0, s3")                 // s9 = si*dr + sr*di
                __ASM_EMIT("vstm.32     %[dst]!, {s8-s9}")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         5b")

                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_div2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("mov             %[src2], %[dst]")

                // x8 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")            // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {q8-q9}, [%[src2]]!")           // q8  = dr, q9 = di
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src]]!")
                __ASM_EMIT("vld2.32         {q10-q11}, [%[src2]]!")
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q2, q6, q10")
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmul.f32        q3, q6, q11")
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmla.f32        q2, q7, q11")
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmls.f32        q3, q7, q10")
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmul.f32        q6, q6, q6")
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                __ASM_EMIT("vmla.f32        q6, q7, q7")
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecpe.f32      q7, q6")
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q10, q7, q6")
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q7, q10, q7")
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vrecps.f32      q10, q7, q6")
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q6, q10, q7")
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q2, q2, q6")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vmul.f32        q3, q3, q6")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")

                // x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")            // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {q8-q9}, [%[dst]]")             // q8  = dr, q9 = di
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #4")

                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vld2.32         {d8[], d10[]}, [%[src]]!")      // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {d16[], d18[]}, [%[dst]]")      // q8  = dr, q9 = di
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vst2.32         {d0[0], d2[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             5b")

                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [src2] "=&r" (src2),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_rdiv2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("mov             %[src2], %[dst]")

                // x8 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src2]]!")           // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {q8-q9}, [%[src]]!")            // q8  = dr, q9 = di
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src2]]!")
                __ASM_EMIT("vld2.32         {q10-q11}, [%[src]]!")
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q2, q6, q10")
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmul.f32        q3, q6, q11")
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmla.f32        q2, q7, q11")
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmls.f32        q3, q7, q10")
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmul.f32        q6, q6, q6")
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                __ASM_EMIT("vmla.f32        q6, q7, q7")
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecpe.f32      q7, q6")
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q10, q7, q6")
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q7, q10, q7")
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vrecps.f32      q10, q7, q6")
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q6, q10, q7")
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q2, q2, q6")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vmul.f32        q3, q3, q6")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")

                // x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q4-q5}, [%[dst]]")             // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {q8-q9}, [%[src]]!")            // q8  = dr, q9 = di
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #4")

                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vld2.32         {d8[], d10[]}, [%[dst]]")       // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {d16[], d18[]}, [%[src]]!")     // q8  = dr, q9 = di
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vst2.32         {d0[0], d2[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             5b")

                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [src2] "=&r" (src2),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")

                // x8 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q4-q5}, [%[b]]!")              // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {q8-q9}, [%[t]]!")              // q8  = dr, q9 = di
                __ASM_EMIT("vld2.32         {q6-q7}, [%[b]]!")
                __ASM_EMIT("vld2.32         {q10-q11}, [%[t]]!")
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q2, q6, q10")
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmul.f32        q3, q6, q11")
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmla.f32        q2, q7, q11")
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmls.f32        q3, q7, q10")
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmul.f32        q6, q6, q6")
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                __ASM_EMIT("vmla.f32        q6, q7, q7")
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecpe.f32      q7, q6")
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q10, q7, q6")
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q7, q10, q7")
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vrecps.f32      q10, q7, q6")
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q6, q10, q7")
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q2, q2, q6")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vmul.f32        q3, q3, q6")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")

                // x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q4-q5}, [%[b]]!")              // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {q8-q9}, [%[t]]!")              // q8  = dr, q9 = di
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #4")

                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vld2.32         {d8[], d10[]}, [%[b]]!")        // q4  = sr, q5 = si
                __ASM_EMIT("vld2.32         {d16[], d18[]}, [%[t]]!")       // q8  = dr, q9 = di
                __ASM_EMIT("vmul.f32        q0, q4, q8")                    // q0  = sr*dr
                __ASM_EMIT("vmul.f32        q1, q4, q9")                    // q1  = sr*di
                __ASM_EMIT("vmla.f32        q0, q5, q9")                    // q0  = sr*dr + si*di
                __ASM_EMIT("vmls.f32        q1, q5, q8")                    // q1  = sr*di - si*dr
                __ASM_EMIT("vmul.f32        q4, q4, q4")                    // q4  = sr*sr
                __ASM_EMIT("vmla.f32        q4, q5, q5")                    // q4  = sr*sr + si*si
                // q4 = 1 / (sr*sr + si*si)
                __ASM_EMIT("vrecpe.f32      q5, q4")                        // q5 = x0
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q8, q5")                    // q5 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q8, q5, q4")                    // q8 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q8, q5")                    // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q0, q0, q4")
                __ASM_EMIT("vmul.f32        q1, q1, q4")
                __ASM_EMIT("vst2.32         {d0[0], d2[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             5b")

                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [b] "+r" (b), [t] "=&r" (t),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_mod(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("blo             2f")

                //-----------------------------------------------------------------
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vmul.f32        q0, q0, q0")                // q0 = r*r
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")
                __ASM_EMIT("vmul.f32        q2, q2, q2")
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src]]!")
                __ASM_EMIT("vmul.f32        q4, q4, q4")
                __ASM_EMIT("vmul.f32        q6, q6, q6")
                __ASM_EMIT("vmla.f32        q0, q1, q1")                // q0 = R = r*r + i*i
                __ASM_EMIT("vmla.f32        q2, q3, q3")
                __ASM_EMIT("vmla.f32        q4, q5, q5")
                __ASM_EMIT("vmla.f32        q6, q7, q7")

                // 1/sqrt(R) calculation
                __ASM_EMIT("vrsqrte.f32     q1, q0")                    // q1 = x0
                __ASM_EMIT("vrsqrte.f32     q3, q2")
                __ASM_EMIT("vrsqrte.f32     q5, q4")
                __ASM_EMIT("vrsqrte.f32     q7, q6")
                __ASM_EMIT("vmul.f32        q8, q1, q0")                // q8 = R * x0
                __ASM_EMIT("vmul.f32        q10, q3, q2")
                __ASM_EMIT("vmul.f32        q12, q5, q4")
                __ASM_EMIT("vmul.f32        q14, q7, q6")
                __ASM_EMIT("vrsqrts.f32     q9, q8, q1")                // q9 = (3 - R * x0 * x0) / 2
                __ASM_EMIT("vrsqrts.f32     q11, q10, q3")
                __ASM_EMIT("vrsqrts.f32     q13, q12, q5")
                __ASM_EMIT("vrsqrts.f32     q15, q14, q7")
                __ASM_EMIT("vmul.f32        q1, q1, q9")                // q1 = x1 = x0 * (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q3, q3, q11")
                __ASM_EMIT("vmul.f32        q5, q5, q13")
                __ASM_EMIT("vmul.f32        q7, q7, q15")
                __ASM_EMIT("vmul.f32        q8, q1, q0")                // q8 = R * x1
                __ASM_EMIT("vmul.f32        q10, q3, q2")
                __ASM_EMIT("vmul.f32        q12, q5, q4")
                __ASM_EMIT("vmul.f32        q14, q7, q6")
                __ASM_EMIT("vrsqrts.f32     q9, q8, q1")                // q9 = (3 - R * x1 * x1) / 2
                __ASM_EMIT("vrsqrts.f32     q11, q10, q3")
                __ASM_EMIT("vrsqrts.f32     q13, q12, q5")
                __ASM_EMIT("vrsqrts.f32     q15, q14, q7")
                __ASM_EMIT("vmul.f32        q1, q1, q9")                // q1 = 1 / sqrt(R) x2 = x1 * (3 - R * x1 * x1) / 2
                __ASM_EMIT("vmul.f32        q3, q3, q11")
                __ASM_EMIT("vmul.f32        q5, q5, q13")
                __ASM_EMIT("vmul.f32        q7, q7, q15")

                __ASM_EMIT("vmul.f32        q0, q0, q1")                // q0 = R / sqrt(R) = sqrt(R)
                __ASM_EMIT("vmul.f32        q1, q2, q3")
                __ASM_EMIT("vmul.f32        q2, q4, q5")
                __ASM_EMIT("vmul.f32        q3, q6, q7")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("vst1.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")

                //-----------------------------------------------------------------
                // x8 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vmul.f32        q0, q0, q0")                // q0 = r*r
                __ASM_EMIT("vmul.f32        q2, q2, q2")
                __ASM_EMIT("vmla.f32        q0, q1, q1")                // q0 = R = r*r + i*i
                __ASM_EMIT("vmla.f32        q2, q3, q3")
                // 1/sqrt(R) calculation
                __ASM_EMIT("vrsqrte.f32     q1, q0")                    // q1 = x0
                __ASM_EMIT("vrsqrte.f32     q3, q2")
                __ASM_EMIT("vmul.f32        q8, q1, q0")                // q8 = R * x0
                __ASM_EMIT("vmul.f32        q10, q3, q2")
                __ASM_EMIT("vrsqrts.f32     q9, q8, q1")                // q9 = (3 - R * x0 * x0) / 2
                __ASM_EMIT("vrsqrts.f32     q11, q10, q3")
                __ASM_EMIT("vmul.f32        q1, q1, q9")                // q1 = x1 = x0 * (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q3, q3, q11")
                __ASM_EMIT("vmul.f32        q8, q1, q0")                // q8 = R * x1
                __ASM_EMIT("vmul.f32        q10, q3, q2")
                __ASM_EMIT("vrsqrts.f32     q9, q8, q1")                // q9 = (3 - R * x1 * x1) / 2
                __ASM_EMIT("vrsqrts.f32     q11, q10, q3")
                __ASM_EMIT("vmul.f32        q1, q1, q9")                // q1 = 1 / sqrt(R) x2 = x1 * (3 - R * x1 * x1) / 2
                __ASM_EMIT("vmul.f32        q3, q3, q11")

                // 1 / (1/sqrt(R)) = sqrt(R) calculation
                __ASM_EMIT("vmul.f32        q0, q0, q1")                // q0 = R / sqrt(R) = sqrt(R)
                __ASM_EMIT("vmul.f32        q1, q2, q3")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")

                //-----------------------------------------------------------------
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vmul.f32        q0, q0, q0")                // q0 = r*r
                __ASM_EMIT("vmla.f32        q0, q1, q1")                // q0 = R = r*r + i*i
                // 1/sqrt(R) calculation
                __ASM_EMIT("vrsqrte.f32     q1, q0")                    // q1 = x0
                __ASM_EMIT("vmul.f32        q2, q1, q0")                // q2 = R * x0
                __ASM_EMIT("vrsqrts.f32     q3, q2, q1")                // q3 = (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q1, q1, q3")                // q1 = x1 = x0 * (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q2, q1, q0")                // q2 = R * x1
                __ASM_EMIT("vrsqrts.f32     q3, q2, q1")                // q3 = (3 - R * x1 * x1) / 2
                __ASM_EMIT("vmul.f32        q1, q1, q3")                // q0 = x2 = x1 * (3 - R * x1 * x1) / 2
                // 1 / (1/sqrt(R)) = sqrt(R) calculation
                __ASM_EMIT("vmul.f32        q0, q1")                    // q0 = R / sqrt(R) = sqrt(R)
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")

                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vldm.32     %[src]!, {s0-s1}")
                __ASM_EMIT("vmul.f32    s0, s0, s0")                    // s0 = r*r
                __ASM_EMIT("vmla.f32    s0, s1, s1")                    // s0 = r*r + i*i
                __ASM_EMIT("vsqrt.f32   s0, s0")                        // s0 = sqrt(r*r + i*i)
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vstm.32     %[dst]!, {s0}")
                __ASM_EMIT("bge         7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        void pcomplex_rcp1(float *dst, size_t count)
        {
            IF_ARCH_ARM(float *src);

            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")

                //-----------------------------------------------------------------
                // x8 blocks
                __ASM_EMIT("mov             %[src], %[dst]")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
                __ASM_EMIT("vmul.f32        q5, q2, q2")
                __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
                __ASM_EMIT("vmla.f32        q5, q3, q3")
                // 1 / R calculation
                __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
                __ASM_EMIT("vrecpe.f32      q8, q5")
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q9, q8, q5")
                __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q8, q9, q8")
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
                __ASM_EMIT("vrecps.f32      q9, q8, q5")
                __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q9, q8")
                // r/R, -i/R
                __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
                __ASM_EMIT("vneg.f32        q3, q3")
                __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
                __ASM_EMIT("vmul.f32        q2, q2, q5")
                __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
                __ASM_EMIT("vmul.f32        q3, q3, q5")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")

                //-----------------------------------------------------------------
                // x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[dst]]")         // q0 = r, q1 = i
                __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
                __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
                // 1 / R calculation
                __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
                // r/R, -i/R
                __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
                __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
                __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")

                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vldm.32         %[dst], {s0-s1}")           // s0 = r, s1 = i
                __ASM_EMIT("vmul.f32        s4, s0, s0")                // s4 = r*r
                __ASM_EMIT("vmla.f32        s4, s1, s1")                // s4 = R = r*r + i*i
                __ASM_EMIT("vneg.f32        s1, s1")                    // s1 = -i
                __ASM_EMIT("vdiv.f32        s0, s0, s4")                // s0 = r / R
                __ASM_EMIT("vdiv.f32        s1, s1, s4")                // s1 = -i / R
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("vstm.32         %[dst]!, {s0-s1}")
                __ASM_EMIT("bge             5b")

                __ASM_EMIT("6:")

                : [src] "=&r" (src), [dst] "+r" (dst), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_rcp2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")

                //-----------------------------------------------------------------
                // x8 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
                __ASM_EMIT("vmul.f32        q5, q2, q2")
                __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
                __ASM_EMIT("vmla.f32        q5, q3, q3")
                // 1 / R calculation
                __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
                __ASM_EMIT("vrecpe.f32      q8, q5")
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q9, q8, q5")
                __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q8, q9, q8")
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
                __ASM_EMIT("vrecps.f32      q9, q8, q5")
                __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q9, q8")
                // r/R, -i/R
                __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
                __ASM_EMIT("vneg.f32        q3, q3")
                __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
                __ASM_EMIT("vmul.f32        q2, q2, q5")
                __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
                __ASM_EMIT("vmul.f32        q3, q3, q5")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")

                //-----------------------------------------------------------------
                // x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
                __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
                // 1 / R calculation
                __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
                // r/R, -i/R
                __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
                __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
                __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")

                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vldm.32         %[src]!, {s0-s1}")          // s0 = r, s1 = i
                __ASM_EMIT("vmul.f32        s4, s0, s0")                // s4 = r*r
                __ASM_EMIT("vmla.f32        s4, s1, s1")                // s4 = R = r*r + i*i
                __ASM_EMIT("vneg.f32        s1, s1")                    // s1 = -i
                __ASM_EMIT("vdiv.f32        s0, s0, s4")                // s0 = r / R
                __ASM_EMIT("vdiv.f32        s1, s1, s4")                // s1 = -i / R
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("vstm.32         %[dst]!, {s0-s1}")
                __ASM_EMIT("bge             5b")

                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_c2r(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs            %[count], #32")
                __ASM_EMIT("blo             2f")

                // x32 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")
                __ASM_EMIT("vmov            q1, q2")
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src]]!")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vmov            q2, q4")
                __ASM_EMIT("vld2.32         {q8-q9}, [%[src]]!")
                __ASM_EMIT("vmov            q3, q6")
                __ASM_EMIT("vld2.32         {q10-q11}, [%[src]]!")
                __ASM_EMIT("vst1.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vmov            q4, q8")
                __ASM_EMIT("vld2.32         {q12-q13}, [%[src]]!")
                __ASM_EMIT("vmov            q5, q10")
                __ASM_EMIT("vld2.32         {q14-q15}, [%[src]]!")
                __ASM_EMIT("vst1.32         {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vmov            q6, q12")
                __ASM_EMIT("vmov            q7, q14")
                __ASM_EMIT("subs            %[count], #32")
                __ASM_EMIT("vst1.32         {q6-q7}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")

                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #16")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")
                __ASM_EMIT("vmov            q1, q2")
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src]]!")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vmov            q2, q4")
                __ASM_EMIT("vmov            q3, q6")
                __ASM_EMIT("sub             %[count], #16")
                __ASM_EMIT("vst1.32         {q2-q3}, [%[dst]]!")

                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vmov            q1, q2")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")

                // x4 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")

                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vldm.32         %[src]!, {s0, s1}")         // s0 = r, s1 = i
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("vstm.32         %[dst]!, {s0}")
                __ASM_EMIT("bge             9b")
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        void pcomplex_r2c(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs            %[count], #32")
                __ASM_EMIT("blo             2f")

                // x32 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = r
                __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
                __ASM_EMIT("vld1.32         {q6-q7}, [%[src]]!")
                __ASM_EMIT("veor            q15, q15")
                __ASM_EMIT("veor            q13, q13")
                __ASM_EMIT("veor            q11, q11")
                __ASM_EMIT("veor            q9, q9")
                __ASM_EMIT("vmov            q14, q7")
                __ASM_EMIT("vmov            q12, q6")
                __ASM_EMIT("veor            q7, q7")
                __ASM_EMIT("vmov            q10, q5")
                __ASM_EMIT("vmov            q8, q4")
                __ASM_EMIT("veor            q5, q5")
                __ASM_EMIT("vmov            q6, q3")
                __ASM_EMIT("vmov            q4, q2")
                __ASM_EMIT("veor            q3, q3")
                __ASM_EMIT("vmov            q2, q1")
                __ASM_EMIT("veor            q1, q1")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q6-q7}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q8-q9}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q10-q11}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q12-q13}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q14-q15}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #32")
                __ASM_EMIT("bhs             1b")

                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #16")
                __ASM_EMIT("blt             4f")

                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = r
                __ASM_EMIT("veor            q7, q7")
                __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("vmov            q6, q3")
                __ASM_EMIT("veor            q5, q5")
                __ASM_EMIT("vmov            q4, q2")
                __ASM_EMIT("veor            q3, q3")
                __ASM_EMIT("vmov            q2, q1")
                __ASM_EMIT("veor            q1, q1")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q6-q7}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #16")

                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = r
                __ASM_EMIT("veor            q3, q3")
                __ASM_EMIT("vmov            q2, q1")
                __ASM_EMIT("vmov            q1, q2")
                __ASM_EMIT("veor            q1, q1")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")

                // x4 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("vld1.32         {q0}, [%[src]]!")           // q0 = r
                __ASM_EMIT("vmov            q2, q1")
                __ASM_EMIT("veor            q1, q1")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")

                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             10f")
                __ASM_EMIT("veor            q0, q0")
                __ASM_EMIT("9:")
                __ASM_EMIT("vldm.32         %[src]!, {s0}")         // s0 = r, s1 = i
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("vstm.32         %[dst]!, {s0, s1}")
                __ASM_EMIT("bge             9b")
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        void pcomplex_r2c_add2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("mov             %[src2], %[dst]")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src2]]!")       // q4 = r2, q5 = i2
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src2]]!")       // q6 = r3, q7 = i3
                __ASM_EMIT("vldm            %[src]!, {d16-d23}")        // q8-q11 = s
                __ASM_EMIT("vadd.f32        q0, q0, q8")
                __ASM_EMIT("vadd.f32        q2, q2, q9")
                __ASM_EMIT("vadd.f32        q4, q4, q10")
                __ASM_EMIT("vadd.f32        q6, q6, q11")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q6-q7}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vldm            %[src]!, {d16-d19}")        // q8-q9 = s
                __ASM_EMIT("vadd.f32        q0, q0, q8")
                __ASM_EMIT("vadd.f32        q2, q2, q9")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vldm            %[src]!, {d16-d17}")        // q8 = s
                __ASM_EMIT("vadd.f32        q0, q0, q8")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld2.32         {d0[], d1[]}, [%[src2]]!")  // d0 = r0, d1 = i0
                __ASM_EMIT("vld1.32         {d2[]}, [%[src]]!")         // d2 = s
                __ASM_EMIT("vadd.f32        d0, d0, d2")
                __ASM_EMIT("vst2.32         {d0[0], d1[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [src2] "=&r" (src2), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_r2c_sub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("mov             %[src2], %[dst]")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src2]]!")       // q4 = r2, q5 = i2
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src2]]!")       // q6 = r3, q7 = i3
                __ASM_EMIT("vldm            %[src]!, {d16-d23}")        // q8-q11 = s
                __ASM_EMIT("vsub.f32        q0, q0, q8")
                __ASM_EMIT("vsub.f32        q2, q2, q9")
                __ASM_EMIT("vsub.f32        q4, q4, q10")
                __ASM_EMIT("vsub.f32        q6, q6, q11")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q6-q7}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vldm            %[src]!, {d16-d19}")        // q8-q9 = s
                __ASM_EMIT("vsub.f32        q0, q0, q8")
                __ASM_EMIT("vsub.f32        q2, q2, q9")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vldm            %[src]!, {d16-d17}")        // q8 = s
                __ASM_EMIT("vsub.f32        q0, q0, q8")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld2.32         {d0[], d1[]}, [%[src2]]!")  // d0 = r0, d1 = i0
                __ASM_EMIT("vld1.32         {d2[]}, [%[src]]!")         // d2 = s
                __ASM_EMIT("vsub.f32        d0, d0, d2")
                __ASM_EMIT("vst2.32         {d0[0], d1[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [src2] "=&r" (src2), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_r2c_rsub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("mov             %[src2], %[dst]")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src2]]!")       // q4 = r2, q5 = i2
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src2]]!")       // q6 = r3, q7 = i3
                __ASM_EMIT("vldm            %[src]!, {d16-d23}")        // q8-q11 = s
                __ASM_EMIT("vsub.f32        q0, q8, q0")
                __ASM_EMIT("vneg.f32        q1, q1")
                __ASM_EMIT("vsub.f32        q2, q9, q2")
                __ASM_EMIT("vneg.f32        q3, q3")
                __ASM_EMIT("vsub.f32        q4, q10, q4")
                __ASM_EMIT("vneg.f32        q5, q5")
                __ASM_EMIT("vsub.f32        q6, q11, q6")
                __ASM_EMIT("vneg.f32        q7, q7")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q6-q7}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vldm            %[src]!, {d16-d19}")        // q8-q9 = s
                __ASM_EMIT("vsub.f32        q0, q8, q0")
                __ASM_EMIT("vneg.f32        q1, q1")
                __ASM_EMIT("vsub.f32        q2, q9, q2")
                __ASM_EMIT("vneg.f32        q3, q3")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vldm            %[src]!, {d16-d17}")        // q8 = s
                __ASM_EMIT("vsub.f32        q0, q8, q0")
                __ASM_EMIT("vneg.f32        q1, q1")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld2.32         {d0[], d1[]}, [%[src2]]!")  // d0 = r0, d1 = i0
                __ASM_EMIT("vld1.32         {d2[]}, [%[src]]!")         // d2 = s
                __ASM_EMIT("vsub.f32        d0, d2, d0")
                __ASM_EMIT("vneg.f32        d1, d1")
                __ASM_EMIT("vst2.32         {d0[0], d1[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [src2] "=&r" (src2), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_r2c_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("mov             %[src2], %[dst]")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src2]]!")       // q4 = r2, q5 = i2
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src2]]!")       // q6 = r3, q7 = i3
                __ASM_EMIT("vldm            %[src]!, {d16-d23}")        // q8-q11 = s
                __ASM_EMIT("vmul.f32        q0, q0, q8")
                __ASM_EMIT("vmul.f32        q2, q2, q9")
                __ASM_EMIT("vmul.f32        q4, q4, q10")
                __ASM_EMIT("vmul.f32        q6, q6, q11")
                __ASM_EMIT("vmul.f32        q1, q1, q8")
                __ASM_EMIT("vmul.f32        q3, q3, q9")
                __ASM_EMIT("vmul.f32        q5, q5, q10")
                __ASM_EMIT("vmul.f32        q7, q7, q11")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q6-q7}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vldm            %[src]!, {d16-d19}")        // q8-q9 = s
                __ASM_EMIT("vmul.f32        q0, q0, q8")
                __ASM_EMIT("vmul.f32        q2, q2, q9")
                __ASM_EMIT("vmul.f32        q1, q1, q8")
                __ASM_EMIT("vmul.f32        q3, q3, q9")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vldm            %[src]!, {d16-d17}")        // q8 = s
                __ASM_EMIT("vmul.f32        q0, q0, q8")
                __ASM_EMIT("vmul.f32        q1, q1, q8")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld2.32         {d0[], d1[]}, [%[src2]]!")  // d0 = r0, d1 = i0
                __ASM_EMIT("vld1.32         {d2[]}, [%[src]]!")         // d2 = s
                __ASM_EMIT("vmul.f32        d0, d0, d2")
                __ASM_EMIT("vmul.f32        d1, d1, d2")
                __ASM_EMIT("vst2.32         {d0[0], d1[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [src2] "=&r" (src2), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_r2c_div2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("mov             %[src2], %[dst]")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[src]!, {d16-d23}")        // q8-q11 = s
                __ASM_EMIT("vrecpe.f32      q0, q8")                    // q0 = s2
                __ASM_EMIT("vrecpe.f32      q1, q9")
                __ASM_EMIT("vrecpe.f32      q2, q10")
                __ASM_EMIT("vrecpe.f32      q3, q11")
                __ASM_EMIT("vrecps.f32      q4, q0, q8")                // q4 = (2 - R*s2)
                __ASM_EMIT("vrecps.f32      q5, q1, q9")
                __ASM_EMIT("vrecps.f32      q6, q2, q10")
                __ASM_EMIT("vrecps.f32      q7, q3, q11")
                __ASM_EMIT("vmul.f32        q0, q4, q0")                // q0 = s2' = s2 * (2 - R*s2)
                __ASM_EMIT("vmul.f32        q1, q5, q1")
                __ASM_EMIT("vmul.f32        q2, q6, q2")
                __ASM_EMIT("vmul.f32        q3, q7, q3")
                __ASM_EMIT("vrecps.f32      q4, q0, q8")                // q4 = (2 - R*s2')
                __ASM_EMIT("vrecps.f32      q5, q1, q9")
                __ASM_EMIT("vrecps.f32      q6, q2, q10")
                __ASM_EMIT("vrecps.f32      q7, q3, q11")
                __ASM_EMIT("vmul.f32        q8, q4, q0")                // q8 = s2" = s2' * (2 - R*s2) = 1/s2
                __ASM_EMIT("vmul.f32        q9, q5, q1")
                __ASM_EMIT("vmul.f32        q10, q6, q2")
                __ASM_EMIT("vmul.f32        q11, q7, q3")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src2]]!")       // q4 = r2, q5 = i2
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src2]]!")       // q6 = r3, q7 = i3
                __ASM_EMIT("vmul.f32        q0, q0, q8")
                __ASM_EMIT("vmul.f32        q2, q2, q9")
                __ASM_EMIT("vmul.f32        q4, q4, q10")
                __ASM_EMIT("vmul.f32        q6, q6, q11")
                __ASM_EMIT("vmul.f32        q1, q1, q8")
                __ASM_EMIT("vmul.f32        q3, q3, q9")
                __ASM_EMIT("vmul.f32        q5, q5, q10")
                __ASM_EMIT("vmul.f32        q7, q7, q11")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vst2.32         {q6-q7}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #16")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldm            %[src]!, {d16-d19}")        // q8-q9 = s
                __ASM_EMIT("vrecpe.f32      q0, q8")                    // q0 = s2
                __ASM_EMIT("vrecpe.f32      q1, q9")
                __ASM_EMIT("vrecps.f32      q4, q0, q8")                // q4 = (2 - R*s2)
                __ASM_EMIT("vrecps.f32      q5, q1, q9")
                __ASM_EMIT("vmul.f32        q0, q4, q0")                // q0 = s2' = s2 * (2 - R*s2)
                __ASM_EMIT("vmul.f32        q1, q5, q1")
                __ASM_EMIT("vrecps.f32      q4, q0, q8")                // q4 = (2 - R*s2')
                __ASM_EMIT("vrecps.f32      q5, q1, q9")
                __ASM_EMIT("vmul.f32        q8, q4, q0")                // q8 = s2" = s2' * (2 - R*s2) = 1/s2
                __ASM_EMIT("vmul.f32        q9, q5, q1")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       // q2 = r1, q3 = i1
                __ASM_EMIT("vmul.f32        q0, q0, q8")
                __ASM_EMIT("vmul.f32        q2, q2, q9")
                __ASM_EMIT("vmul.f32        q1, q1, q8")
                __ASM_EMIT("vmul.f32        q3, q3, q9")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub             %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vldm            %[src]!, {d16-d17}")        // q8 = s
                __ASM_EMIT("vrecpe.f32      q0, q8")                    // q0 = s2
                __ASM_EMIT("vrecps.f32      q4, q0, q8")                // q4 = (2 - R*s2)
                __ASM_EMIT("vmul.f32        q0, q4, q0")                // q0 = s2' = s2 * (2 - R*s2)
                __ASM_EMIT("vrecps.f32      q4, q0, q8")                // q4 = (2 - R*s2')
                __ASM_EMIT("vmul.f32        q8, q4, q0")                // q8 = s2" = s2' * (2 - R*s2) = 1/s2
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r0, q1 = i0
                __ASM_EMIT("vmul.f32        q0, q0, q8")
                __ASM_EMIT("vmul.f32        q1, q1, q8")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32         {d8[]}, [%[src]]!")         // d2 = s
                __ASM_EMIT("vrecpe.f32      d0, d8")                    // d0 = s2
                __ASM_EMIT("vrecps.f32      d4, d0, d8")                // d4 = (2 - R*s2)
                __ASM_EMIT("vmul.f32        d0, d4, d0")                // d0 = s2' = s2 * (2 - R*s2)
                __ASM_EMIT("vrecps.f32      d4, d0, d8")                // d4 = (2 - R*s2')
                __ASM_EMIT("vmul.f32        d8, d4, d0")                // d8 = s2" = s2' * (2 - R*s2) = 1/s2
                __ASM_EMIT("vld2.32         {d0[], d1[]}, [%[src2]]!")  // d0 = r0, d1 = i0
                __ASM_EMIT("vmul.f32        d0, d0, d8")
                __ASM_EMIT("vmul.f32        d1, d1, d8")
                __ASM_EMIT("vst2.32         {d0[0], d1[0]}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("bge             7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [src2] "=&r" (src2), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pcomplex_r2c_rdiv2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_ARM(float *src2);

            ARCH_ARM_ASM
            (
                // x8 blocks
                __ASM_EMIT("mov             %[src2], %[dst]")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src2]]!")       // q0 = r, q1 = i
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")
                __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
                __ASM_EMIT("vmul.f32        q5, q2, q2")
                __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
                __ASM_EMIT("vmla.f32        q5, q3, q3")
                // 1 / R calculation
                __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
                __ASM_EMIT("vrecpe.f32      q8, q5")
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q9, q8, q5")
                __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q8, q9, q8")
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
                __ASM_EMIT("vrecps.f32      q9, q8, q5")
                __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
                __ASM_EMIT("vmul.f32        q5, q9, q8")
                // r/R, -i/R
                __ASM_EMIT("vldm            %[src]!, {d16-d19}")        // q8-q9 = s
                __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
                __ASM_EMIT("vneg.f32        q3, q3")
                __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
                __ASM_EMIT("vmul.f32        q2, q2, q5")
                __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
                __ASM_EMIT("vmul.f32        q3, q3, q5")
                __ASM_EMIT("vmul.f32        q0, q0, q8")                // q0 = (r*s) / R
                __ASM_EMIT("vmul.f32        q2, q2, q9")
                __ASM_EMIT("vmul.f32        q1, q1, q8")                // q1 = -(i*s) / R
                __ASM_EMIT("vmul.f32        q3, q3, q9")

                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")

                //-----------------------------------------------------------------
                // x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[dst]]")         // q0 = r, q1 = i
                __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
                __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
                // 1 / R calculation
                __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
                // r/R, -i/R
                __ASM_EMIT("vldm            %[src]!, {d16-d17}")        // q8 = s
                __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
                __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
                __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
                __ASM_EMIT("vmul.f32        q0, q0, q8")                // q0 = (r*s) / R
                __ASM_EMIT("vmul.f32        q1, q1, q8")                // q1 = -(i*s) / R
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")

                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vld2.32         {d0[], d1[]}, [%[dst]]")    // d0 = r, d1 = i
                __ASM_EMIT("vmul.f32        d4, d0, d0")                // d4 = r*r
                __ASM_EMIT("vmla.f32        d4, d1, d1")                // d4 = R = r*r + i*i
                // 1 / R calculation
                __ASM_EMIT("vrecpe.f32      d6, d4")                    // d6 = x0
                __ASM_EMIT("vrecps.f32      d7, d6, d4")                // d7 = (2 - R*x0)
                __ASM_EMIT("vmul.f32        d6, d7, d6")                // d6 = x1 = x0 * (2 - R*x0)
                __ASM_EMIT("vrecps.f32      d7, d6, d4")                // d7 = (2 - R*x1)
                __ASM_EMIT("vmul.f32        d4, d7, d6")                // d4 = x2 = x1 * (2 - R*x0)
                // r/R, -i/R
                __ASM_EMIT("vld1.32         {d8[]}, [%[src]]!")         // d8 = s
                __ASM_EMIT("vneg.f32        d1, d1")                    // q1 = -i
                __ASM_EMIT("vmul.f32        d0, d0, d4")                // q0 = r / R
                __ASM_EMIT("vmul.f32        d1, d1, d4")                // q1 = -i / R
                __ASM_EMIT("vmul.f32        d0, d0, d8")                // q0 = (r*s) / R
                __ASM_EMIT("vmul.f32        d1, d1, d8")                // q1 = -(i*s) / R
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("vst2.32         {d0[0], d1[0]}, [%[dst]]!")
                __ASM_EMIT("bge             5b")

                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [src2] "=&r" (src2), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        IF_ARCH_ARM(
            static float pcomplex_corr_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1e-36f),
                LSP_DSP_VEC4(1e-36f)
            };
        );

        void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count)
        {
            /*
             * src1[i] = a + j*b, src2[i] = c + j*d
             * den  = (a*a + b*b)*(c*c + d*d)
             * nom  = a*c + b*d
             * corr[i] = (den > threshold) ? nom / sqrt(den) : 0.0
             */
            ARCH_ARM_ASM
            (
                // x8 blocks
                __ASM_EMIT("vldm            %[CC], {d24-d31}")          // q14-q15 = threshold
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src1]]!")       /* q0   = A0, q1 = B0 */
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       /* q2   = C0, q3 = D0 */
                __ASM_EMIT("vld2.32         {q4-q5}, [%[src1]]!")       /* q4   = A1, q5 = B1 */
                __ASM_EMIT("vld2.32         {q6-q7}, [%[src2]]!")       /* q6   = C1, q7 = D1 */
                __ASM_EMIT("vmul.f32        q8, q0, q2")                /* q8   = A0*C0 */
                __ASM_EMIT("vmul.f32        q9, q4, q6")                /* q9   = A1*C1 */
                __ASM_EMIT("vmul.f32        q10, q0, q0")               /* q10  = A0*A0 */
                __ASM_EMIT("vmul.f32        q11, q4, q4")               /* q11  = A1*A1 */
                __ASM_EMIT("vmul.f32        q12, q2, q2")               /* q12  = C0*C0 */
                __ASM_EMIT("vmul.f32        q13, q6, q6")               /* q13  = C1*C1 */
                __ASM_EMIT("vmla.f32        q8, q1, q3")                /* q8   = nom0 = A0*C0 + B0*D0 */
                __ASM_EMIT("vmla.f32        q9, q5, q7")                /* q9   = nom1 = A1*C1 + B1*D1 */
                __ASM_EMIT("vmla.f32        q10, q1, q1")               /* q10  = A0*A0 + B0*B0 */
                __ASM_EMIT("vmla.f32        q11, q5, q5")               /* q11  = A1*A1 + B1*B1 */
                __ASM_EMIT("vmla.f32        q12, q3, q3")               /* q12  = C0*C0 + D0*D0 */
                __ASM_EMIT("vmla.f32        q13, q7, q7")               /* q13  = C1*C1 + D1*D1 */
                __ASM_EMIT("vmul.f32        q0, q10, q12")              /* q0   = den0 = (A0*A0 + B0*B0)*(C0*C0 + D0*D0) */
                __ASM_EMIT("vmul.f32        q1, q11, q13")              /* q1   = den1 = (A1*A1 + B1*B1)*(C1*C1 + D1*D1) */
                __ASM_EMIT("vrsqrte.f32     q2, q0")                    /* q2 = x0 */
                __ASM_EMIT("vrsqrte.f32     q3, q1")
                __ASM_EMIT("vmul.f32        q4, q2, q0")                /* q4 = R * x0 */
                __ASM_EMIT("vmul.f32        q5, q3, q1")
                __ASM_EMIT("vrsqrts.f32     q6, q4, q2")                /* q6 = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vrsqrts.f32     q7, q5, q3")
                __ASM_EMIT("vmul.f32        q2, q2, q6")                /* q2 = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32        q3, q3, q7")
                __ASM_EMIT("vmul.f32        q4, q2, q0")                /* q4 = R * x1 */
                __ASM_EMIT("vmul.f32        q5, q3, q1")
                __ASM_EMIT("vrsqrts.f32     q6, q4, q2")                /* q6 = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vrsqrts.f32     q7, q5, q3")
                __ASM_EMIT("vmul.f32        q2, q2, q6")                /* q2 = 1/den0 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vmul.f32        q3, q3, q7")                /* q3 = 1/den1 */
                __ASM_EMIT("vcge.f32        q0, q0, q14")               /* q0 = (den0 >= threshold) */
                __ASM_EMIT("vcge.f32        q1, q1, q15")               /* q1 = (den1 >= threshold) */
                __ASM_EMIT("vmul.f32        q2, q2, q8")                /* q2 = nom0/den0 */
                __ASM_EMIT("vmul.f32        q3, q3, q9")                /* q3 = nom1/den1 */
                __ASM_EMIT("vand            q0, q0, q2")                /* q0 = (den0 >= threshold) ? nom0/den0 : 0.0f */
                __ASM_EMIT("vand            q1, q1, q3")                /* q1 = (den1 >= threshold) ? nom1/den1 : 0.0f */
                __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("bhs             1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld2.32         {q0-q1}, [%[src1]]!")       /* q0   = A0, q1 = B0 */
                __ASM_EMIT("vld2.32         {q2-q3}, [%[src2]]!")       /* q2   = C0, q3 = D0 */
                __ASM_EMIT("vmul.f32        q8, q0, q2")                /* q8   = A0*C0 */
                __ASM_EMIT("vmul.f32        q10, q0, q0")               /* q10  = A0*A0 */
                __ASM_EMIT("vmul.f32        q12, q2, q2")               /* q12  = C0*C0 */
                __ASM_EMIT("vmla.f32        q8, q1, q3")                /* q8   = nom0 = A0*C0 + B0*D0 */
                __ASM_EMIT("vmla.f32        q10, q1, q1")               /* q10  = A0*A0 + B0*B0 */
                __ASM_EMIT("vmla.f32        q12, q3, q3")               /* q12  = C0*C0 + D0*D0 */
                __ASM_EMIT("vmul.f32        q0, q10, q12")              /* q0   = den0 = (A0*A0 + B0*B0)*(C0*C0 + D0*D0) */
                __ASM_EMIT("vrsqrte.f32     q2, q0")                    /* q2 = x0 */
                __ASM_EMIT("vmul.f32        q4, q2, q0")                /* q4 = R * x0 */
                __ASM_EMIT("vrsqrts.f32     q6, q4, q2")                /* q6 = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32        q2, q2, q6")                /* q2 = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32        q4, q2, q0")                /* q4 = R * x1 */
                __ASM_EMIT("vrsqrts.f32     q6, q4, q2")                /* q6 = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vmul.f32        q2, q2, q6")                /* q2 = 1/den0 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vcge.f32        q0, q0, q14")               /* q0 = (den0 >= threshold) */
                __ASM_EMIT("vmul.f32        q2, q2, q8")                /* q2 = nom0/den0 */
                __ASM_EMIT("vand            q0, q0, q2")                /* q0 = (den0 >= threshold) ? nom0/den0 : 0.0f */
                __ASM_EMIT("vstm            %[dst]!, {q0}")
                __ASM_EMIT("sub             %[count], #4")
                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vld2.32         {d0[], d1[]}, [%[src1]]!")  /* d0 = A0, d1 = B0 */
                __ASM_EMIT("vld2.32         {d2[], d3[]}, [%[src2]]!")  /* d2 = C0, d3 = D0 */
                __ASM_EMIT("vmul.f32        d8, d0, d2")                /* d8   = A0*C0 */
                __ASM_EMIT("vmul.f32        d10, d0, d0")               /* d10  = A0*A0 */
                __ASM_EMIT("vmul.f32        d12, d2, d2")               /* d12  = C0*C0 */
                __ASM_EMIT("vmla.f32        d8, d1, d3")                /* d8   = nom0 = A0*C0 + B0*D0 */
                __ASM_EMIT("vmla.f32        d10, d1, d1")               /* d10  = A0*A0 + B0*B0 */
                __ASM_EMIT("vmla.f32        d12, d3, d3")               /* d12  = C0*C0 + D0*D0 */
                __ASM_EMIT("vmul.f32        d0, d10, d12")              /* d0   = den0 = (A0*A0 + B0*B0)*(C0*C0 + D0*D0) */
                __ASM_EMIT("vrsqrte.f32     d2, d0")                    /* d2 = x0 */
                __ASM_EMIT("vmul.f32        d4, d2, d0")                /* d4 = R * x0 */
                __ASM_EMIT("vrsqrts.f32     d6, d4, d2")                /* d6 = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32        d2, d2, d6")                /* d2 = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32        d4, d2, d0")                /* d4 = R * x1 */
                __ASM_EMIT("vrsqrts.f32     d6, d4, d2")                /* d6 = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vmul.f32        d2, d2, d6")                /* d2 = 1/den0 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vcge.f32        d0, d0, d28")               /* d0 = (den0 >= threshold) */
                __ASM_EMIT("vmul.f32        d2, d2, d8")                /* d2 = nom0/den0 */
                __ASM_EMIT("vand            d0, d0, d2")                /* d0 = (den0 >= threshold) ? nom0/den0 : 0.0f */
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge             5b")

                __ASM_EMIT("6:")

                : [dst] "+r" (dst_corr), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
                : [CC] "r" (&pcomplex_corr_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }


    } /* namespace neon_d32 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PCOMPLEX_H_ */
