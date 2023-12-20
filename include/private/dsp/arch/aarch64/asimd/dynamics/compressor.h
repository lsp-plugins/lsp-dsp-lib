/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 8 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_DYNAMICS_COMPRESSOR_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_DYNAMICS_COMPRESSOR_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <private/dsp/arch/aarch64/asimd/pmath/exp.h>
#include <private/dsp/arch/aarch64/asimd/pmath/log.h>

namespace lsp
{
    namespace asimd
    {
    #define KNEE_LOAD \
        __ASM_EMIT("add                 %[off], %[comp], #0x10") \
        __ASM_EMIT("ld4r                {v12.4s, v13.4s, v14.4s, v15.4s}, [%[comp]]") /* v12=start, v13=end, v14=gain, v15=herm[0] */ \
        __ASM_EMIT("ld4r                {v28.4s, v29.4s, v30.4s, v31.4s}, [%[off]]" ) /* v28=herm[1], v29=herm[2], v30=tilt[0], v31=tilt[1] */ \

    #define PROCESS_KNEE_SINGLE_X8 \
        /* in: v0 = lx0, v1 = lx1 */ \
        KNEE_LOAD \
        __ASM_EMIT("ldp                 q6, q7, [%[mem], #0x00]")           /* v6 = x0 */ \
        __ASM_EMIT("fmul                v2.4s, v0.4s, v15.4s")              /* v2 = herm[0]*lx0 */ \
        __ASM_EMIT("fmul                v3.4s, v1.4s, v15.4s") \
        __ASM_EMIT("fadd                v2.4s, v2.4s, v28.4s")              /* v2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("fadd                v3.4s, v3.4s, v28.4s") \
        __ASM_EMIT("fmul                v2.4s, v2.4s, v0.4s")               /* v2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("fmul                v3.4s, v3.4s, v1.4s") \
        __ASM_EMIT("fmul                v4.4s, v0.4s, v30.4s")              /* v4 = tilt[0]*lx0 */ \
        __ASM_EMIT("fmul                v5.4s, v1.4s, v30.4s") \
        __ASM_EMIT("fcmge               v6.4s, v6.4s, v13.4s")              /* v6 = [x0 >= end] */ \
        __ASM_EMIT("fcmge               v7.4s, v7.4s, v13.4s") \
        __ASM_EMIT("fadd                v0.4s, v4.4s, v31.4s")              /* v0 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("fadd                v1.4s, v5.4s, v31.4s") \
        __ASM_EMIT("fadd                v2.4s, v2.4s, v29.4s")              /* v2 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("fadd                v3.4s, v3.4s, v29.4s") \
        __ASM_EMIT("bif                 v0.16b, v2.16b, v6.16b")            /* v0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("bif                 v1.16b, v3.16b, v7.16b") \
        EXP_CORE_X8                                                         /* v0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("ldp                 q6, q7, [%[mem], #0x00]")           /* v6 = x0 */ \
        __ASM_EMIT("fcmge               v6.4s, v12.4s, v6.4s")              /* v6 = [x0 <= start] */ \
        __ASM_EMIT("fcmge               v7.4s, v12.4s, v7.4s") \
        __ASM_EMIT("bit                 v0.16b, v14.16b, v6.16b")           /* q0 = G = [x0 <= start] ? gain : EV */ \
        __ASM_EMIT("bit                 v1.16b, v14.16b, v7.16b") \
        /* out: v0 = g0, v1 = g1 */

    #define PROCESS_KNEE_SINGLE_X4 \
        /* in: v0 = lx0 */ \
        KNEE_LOAD \
        __ASM_EMIT("ldr                 q6, [%[mem], #0x00]")               /* v6 = x0 */ \
        __ASM_EMIT("fmul                v2.4s, v0.4s, v15.4s")              /* v2 = herm[0]*lx0 */ \
        __ASM_EMIT("fadd                v2.4s, v2.4s, v28.4s")              /* v2 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("fmul                v2.4s, v2.4s, v0.4s")               /* v2 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("fmul                v4.4s, v0.4s, v30.4s")              /* v4 = tilt[0]*lx0 */ \
        __ASM_EMIT("fcmge               v6.4s, v6.4s, v13.4s")              /* v6 = [x0 >= end] */ \
        __ASM_EMIT("fadd                v0.4s, v4.4s, v31.4s")              /* v0 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("fadd                v2.4s, v2.4s, v29.4s")              /* v2 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("bif                 v0.16b, v2.16b, v6.16b")            /* v0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X4                                                         /* v0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("ldr                 q6, [%[mem], #0x00]")               /* v6 = x0 */ \
        __ASM_EMIT("fcmge               v6.4s, v12.4s, v6.4s")              /* v6 = [x0 <= start] */ \
        __ASM_EMIT("bit                 v0.16b, v14.16b, v6.16b")           /* q0 = G = [x0 <= start] ? gain : EV */ \
        /* out: v0 = g0 */

