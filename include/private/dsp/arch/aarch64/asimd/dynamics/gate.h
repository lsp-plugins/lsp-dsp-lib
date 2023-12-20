/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_DYNAMICS_GATE_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_DYNAMICS_GATE_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <private/dsp/arch/aarch64/asimd/pmath/exp.h>
#include <private/dsp/arch/aarch64/asimd/pmath/log.h>

namespace lsp
{
    namespace asimd
    {
    #define PROCESS_KNEE_SINGLE_X8 \
        /* in: v0 = lx0, v1 = lx1 */ \
        __ASM_EMIT("add                 %[off], %[gate], #0x10") \
        __ASM_EMIT("ld4r                {v8.4s, v9.4s, v10.4s, v11.4s}, [%[off]]" ) /* v8=herm[0], v9=herm[1], v10=herm[2], v11=herm[3] */ \
        __ASM_EMIT("fmul                v2.4s, v0.4s, v8.4s")               /* v2 = herm[0]*lx0 */ \
        __ASM_EMIT("fmul                v3.4s, v1.4s, v8.4s") \
        __ASM_EMIT("fadd                v2.4s, v2.4s, v9.4s")               /* v2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("fadd                v3.4s, v3.4s, v9.4s") \
        __ASM_EMIT("fmul                v2.4s, v2.4s, v0.4s")               /* v2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("fmul                v3.4s, v3.4s, v1.4s") \
        __ASM_EMIT("fadd                v2.4s, v2.4s, v10.4s")              /* v2 = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("fadd                v3.4s, v3.4s, v10.4s") \
        __ASM_EMIT("fmul                v2.4s, v2.4s, v0.4s")               /* v2 = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0 */ \
        __ASM_EMIT("fmul                v3.4s, v3.4s, v1.4s") \
        __ASM_EMIT("fadd                v0.4s, v2.4s, v11.4s")              /* v0 = TV = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0+herm[3] */ \
        __ASM_EMIT("fadd                v1.4s, v3.4s, v11.4s") \
        EXP_CORE_X8                                                         /* v0 = KV = expf(TV) */ \
        __ASM_EMIT("ldp                 q6, q7, [%[mem], #0x00]")           /* v6 = x0 */ \
        __ASM_EMIT("ld4r                {v2.4s, v3.4s, v4.4s, v5.4s}, [%[gate]]" ) /* v2=start, v3=end, v4=gain_start, v5=gain_end */ \
        __ASM_EMIT("fcmgt               v8.4s, v6.4s, v2.4s")               /* v8 = [x0 > start] */ \
        __ASM_EMIT("fcmgt               v9.4s, v7.4s, v2.4s") \
        __ASM_EMIT("fcmge               v6.4s, v6.4s, v3.4s")               /* v6 = [x0 >= end] */ \
        __ASM_EMIT("fcmge               v7.4s, v7.4s, v3.4s") \
        __ASM_EMIT("bif                 v0.16b, v4.16b, v8.16b")            /* v0 = [x0 <= start] ? gain_start : KV */ \
        __ASM_EMIT("bif                 v1.16b, v4.16b, v9.16b") \
        __ASM_EMIT("bit                 v0.16b, v5.16b, v6.16b")            /* v0 = [x0 >= end] ? gain_end : [x0 <= start] ? gain_start : KV */ \
        __ASM_EMIT("bit                 v1.16b, v5.16b, v7.16b") \
        /* out: v0 = g0, v1 = g1 */

    #define PROCESS_KNEE_SINGLE_X4 \
        /* in: v0 = lx0 */ \
        __ASM_EMIT("add                 %[off], %[gate], #0x10") \
        __ASM_EMIT("ld4r                {v8.4s, v9.4s, v10.4s, v11.4s}, [%[off]]" ) /* v8=herm[0], v9=herm[1], v10=herm[2], v11=herm[3] */ \
        __ASM_EMIT("fmul                v2.4s, v0.4s, v8.4s")               /* v2 = herm[0]*lx0 */ \
        __ASM_EMIT("fadd                v2.4s, v2.4s, v9.4s")               /* v2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("fmul                v2.4s, v2.4s, v0.4s")               /* v2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("fadd                v2.4s, v2.4s, v10.4s")              /* v2 = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("fmul                v2.4s, v2.4s, v0.4s")               /* v2 = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0 */ \
        __ASM_EMIT("fadd                v0.4s, v2.4s, v11.4s")              /* v0 = TV = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0+herm[3] */ \
        EXP_CORE_X4                                                         /* v0 = KV = expf(TV) */ \
        __ASM_EMIT("ldp                 q6, q7, [%[mem], #0x00]")           /* v6 = x0 */ \
        __ASM_EMIT("ld4r                {v2.4s, v3.4s, v4.4s, v5.4s}, [%[gate]]" ) /* v2=start, v3=end, v4=gain_start, v5=gain_end */ \
        __ASM_EMIT("fcmgt               v8.4s, v6.4s, v2.4s")               /* v8 = [x0 > start] */ \
        __ASM_EMIT("fcmge               v6.4s, v6.4s, v3.4s")               /* v6 = [x0 >= end] */ \
        __ASM_EMIT("bif                 v0.16b, v4.16b, v8.16b")            /* v0 = [x0 <= start] ? gain_start : KV */ \
        __ASM_EMIT("bit                 v0.16b, v5.16b, v6.16b")            /* v0 = [x0 >= end] ? gain_end : [x0 <= start] ? gain_start : KV */ \
        /* out: v0 = g0 */

