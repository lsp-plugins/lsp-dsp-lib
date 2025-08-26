/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 22 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LANCZOS_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LANCZOS_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <private/dsp/arch/aarch64/asimd/pmath/sin.h>

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const uint32_t lanczos_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x38d1b717),                   // +0x00: Sinc threshold = 1e-4
                LSP_DSP_VEC4(0x3f800000),                   // +0x10: 1.0
            };
        )

        typedef struct lanczos_gen_t
        {
            float x1[8];                        // +0x00: Computed X1
            float n[8];                         // +0x20: Numerator
        } lanczos_gen_t;

        #define LANCZOS_GEN_FUNC_X8 \
            /* v0 = x1[0] */ \
            /* v1 = x1[1] */ \
            __ASM_EMIT("fmul            v2.4s, v0.4s, v27.4s")          /* v2   = x2 = x1*a */ \
            __ASM_EMIT("fmul            v3.4s, v1.4s, v27.4s") \
            __ASM_EMIT("fabs            v4.4s, v0.4s")                  /* v4   = fabsf(x1) */ \
            __ASM_EMIT("fabs            v5.4s, v1.4s") \
            __ASM_EMIT("fmul            v8.4s, v0.4s, v2.4s")           /* v8   = x1*x2 */ \
            __ASM_EMIT("fmul            v9.4s, v1.4s, v3.4s") \
            __ASM_EMIT("frecpe          v10.4s, v8.4s")                 /* v10  = s2 */ \
            __ASM_EMIT("frecpe          v11.4s, v9.4s") \
            __ASM_EMIT("frecps          v12.4s, v10.4s, v8.4s")         /* v12  = (2 - R*s2) */ \
            __ASM_EMIT("frecps          v13.4s, v11.4s, v9.4s") \
            __ASM_EMIT("fmul            v10.4s, v12.4s, v10.4s")        /* v10  = s2' = s2 * (2 - R*s2) */ \
            __ASM_EMIT("fmul            v11.4s, v13.4s, v11.4s") \
            __ASM_EMIT("frecps          v12.4s, v10.4s, v8.4s")         /* v12  = (2 - R*s2') */ \
            __ASM_EMIT("frecps          v13.4s, v11.4s, v9.4s") \
            __ASM_EMIT("fmul            v8.4s, v12.4s, v10.4s")         /* v8   = n = 1/(x1*x2) */  \
            __ASM_EMIT("fmul            v9.4s, v13.4s, v11.4s") \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v16.4s")          /* v0   = x1 + PI/2 */ \
            __ASM_EMIT("fadd            v1.4s, v1.4s, v16.4s") \
            __ASM_EMIT("fadd            v2.4s, v2.4s, v16.4s")          /* v2   = x2 + PI/2 */ \
            __ASM_EMIT("fadd            v3.4s, v3.4s, v16.4s") \
            __ASM_EMIT("stp             q4, q5, [%[state], 0x00]")      /* save fabfs(x1) */ \
            __ASM_EMIT("stp             q8, q9, [%[state], 0x20]")      /* save n */ \
            SINF_X_PLUS_PI_2_CORE_X16 \
            /* v0   = sinf(x1[0]), v1 = sinf(x1[1]) */ \
            /* v2   = sinf(x2[0]), v3 = sinf(x2[1]) */ \
            __ASM_EMIT("ldp             q4, q5, [%[state], 0x00]")      /* v4   = fabsf(x1) */ \
            __ASM_EMIT("ldp             q6, q7, [%[state], 0x20]")      /* v6   = n = 1/(x1*x2) */ \
            __ASM_EMIT("ldp             q8, q9, [%[LC]]")               /* v8   = 1e-4, v9 = 1.0 */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v2.4s")           /* v0   = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("fmul            v1.4s, v1.4s, v3.4s") \
            __ASM_EMIT("fcmge           v2.4s, v4.4s, v8.4s")           /* v2   = [ fabsf(x1) >= 1e-4 ] */ \
            __ASM_EMIT("fcmge           v3.4s, v5.4s, v8.4s") \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v6.4s")           /* v6   = sinf(x1)*sinf(x2)/(x1*x2) */ \
            __ASM_EMIT("fmul            v1.4s, v1.4s, v7.4s") \
            __ASM_EMIT("fcmgt           v4.4s, v26.4s, v4.4s")          /* v4   = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("fcmgt           v5.4s, v26.4s, v5.4s") \
            __ASM_EMIT("bif             v0.16b, v9.16b, v2.16b")        /* v0   = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("bif             v1.16b, v9.16b, v3.16b") \
            __ASM_EMIT("and             v0.16b, v0.16b, v4.16b")        /* v0   = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */ \
            __ASM_EMIT("and             v1.16b, v1.16b, v5.16b")

        #define LANCZOS_GEN_FUNC_X4 \
            /* v0 = x1 */ \
            __ASM_EMIT("fmul            v1.4s, v0.4s, v27.4s")          /* v1   = x2 = x1*a */ \
            __ASM_EMIT("fabs            v2.4s, v0.4s")                  /* v2   = fabsf(x1) */ \
            __ASM_EMIT("fmul            v3.4s, v0.4s, v1.4s")           /* v3   = x1*x2 */ \
            __ASM_EMIT("frecpe          v10.4s, v3.4s")                 /* v10  = s2 */ \
            __ASM_EMIT("frecps          v12.4s, v10.4s, v3.4s")         /* v12  = (2 - R*s2) */ \
            __ASM_EMIT("fmul            v10.4s, v12.4s, v10.4s")        /* v10  = s2' = s2 * (2 - R*s2) */ \
            __ASM_EMIT("frecps          v12.4s, v10.4s, v3.4s")         /* v12  = (2 - R*s2') */ \
            __ASM_EMIT("fmul            v3.4s, v12.4s, v10.4s")         /* v3   = n = 1/(x1*x2) */  \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v16.4s")          /* v0   = x1 + PI/2 */ \
            __ASM_EMIT("fadd            v1.4s, v1.4s, v16.4s")          /* v1   = x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X8 \
            /* v0   = sinf(x1), v1 = sinf(x2) */ \
            __ASM_EMIT("ldp             q8, q9, [%[LC]]")               /* v8   = 1e-4, v9 = 1.0 */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v1.4s")           /* v0   = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("fcmge           v4.4s, v2.4s, v8.4s")           /* v4   = [ fabsf(x1) >= 1e-4 ] */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v3.4s")           /* v0   = sinf(x1)*sinf(x2)/(x1*x2) */ \
            __ASM_EMIT("fcmgt           v5.4s, v26.4s, v2.4s")          /* v5   = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("bif             v0.16b, v9.16b, v4.16b")        /* v0   = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("and             v0.16b, v0.16b, v5.16b")        /* v0   = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */


        void lanczos1(float *dst, float k, float p, float t, float a, size_t count)
        {
            IF_ARCH_AARCH64(
                lanczos_gen_t state;
            );
            ARCH_AARCH64_ASM(
                __ASM_EMIT("ldp             q16, q17, [%[S2C], #0x00]")     /* v16  = PI/2, v17 = PI */
                __ASM_EMIT("ldp             q18, q19, [%[S2C], #0x20]")     /* v18  = 1/(2*PI), v19 = C0 */
                __ASM_EMIT("ldp             q20, q21, [%[S2C], #0x40]")     /* v20  = C1, v21 = C2 */
                __ASM_EMIT("ldp             q22, q23, [%[S2C], #0x60]")     /* v22  = C3, v23 = C4 */
                __ASM_EMIT("ld1r            {v24.4s}, [%[k]]")              /* v24  = k */
                __ASM_EMIT("ld1r            {v25.4s}, [%[p]]")              /* v25  = p */
                __ASM_EMIT("ld1r            {v26.4s}, [%[t]]")              /* v26  = t */
                __ASM_EMIT("ld1r            {v27.4s}, [%[a]]")              /* v27  = a */
                __ASM_EMIT("ldp             q28, q29, [%[S2KP], #0x00]")    /* v28  = i0 = 0 1 2 3, v29 = i1 = 4 5 6 7 */
                __ASM_EMIT("ldp             q30, q31, [%[S2KP], #0x20]")    /* v30  = s0 = 8 8 8 8, v31 = s1 = 4 4 4 4 */
                // x8 blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("fmul            v0.4s, v28.4s, v24.4s")         // v0   = k*i0
                __ASM_EMIT("fmul            v1.4s, v29.4s, v24.4s")         // v1   = k*i1
                __ASM_EMIT("fadd            v28.4s, v28.4s, v30.4s")        // v28  = i0 + step
                __ASM_EMIT("fadd            v29.4s, v29.4s, v30.4s")        // v29  = i1 + step
                __ASM_EMIT("fsub            v0.4s, v0.4s, v25.4s")          // v0   = k*i0 - p
                __ASM_EMIT("fsub            v1.4s, v1.4s, v25.4s")          // v1   = k*i1 - p
                LANCZOS_GEN_FUNC_X8
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("fmul            v0.4s, v28.4s, v24.4s")         // v0   = k*i0
                __ASM_EMIT("fadd            v28.4s, v28.4s, v31.4s")        // v28  = i0 + step
                __ASM_EMIT("fsub            v0.4s, v0.4s, v25.4s")          // v0   = k*i0 - p
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst], #0x00]")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            8f")
                __ASM_EMIT("fmul            v0.4s, v28.4s, v24.4s")         // v0   = k*i0
                __ASM_EMIT("fsub            v0.4s, v0.4s, v25.4s")          // v0   = k*i0 - p
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #4")    // v0   = S1 S2 S3 S0
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("st1             {v0.d}[0], [%[dst]]")
                // End
                __ASM_EMIT("8:")

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

    } /* namespace asimd */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_LANCZOS_H_ */
