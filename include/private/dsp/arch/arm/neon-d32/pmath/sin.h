/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 13 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SIN_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SIN_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        IF_ARCH_ARM(
            static const float kp_gen_const[] __lsp_aligned16 =
            {
                0.0f, 1.0f, 2.0f, 3.0f,         // +0x00: Initial values
                LSP_DSP_VEC4(4.0),              // +0x10: Step x4
            };

            static const uint32_t sinf_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x3fc90fdb),       // +0x000:  PI/2
                LSP_DSP_VEC4(0x40490fdb),       // +0x010:  PI
                LSP_DSP_VEC4(0x3e22f983),       // +0x020:  1/(2*PI)
                LSP_DSP_VEC4(0xb2d7322b),       // +0x030:  C0 = -1/11! = -2.50521083854e-08
                LSP_DSP_VEC4(0x3638ef1d),       // +0x040:  C1 = 1/9! = 2.7557319224e-06
                LSP_DSP_VEC4(0xb9500d01),       // +0x050:  C2 = -1/7! = -0.000198412698413
                LSP_DSP_VEC4(0x3c088889),       // +0x060:  C3 = 1/5! = 0.00833333333333
                LSP_DSP_VEC4(0xbe2aaaab),       // +0x070:  C4 = -1/3! = -0.166666666667
            };
        )

        #define SINF_X_PLUS_PI_2_CORE_X8                \
            /* q0   = X = x + PI/2 */ \
            /* q8   = PI/2 */ \
            /* q9   = PI */ \
            /* q10  = 1/(2*PI) */ \
            /* q11  = C0 */ \
            /* q12  = C1 */ \
            /* q13  = C2 */ \
            /* q14  = C3 */ \
            /* q15  = C4 */ \
            __ASM_EMIT("vmul.f32        q2, q0, q10")                   /* q2   = x / (2*PI) */ \
            __ASM_EMIT("vmul.f32        q3, q1, q10") \
            __ASM_EMIT("vcvt.s32.f32    q4, q2")                        /* q4   = int(X / (2*PI)) */ \
            __ASM_EMIT("vcvt.s32.f32    q5, q3") \
            __ASM_EMIT("vadd.f32        q6, q9, q9")                    /* q6   = 2*PI */ \
            __ASM_EMIT("vsra.s32        q4, q0, #31")                   /* q4   = int(X / (2*PI)) + [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vsra.s32        q5, q1, #31") \
            __ASM_EMIT("vcvt.f32.s32    q4, q4")                        /* q4   = period = int(X / (2*PI)) [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvt.f32.s32    q5, q5") \
            __ASM_EMIT("vmls.f32        q0, q4, q6")                    /* q0   = Y = X - period * 2 * PI */ \
            __ASM_EMIT("vmls.f32        q1, q5, q6") \
            __ASM_EMIT("vadd.f32        q6, q8, q9")                    /* q6   = 3*PI/2 */ \
            __ASM_EMIT("vcge.f32        q2, q0, q9")                    /* q2   = [ Y >= PI ] */ \
            __ASM_EMIT("vcge.f32        q3, q1, q9") \
            __ASM_EMIT("vsub.f32        q4, q6, q0")                    /* q4   = 3*PI/2 - Y */ \
            __ASM_EMIT("vsub.f32        q5, q6, q1") \
            __ASM_EMIT("vsub.f32        q0, q0, q8")                    /* q0   = Y - PI/2 */ \
            __ASM_EMIT("vsub.f32        q1, q1, q8") \
            __ASM_EMIT("vbit            q0, q4, q2")                    /* q0   = XX = [ Y < PI ] ? (Y - PI/2) : (3*PI/2 - Y) */ \
            __ASM_EMIT("vbit            q1, q5, q3") \
            /* q0   = XX */ \
            __ASM_EMIT("vmul.f32        q2, q0, q0")                    /* q2   = X2 = XX*XX */ \
            __ASM_EMIT("vmul.f32        q3, q1, q1") \
            __ASM_EMIT("vmul.f32        q4, q2, q11")                   /* q4   = X2*C0 */ \
            __ASM_EMIT("vmul.f32        q5, q3, q11") \
            __ASM_EMIT("vadd.f32        q4, q4, q12")                   /* q4   = C1 + X2*C0 */ \
            __ASM_EMIT("vadd.f32        q5, q5, q12") \
            __ASM_EMIT("vmul.f32        q4, q2, q4")                    /* q4   = X2*(C1 + X2*C0) */ \
            __ASM_EMIT("vmul.f32        q5, q3, q5") \
            __ASM_EMIT("vadd.f32        q4, q4, q13")                   /* q4   = C2 + X2*(C1 + X2*C0) */ \
            __ASM_EMIT("vadd.f32        q5, q5, q13") \
            __ASM_EMIT("vmul.f32        q4, q2, q4")                    /* q4   = X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("vmul.f32        q5, q3, q5") \
            __ASM_EMIT("vadd.f32        q4, q4, q14")                   /* q4   = C3 + X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("vadd.f32        q5, q5, q14") \
            __ASM_EMIT("vmul.f32        q4, q2, q4")                    /* q4   = X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("vmul.f32        q5, q3, q5") \
            __ASM_EMIT("vadd.f32        q4, q4, q15")                   /* q4   = C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("vadd.f32        q5, q5, q15") \
            __ASM_EMIT("vmul.f32        q4, q2, q4")                    /* q4   = X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("vmul.f32        q5, q3, q5") \
            __ASM_EMIT("vmla.f32        q0, q0, q4")                    /* q0   = XX*(1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))))) */ \
            __ASM_EMIT("vmla.f32        q1, q1, q5")

        #define SINF_X_PLUS_PI_2_CORE_X4                \
            /* q0   = X = x + PI/2 */ \
            /* q8   = PI/2 */ \
            /* q9   = PI */ \
            /* q10  = 1/(2*PI) */ \
            /* q11  = C0 */ \
            /* q12  = C1 */ \
            /* q13  = C2 */ \
            /* q14  = C3 */ \
            /* q15  = C4 */ \
            __ASM_EMIT("vmul.f32        q1, q0, q10")                   /* q1   = x / (2*PI) */ \
            __ASM_EMIT("vcvt.s32.f32    q2, q1")                        /* q2   = int(X / (2*PI)) */ \
            __ASM_EMIT("vadd.f32        q3, q9, q9")                    /* q3   = 2*PI */ \
            __ASM_EMIT("vsra.s32        q2, q0, #31")                   /* q2   = int(X / (2*PI)) + [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvt.f32.s32    q2, q2")                        /* q2   = period = int(X / (2*PI)) [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vmls.f32        q0, q2, q3")                    /* q0   = Y = X - period * 2 * PI */ \
            __ASM_EMIT("vadd.f32        q3, q8, q9")                    /* q3   = 3*PI/2 */ \
            __ASM_EMIT("vcge.f32        q1, q0, q9")                    /* q1   = [ Y >= PI ] */ \
            __ASM_EMIT("vsub.f32        q2, q3, q0")                    /* q2   = 3*PI/2 - Y */ \
            __ASM_EMIT("vsub.f32        q0, q0, q8")                    /* q0   = Y - PI/2 */ \
            __ASM_EMIT("vbit            q0, q2, q1")                    /* q0   = XX = [ Y < PI ] ? (Y - PI/2) : (3*PI/2 - Y) */ \
            /* q0   = XX */ \
            __ASM_EMIT("vmul.f32        q1, q0, q0")                    /* q1   = X2 = XX*XX */ \
            __ASM_EMIT("vmul.f32        q2, q1, q11")                   /* q2   = X2*C0 */ \
            __ASM_EMIT("vadd.f32        q2, q2, q12")                   /* q2   = C1 + X2*C0 */ \
            __ASM_EMIT("vmul.f32        q2, q1, q2")                    /* q2   = X2*(C1 + X2*C0) */ \
            __ASM_EMIT("vadd.f32        q2, q2, q13")                   /* q2   = C2 + X2*(C1 + X2*C0) */ \
            __ASM_EMIT("vmul.f32        q2, q1, q2")                    /* q2   = X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("vadd.f32        q2, q2, q14")                   /* q2   = C3 + X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("vmul.f32        q2, q1, q2")                    /* q2   = X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("vadd.f32        q2, q2, q15")                   /* q2   = C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("vmul.f32        q2, q1, q2")                    /* q2   = X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("vmla.f32        q0, q0, q2")                    /* q0   = XX*(1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))))) */

        void sinf1(float *dst, size_t count)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("vldm            %[S2C], {q8-q15}")
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[dst], {q0-q1}")
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0   = x1 + PI/2
                __ASM_EMIT("vadd.f32        q1, q1, q8")                // q1   = x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[dst]]")
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0   = x1 + PI/2
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
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0   = x1 + PI/2
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

        void sinf2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("vldm            %[S2C], {q8-q15}")
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[src]!, {q0-q1}")
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0   = x1 + PI/2
                __ASM_EMIT("vadd.f32        q1, q1, q8")                // q1   = x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0   = x1 + PI/2
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
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0   = x1 + PI/2
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

        void sinf_kp1(float *dst, float k, float p, size_t count)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("vld1.f32        {d8[], d9[]}, [%[k]]")      // q4   = k
                __ASM_EMIT("vld1.f32        {d10[], d11[]}, [%[p]]")    // q5   = p
                __ASM_EMIT("vldm            %[S2C], {q8-q15}")
                __ASM_EMIT("vldm            %[S2KP], {q6-q7}")          // q6   = i = 0 1 2 3, q7 = 4 ...
                __ASM_EMIT("vadd.f32        q5, q5, q8")                // q5   = p + PI/2
                // x4 blocks
                __ASM_EMIT("subs            %[count], #4")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmul.f32        q0, q6, q4")                // q0   = k * i
                __ASM_EMIT("vadd.f32        q6, q6, q7")                // q6   = i + 4
                __ASM_EMIT("vadd.f32        q0, q0, q5")                // q0   = k*i + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("subs            %[count], #4")
                __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("bls             6f")
                __ASM_EMIT("vmul.f32        q0, q6, q4")                // q0   = k * i
                __ASM_EMIT("vadd.f32        q0, q0, q5")                // q0   = k*i + p + PI/2
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



#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SIN_H_ */
