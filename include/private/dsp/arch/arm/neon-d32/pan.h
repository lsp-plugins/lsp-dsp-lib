/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 29 нояб. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PAN_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PAN_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        IF_ARCH_ARM(
            static const float depan_lin_const_f[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(1e-18f)
            };
        );

        void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count)
        {
            /*
                const float sl  = fabsf(l[i]);
                const float sr  = fabsf(r[i]);
                const float den = sl + sr;
                dst[i]          = (den >= 1e-18f) ? sr / den : dfl;
            */
            ARCH_ARM_ASM
            (
                __ASM_EMIT("vdup.32         q15, %y[dfl]")              /* q15  = dfl */
                __ASM_EMIT("vldm            %[CC], {q12-q13}")          /* q12-q13  = thresh */
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                /* 8x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[l]!, {q0-q1}")            /* q0-q1 = l */
                __ASM_EMIT("vldm            %[r]!, {q2-q3}")            /* q2-q3 = r */
                __ASM_EMIT("vabs.f32        q0, q0")                    /* q0   = fabsf(l) */
                __ASM_EMIT("vabs.f32        q1, q1")
                __ASM_EMIT("vabs.f32        q2, q2")                    /* q2   = fabsf(r) */
                __ASM_EMIT("vabs.f32        q3, q3")
                __ASM_EMIT("vadd.f32        q0, q0, q2")                /* q0   = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("vadd.f32        q1, q1, q3")
                __ASM_EMIT("vcge.f32        q4, q0, q12")               /* q4   = [den >= thresh] */
                __ASM_EMIT("vcge.f32        q5, q1, q13")
                __ASM_EMIT("vrecpe.f32      q6, q0")                    /* q6   = s2 */
                __ASM_EMIT("vrecpe.f32      q7, q1")
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2) */
                __ASM_EMIT("vrecps.f32      q9, q7, q1")
                __ASM_EMIT("vmul.f32        q6, q8, q6")                /* q6   = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("vmul.f32        q7, q9, q7")
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2') */
                __ASM_EMIT("vrecps.f32      q9, q7, q1")
                __ASM_EMIT("vmul.f32        q0, q8, q6")                /* q0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("vmul.f32        q1, q9, q7")
                __ASM_EMIT("vmul.f32        q0, q0, q2")                /* q0   = pan = fabsf(r) / den */
                __ASM_EMIT("vmul.f32        q1, q1, q3")
                __ASM_EMIT("vbif            q0, q15, q4")               /* q0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("vbif            q1, q15, q5")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                __ASM_EMIT("bhs             1b")
                /* 4x block */
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldm            %[l]!, {q0}")               /* q0   = l */
                __ASM_EMIT("vldm            %[r]!, {q2}")               /* q2   = r */
                __ASM_EMIT("vabs.f32        q0, q0")                    /* q0   = fabsf(l) */
                __ASM_EMIT("vabs.f32        q2, q2")                    /* q2   = fabsf(r) */
                __ASM_EMIT("vadd.f32        q0, q0, q2")                /* q0   = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("vcge.f32        q4, q0, q12")               /* q4   = [den >= thresh] */
                __ASM_EMIT("vrecpe.f32      q6, q0")                    /* q6   = s2 */
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2) */
                __ASM_EMIT("vmul.f32        q6, q8, q6")                /* q6   = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2') */
                __ASM_EMIT("vmul.f32        q0, q8, q6")                /* q0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("vmul.f32        q0, q0, q2")                /* q0   = pan = fabsf(r) / den */
                __ASM_EMIT("vbif            q0, q15, q4")               /* q0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vstm            %[dst]!, {q0}")
                /* 1x blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[l]]!")
                __ASM_EMIT("vld1.32         {d4[], d5[]}, [%[r]]!")
                __ASM_EMIT("vabs.f32        q0, q0")                    /* q0   = fabsf(l) */
                __ASM_EMIT("vabs.f32        q2, q2")                    /* q2   = fabsf(r) */
                __ASM_EMIT("vadd.f32        q0, q0, q2")                /* q0   = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("vcge.f32        q4, q0, q12")               /* q4   = [den >= thresh] */
                __ASM_EMIT("vrecpe.f32      q6, q0")                    /* q6   = s2 */
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2) */
                __ASM_EMIT("vmul.f32        q6, q8, q6")                /* q6   = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2') */
                __ASM_EMIT("vmul.f32        q0, q8, q6")                /* q0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("vmul.f32        q0, q0, q2")                /* q0   = pan = fabsf(r) / den */
                __ASM_EMIT("vbif            q0, q15, q4")               /* q0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge             5b")
                /* end */
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [l] "+r" (l), [r] "+r" (r),
                  [count] "+r" (count),
                  [dfl] "+t" (dfl)
                : [CC] "r" (&depan_lin_const_f[0])
                : "cc", "memory",
                  /* "q0", */ "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9",
                  "q12", "q13", "q14", "q15"
            );
        }

        IF_ARCH_ARM(
            static const float depan_eqpow_const_f[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(1e-36f)
            };
        );

        void depan_eqpow(float *dst, const float *l, const float *r, float dfl, size_t count)
        {
            /*
                const float sl  = l[i] * l[i];
                const float sr  = r[i] * r[i];
                const float den = sl + sr;
                dst[i]          = (den >= 1e-36f) ? sr / den : dfl;
            */
            ARCH_ARM_ASM
            (
                __ASM_EMIT("vdup.32         q15, %y[dfl]")              /* q15  = dfl */
                __ASM_EMIT("vldm            %[CC], {q12-q13}")          /* q12-q13  = thresh */
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                /* 8x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[l]!, {q0-q1}")            /* q0-q1 = l */
                __ASM_EMIT("vldm            %[r]!, {q2-q3}")            /* q2-q3 = r */
                __ASM_EMIT("vmul.f32        q0, q0, q0")                /* q0   = l*l */
                __ASM_EMIT("vmul.f32        q1, q1, q1")
                __ASM_EMIT("vmul.f32        q2, q2, q2")                /* q2   = r*r */
                __ASM_EMIT("vmul.f32        q3, q3, q3")
                __ASM_EMIT("vadd.f32        q0, q0, q2")                /* q0   = den = l*l + r*r */
                __ASM_EMIT("vadd.f32        q1, q1, q3")
                __ASM_EMIT("vcge.f32        q4, q0, q12")               /* q4   = [den >= thresh] */
                __ASM_EMIT("vcge.f32        q5, q1, q13")
                __ASM_EMIT("vrecpe.f32      q6, q0")                    /* q6   = s2 */
                __ASM_EMIT("vrecpe.f32      q7, q1")
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2) */
                __ASM_EMIT("vrecps.f32      q9, q7, q1")
                __ASM_EMIT("vmul.f32        q6, q8, q6")                /* q6   = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("vmul.f32        q7, q9, q7")
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2') */
                __ASM_EMIT("vrecps.f32      q9, q7, q1")
                __ASM_EMIT("vmul.f32        q0, q8, q6")                /* q0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("vmul.f32        q1, q9, q7")
                __ASM_EMIT("vmul.f32        q0, q0, q2")                /* q0   = pan = r*r / den */
                __ASM_EMIT("vmul.f32        q1, q1, q3")
                __ASM_EMIT("vbif            q0, q15, q4")               /* q0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("vbif            q1, q15, q5")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                __ASM_EMIT("bhs             1b")
                /* 4x block */
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldm            %[l]!, {q0}")               /* q0   = l */
                __ASM_EMIT("vldm            %[r]!, {q2}")               /* q2   = r */
                __ASM_EMIT("vmul.f32        q0, q0, q0")                /* q0   = l*l */
                __ASM_EMIT("vmul.f32        q2, q2, q2")                /* q2   = r*r */
                __ASM_EMIT("vadd.f32        q0, q0, q2")                /* q0   = den = l*l + r*r */
                __ASM_EMIT("vcge.f32        q4, q0, q12")               /* q4   = [den >= thresh] */
                __ASM_EMIT("vrecpe.f32      q6, q0")                    /* q6   = s2 */
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2) */
                __ASM_EMIT("vmul.f32        q6, q8, q6")                /* q6   = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2') */
                __ASM_EMIT("vmul.f32        q0, q8, q6")                /* q0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("vmul.f32        q0, q0, q2")                /* q0   = pan = r*r / den */
                __ASM_EMIT("vbif            q0, q15, q4")               /* q0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vstm            %[dst]!, {q0}")
                /* 1x blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[l]]!")
                __ASM_EMIT("vld1.32         {d4[], d5[]}, [%[r]]!")
                __ASM_EMIT("vmul.f32        q0, q0, q0")                /* q0   = l*l */
                __ASM_EMIT("vmul.f32        q2, q2, q2")                /* q2   = r*r */
                __ASM_EMIT("vadd.f32        q0, q0, q2")                /* q0   = den = l*l + r*r */
                __ASM_EMIT("vcge.f32        q4, q0, q12")               /* q4   = [den >= thresh] */
                __ASM_EMIT("vrecpe.f32      q6, q0")                    /* q6   = s2 */
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2) */
                __ASM_EMIT("vmul.f32        q6, q8, q6")                /* q6   = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("vrecps.f32      q8, q6, q0")                /* q8   = (2 - R*s2') */
                __ASM_EMIT("vmul.f32        q0, q8, q6")                /* q0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("vmul.f32        q0, q0, q2")                /* q0   = pan = r*r / den */
                __ASM_EMIT("vbif            q0, q15, q4")               /* q0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("subs            %[count], #1")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge             5b")
                /* end */
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [l] "+r" (l), [r] "+r" (r),
                  [count] "+r" (count),
                  [dfl] "+t" (dfl)
                : [CC] "r" (&depan_eqpow_const_f[0])
                : "cc", "memory",
                  /* "q0", */ "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9",
                  "q12", "q13", "q14", "q15"
            );
        }
    } /* namespace neon_d32 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PAN_H_ */
