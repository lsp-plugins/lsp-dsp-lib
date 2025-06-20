/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 16 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_PMATH_LANCZOS_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_PMATH_LANCZOS_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE2_IMPL */

#include <private/dsp/arch/x86/sse2/pmath/sin.h>

namespace lsp
{
    namespace sse2
    {
        IF_ARCH_X86(
            static const uint32_t lanczos_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7fffffff),                   // +0x00: Mask for fabsf
                LSP_DSP_VEC4(0x38d1b717),                   // +0x10: Sinc threshold = 1e-4
                LSP_DSP_VEC4(0x3f800000),                   // +0x20: 1.0
            };
        )

        typedef struct lanczos_gen_t
        {
            float k[4];                         // +0x00: Multiplier
            float p[4];                         // +0x10: Initial phase
            float a[4];                         // +0x20: Number of lobes
            float i[4];                         // +0x30: Indices
            float x1[4];                        // +0x40: Computed X1
            float d[4];                         // +0x50: Denominator
            float t[4];                         // +0x60: Time range
        } lanczos_gen_t;

        #define LANCZOS_GEN_FUNC_X4 \
            /* xmm0 = x1 */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm4")                /* xmm4     = x1 */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")                /* xmm2     = x1 */ \
            __ASM_EMIT("mulps           0x20 + %[state], %%xmm4")       /* xmm4     = x2 = x1*a */ \
            __ASM_EMIT("movaps          %%xmm0, 0x40 + %[state]")       /* *x       = x1 */ \
            __ASM_EMIT("mulps           %%xmm4, %%xmm2")                /* xmm2     = d = x1 * x2 */ \
            __ASM_EMIT("addps           0x00 + %[S2C], %%xmm0")         /* xmm0     = x1 + PI/2 */ \
            __ASM_EMIT("addps           0x00 + %[S2C], %%xmm4")         /* xmm4     = x2 + PI/2 */ \
            __ASM_EMIT("movaps          %%xmm2, 0x50 + %[state]")       /* *d       = d */ \
            SINF_X_PLUS_PI_2_CORE_X8                                    /* xmm0     = sinf(x1), xmm4 = sinf(x2) */ \
            __ASM_EMIT("movaps          0x40 + %[state], %%xmm1")       /* xmm1     = x */ \
            __ASM_EMIT("mulps           %%xmm4, %%xmm0")                /* xmm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("andps           0x00 + %[LC], %%xmm1")          /* xmm1     = fabsf(x) */ \
            __ASM_EMIT("divps           0x50 + %[state], %%xmm0")       /* xmm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("movaps          %%xmm1, %%xmm2")                /* xmm2     = fabsf(x) */ \
            __ASM_EMIT("cmpps           $5, 0x10 + %[LC], %%xmm1")      /* xmm1     = [ fabsf(x) >= 1e-4 ] */ \
            __ASM_EMIT("andps           %%xmm1, %%xmm0")                /* xmm0     = [ fabsf(x) >= 1e-4 ] & f */ \
            __ASM_EMIT("cmpps           $1, 0x60 + %[state], %%xmm2")   /* xmm2     = [ fabsf(x) < t ] */ \
            __ASM_EMIT("andnps          0x20 + %[LC], %%xmm1")          /* xmm1     = [ fabsf(x) < 1e-4 ] & 1.0 */ \
            __ASM_EMIT("orps            %%xmm1, %%xmm0")                /* xmm0     = [ fabsf(x) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("andps           %%xmm2, %%xmm0")                /* xmm0     = [ fabsf(x) < t ] ? ([ fabsf(x) >= 1e-4 ] ? f : 1.0) : 0.0 */

        void lanczos1(float *dst,  float k, float p, float t, float a, size_t count)
        {
            IF_ARCH_X86(
                lanczos_gen_t state __lsp_aligned16;
                float stub[4] __lsp_aligned16;
            );

            ARCH_X86_ASM(
                // Prepare
                __ASM_EMIT("movss           %[k], %%xmm4")                  // xmm4     = k
                __ASM_EMIT("movss           %[p], %%xmm5")                  // xmm5     = p
                __ASM_EMIT("movss           %[a], %%xmm6")                  // xmm6     = a
                __ASM_EMIT("movss           %[t], %%xmm7")                  // xmm7     = t
                __ASM_EMIT("shufps          $0x00, %%xmm4, %%xmm4")         // xmm4     = k k k k
                __ASM_EMIT("shufps          $0x00, %%xmm5, %%xmm5")         // xmm5     = p p p p
                __ASM_EMIT("shufps          $0x00, %%xmm6, %%xmm6")         // xmm6     = a a a a
                __ASM_EMIT("shufps          $0x00, %%xmm7, %%xmm7")         // xmm7     = t t t t
                __ASM_EMIT("movaps          %%xmm4, 0x00 + %[state]")
                __ASM_EMIT("movaps          %%xmm5, 0x10 + %[state]")
                __ASM_EMIT("movaps          %%xmm6, 0x20 + %[state]")
                __ASM_EMIT("movaps          %%xmm7, 0x60 + %[state]")
                __ASM_EMIT("movaps          0x00 + %[LGEN], %%xmm1")        // xmm1     = i = 0 1 2 3
                // x4 blocks
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movaps          %%xmm1, %%xmm0")                // xmm0     = i
                __ASM_EMIT("mulps           0x00 + %[state], %%xmm0")       // xmm0     = k*i
                __ASM_EMIT("addps           0x10 + %[LGEN], %%xmm1")        // xmm1     = i' = i + step
                __ASM_EMIT("subps           0x10 + %[state], %%xmm0")       // xmm0     = x1 = k*i - p
                __ASM_EMIT("movaps          %%xmm1, 0x30 + %[state]")       // *i       = i'
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("movaps          0x30 + %[state], %%xmm1")       // xmm1     = i'
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             6f")
                __ASM_EMIT("movaps          %%xmm1, %%xmm0")                // xmm0     = i
                __ASM_EMIT("mulps           0x00 + %[state], %%xmm0")       // xmm0     = k*i
                __ASM_EMIT("subps           0x10 + %[state], %%xmm0")       // xmm0     = x1 = k*i - p
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              4f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("shufps          $0x39, %%xmm0, %%xmm0")         // xmm0     = S1 S2 S3 S0
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("4:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movlps          %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [state] "o" (state),
                  [S2C] "o" (sinf_const),
                  [LGEN] "o" (kp_gen_const),
                  [LC] "o" (lanczos_const),
                  [stub] "o" (stub),
                  [k] "m" (k),
                  [p] "m" (p),
                  [a] "m" (a),
                  [t] "m" (t)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace sse2 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_SSE2_PMATH_LANCZOS_H_ */
