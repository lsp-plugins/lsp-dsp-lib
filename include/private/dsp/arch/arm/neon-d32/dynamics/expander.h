/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 3 нояб. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_DYNAMICS_EXPANDER_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_DYNAMICS_EXPANDER_H_

#include <private/dsp/arch/arm/neon-d32/pmath/exp.h>
#include <private/dsp/arch/arm/neon-d32/pmath/log.h>

namespace lsp
{
    namespace neon_d32
    {
        IF_ARCH_ARM(
            static const float expander_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1.0f)
            };
        )

    #define PROCESS_UKNEE_SINGLE_X8 \
        /* in: q0 = lx0, q1 = lx1 */ \
        __ASM_EMIT("add                 %[off], %[exp], #0x10") \
        __ASM_EMIT("vld4.f32            {d16[], d18[], d20[], d22[]}, [%[exp]]")    /* q8 = start, q9=end, q10=threshold q11=herm[0] */ \
        __ASM_EMIT("vld4.f32            {d17[], d19[], d21[], d23[]}, [%[exp]]")    /* q8 = start, q9=end, q10=threshold q11=herm[0] */ \
        __ASM_EMIT("vld4.f32            {d24[], d26[], d28[], d30[]}, [%[off]]")    /* q12 = herm[1], q13=herm[2], q14=tilt[0] q15=tilt[1] */ \
        __ASM_EMIT("vld4.f32            {d25[], d27[], d29[], d31[]}, [%[off]]")    /* q12 = herm[1], q13=herm[2], q14=tilt[0] q15=tilt[1] */ \
        __ASM_EMIT("vmul.f32            q2, q0, q11")                       /* q2 = herm[0]*lx0 */ \
        __ASM_EMIT("vmul.f32            q3, q1, q11") \
        __ASM_EMIT("vmul.f32            q4, q0, q14")                       /* q4 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmul.f32            q5, q1, q14") \
        __ASM_EMIT("vadd.f32            q2, q2, q12")                       /* q2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vadd.f32            q3, q3, q12") \
        __ASM_EMIT("vldm                %[mem], {q6-q7}")                   /* q6 = x0, q7 = x1 */ \
        __ASM_EMIT("vmul.f32            q2, q2, q0")                        /* q2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmul.f32            q3, q3, q1") \
        __ASM_EMIT("vcge.f32            q6, q6, q9")                        /* q6 = [x0 >= end] */ \
        __ASM_EMIT("vcge.f32            q7, q7, q9") \
        __ASM_EMIT("vadd.f32            q2, q2, q13")                       /* q2 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vadd.f32            q3, q3, q13") \
        __ASM_EMIT("vadd.f32            q0, q4, q15")                       /* q0 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vadd.f32            q1, q5, q15") \
        __ASM_EMIT("vldm                %[LOG2E], {q15}") \
        __ASM_EMIT("vbif                q0, q2, q6")                        /* q0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vbif                q1, q3, q7") \
        EXP_CORE_X8                                                         /* q0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vld1.f32            {d20[], d21[]}, [%[exp]]")          /* q10= start */ \
        __ASM_EMIT("vldm                %[mem], {q2-q3}")                   /* q2 = x0, q3=x1 */ \
        __ASM_EMIT("vldm                %[X2C], {q11}")                     /* q11= 1.0 */ \
        __ASM_EMIT("vcle.f32            q6, q2, q10")                       /* q6 = [x0 <= start] */ \
        __ASM_EMIT("vcle.f32            q7, q3, q10") \
        __ASM_EMIT("vbit                q0, q11, q6")                       /* q0 = [x0 <= start] ? 1.0 : EV */ \
        __ASM_EMIT("vbit                q1, q11, q7") \
        /* out: q0 = g0, q1 = g1 */

    #define PROCESS_UKNEE_SINGLE_X4 \
        /* in: q0 = lx0 */ \
        __ASM_EMIT("add                 %[off], %[exp], #0x10") \
        __ASM_EMIT("vld4.f32            {d16[], d18[], d20[], d22[]}, [%[exp]]")    /* q8 = start, q9=end, q10=threshold q11=herm[0] */ \
        __ASM_EMIT("vld4.f32            {d17[], d19[], d21[], d23[]}, [%[exp]]")    /* q8 = start, q9=end, q10=threshold q11=herm[0] */ \
        __ASM_EMIT("vld4.f32            {d24[], d26[], d28[], d30[]}, [%[off]]")    /* q12 = herm[1], q13=herm[2], q14=tilt[0] q15=tilt[1] */ \
        __ASM_EMIT("vld4.f32            {d25[], d27[], d29[], d31[]}, [%[off]]")    /* q12 = herm[1], q13=herm[2], q14=tilt[0] q15=tilt[1] */ \
        __ASM_EMIT("vmul.f32            q2, q0, q11")                       /* q2 = herm[0]*lx0 */ \
        __ASM_EMIT("vmul.f32            q4, q0, q14")                       /* q4 = tilt[0]*lx0 */ \
        __ASM_EMIT("vadd.f32            q2, q2, q12")                       /* q2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vldm                %[mem], {q6}")                      /* q6 = x0, q7 = x1 */ \
        __ASM_EMIT("vmul.f32            q2, q2, q0")                        /* q2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vcge.f32            q6, q6, q9")                        /* q6 = [x0 >= end] */ \
        __ASM_EMIT("vadd.f32            q2, q2, q13")                       /* q2 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vadd.f32            q0, q4, q15")                       /* q0 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vldm                %[LOG2E], {q15}") \
        __ASM_EMIT("vbif                q0, q2, q6")                        /* q0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X4                                                         /* q0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vld1.f32            {d20[], d21[]}, [%[exp]]")          /* q10= start */ \
        __ASM_EMIT("vldm                %[mem], {q2-q3}")                   /* q2 = x0, q3=x1 */ \
        __ASM_EMIT("vldm                %[X2C], {q11}")                     /* q11= 1.0 */ \
        __ASM_EMIT("vcle.f32            q6, q2, q10")                       /* q6 = [x0 <= start] */ \
        __ASM_EMIT("vbit                q0, q11, q6")                       /* q0 = [x0 <= start] ? 1.0 : EV */ \
        /* out: q0 = g0 */

    #define PROCESS_UEXP_FULL_X8 \
        /* in: q0 = x0, q1 = x1 */ \
        __ASM_EMIT("vld3.f32            {d26[], d28[], d30[]}, [%[exp]]")   /* q13 = start, q14=end, q15=thresh */ \
        __ASM_EMIT("vld3.f32            {d27[], d29[], d31[]}, [%[exp]]")   /* q13 = start, q14=end, q15=thresh */ \
        __ASM_EMIT("vabs.f32            q0, q0")                            /* q0  = fabsf(x0) */ \
        __ASM_EMIT("vabs.f32            q1, q1") \
        __ASM_EMIT("vmin.f32            q0, q0, q15")                       /* q0  = min(fabsf(x0), thresh */ \
        __ASM_EMIT("vmin.f32            q1, q1, q15") \
        __ASM_EMIT("vcgt.f32            q2, q0, q13")                       /* q2 = [fabs(x0) > start] */ \
        __ASM_EMIT("vcgt.f32            q3, q1, q13") \
        __ASM_EMIT("vorr                q2, q2, q3")  \
        __ASM_EMIT("vext.32             q4, q2, q2, #2") \
        __ASM_EMIT("vorr                q2, q2, q4")  \
        __ASM_EMIT("vext.32             q4, q2, q2, #1") \
        __ASM_EMIT("vorr                q2, q2, q4") \
        __ASM_EMIT("vmov.32             %[off], d4[0]") \
        __ASM_EMIT("cmp                 %[off], #0") \
        __ASM_EMIT("bne                 100f") \
        __ASM_EMIT("vldm                %[X2C], {q0}")                      /* q0 = 1.0f */ \
        __ASM_EMIT("vmov                q1, q0") \
        __ASM_EMIT("b                   200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vstm                %[mem], {q0-q1}")                   /* mem[0x00] = fabfs(x0) */ \
        __ASM_EMIT("vldm                %[LOGC], {q14-q15}") \
        LOGE_CORE_X8                                                        /* q0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X8                                             /* apply knee 0 */ \
        __ASM_EMIT("200:") \
        /* out: q0 = G0, q1= G1 */

    #define PROCESS_UEXP_FULL_X4 \
        /* in: q0 = x0, q1 = x1 */ \
        __ASM_EMIT("vld3.f32            {d26[], d28[], d30[]}, [%[exp]]")   /* q13 = start, q14=end, q15=thresh */ \
        __ASM_EMIT("vld3.f32            {d27[], d29[], d31[]}, [%[exp]]")   /* q13 = start, q14=end, q15=thresh */ \
        __ASM_EMIT("vabs.f32            q0, q0")                            /* q0 = fabsf(x0) */ \
        __ASM_EMIT("vmin.f32            q0, q0, q15")                       /* q0  = min(fabsf(x0), thresh */ \
        __ASM_EMIT("vstm                %[mem], {q0}")                      /* mem[0x00] = fabfs(x0) */ \
        __ASM_EMIT("vldm                %[LOGC], {q14-q15}") \
        LOGE_CORE_X4                                                        /* q0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X4                                             /* apply knee 0 */ \
        /* out: q0 = G0, q1= G1 */

        void uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_ARM(
                float mem[16] __lsp_aligned16;
                size_t off;
            );

            ARCH_ARM_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
                PROCESS_UEXP_FULL_X8
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
                PROCESS_UEXP_FULL_X4
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
                PROCESS_UEXP_FULL_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             12f")
                __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [off] "=&r" (off)
                : [exp] "r" (c),
                  [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0]),
                  [mem] "r" (&mem[0]),
                  [X2C] "r" (&expander_const[0]),
                  [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void uexpander_x1_curve(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_ARM(
                float mem[16] __lsp_aligned16;
                size_t off;
            );

            ARCH_ARM_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]")
                PROCESS_UEXP_FULL_X8
                __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vmul.f32        q0, q0, q2")
                __ASM_EMIT("vmul.f32        q1, q1, q3")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[src]]")
                PROCESS_UEXP_FULL_X4
                __ASM_EMIT("vld1.32         {q2}, [%[src]]!")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vmul.f32        q0, q0, q2")
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
                __ASM_EMIT("vmov            q1, q0")
                PROCESS_UEXP_FULL_X4
                __ASM_EMIT("vmul.f32        q0, q0, q1")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             12f")
                __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [off] "=&r" (off)
                : [exp] "r" (c),
                  [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0]),
                  [mem] "r" (&mem[0]),
                  [X2C] "r" (&expander_const[0]),
                  [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

    #undef PROCESS_UEXP_FULL_X4
    #undef PROCESS_UEXP_FULL_X8
    #undef PROCESS_UKNEE_SINGLE_X4
    #undef PROCESS_UKNEE_SINGLE_X8

    #define PROCESS_DKNEE_SINGLE_X8 \
        /* in: q0 = lx0, q1 = lx1 */ \
        __ASM_EMIT("add                 %[off], %[exp], #0x10") \
        __ASM_EMIT("vld4.f32            {d16[], d18[], d20[], d22[]}, [%[exp]]")    /* q8 = start, q9=end, q10=threshold q11=herm[0] */ \
        __ASM_EMIT("vld4.f32            {d17[], d19[], d21[], d23[]}, [%[exp]]")    /* q8 = start, q9=end, q10=threshold q11=herm[0] */ \
        __ASM_EMIT("vld4.f32            {d24[], d26[], d28[], d30[]}, [%[off]]")    /* q12 = herm[1], q13=herm[2], q14=tilt[0] q15=tilt[1] */ \
        __ASM_EMIT("vld4.f32            {d25[], d27[], d29[], d31[]}, [%[off]]")    /* q12 = herm[1], q13=herm[2], q14=tilt[0] q15=tilt[1] */ \
        __ASM_EMIT("vmul.f32            q2, q0, q11")                       /* q2 = herm[0]*lx0 */ \
        __ASM_EMIT("vmul.f32            q3, q1, q11") \
        __ASM_EMIT("vmul.f32            q4, q0, q14")                       /* q4 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmul.f32            q5, q1, q14") \
        __ASM_EMIT("vadd.f32            q2, q2, q12")                       /* q2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vadd.f32            q3, q3, q12") \
        __ASM_EMIT("vldm                %[mem], {q6-q7}")                   /* q6 = x0, q7=x7 */ \
        __ASM_EMIT("vmul.f32            q2, q2, q0")                        /* q2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmul.f32            q3, q3, q1") \
        __ASM_EMIT("vcle.f32            q6, q6, q8")                        /* q6 = [x0 <= start] */ \
        __ASM_EMIT("vcle.f32            q7, q7, q8") \
        __ASM_EMIT("vadd.f32            q2, q2, q13")                       /* q2 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vadd.f32            q3, q3, q13") \
        __ASM_EMIT("vadd.f32            q0, q4, q15")                       /* q0 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vadd.f32            q1, q5, q15") \
        __ASM_EMIT("vldm                %[LOG2E], {q15}") \
        __ASM_EMIT("vbif                q0, q2, q6")                        /* q0 = [x0 <= start] ? TV : KV */ \
        __ASM_EMIT("vbif                q1, q3, q7") \
        EXP_CORE_X8                                                         /* q0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vld3.f32            {d20[], d22[], d24[]}, [%[exp]]")   /* q10= start, q11=end, q12=threshold */ \
        __ASM_EMIT("vld3.f32            {d21[], d23[], d25[]}, [%[exp]]")   /* q10= start, q11=end, q12=threshold */ \
        __ASM_EMIT("vldm                %[mem], {q2-q3}")                   /* q2 = x0, q3=x1 */ \
        __ASM_EMIT("vldm                %[X2C], {q13}")                     /* q13= 1.0 */ \
        __ASM_EMIT("vcge.f32            q6, q2, q11")                       /* q6 = [x0 >= end] */ \
        __ASM_EMIT("vcge.f32            q7, q3, q11") \
        __ASM_EMIT("vcge.f32            q8, q2, q12")                       /* q8 = [x0 >= threshold] */ \
        __ASM_EMIT("vcge.f32            q9, q3, q12") \
        __ASM_EMIT("vbit                q0, q13, q6")                       /* q0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vbit                q1, q13, q7") \
        __ASM_EMIT("vand                q0, q0, q8")                        /* q0 = [x0 < threshold] ? 0.0 : [x0 <= start] ? gain : EV */ \
        __ASM_EMIT("vand                q1, q1, q8") \
        /* out: q0 = g0, q1 = g1 */

    #define PROCESS_DKNEE_SINGLE_X4 \
        /* in: q0 = lx0 */ \
        __ASM_EMIT("add                 %[off], %[exp], #0x10") \
        __ASM_EMIT("vld4.f32            {d16[], d18[], d20[], d22[]}, [%[exp]]")    /* q8 = start, q9=end, q10=threshold q11=herm[0] */ \
        __ASM_EMIT("vld4.f32            {d17[], d19[], d21[], d23[]}, [%[exp]]")    /* q8 = start, q9=end, q10=threshold q11=herm[0] */ \
        __ASM_EMIT("vld4.f32            {d24[], d26[], d28[], d30[]}, [%[off]]")    /* q12 = herm[1], q13=herm[2], q14=tilt[0] q15=tilt[1] */ \
        __ASM_EMIT("vld4.f32            {d25[], d27[], d29[], d31[]}, [%[off]]")    /* q12 = herm[1], q13=herm[2], q14=tilt[0] q15=tilt[1] */ \
        __ASM_EMIT("vmul.f32            q2, q0, q11")                       /* q2 = herm[0]*lx0 */ \
        __ASM_EMIT("vmul.f32            q4, q0, q14")                       /* q4 = tilt[0]*lx0 */ \
        __ASM_EMIT("vadd.f32            q2, q2, q12")                       /* q2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vldm                %[mem], {q6}")                      /* q6 = x0 */ \
        __ASM_EMIT("vmul.f32            q2, q2, q0")                        /* q2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vcle.f32            q6, q6, q8")                        /* q6 = [x0 <= start] */ \
        __ASM_EMIT("vadd.f32            q2, q2, q13")                       /* q2 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vadd.f32            q0, q4, q15")                       /* q0 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vldm                %[LOG2E], {q15}") \
        __ASM_EMIT("vbif                q0, q2, q6")                        /* q0 = [x0 <= start] ? TV : KV */ \
        EXP_CORE_X4                                                         /* q0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vld3.f32            {d20[], d22[], d24[]}, [%[exp]]")   /* q10= start, q11=end, q12=threshold */ \
        __ASM_EMIT("vld3.f32            {d21[], d23[], d25[]}, [%[exp]]")   /* q10= start, q11=end, q12=threshold */ \
        __ASM_EMIT("vldm                %[mem], {q2}")                      /* q2 = x0 */ \
        __ASM_EMIT("vldm                %[X2C], {q13}")                     /* q13= 1.0 */ \
        __ASM_EMIT("vcge.f32            q6, q2, q11")                       /* q6 = [x0 >= end] */ \
        __ASM_EMIT("vcge.f32            q8, q2, q12")                       /* q8 = [x0 >= threshold] */ \
        __ASM_EMIT("vbit                q0, q13, q6")                       /* q0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vand                q0, q0, q8")                        /* q0 = [x0 < threshold] ? 0.0 : [x0 <= start] ? gain : EV */ \
        /* out: q0 = g0 */

    #define PROCESS_DEXP_FULL_X8 \
        /* in: q0 = x0, q1 = x1 */ \
        __ASM_EMIT("add                 %[off], %[exp], #0x4") \
        __ASM_EMIT("vld1.f32            {d26[], d27[]}, [%[off]]")          /* q13 = end */ \
        __ASM_EMIT("vabs.f32            q0, q0")                            /* q0  = fabsf(x0) */ \
        __ASM_EMIT("vabs.f32            q1, q1") \
        __ASM_EMIT("vclt.f32            q2, q0, q13")                       /* q2 = [fabs(x0) < end] */ \
        __ASM_EMIT("vclt.f32            q3, q1, q13") \
        __ASM_EMIT("vorr                q2, q2, q3") \
        __ASM_EMIT("vext.32             q4, q2, q2, #2") \
        __ASM_EMIT("vorr                q2, q2, q4")  \
        __ASM_EMIT("vext.32             q4, q2, q2, #1") \
        __ASM_EMIT("vorr                q2, q2, q4") \
        __ASM_EMIT("vmov.32             %[off], d4[0]") \
        __ASM_EMIT("cmp                 %[off], #0") \
        __ASM_EMIT("bne                 100f") \
        __ASM_EMIT("vldm                %[X2C], {q0}")                      /* q0 = 1.0f */ \
        __ASM_EMIT("vmov                q1, q0") \
        __ASM_EMIT("b                   200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vstm                %[mem], {q0-q1}")                   /* mem[0x00] = fabfs(x0) */ \
        __ASM_EMIT("vldm                %[LOGC], {q14-q15}") \
        LOGE_CORE_X8                                                        /* q0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X8                                             /* apply knee 0 */ \
        __ASM_EMIT("200:") \
        /* out: q0 = G0, q1= G1 */

    #define PROCESS_DEXP_FULL_X4 \
        /* in: q0 = x0, q1 = x1 */ \
        __ASM_EMIT("vabs.f32            q0, q0")                            /* q0 = fabsf(x0) */ \
        __ASM_EMIT("vstm                %[mem], {q0}")                      /* mem[0x00] = fabfs(x0) */ \
        __ASM_EMIT("vldm                %[LOGC], {q14-q15}") \
        LOGE_CORE_X4                                                        /* q0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X4                                             /* apply knee 0 */ \
        /* out: q0 = G0, q1= G1 */

        void dexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_ARM(
                float mem[16] __lsp_aligned16;
                size_t off;
            );

            ARCH_ARM_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
                PROCESS_DEXP_FULL_X8
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
                PROCESS_DEXP_FULL_X4
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
                PROCESS_DEXP_FULL_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             12f")
                __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [off] "=&r" (off)
                : [exp] "r" (c),
                  [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0]),
                  [mem] "r" (&mem[0]),
                  [X2C] "r" (&expander_const[0]),
                  [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void dexpander_x1_curve(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_ARM(
                float mem[16] __lsp_aligned16;
                size_t off;
            );

            ARCH_ARM_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]")
                PROCESS_DEXP_FULL_X8
                __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vmul.f32        q0, q0, q2")
                __ASM_EMIT("vmul.f32        q1, q1, q3")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[src]]")
                PROCESS_DEXP_FULL_X4
                __ASM_EMIT("vld1.32         {q2}, [%[src]]!")
                __ASM_EMIT("sub             %[count], #4")
                __ASM_EMIT("vmul.f32        q0, q0, q2")
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
                __ASM_EMIT("vmov            q1, q0")
                PROCESS_DEXP_FULL_X4
                __ASM_EMIT("vmul.f32        q0, q0, q1")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             12f")
                __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [off] "=&r" (off)
                : [exp] "r" (c),
                  [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0]),
                  [mem] "r" (&mem[0]),
                  [X2C] "r" (&expander_const[0]),
                  [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

    #undef PROCESS_DEXP_FULL_X4
    #undef PROCESS_DEXP_FULL_X8
    #undef PROCESS_DKNEE_SINGLE_X4
    #undef PROCESS_DKNEE_SINGLE_X8

    } /* namespace neon_d32 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_DYNAMICS_EXPANDER_H_ */
