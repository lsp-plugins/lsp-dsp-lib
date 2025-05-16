/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 14 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_COS_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_COS_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

#include <private/dsp/arch/arm/neon-d32/pmath/sin.h>

namespace lsp
{
    namespace neon_d32
    {
        void cosf1(float *dst, size_t count)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("vldm            %[S2C], {q8-q15}")
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[dst], {q0-q1}")
                __ASM_EMIT("vsub.f32        q0, q9, q0")                // q0   = PI/2 - x1 + PI/2
                __ASM_EMIT("vsub.f32        q1, q9, q1")                // q1   = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[dst]]")
                __ASM_EMIT("vsub.f32        q0, q9, q0")                // q0   = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("bls             12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             6f")
                __ASM_EMIT("vld1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             8f")
                __ASM_EMIT("vld1.32         {d1}, [%[dst]]")
                __ASM_EMIT("8:")
                __ASM_EMIT("vsub.f32        q0, q9, q0")                // q0   = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("sub             %[dst], #0x04")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             12f")
                __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "r" (&sinf_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void cosf2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("vldm            %[S2C], {q8-q15}")
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[src]!, {q0-q1}")
                __ASM_EMIT("vsub.f32        q0, q9, q0")                // q0   = PI/2 - x1 + PI/2
                __ASM_EMIT("vsub.f32        q1, q9, q1")                // q1   = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
                __ASM_EMIT("vsub.f32        q0, q9, q0")                // q0   = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("bls             12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             6f")
                __ASM_EMIT("vld1.32         {d0[0]}, [%[src]]!")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             8f")
                __ASM_EMIT("vld1.32         {d1}, [%[src]]")
                __ASM_EMIT("8:")
                __ASM_EMIT("vsub.f32        q0, q9, q0")                // q0   = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             12f")
                __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [S2C] "r" (&sinf_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void cosf_kp1(float *dst, float k, float p, size_t count)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("vld1.f32        {d8[], d9[]}, [%[k]]")      // q4   = k
                __ASM_EMIT("vld1.f32        {d10[], d11[]}, [%[p]]")    // q5   = p
                __ASM_EMIT("vldm            %[S2C], {q8-q15}")
                __ASM_EMIT("vldm            %[S2KP], {q6-q7}")          // q6   = i = 0 1 2 3, q7 = 4 ...
                __ASM_EMIT("vsub.f32        q5, q9, q5")                // q5   = PI/2 - p + PI/2
                // x4 blocks
                __ASM_EMIT("subs            %[count], #4")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmul.f32        q0, q6, q4")                // q0   = k * i
                __ASM_EMIT("vadd.f32        q6, q6, q7")                // q6   = i + 4
                __ASM_EMIT("vsub.f32        q0, q5, q0")                // q0   = PI/2 - (k*i + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("subs            %[count], #4")
                __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("bls             6f")
                __ASM_EMIT("vmul.f32        q0, q6, q4")                // q0   = k * i
                __ASM_EMIT("vsub.f32        q0, q5, q0")                // q0   = PI/2 - (k*i + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             4f")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("vext.32         q0, q0, q0, #1")
                __ASM_EMIT("4:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             6f")
                __ASM_EMIT("vst1.32         {d0}, [%[dst]]")
                // End
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [k] "r" (&k),
                  [p] "r" (&p),
                  [S2C] "r" (&sinf_const[0]),
                  [S2KP] "r" (&kp_gen_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

    } /* namespace neon_d32 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_COS_H_ */
