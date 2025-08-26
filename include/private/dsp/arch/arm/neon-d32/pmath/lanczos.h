/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 21 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LANCZOS_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LANCZOS_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

#include <private/dsp/arch/arm/neon-d32/pmath/sin.h>

namespace lsp
{
    namespace neon_d32
    {
        IF_ARCH_ARM(
            static const uint32_t lanczos_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x38d1b717),                   // +0x00: Sinc threshold = 1e-4
                LSP_DSP_VEC4(0x3f800000),                   // +0x10: 1.0
            };
        )

        typedef struct lanczos_gen_t
        {
            float k[4];                         // +0x00: Multiplier
            float p[4];                         // +0x10: Initial phase
            float a[4];                         // +0x20: Number of lobes
            float i[4];                         // +0x30: Indices
            float t[4];                         // +0x40: Time range
            float x1[4];                        // +0x50: Computed X1
            float n[4];                         // +0x60: Numerator
            float step[4];                      // +0x70: Step
        } lanczos_gen_t;

        #define LANCZOS_GEN_FUNC_X4 \
            /* q0 = x1 */ \
            __ASM_EMIT("vmul.f32        q1, q0, q10")                   /* q1   = x2 = x1*a */ \
            __ASM_EMIT("vabs.f32        q13, q0")                       /* q13  = fabsf(x1) */ \
            __ASM_EMIT("vmul.f32        q14, q0, q1")                   /* q14  = x1*x2 */ \
            __ASM_EMIT("vrecpe.f32      q2, q14")                       /* q2   = s2 */ \
            __ASM_EMIT("vrecps.f32      q4, q2, q14")                   /* q4   = (2 - R*s2) */ \
            __ASM_EMIT("vmul.f32        q2, q4, q2")                    /* q2   = s2' = s2 * (2 - R*s2) */ \
            __ASM_EMIT("vrecps.f32      q4, q2, q14")                   /* q4   = (2 - R*s2') */ \
            __ASM_EMIT("vmul.f32        q14, q4, q2")                   /* q14  = n = 1/(x1 * x2) */  \
            __ASM_EMIT("vstm            %[state], {q8-q15}")            /* save state */ \
            __ASM_EMIT("vldm            %[S2C], {q8-q15}")              /* load sine coefficients */ \
            __ASM_EMIT("vadd.f32        q0, q0, q8")                    /* q0   = x1 + PI/2 */ \
            __ASM_EMIT("vadd.f32        q1, q1, q8")                    /* q1   = x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X8                                    /* q0   = sinf(x1), q1 = sinf(x2) */ \
            __ASM_EMIT("vldm            %[state], {q8-q15}")            /* load state */ \
            __ASM_EMIT("vldm            %[LC], {q6-q7}")                /* q6   = 1e-4, q7 = 1.0 */ \
            __ASM_EMIT("vmul.f32        q0, q0, q1")                    /* q0   = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vcge.f32        q2, q13, q6")                   /* q2   = [ fabsf(x1) >= 1e-4 ] */ \
            __ASM_EMIT("vmul.f32        q0, q0, q14")                   /* q0   = f = sinf(x1)*sinf(x2)/(x1*x2) */ \
            __ASM_EMIT("vcgt.f32        q3, q12, q13")                  /* q3   = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("vbif            q0, q7, q2")                    /* q0   = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vand            q0, q0, q3")                    /* q0   = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */

        void lanczos1(float *dst, float k, float p, float t, float a, size_t count)
        {
            IF_ARCH_ARM(
                lanczos_gen_t state __lsp_aligned16;
            );

            ARCH_ARM_ASM(
                __ASM_EMIT("vldm            %[S2KP], {q11-q12}")        // q11  = i = 0 1 2 3, q12 = 4 ...
                __ASM_EMIT("vld1.f32        {d16[], d17[]}, [%[k]]")    // q8   = k
                __ASM_EMIT("vld1.f32        {d18[], d19[]}, [%[p]]")    // q9   = p
                __ASM_EMIT("vmov            q15, q12")                  // q15  = step = 4
                __ASM_EMIT("vld1.f32        {d20[], d21[]}, [%[a]]")    // q10  = a
                __ASM_EMIT("vld1.f32        {d24[], d25[]}, [%[t]]")    // q12  = t
                // x4 blocks
                __ASM_EMIT("subs            %[count], #4")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmul.f32        q0, q8, q11")               // q0   = k * i
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11  = i' = i + 4
                __ASM_EMIT("vsub.f32        q0, q0, q9")                // q0   = k*i - p
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("subs            %[count], #4")
                __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("bls             6f")
                __ASM_EMIT("vmul.f32        q0, q8, q11")               // q0   = k * i
                __ASM_EMIT("vsub.f32        q0, q0, q9")                // q0   = k*i - p
                LANCZOS_GEN_FUNC_X4
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
                  [t] "r" (&t),
                  [a] "r" (&a),
                  [S2C] "r" (&sinf_const[0]),
                  [S2KP] "r" (&kp_gen_const[0]),
                  [LC] "r" (&lanczos_const[0]),
                  [state] "r" (&state)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

    } /* namespace neon_d32 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_LANCZOS_H_ */
