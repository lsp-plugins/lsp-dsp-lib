/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LERP_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LERP_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

#include <private/dsp/arch/arm/neon-d32/pmath/sin.h>

namespace lsp
{
    namespace neon_d32
    {

        void lerp_vvv(float *dst, const float *a, const float *b, const float *k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[a]!, {q0-q3}")            // q0   = a
                __ASM_EMIT("vldm        %[b]!, {q4-q7}")            // q4   = b
                __ASM_EMIT("vldm        %[k]!, {q8-q11}")           // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = b-a
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vsub.f32    q6, q6, q2")
                __ASM_EMIT("vsub.f32    q7, q7, q3")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = a + (b-a)*k
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
                __ASM_EMIT("vldm        %[a]!, {q0-q1}")            // q0   = a
                __ASM_EMIT("vldm        %[b]!, {q4-q5}")            // q4   = b
                __ASM_EMIT("vldm        %[k]!, {q8-q9}")            // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = b-a
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[a]!, {q0}")               // q0   = a
                __ASM_EMIT("vldm        %[b]!, {q4}")               // q4   = b
                __ASM_EMIT("vldm        %[k]!, {q8}")               // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = b-a
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]!")     // q0   = a
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[b]]!")     // q4   = b
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[k]]!")   // q8   = k
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = b-a
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [k] "+r" (k),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lerp_vvk(float *dst, const float *a, const float *b, float k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[k]]")    // q8   = k
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[a]!, {q0-q3}")            // q0   = a
                __ASM_EMIT("vldm        %[b]!, {q4-q7}")            // q4   = b
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = b-a
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vsub.f32    q6, q6, q2")
                __ASM_EMIT("vsub.f32    q7, q7, q3")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = a + (b-a)*k
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
                __ASM_EMIT("vldm        %[a]!, {q0-q1}")            // q0   = a
                __ASM_EMIT("vldm        %[b]!, {q4-q5}")            // q4   = b
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = b-a
                __ASM_EMIT("vsub.f32    q5, q5, q1")
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q5, q8")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[a]!, {q0}")               // q0   = a
                __ASM_EMIT("vldm        %[b]!, {q4}")               // q4   = b
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = b-a
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]!")     // q0   = a
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[b]]!")     // q4   = b
                __ASM_EMIT("vsub.f32    q4, q4, q0")                // q4   = b-a
                __ASM_EMIT("vmla.f32    q0, q4, q8")                // q0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [k] "r" (&k)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8"
            );
        }

        void lerp_vkv(float *dst, const float *a, float b, const float *k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[b]]")    // q8   = b
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[a]!, {q0-q3}")            // q0   = a
                __ASM_EMIT("vldm        %[k]!, {q4-q7}")            // q4   = k
                __ASM_EMIT("vsub.f32    q12, q8, q0")               // q12  = b-a
                __ASM_EMIT("vsub.f32    q13, q8, q1")
                __ASM_EMIT("vsub.f32    q14, q8, q2")
                __ASM_EMIT("vsub.f32    q15, q8, q3")
                __ASM_EMIT("vmla.f32    q0, q12, q4")               // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q13, q5")
                __ASM_EMIT("vmla.f32    q2, q14, q6")
                __ASM_EMIT("vmla.f32    q3, q15, q7")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[a]!, {q0-q1}")            // q0   = a
                __ASM_EMIT("vldm        %[k]!, {q4-q5}")            // q4   = k
                __ASM_EMIT("vsub.f32    q12, q8, q0")               // q12  = b-a
                __ASM_EMIT("vsub.f32    q13, q8, q1")
                __ASM_EMIT("vmla.f32    q0, q12, q4")               // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q13, q5")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[a]!, {q0}")               // q0   = a
                __ASM_EMIT("vldm        %[k]!, {q4}")               // q4   = k
                __ASM_EMIT("vsub.f32    q12, q8, q0")               // q12  = b-a
                __ASM_EMIT("vmla.f32    q0, q12, q4")               // q0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]!")     // q0   = a
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[k]]!")     // q4   = k
                __ASM_EMIT("vsub.f32    q12, q8, q0")               // q12  = b-a
                __ASM_EMIT("vmla.f32    q0, q12, q4")               // q0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [k] "+r" (k),
                  [count] "+r" (count)
                : [b] "r" (&b)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lerp_vkk(float *dst, const float *a, float b, float k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[b]]")    // q8   = b
                __ASM_EMIT("vld1.32     {d18[], d19[]}, [%[k]]")    // q9   = k
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[a]!, {q0-q3}")            // q0   = a
                __ASM_EMIT("vsub.f32    q4, q8, q0")                // q4   = b-a
                __ASM_EMIT("vsub.f32    q5, q8, q1")
                __ASM_EMIT("vsub.f32    q6, q8, q2")
                __ASM_EMIT("vsub.f32    q7, q8, q3")
                __ASM_EMIT("vmla.f32    q0, q4, q9")                // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("vmla.f32    q2, q6, q9")
                __ASM_EMIT("vmla.f32    q3, q7, q9")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[a]!, {q0-q1}")            // q0   = a
                __ASM_EMIT("vldm        %[k]!, {q4-q5}")            // q4   = k
                __ASM_EMIT("vsub.f32    q4, q8, q0")                // q4   = b-a
                __ASM_EMIT("vsub.f32    q5, q8, q1")
                __ASM_EMIT("vmla.f32    q0, q4, q9")                // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[a]!, {q0}")               // q0   = a
                __ASM_EMIT("vldm        %[k]!, {q4}")               // q4   = k
                __ASM_EMIT("vsub.f32    q4, q8, q0")                // q4   = b-a
                __ASM_EMIT("vmla.f32    q0, q4, q9")                // q0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]!")     // q0   = a
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[k]]!")     // q4   = k
                __ASM_EMIT("vsub.f32    q4, q8, q0")                // q4   = b-a
                __ASM_EMIT("vmla.f32    q0, q4, q9")                // q0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a),
                  [count] "+r" (count)
                : [b] "r" (&b),
                  [k] "r" (&k)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9"
            );
        }

        void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[a]]")    // q8   = a
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[b]!, {q12-q15}")          // q12  = b
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vmov        q1, q8")
                __ASM_EMIT("vmov        q2, q8")
                __ASM_EMIT("vmov        q3, q8")
                __ASM_EMIT("vldm        %[k]!, {q4-q7}")            // q4   = k
                __ASM_EMIT("vsub.f32    q12, q12, q0")              // q12  = b-a
                __ASM_EMIT("vsub.f32    q13, q13, q1")
                __ASM_EMIT("vsub.f32    q14, q14, q2")
                __ASM_EMIT("vsub.f32    q15, q15, q3")
                __ASM_EMIT("vmla.f32    q0, q12, q4")               // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q13, q5")
                __ASM_EMIT("vmla.f32    q2, q14, q6")
                __ASM_EMIT("vmla.f32    q3, q15, q7")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[b]!, {q12-q13}")          // q12  = b
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vmov        q1, q8")
                __ASM_EMIT("vldm        %[k]!, {q4-q5}")            // q4   = k
                __ASM_EMIT("vsub.f32    q12, q12, q0")              // q12  = b-a
                __ASM_EMIT("vsub.f32    q13, q13, q1")
                __ASM_EMIT("vmla.f32    q0, q12, q4")               // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q13, q5")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[b]!, {q12}")              // q12  = b
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vldm        %[k]!, {q4}")               // q4   = k
                __ASM_EMIT("vsub.f32    q12, q12, q0")              // q12  = b-a
                __ASM_EMIT("vmla.f32    q0, q12, q4")               // q0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d24[], d25[]}, [%[b]]!")   // q12  = b
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[k]]!")     // q4   = k
                __ASM_EMIT("vsub.f32    q12, q12, q0")              // q12  = b-a
                __ASM_EMIT("vmla.f32    q0, q12, q4")               // q0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [b] "+r" (b), [k] "+r" (k),
                  [count] "+r" (count)
                : [a] "r" (&a)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lerp_kvk(float *dst, float a, const float *b, float k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[a]]")    // q8   = a
                __ASM_EMIT("vld1.32     {d18[], d19[]}, [%[k]]")    // q9   = k
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[b]!, {q12-q15}")          // q12  = b
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vmov        q1, q8")
                __ASM_EMIT("vmov        q2, q8")
                __ASM_EMIT("vmov        q3, q8")
                __ASM_EMIT("vsub.f32    q12, q12, q0")              // q12  = b-a
                __ASM_EMIT("vsub.f32    q13, q13, q1")
                __ASM_EMIT("vsub.f32    q14, q14, q2")
                __ASM_EMIT("vsub.f32    q15, q15, q3")
                __ASM_EMIT("vmla.f32    q0, q12, q9")               // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q13, q9")
                __ASM_EMIT("vmla.f32    q2, q14, q9")
                __ASM_EMIT("vmla.f32    q3, q15, q9")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[b]!, {q12-q13}")          // q12  = b
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vmov        q1, q8")
                __ASM_EMIT("vsub.f32    q12, q12, q0")              // q12  = b-a
                __ASM_EMIT("vsub.f32    q13, q13, q1")
                __ASM_EMIT("vmla.f32    q0, q12, q9")               // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q13, q9")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[b]!, {q12}")              // q12  = b
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vsub.f32    q12, q12, q0")              // q12  = b-a
                __ASM_EMIT("vmla.f32    q0, q12, q9")               // q0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d24[], d25[]}, [%[b]]!")   // q12  = b
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vsub.f32    q12, q12, q0")              // q12  = b-a
                __ASM_EMIT("vmla.f32    q0, q12, q9")               // q0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [b] "+r" (b),
                  [count] "+r" (count)
                : [a] "r" (&a),
                  [k] "r" (&k)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8",
                  "q12", "q13", "q14", "q15"
            );
        }

        void lerp_kkv(float *dst, float a, float b, const float *k, size_t count)
        {
            ARCH_ARM_ASM
            (
                // x16 blocks
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[a]]")    // q8   = a
                __ASM_EMIT("vld1.32     {d18[], d19[]}, [%[b]]")    // q9   = b
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vsub.f32    q9, q9, q8")                // q9   = b-a
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[k]!, {q4-q7}")            // q4   = k
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vmov        q1, q8")
                __ASM_EMIT("vmov        q2, q8")
                __ASM_EMIT("vmov        q3, q8")
                __ASM_EMIT("vmla.f32    q0, q4, q9")                // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("vmla.f32    q2, q6, q9")
                __ASM_EMIT("vmla.f32    q3, q7, q9")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[k]!, {q4-q5}")            // q4   = k
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vmov        q1, q8")
                __ASM_EMIT("vmla.f32    q0, q4, q9")                // q0   = a + (b-a)*k
                __ASM_EMIT("vmla.f32    q1, q5, q9")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[k]!, {q4}")               // q4   = k
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vmla.f32    q0, q4, q9")                // q0   = a + (b-a)*k
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[k]]!")     // q4   = k
                __ASM_EMIT("vmov        q0, q8")                    // q0   = a
                __ASM_EMIT("vmla.f32    q0, q4, q9")                // q0   = a + (b-a)*k
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         7b")
                // end
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [k] "+r" (k),
                  [count] "+r" (count)
                : [a] "r" (&a),
                  [b] "r" (&b)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9"
            );
        }

    } /* namespace neon_d32 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LERP_H_ */
