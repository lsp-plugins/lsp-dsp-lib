/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 7 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_DYNAMICS_COMPRESSOR_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_DYNAMICS_COMPRESSOR_H_

#include <private/dsp/arch/arm/neon-d32/pmath/exp.h>
#include <private/dsp/arch/arm/neon-d32/pmath/log.h>

namespace lsp
{
    namespace neon_d32
    {
    #define PROCESS_KNEE_SINGLE_X8 \
        /* in: q0 = lx0, q1 = lx1 */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x0c") \
        __ASM_EMIT("vld3.f32            {d16[], d18[], d20[]}, [%[off]]")   /* q8 = herm[0] q9=herm[1] q10=herm[2] */ \
        __ASM_EMIT("vld3.f32            {d17[], d19[], d21[]}, [%[off]]")   /* q8 = herm[0] q9=herm[1] q10=herm[2] */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x18") \
        __ASM_EMIT("vld2.f32            {d22[], d24[]}, [%[off]]")          /* q11= tilt[0] q12=tilt[1] */ \
        __ASM_EMIT("vld2.f32            {d23[], d25[]}, [%[off]]")          /* q11= tilt[0] q12=tilt[1] */ \
        __ASM_EMIT("vmul.f32            q2, q0, q8")                        /* q2 = herm[0]*lx0 */ \
        __ASM_EMIT("vmul.f32            q3, q1, q8") \
        __ASM_EMIT("vadd.f32            q2, q2, q9")                        /* q2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vadd.f32            q3, q3, q9") \
        __ASM_EMIT("vmul.f32            q2, q2, q0")                        /* q2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmul.f32            q3, q3, q1") \
        __ASM_EMIT("add                 %[off], %[comp], #0x04") \
        __ASM_EMIT("vld1.f32            {d26[], d27[]}, [%[off]]")          /* q13= end */ \
        __ASM_EMIT("vmul.f32            q4, q0, q11")                       /* q4 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmul.f32            q5, q1, q11") \
        __ASM_EMIT("vadd.f32            q2, q2, q10")                       /* q2 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vadd.f32            q3, q3, q10") \
        __ASM_EMIT("vadd.f32            q0, q4, q12")                       /* q0 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vadd.f32            q1, q5, q12") \
        __ASM_EMIT("vldm                %[mem], {q6-q7}")                   /* q6 = x0 */ \
        __ASM_EMIT("vcge.f32            q6, q6, q13")                       /* q6 = [x0 >= end] */ \
        __ASM_EMIT("vcge.f32            q7, q7, q13") \
        __ASM_EMIT("vldm                %[LOG2E], {q15}") \
        __ASM_EMIT("vbif                q0, q2, q6")                        /* q0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vbif                q1, q3, q7") \
        EXP_CORE_X8                                                         /* q0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x08") \
        __ASM_EMIT("vldm                %[mem], {q6-q7}")                   /* q6 = x0 */ \
        __ASM_EMIT("vld1.f32            {d26[], d27[]}, [%[comp]]")         /* q8 = start */ \
        __ASM_EMIT("vld1.f32            {d22[], d23[]}, [%[off]]")          /* q11= gain */ \
        __ASM_EMIT("vcle.f32            q6, q6, q13")                       /* q6 = [x0 <= start] */ \
        __ASM_EMIT("vcle.f32            q7, q7, q13") \
        __ASM_EMIT("vbit                q0, q11, q6")                       /* q0 = G = [x0 <= start] ? gain : EV */ \
        __ASM_EMIT("vbit                q1, q11, q7")
        /* out: q0 = g0, q1 = g1 */