    #define PROCESS_GATE_FULL_X8 \
        /* in: v0 = x0, v1 = x1 */ \
        __ASM_EMIT("ld2r                {v12.4s, v13.4s}, [%[gate]]")       /* v12 = begin, v13 = end */ \
        __ASM_EMIT("fabs                v0.4s, v0.4s")                      /* v0  = fabsf(x0) */ \
        __ASM_EMIT("fabs                v1.4s, v1.4s") \
        __ASM_EMIT("fcmgt               v2.4s, v0.4s, v12.4s")              /* v2  = [fabs(x0) > begin] */ \
        __ASM_EMIT("fcmgt               v3.4s, v1.4s, v12.4s") \
        __ASM_EMIT("fcmgt               v4.4s, v13.4s, v0.4s")              /* v4  = [fabs(x0) < end] */ \
        __ASM_EMIT("fcmgt               v5.4s, v13.4s, v1.4s") \
        __ASM_EMIT("and                 v6.16b, v2.16b, v4.16b")            /* v6  = [fabs(x0) > begin] && [fabs(x0) < end] */ \
        __ASM_EMIT("and                 v7.16b, v3.16b, v5.16b") \
        __ASM_EMIT("orr                 v6.16b, v6.16b, v7.16b") \
        __ASM_EMIT("ext                 v7.16b, v6.16b, v6.16b, #8") \
        __ASM_EMIT("orr                 v6.16b, v6.16b, v7.16b") \
        __ASM_EMIT("ext                 v7.16b, v6.16b, v6.16b, #4") \
        __ASM_EMIT("orr                 v6.16b, v6.16b, v7.16b") \
        __ASM_EMIT("mov                 %w[off], v6.s[0]") \
        __ASM_EMIT("cmp                 %w[off], #0") \
        __ASM_EMIT("b.ne                100f") \
        __ASM_EMIT("add                 %[off], %[gate], #0x08") \
        __ASM_EMIT("ld1r                {v0.4s}, [%[off]]")                 /* v0 = gain_begin */ \
        __ASM_EMIT("add                 %[off], %[gate], #0x0c") \
        __ASM_EMIT("ld1r                {v4.4s}, [%[off]]")                 /* v4 = gain_end */ \
        __ASM_EMIT("mov                 v1.16b, v0.16b") \
        __ASM_EMIT("bit                 v0.16b, v4.16b, v2.16b")            /* v0   = [fabs(x0) > start] ? gain_end : gain_start */ \
        __ASM_EMIT("bit                 v1.16b, v4.16b, v3.16b") \
        __ASM_EMIT("b                   200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("stp                 q0, q1, [%[mem], #0x00]")           /* mem[0x00] = fabfs(x0) */ \
        LOGE_CORE_X8                                                        /* v0= lx0 = logf(fabsf(x0)) */ \
        PROCESS_KNEE_SINGLE_X8                                              /* apply knee 0 */ \
        __ASM_EMIT("200:") \
        /* out: v0 = G0, v1= G1 */

    #define PROCESS_GATE_FULL_X4 \
        /* in: v0 = x0 */ \
        __ASM_EMIT("fabs                v0.4s, v0.4s")                      /* v0  = fabsf(x0) */ \
        __ASM_EMIT("str                 q0, [%[mem], #0x00]")               /* mem[0x00] = fabfs(x0) */ \
        LOGE_CORE_X4                                                        /* v0= lx0 = logf(fabsf(x0)) */ \
        PROCESS_KNEE_SINGLE_X4                                              /* apply knee 0 */ \
        /* out: v0 = G0 */

        void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count)
        {
            IF_ARCH_AARCH64(
                float mem[8] __lsp_aligned16;
                size_t off;
            );

            ARCH_AARCH64_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                PROCESS_GATE_FULL_X8
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("add             %[src], %[src], 0x20")
                __ASM_EMIT("add             %[dst], %[dst], 0x20")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[src], #0x00]")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst], #0x00]")
                __ASM_EMIT("add             %[src], %[src], 0x10")
                __ASM_EMIT("add             %[dst], %[dst], 0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
                __ASM_EMIT("8:")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [off] "=&r" (off)
                : [gate] "r" (c),
                  [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0]),
                  [mem] "r" (&mem[0]),
                  [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        void gate_x1_curve(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count)
        {
            IF_ARCH_AARCH64(
                float mem[8] __lsp_aligned16;
                size_t off;
            );

            ARCH_AARCH64_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                PROCESS_GATE_FULL_X8
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x00]")
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("fmul            v0.4s, v0.4s, v2.4s")
                __ASM_EMIT("fmul            v1.4s, v1.4s, v3.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("add             %[src], %[src], 0x20")
                __ASM_EMIT("add             %[dst], %[dst], 0x20")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[src], #0x00]")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("ldr             q2, [%[src], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("fmul            v0.4s, v0.4s, v2.4s")
                __ASM_EMIT("str             q0, [%[dst], #0x00]")
                __ASM_EMIT("add             %[src], %[src], 0x10")
                __ASM_EMIT("add             %[dst], %[dst], 0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
                __ASM_EMIT("8:")
                __ASM_EMIT("mov             v1.16b, v0.16b")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("fmul            v0.4s, v0.4s, v1.4s")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [off] "=&r" (off)
                : [gate] "r" (c),
                  [L2C] "r" (&LOG2_CONST[0]),
                  [LOGC] "r" (&LOGE_C[0]),
                  [mem] "r" (&mem[0]),
                  [E2C] "r" (&EXP2_CONST[0]),
                  [LOG2E] "r" (&EXP_LOG2E[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

    #undef KNEE_LOAD
    #undef PROCESS_KNEE_SINGLE_X4
    #undef PROCESS_KNEE_SINGLE_X8
    #undef PROCESS_GATE_FULL_X8
    #undef PROCESS_GATE_FULL_X4

    } /* namespace asimd */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_DYNAMICS_GATE_H_ */