    #define PROCESS_COMP_FULL_X8 \
        /* in: v0 = x0, v1 = x1 */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x20") \
        __ASM_EMIT("ld1r                {v12.4s}, [%[comp]]")               /* v12 = start[0] */ \
        __ASM_EMIT("ld1r                {v13.4s}, [%[off]]")                /* v13 = start[1] */ \
        __ASM_EMIT("fabs                v0.4s, v0.4s")                      /* v0  = fabsf(x0) */ \
        __ASM_EMIT("fabs                v1.4s, v1.4s") \
        __ASM_EMIT("fcmgt               v2.4s, v0.4s, v12.4s")              /* v2  = [fabs(x0) > start[0]] */ \
        __ASM_EMIT("fcmgt               v3.4s, v1.4s, v12.4s") \
        __ASM_EMIT("fcmgt               v4.4s, v0.4s, v13.4s")              /* v4  = [fabs(x0) > start[1]] */ \
        __ASM_EMIT("fcmgt               v5.4s, v1.4s, v13.4s") \
        __ASM_EMIT("orr                 v2.16b, v2.16b, v3.16b") \
        __ASM_EMIT("orr                 v4.16b, v4.16b, v5.16b") \
        __ASM_EMIT("orr                 v2.16b, v2.16b, v4.16b") \
        __ASM_EMIT("ext                 v4.16b, v2.16b, v2.16b, #8") \
        __ASM_EMIT("orr                 v2.16b, v2.16b, v4.16b") \
        __ASM_EMIT("ext                 v4.16b, v2.16b, v2.16b, #4") \
        __ASM_EMIT("orr                 v2.16b, v2.16b, v4.16b") \
        __ASM_EMIT("mov                 %w[off], v2.s[0]") \
        __ASM_EMIT("cmp                 %w[off], #0") \
        __ASM_EMIT("b.ne                100f") \
        __ASM_EMIT("add                 %[off], %[comp], #0x08") \
        __ASM_EMIT("ld1r                {v12.4s}, [%[off]]")                /* v12  = g1 */ \
        __ASM_EMIT("add                 %[off], %[comp], #0x28") \
        __ASM_EMIT("ld1r                {v13.4s}, [%[off]]")                /* v13  = g2 */ \
        __ASM_EMIT("fmul                v0.4s, v12.4s, v13.4s")             /* v0   = g1*g2 */ \
        __ASM_EMIT("fmul                v1.4s, v12.4s, v13.4s") \
        __ASM_EMIT("b                   200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("stp                 q0, q1, [%[mem], #0x00]")           /* mem[0x00] = fabfs(x0) */ \
        LOGE_CORE_X8                                                        /* v0= lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("stp                 q0, q1, [%[mem], #0x20]")           /* mem[0x20] = lx0 */ \
        PROCESS_KNEE_SINGLE_X8                                              /* apply knee 0 */ \
        __ASM_EMIT("stp                 q0, q1, [%[mem], #0x40]")           /* mem[0x40] = g0 */ \
        __ASM_EMIT("ldp                 q0, q1, [%[mem], #0x20]")           /* q0 = lx0 */ \
        __ASM_EMIT("add                 %[comp], %[comp], #0x20") \
        PROCESS_KNEE_SINGLE_X8                                              /* apply knee 1 */ \
        __ASM_EMIT("ldp                 q2, q3, [%[mem], #0x40]")           /* v2 = g0 */ \
        __ASM_EMIT("sub                 %[comp], %[comp], #0x20") \
        __ASM_EMIT("fmul                v0.4s, v0.4s, v2.4s")               /* v0 = G = */ \
        __ASM_EMIT("fmul                v1.4s, v1.4s, v3.4s") \
        __ASM_EMIT("200:") \
        /* out: v0 = G0, v1= G1 */

    #define PROCESS_COMP_FULL_X4 \
        /* in: q0 = x0, q1 = x1 */ \
        __ASM_EMIT("fabs                v0.4s, v0.4s")                      /* v0  = fabsf(x0) */ \
        __ASM_EMIT("str                 q0, [%[mem], #0x00]")               /* mem[0x00] = fabfs(x0) */ \
        LOGE_CORE_X4                                                        /* v0= lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("str                 q0, [%[mem], #0x20]")               /* mem[0x20] = lx0 */ \
        PROCESS_KNEE_SINGLE_X4                                              /* apply knee 0 */ \
        __ASM_EMIT("str                 q0, [%[mem], #0x40]")               /* mem[0x40] = g0 */ \
        __ASM_EMIT("ldr                 q0, [%[mem], #0x20]")               /* q0 = lx0 */ \
        __ASM_EMIT("add                 %[comp], %[comp], #0x20") \
        PROCESS_KNEE_SINGLE_X4                                              /* apply knee 1 */ \
        __ASM_EMIT("ldr                 q2, [%[mem], #0x40]")               /* v2 = g0 */ \
        __ASM_EMIT("sub                 %[comp], %[comp], #0x20") \
        __ASM_EMIT("fmul                v0.4s, v0.4s, v2.4s")               /* v0 = G = */ \
        /* out: q0 = G0, q1= G1 */

        void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_AARCH64(
                float mem[24] __lsp_aligned16;
                size_t off;
            );

            ARCH_AARCH64_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                PROCESS_COMP_FULL_X8
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
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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
                : [comp] "r" (c),
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

        void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_AARCH64(
                float mem[24] __lsp_aligned16;
                size_t off;
            );

            ARCH_AARCH64_ASM(
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                PROCESS_COMP_FULL_X8
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
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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
                : [comp] "r" (c),
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
    #undef PROCESS_COMP_FULL_X8
    #undef PROCESS_COMP_FULL_X4

    } /* namespace asimd */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_DYNAMICS_COMPRESSOR_H_ */