    #define PROCESS_KNEE_SINGLE_X4 \
        /* in: q0 = lx0 */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x0c") \
        __ASM_EMIT("vld3.f32            {d16[], d18[], d20[]}, [%[off]]")   /* q8 = herm[0] q9=herm[1] q10=herm[2] */ \
        __ASM_EMIT("vld3.f32            {d17[], d19[], d21[]}, [%[off]]")   /* q8 = herm[0] q9=herm[1] q10=herm[2] */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x18") \
        __ASM_EMIT("vld2.f32            {d22[], d24[]}, [%[off]]")          /* q11= tilt[0] q12=tilt[1] */ \
        __ASM_EMIT("vld2.f32            {d23[], d25[]}, [%[off]]")          /* q11= tilt[0] q12=tilt[1] */ \
        __ASM_EMIT("vmul.f32            q2, q0, q8")                        /* q2 = herm[0]*lx0 */ \
        __ASM_EMIT("vadd.f32            q2, q2, q9")                        /* q2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vmul.f32            q2, q2, q0")                        /* q2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x04") \
        __ASM_EMIT("vld1.f32            {d26[], d27[]}, [%[off]]")          /* q13= end */ \
        __ASM_EMIT("vmul.f32            q4, q0, q11")                       /* q4 = tilt[0]*lx0 */ \
        __ASM_EMIT("vadd.f32            q2, q2, q10")                       /* q2 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vadd.f32            q0, q4, q12")                       /* q0 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vldm                %[mem], {q6}")                      /* q6 = x0 */ \
        __ASM_EMIT("vcge.f32            q6, q6, q13")                       /* q6 = [x0 >= end] */ \
        __ASM_EMIT("vldm                %[LOG2E], {q15}") \
        __ASM_EMIT("vbif                q0, q2, q6")                        /* q0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X4                                                         /* q0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x08") \
        __ASM_EMIT("vldm                %[mem], {q6}")                      /* q6 = x0 */ \
        __ASM_EMIT("vld1.f32            {d26[], d27[]}, [%[comp]]")         /* q8 = start */ \
        __ASM_EMIT("vld1.f32            {d22[], d23[]}, [%[off]]")          /* q11= gain */ \
        __ASM_EMIT("vcle.f32            q6, q6, q13")                       /* q6 = [x0 <= start] */ \
        __ASM_EMIT("vbit                q0, q11, q6")                       /* q0 = G = [x0 <= start] ? gain : EV */ \
        /* out: q0 = g0 */

    #define PROCESS_COMP_FULL_X8 \
        /* in: q0 = x0, q1 = x1 */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x08") \
        __ASM_EMIT("vld1.f32            {d26[], d27[]}, [%[comp]]")     /* q13 = start[0] */ \
        __ASM_EMIT("vld1.f32            {d28[], d29[]}, [%[off]]")      /* q14 = start[1] */ \
        __ASM_EMIT("vabs.f32            q0, q0")                        /* q0  = fabsf(x0) */ \
        __ASM_EMIT("vabs.f32            q1, q1") \
        __ASM_EMIT("vcgt.f32            q2, q0, q13")                   /* q13 = [fabs(x0) > start[0]] */ \
        __ASM_EMIT("vcgt.f32            q3, q1, q13") \
        __ASM_EMIT("vcgt.f32            q4, q0, q14")                   /* q14 = [fabs(x0) > start[1]] */ \
        __ASM_EMIT("vcgt.f32            q5, q1, q14") \
        __ASM_EMIT("vorr                q2, q2, q3") \
        __ASM_EMIT("vorr                q4, q4, q5") \
        __ASM_EMIT("vorr                q2, q2, q4") \
        __ASM_EMIT("vext.32             q4, q2, q2, #2") \
        __ASM_EMIT("vorr                q2, q2, q4")  \
        __ASM_EMIT("vext.32             q4, q2, q2, #1") \
        __ASM_EMIT("vorr                q2, q2, q4") \
        __ASM_EMIT("vmov.32             %[off], d4[0]") \
        __ASM_EMIT("cmp                 %[off], #0") \
        __ASM_EMIT("bne                 100f") \
        __ASM_EMIT("add                 %[off], %[comp], #0x08") \
        __ASM_EMIT("vld1.f32            {d22[], d23[]}, [%[off]]")      /* q11= g1*/ \
        __ASM_EMIT("add                 %[off], %[comp], #0x28") \
        __ASM_EMIT("vld1.f32            {d24[], d25[]}, [%[off]]")      /* q12= g2 */ \
        __ASM_EMIT("vmul.f32            q0, q11, q12")                  /* q0 = g1*g2 */ \
        __ASM_EMIT("vmul.f32            q1, q11, q12") \
        __ASM_EMIT("b                   200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vstm                %[mem], {q0-q1}")               /* mem[0x00] = fabfs(x0) */ \
        __ASM_EMIT("vldm                %[LOGC], {q14-q15}") \
        LOGE_CORE_X8                                                    /* q0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("add                 %[off], %[mem], #0x20") \
        __ASM_EMIT("vstm                %[off], {q0-q1}")               /* mem[0x20] = lx0 */ \
        PROCESS_KNEE_SINGLE_X8                                          /* apply knee 0 */ \
        __ASM_EMIT("add                 %[off], %[mem], #0x40") \
        __ASM_EMIT("vstm                %[off], {q0-q1}")               /* mem[0x40] = g0 */ \
        __ASM_EMIT("add                 %[off], %[mem], #0x20") \
        __ASM_EMIT("vldm                %[off], {q0-q1}")               /* q0 = lx0 */ \
        __ASM_EMIT("add                 %[comp], %[comp], #0x20") \
        PROCESS_KNEE_SINGLE_X8                                          /* apply knee 1 */ \
        __ASM_EMIT("add                 %[off], %[mem], #0x40") \
        __ASM_EMIT("vldm                %[off], {q2-q3}")               /* q2 = g0 */ \
        __ASM_EMIT("sub                 %[comp], %[comp], #0x20") \
        __ASM_EMIT("vmul.f32            q0, q0, q2")                    /* q0 = G = */ \
        __ASM_EMIT("vmul.f32            q1, q1, q3") \
        __ASM_EMIT("200:") \
        /* out: q0 = G0, q1= G1 */

    #define PROCESS_COMP_FULL_X4 \
        /* in: v0 = x0 */ \
        __ASM_EMIT("vabs.f32            q0, q0")                        /* q0 = fabsf(x0) */ \
        __ASM_EMIT("vstm                %[mem], {q0}")                  /* mem[0x00] = fabfs(x0) */ \
        __ASM_EMIT("vldm                %[LOGC], {q14-q15}") \
        LOGE_CORE_X4                                                    /* q0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("add                 %[off], %[mem], #0x10") \
        __ASM_EMIT("vstm                %[off], {q0}")                  /* mem[0x10] = lx0 */ \
        PROCESS_KNEE_SINGLE_X4                                          /* apply knee 0 */ \
        __ASM_EMIT("add                 %[off], %[mem], #0x20") \
        __ASM_EMIT("vstm                %[off], {q0-q1}")               /* mem[0x20] = g0 */ \
        __ASM_EMIT("add                 %[off], %[mem], #0x10") \
        __ASM_EMIT("vldm                %[off], {q0}")                  /* q0 = lx0 */ \
        __ASM_EMIT("add                 %[comp], %[comp], #0x20") \
        PROCESS_KNEE_SINGLE_X4                                          /* apply knee 1 */ \
        __ASM_EMIT("add                 %[off], %[mem], #0x20") \
        __ASM_EMIT("vldm                %[off], {q2}")                  /* q2 = g0 */ \
        __ASM_EMIT("sub                 %[comp], %[comp], #0x20") \
        __ASM_EMIT("vmul.f32            q0, q0, q2")                    /* q0 = G = */ \
        /* out: v0 = G0 */

        void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_ARM(
                float mem[24] __lsp_aligned16;
                size_t off;
            );

            ARCH_ARM_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
                PROCESS_COMP_FULL_X8
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                __ASM_EMIT("bhs             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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
                : [comp] "r" (c),
                  [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0]),
                  [mem] "r" (&mem[0]),
                  [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_ARM(
                float mem[24] __lsp_aligned16;
                size_t off;
            );

            ARCH_ARM_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], #8")
                __ASM_EMIT("blo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]")
                PROCESS_COMP_FULL_X8
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
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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
                : [comp] "r" (c),
                  [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0]),
                  [mem] "r" (&mem[0]),
                  [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

    #undef PROCESS_COMP_FULL_X4
    #undef PROCESS_COMP_FULL_X8
    #undef PROCESS_KNEE_SINGLE_X4
    #undef PROCESS_KNEE_SINGLE_X8

    } /* namespace neon_d32 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_DYNAMICS_COMPRESSOR_H_ */
