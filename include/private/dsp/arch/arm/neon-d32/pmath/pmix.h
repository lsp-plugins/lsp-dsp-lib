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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_PMIX_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_PMIX_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        void pmix_v1(float *dst, const float *src, const float *k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[dst], {q0-q3}")           // q0   = d
                __ASM_EMIT("vldm        %[src]!, {q4-q7}")          // q4   = s
                __ASM_EMIT("vldm        %[k]!, {q8-q11}")           // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vsub.f32    q6, q6, q2")
                __ASM_EMIT("vsub.f32    q7, q7, q3")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("vmla.f32    q2, q6, q10")
                __ASM_EMIT("vmla.f32    q3, q7, q11")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[dst], {q0-q1}")           // q0   = d
                __ASM_EMIT("vldm        %[src]!, {q4-q5}")          // q4   = s
                __ASM_EMIT("vldm        %[k]!, {q8-q9}")            // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[dst], {q0}")              // q0   = d
                __ASM_EMIT("vldm        %[src]!, {q4}")             // q4   = s
                __ASM_EMIT("vldm        %[k]!, {q8}")               // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[dst]]")    // q0   = d
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[src]]!")   // q4   = s
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[k]]!")   // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [k] "+r" (k),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[src1]!, {q0-q3}")         // q0   = d
                __ASM_EMIT("vldm        %[src2]!, {q4-q7}")         // q4   = s
                __ASM_EMIT("vldm        %[k]!, {q8-q11}")           // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vsub.f32    q6, q6, q2")
                __ASM_EMIT("vsub.f32    q7, q7, q3")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("vmla.f32    q2, q6, q10")
                __ASM_EMIT("vmla.f32    q3, q7, q11")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[src1]!, {q0-q1}")         // q0   = d
                __ASM_EMIT("vldm        %[src2]!, {q4-q5}")         // q4   = s
                __ASM_EMIT("vldm        %[k]!, {q8-q9}")            // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[src1]!, {q0}")            // q0   = d
                __ASM_EMIT("vldm        %[src2]!, {q4}")            // q4   = s
                __ASM_EMIT("vldm        %[k]!, {q8}")               // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src1]]!")  // q0   = d
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[src2]]!")  // q4   = s
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[k]]!")   // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [k] "+r" (k),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void pmix_k1(float *dst, const float *src, float k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[k]]")    // q8   = k
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[dst], {q0-q3}")           // q0   = d
                __ASM_EMIT("vldm        %[src]!, {q4-q7}")          // q4   = s
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vsub.f32    q6, q6, q2")
                __ASM_EMIT("vsub.f32    q7, q7, q3")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("vmla.f32    q1, q5, q8")
                __ASM_EMIT("vmla.f32    q2, q6, q8")
                __ASM_EMIT("vmla.f32    q3, q7, q8")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[dst], {q0-q1}")           // q0   = d
                __ASM_EMIT("vldm        %[src]!, {q4-q5}")          // q4   = s
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("vmla.f32    q1, q5, q8")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[dst], {q0}")              // q0   = d
                __ASM_EMIT("vldm        %[src]!, {q4}")             // q4   = s
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[dst]]")    // q0   = d
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[src]]!")   // q4   = s
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [k] "r" (&k)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8"
            );
        }

        void pmix_k2(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[k]]")    // q8   = k
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[src1]!, {q0-q3}")         // q0   = d
                __ASM_EMIT("vldm        %[src2]!, {q4-q7}")         // q4   = s
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vsub.f32    q6, q6, q2")
                __ASM_EMIT("vsub.f32    q7, q7, q3")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("vmla.f32    q1, q5, q8")
                __ASM_EMIT("vmla.f32    q2, q6, q8")
                __ASM_EMIT("vmla.f32    q3, q7, q8")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[src1]!, {q0-q1}")         // q0   = d
                __ASM_EMIT("vldm        %[src2]!, {q4-q5}")         // q4   = s
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("vmla.f32    q1, q5, q8")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[src1]!, {q0}")            // q0   = d
                __ASM_EMIT("vldm        %[src2]!, {q4}")            // q4   = s
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src1]]!")  // q0   = d
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[src2]]!")  // q4   = s
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = s - d
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = d + (s-d)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
                  [count] "+r" (count)
                : [k] "r" (&k)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8"
            );
        }
    } /* namespace neon_d32 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_PMIX_H_ */
