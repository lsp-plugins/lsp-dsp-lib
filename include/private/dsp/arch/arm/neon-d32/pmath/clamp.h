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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_CLAMP_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_CLAMP_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        void clamp_vv1(float *dst, const float *min, const float *max, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[dst], {q0-q7}")           // q0   = x
                __ASM_EMIT("vldm        %[min]!, {q8-q15}")         // q8   = min
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q9")
                __ASM_EMIT("vmax.f32    q2, q2, q10")
                __ASM_EMIT("vmax.f32    q3, q3, q11")
                __ASM_EMIT("vmax.f32    q4, q4, q12")
                __ASM_EMIT("vmax.f32    q5, q5, q13")
                __ASM_EMIT("vmax.f32    q6, q6, q14")
                __ASM_EMIT("vmax.f32    q7, q7, q15")
                __ASM_EMIT("vldm        %[max]!, {q8-q15}")         // q8   = max
                __ASM_EMIT("vmin.f32    q0, q0, q8")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q9")
                __ASM_EMIT("vmin.f32    q2, q2, q10")
                __ASM_EMIT("vmin.f32    q3, q3, q11")
                __ASM_EMIT("vmin.f32    q4, q4, q12")
                __ASM_EMIT("vmin.f32    q5, q5, q13")
                __ASM_EMIT("vmin.f32    q6, q6, q14")
                __ASM_EMIT("vmin.f32    q7, q7, q15")
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
                __ASM_EMIT("bhs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #16")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[dst], {q0-q3}")           // q0   = x
                __ASM_EMIT("vldm        %[min]!, {q8-q11}")         // q8   = min
                __ASM_EMIT("vldm        %[max]!, {q12-q15}")        // q12  = max
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q9")
                __ASM_EMIT("vmax.f32    q2, q2, q10")
                __ASM_EMIT("vmax.f32    q3, q3, q11")
                __ASM_EMIT("vmin.f32    q0, q0, q12")               // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q13")
                __ASM_EMIT("vmin.f32    q2, q2, q14")
                __ASM_EMIT("vmin.f32    q3, q3, q15")
                __ASM_EMIT("sub         %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[dst], {q0-q1}")           // q0   = x
                __ASM_EMIT("vldm        %[min]!, {q8-q9}")          // q8   = min
                __ASM_EMIT("vldm        %[max]!, {q12-q13}")        // q12  = max
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q9")
                __ASM_EMIT("vmin.f32    q0, q0, q12")               // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q13")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("vldm        %[dst], {q0}")              // q0   = x
                __ASM_EMIT("vldm        %[min]!, {q8}")             // q8   = min
                __ASM_EMIT("vldm        %[max]!, {q12}")            // q12  = max
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmin.f32    q0, q0, q12")               // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[dst]]")    // q0   = x
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[min]]!") // q8   = min
                __ASM_EMIT("vld1.32     {d24[], d25[]}, [%[max]]!") // q12  = max
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmin.f32    q0, q0, q12")               // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [min] "+r" (min), [max] "+r" (max),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        void clamp_vv2(float *dst, const float *src, const float *min, const float *max, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[src]!, {q0-q7}")          // q0   = x
                __ASM_EMIT("vldm        %[min]!, {q8-q15}")         // q8   = min
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q9")
                __ASM_EMIT("vmax.f32    q2, q2, q10")
                __ASM_EMIT("vmax.f32    q3, q3, q11")
                __ASM_EMIT("vmax.f32    q4, q4, q12")
                __ASM_EMIT("vmax.f32    q5, q5, q13")
                __ASM_EMIT("vmax.f32    q6, q6, q14")
                __ASM_EMIT("vmax.f32    q7, q7, q15")
                __ASM_EMIT("vldm        %[max]!, {q8-q15}")         // q8   = max
                __ASM_EMIT("vmin.f32    q0, q0, q8")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q9")
                __ASM_EMIT("vmin.f32    q2, q2, q10")
                __ASM_EMIT("vmin.f32    q3, q3, q11")
                __ASM_EMIT("vmin.f32    q4, q4, q12")
                __ASM_EMIT("vmin.f32    q5, q5, q13")
                __ASM_EMIT("vmin.f32    q6, q6, q14")
                __ASM_EMIT("vmin.f32    q7, q7, q15")
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
                __ASM_EMIT("bhs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #16")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[src]!, {q0-q3}")          // q0   = x
                __ASM_EMIT("vldm        %[min]!, {q8-q11}")         // q8   = min
                __ASM_EMIT("vldm        %[max]!, {q12-q15}")        // q12  = max
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q9")
                __ASM_EMIT("vmax.f32    q2, q2, q10")
                __ASM_EMIT("vmax.f32    q3, q3, q11")
                __ASM_EMIT("vmin.f32    q0, q0, q12")               // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q13")
                __ASM_EMIT("vmin.f32    q2, q2, q14")
                __ASM_EMIT("vmin.f32    q3, q3, q15")
                __ASM_EMIT("sub         %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[src]!, {q0-q1}")          // q0   = x
                __ASM_EMIT("vldm        %[min]!, {q8-q9}")          // q8   = min
                __ASM_EMIT("vldm        %[max]!, {q12-q13}")        // q12  = max
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q9")
                __ASM_EMIT("vmin.f32    q0, q0, q12")               // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q13")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("vldm        %[src]!, {q0}")             // q0   = x
                __ASM_EMIT("vldm        %[min]!, {q8}")             // q8   = min
                __ASM_EMIT("vldm        %[max]!, {q12}")            // q12  = max
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmin.f32    q0, q0, q12")               // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")   // q0   = x
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[min]]!") // q8   = min
                __ASM_EMIT("vld1.32     {d24[], d25[]}, [%[max]]!") // q12  = max
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmin.f32    q0, q0, q12")               // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [min] "+r" (min), [max] "+r" (max),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        void clamp_kk1(float *dst, float min, float max, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[pmin]]") // q8   = min
                __ASM_EMIT("vld1.32     {d18[], d19[]}, [%[pmax]]") // q9   = max
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[dst], {q0-q7}")           // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q8")
                __ASM_EMIT("vmax.f32    q2, q2, q8")
                __ASM_EMIT("vmax.f32    q3, q3, q8")
                __ASM_EMIT("vmax.f32    q4, q4, q8")
                __ASM_EMIT("vmax.f32    q5, q5, q8")
                __ASM_EMIT("vmax.f32    q6, q6, q8")
                __ASM_EMIT("vmax.f32    q7, q7, q8")
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q9")
                __ASM_EMIT("vmin.f32    q2, q2, q9")
                __ASM_EMIT("vmin.f32    q3, q3, q9")
                __ASM_EMIT("vmin.f32    q4, q4, q9")
                __ASM_EMIT("vmin.f32    q5, q5, q9")
                __ASM_EMIT("vmin.f32    q6, q6, q9")
                __ASM_EMIT("vmin.f32    q7, q7, q9")
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
                __ASM_EMIT("bhs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #16")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[dst], {q0-q3}")           // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q8")
                __ASM_EMIT("vmax.f32    q2, q2, q8")
                __ASM_EMIT("vmax.f32    q3, q3, q8")
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q9")
                __ASM_EMIT("vmin.f32    q2, q2, q9")
                __ASM_EMIT("vmin.f32    q3, q3, q9")
                __ASM_EMIT("sub         %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[dst], {q0-q1}")           // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q8")
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q9")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("vldm        %[dst], {q0}")              // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[dst]]")    // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [min] "+r" (min), [max] "+r" (max),
                  [count] "+r" (count)
                : [pmin] "r" (&min), [pmax] "r" (&max)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9"
            );
        }

        void clamp_kk2(float *dst, const float *src, float min, float max, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x32 blocks
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[pmin]]") // q8   = min
                __ASM_EMIT("vld1.32     {d18[], d19[]}, [%[pmax]]") // q9   = max
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[src]!, {q0-q7}")          // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q8")
                __ASM_EMIT("vmax.f32    q2, q2, q8")
                __ASM_EMIT("vmax.f32    q3, q3, q8")
                __ASM_EMIT("vmax.f32    q4, q4, q8")
                __ASM_EMIT("vmax.f32    q5, q5, q8")
                __ASM_EMIT("vmax.f32    q6, q6, q8")
                __ASM_EMIT("vmax.f32    q7, q7, q8")
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q9")
                __ASM_EMIT("vmin.f32    q2, q2, q9")
                __ASM_EMIT("vmin.f32    q3, q3, q9")
                __ASM_EMIT("vmin.f32    q4, q4, q9")
                __ASM_EMIT("vmin.f32    q5, q5, q9")
                __ASM_EMIT("vmin.f32    q6, q6, q9")
                __ASM_EMIT("vmin.f32    q7, q7, q9")
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
                __ASM_EMIT("bhs         1b")
                // x16 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #16")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[src]!, {q0-q3}")          // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q8")
                __ASM_EMIT("vmax.f32    q2, q2, q8")
                __ASM_EMIT("vmax.f32    q3, q3, q8")
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q9")
                __ASM_EMIT("vmin.f32    q2, q2, q9")
                __ASM_EMIT("vmin.f32    q3, q3, q9")
                __ASM_EMIT("sub         %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[src]!, {q0-q1}")          // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmax.f32    q1, q1, q8")
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vmin.f32    q1, q1, q9")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("vldm        %[src]!, {q0}")             // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")   // q0   = x
                __ASM_EMIT("vmax.f32    q0, q0, q8")                // q0   = max(x, min)
                __ASM_EMIT("vmin.f32    q0, q0, q9")                // q0   = clamp(x, min, max) = min(max(x, min), max)
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         9b")
                // end
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [min] "+r" (min), [max] "+r" (max),
                  [count] "+r" (count)
                : [pmin] "r" (&min), [pmax] "r" (&max)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9"
            );
        }
    } /* namespace neon_d32 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_CLAMP_H_ */
