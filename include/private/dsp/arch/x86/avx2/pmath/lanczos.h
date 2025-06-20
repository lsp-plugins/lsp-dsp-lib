/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 17 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_PMATH_LANCZOS_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_PMATH_LANCZOS_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

#include <private/dsp/arch/x86/avx2/pmath/sin.h>

namespace lsp
{
    namespace avx2
    {
        IF_ARCH_X86(
            static const uint32_t lanczos_const[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0x7fffffff),                   // +0x00: Mask for fabsf
                LSP_DSP_VEC8(0x38d1b717),                   // +0x20: Sinc threshold = 1e-4
                LSP_DSP_VEC8(0x3f800000),                   // +0x40: 1.0
            };
        )

        typedef struct lanczos_gen_t
        {
            float k[8];                         // +0x00: Multiplier
            float p[8];                         // +0x20: Initial phase
            float a[8];                         // +0x40: Number of lobes
            float i[8];                         // +0x60: Indices
            float x1[8];                        // +0x80: Computed X1
            float d[8];                         // +0xa0: Denominator
            float t[8];                         // +0xc0: Time range
        } lanczos_gen_t;

        #define LANCZOS_GEN_FUNC_X8 \
            /* ymm0 = x1 */ \
            __ASM_EMIT("vmulps          0x40 + %[state], %%ymm0, %%ymm4")           /* ymm4     = x2 = x1*a */ \
            __ASM_EMIT("vmovaps         %%ymm0, 0x80 + %[state]")                   /* *x1      = x1 */ \
            __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm2")                    /* ymm2     = d = x1 * x2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm0, %%ymm0")             /* ymm0     = x1 + PI/2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm4, %%ymm4")             /* ymm4     = x2 + PI/2 */ \
            __ASM_EMIT("vmovaps         %%ymm2, 0xa0 + %[state]")                   /* *d       = d */ \
            SINF_X_PLUS_PI_2_CORE_X16                                               /* ymm0     = sinf(x1), ymm4 = sinf(x2) */ \
            __ASM_EMIT("vmovaps         0x80 + %[state], %%ymm1")                   /* ymm1     = x1 */ \
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%ymm1, %%ymm1")              /* ymm1     = fabsf(x) */ \
            __ASM_EMIT("vdivps          0xa0 + %[state], %%ymm0, %%ymm0")           /* ymm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x20 + %[LC], %%ymm1, %%ymm2")          /* ymm2     = [ fabsf(x) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, 0xc0 + %[state], %%ymm1, %%ymm1")       /* ymm1     = [ fabsf(x) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x40 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = [ fabsf(x) < t ] ? ([ fabsf(x) >= 1e-4 ] ? f : 1.0) : 0.0 */

        #define LANCZOS_GEN_FUNC_X4 \
            /* xmm0 = x1 */ \
            __ASM_EMIT("vmulps          0x40 + %[state], %%xmm0, %%xmm4")           /* xmm4     = x2 = x1*a */ \
            __ASM_EMIT("vmovaps         %%xmm0, 0x80 + %[state]")                   /* *x1      = x1 */ \
            __ASM_EMIT("vmulps          %%xmm0, %%xmm4, %%xmm2")                    /* xmm2     = d = x1 * x2 */ \
            __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm0, %%ymm0")                /* ymm0     = x1 x1 x1 x1 x2 x2 x2 x2 */ \
            __ASM_EMIT("vmovaps         %%xmm2, 0xa0 + %[state]")                   /* *d       = d */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm0, %%ymm0")             /* xmm0     = x1 + PI/2 ... x2 + PI/2 ... */ \
            SINF_X_PLUS_PI_2_CORE_X8                                                /* ymm0     = sinf(x1) ... sinf(x2) ... */ \
            __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")                        /* xmm4     = sif(x2) */ \
            __ASM_EMIT("vmovaps         0x80 + %[state], %%xmm1")                   /* xmm1     = x1 */ \
            __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")                    /* xmm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%xmm1, %%xmm1")              /* xmm1     = fabsf(x) */ \
            __ASM_EMIT("vdivps          0xa0 + %[state], %%xmm0, %%xmm0")           /* xmm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x20 + %[LC], %%xmm1, %%xmm2")          /* xmm2     = [ fabsf(x) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, 0xc0 + %[state], %%xmm1, %%xmm1")       /* xmm1     = [ fabsf(x) < t ] */ \
            __ASM_EMIT("vblendvps       %%xmm2, 0x40 + %[LC], %%xmm0, %%xmm0")      /* xmm0     = [ fabsf(x) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%xmm1, %%xmm0, %%xmm0")                    /* xmm0     = [ fabsf(x) < t ] ? ([ fabsf(x) >= 1e-4 ] ? f : 1.0) : 0.0 */


        void lanczos1(float *dst,  float k, float p, float t, float a, size_t count)
        {
            IF_ARCH_X86(
                lanczos_gen_t state __lsp_aligned32;
                float stub[8] __lsp_aligned16;
            );

            ARCH_X86_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%ymm4")                              // ymm4     = k
                __ASM_EMIT("vbroadcastss    %[p], %%ymm5")                              // ymm5     = p
                __ASM_EMIT("vbroadcastss    %[a], %%ymm6")                              // ymm6     = a
                __ASM_EMIT("vbroadcastss    %[t], %%ymm7")                              // ymm7     = t
                __ASM_EMIT("vmovaps         %%ymm4, 0x00 + %[state]")
                __ASM_EMIT("vmovaps         %%ymm5, 0x20 + %[state]")
                __ASM_EMIT("vmovaps         %%ymm6, 0x40 + %[state]")
                __ASM_EMIT("vmovaps         %%ymm7, 0xc0 + %[state]")
                __ASM_EMIT("vmovaps         0x00 + %[LGEN], %%ymm1")                    // ymm1     = i = 0 1 2 3 4 5 6 7
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x00 + %[state], %%ymm1, %%ymm0")           // ymm0     = k*i
                __ASM_EMIT("vaddps          0x60 + %[LGEN], %%ymm1, %%ymm1")            // ymm1     = i' = i + step
                __ASM_EMIT("vsubps          0x20 + %[state], %%ymm0, %%ymm0")           // ymm0     = x1 = k*i - p
                __ASM_EMIT("vmovaps         %%ymm1, 0x60 + %[state]")                   // *i       = i'
                LANCZOS_GEN_FUNC_X8
                __ASM_EMIT("vmovaps         0x60 + %[state], %%ymm1")                   // ymm1     = i'
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x00 + %[state], %%xmm1, %%xmm0")           // xmm0     = k*i
                __ASM_EMIT("vaddps          0x80 + %[LGEN], %%xmm1, %%xmm1")            // xmm1     = i' = i + step
                __ASM_EMIT("vsubps          0x20 + %[state], %%xmm0, %%xmm0")           // xmm0     = x1 = k*i - p
                __ASM_EMIT("vmovaps         %%xmm1, 0x60 + %[state]")                   // *i       = i'
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("vmovaps         0x60 + %[state], %%xmm1")                   // xmm1     = i'
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             8f")
                __ASM_EMIT("vmulps          0x00 + %[state], %%xmm1, %%xmm0")           // xmm0     = k*i
                __ASM_EMIT("vsubps          0x20 + %[state], %%xmm0, %%xmm0")           // xmm0     = x1 = k*i - p
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("8:")

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

        #define LANCZOS_GEN_X64_FUNC_X8 \
            /* ymm8     = k */ \
            /* ymm9     = p */ \
            /* ymm10    = t */ \
            /* ymm11    = a */ \
            /* ymm12    = x1 */ \
            /* ymm13    = d */ \
            /* ymm14    = i */ \
            /* ymm15    = step */ \
            __ASM_EMIT("vmulps          %%ymm11, %%ymm12, %%ymm4")                  /* ymm12    = x2 = x1*a */ \
            __ASM_EMIT("vmulps          %%ymm12, %%ymm4, %%ymm13")                  /* ymm13    = d = x1 * x2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm12, %%ymm0")            /* ymm0     = x1 + PI/2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm4, %%ymm4")             /* ymm4     = x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X16                                               /* ymm0     = sinf(x1), ymm4 = sinf(x2) */ \
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%ymm12, %%ymm12")            /* ymm12    = fabsf(x1) */ \
            __ASM_EMIT("vdivps          %%ymm13, %%ymm0, %%ymm0")                   /* ymm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x20 + %[LC], %%ymm12, %%ymm2")         /* ymm2     = [ fabsf(x1) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, %%ymm10, %%ymm12, %%ymm1")              /* ymm1     = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x40 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */

        #define LANCZOS_GEN_X64_FUNC_X4 \
            /* xmm0 = x1 */ \
            __ASM_EMIT("vmulps          %%xmm11, %%xmm12, %%xmm4")                  /* xmm12    = x2 = x1*a */ \
            __ASM_EMIT("vmulps          %%xmm12, %%xmm4, %%xmm13")                  /* xmm13    = d = x1 * x2 */ \
            __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm12, %%ymm0")               /* ymm0     = x1 x1 x1 x1 x2 x2 x2 x2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm0, %%ymm0")             /* ymm0     = x1 + PI/2 ... x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X8                                                /* ymm0     = sinf(x1) ... sinf(x2) */ \
            __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")                        /* xmm4     = sif(x2) */ \
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%ymm12, %%ymm12")            /* ymm12    = fabsf(x1) */ \
            __ASM_EMIT("vdivps          %%ymm13, %%ymm0, %%ymm0")                   /* ymm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x20 + %[LC], %%ymm12, %%ymm2")         /* ymm2     = [ fabsf(x1) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, %%ymm10, %%ymm12, %%ymm1")              /* ymm1     = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x40 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */

        void x64_lanczos1(float *dst,  float k, float p, float t, float a, size_t count)
        {
            ARCH_X86_64_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%ymm8")                              // ymm8     = k
                __ASM_EMIT("vbroadcastss    %[p], %%ymm9")                              // ymm9     = p
                __ASM_EMIT("vbroadcastss    %[t], %%ymm10")                             // ymm10    = t
                __ASM_EMIT("vbroadcastss    %[a], %%ymm11")                             // ymm11    = a
                __ASM_EMIT("vmovaps         0x00 + %[LGEN], %%ymm14")                   // ymm14    = i = 0 1 2 3 4 5 6 7
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("vmovaps         0x60 + %[LGEN], %%ymm15")                   // ymm15    = step
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%ymm8, %%ymm14, %%ymm12")                  // ymm12    = k*i
                __ASM_EMIT("vaddps          %%ymm15, %%ymm14, %%ymm14")                 // ymm14    = i' = i + step
                __ASM_EMIT("vsubps          %%ymm9, %%ymm12, %%ymm12")                  // ymm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm14, %%xmm12")                  // xmm12    = k*i
                __ASM_EMIT("vaddps          0x80 + %[LGEN], %%xmm14, %%xmm14")          // xmm14    = i' = i + step
                __ASM_EMIT("vsubps          %%xmm9, %%xmm12, %%xmm12")                  // xmm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             8f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm14, %%xmm12")                   // xmm12    = k*i
                __ASM_EMIT("vsubps          %%xmm9, %%xmm12, %%xmm12")                   // xmm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "o" (sinf_const),
                  [LGEN] "o" (kp_gen_const),
                  [LC] "o" (lanczos_const),
                  [k] "m" (k),
                  [p] "m" (p),
                  [a] "m" (a),
                  [t] "m" (t)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }


        #define LANCZOS_GEN_FUNC_X8_FMA3 \
            /* ymm0 = x1 */ \
            __ASM_EMIT("vmulps          0x40 + %[state], %%ymm0, %%ymm4")           /* ymm4     = x2 = x1*a */ \
            __ASM_EMIT("vmovaps         %%ymm0, 0x80 + %[state]")                   /* *x1      = x1 */ \
            __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm2")                    /* ymm2     = d = x1 * x2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm0, %%ymm0")             /* ymm0     = x1 + PI/2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm4, %%ymm4")             /* ymm4     = x2 + PI/2 */ \
            __ASM_EMIT("vmovaps         %%ymm2, 0xa0 + %[state]")                   /* *d       = d */ \
            SINF_X_PLUS_PI_2_CORE_X16_FMA3                                          /* ymm0     = sinf(x1), ymm4 = sinf(x2) */ \
            __ASM_EMIT("vmovaps         0x80 + %[state], %%ymm1")                   /* ymm1     = x1 */ \
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%ymm1, %%ymm1")              /* ymm1     = fabsf(x) */ \
            __ASM_EMIT("vdivps          0xa0 + %[state], %%ymm0, %%ymm0")           /* ymm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x20 + %[LC], %%ymm1, %%ymm2")          /* ymm2     = [ fabsf(x) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, 0xc0 + %[state], %%ymm1, %%ymm1")       /* ymm1     = [ fabsf(x) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x40 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = [ fabsf(x) < t ] ? ([ fabsf(x) >= 1e-4 ] ? f : 1.0) : 0.0 */

        #define LANCZOS_GEN_FUNC_X4_FMA3 \
            /* xmm0 = x1 */ \
            __ASM_EMIT("vmulps          0x40 + %[state], %%xmm0, %%xmm4")           /* xmm4     = x2 = x1*a */ \
            __ASM_EMIT("vmovaps         %%xmm0, 0x80 + %[state]")                   /* *x1      = x1 */ \
            __ASM_EMIT("vmulps          %%xmm0, %%xmm4, %%xmm2")                    /* xmm2     = d = x1 * x2 */ \
            __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm0, %%ymm0")                /* ymm0     = x1 x1 x1 x1 x2 x2 x2 x2 */ \
            __ASM_EMIT("vmovaps         %%xmm2, 0xa0 + %[state]")                   /* *d       = d */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm0, %%ymm0")             /* xmm0     = x1 + PI/2 ... x2 + PI/2 ... */ \
            SINF_X_PLUS_PI_2_CORE_X8_FMA3                                           /* ymm0     = sinf(x1) ... sinf(x2) */ \
            __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")                        /* xmm4     = sif(x2) */ \
            __ASM_EMIT("vmovaps         0x80 + %[state], %%xmm1")                   /* xmm1     = x1 */ \
            __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")                    /* xmm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%xmm1, %%xmm1")              /* xmm1     = fabsf(x) */ \
            __ASM_EMIT("vdivps          0xa0 + %[state], %%xmm0, %%xmm0")           /* xmm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x20 + %[LC], %%xmm1, %%xmm2")          /* xmm2     = [ fabsf(x) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, 0xc0 + %[state], %%xmm1, %%xmm1")       /* xmm1     = [ fabsf(x) < t ] */ \
            __ASM_EMIT("vblendvps       %%xmm2, 0x40 + %[LC], %%xmm0, %%xmm0")      /* xmm0     = [ fabsf(x) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%xmm1, %%xmm0, %%xmm0")                    /* xmm0     = [ fabsf(x) < t ] ? ([ fabsf(x) >= 1e-4 ] ? f : 1.0) : 0.0 */

        void lanczos1_fma3(float *dst,  float k, float p, float t, float a, size_t count)
        {
            IF_ARCH_X86(
                lanczos_gen_t state __lsp_aligned32;
                float stub[8] __lsp_aligned16;
            );

            ARCH_X86_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%ymm4")                              // ymm4     = k
                __ASM_EMIT("vbroadcastss    %[p], %%ymm5")                              // ymm5     = p
                __ASM_EMIT("vbroadcastss    %[a], %%ymm6")                              // ymm6     = a
                __ASM_EMIT("vbroadcastss    %[t], %%ymm7")                              // ymm7     = t
                __ASM_EMIT("vmovaps         %%ymm4, 0x00 + %[state]")
                __ASM_EMIT("vmovaps         %%ymm5, 0x20 + %[state]")
                __ASM_EMIT("vmovaps         %%ymm6, 0x40 + %[state]")
                __ASM_EMIT("vmovaps         %%ymm7, 0xc0 + %[state]")
                __ASM_EMIT("vmovaps         0x00 + %[LGEN], %%ymm1")                    // ymm1     = i = 0 1 2 3 4 5 6 7
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x00 + %[state], %%ymm1, %%ymm0")           // ymm0     = k*i
                __ASM_EMIT("vaddps          0x60 + %[LGEN], %%ymm1, %%ymm1")            // ymm1     = i' = i + step
                __ASM_EMIT("vsubps          0x20 + %[state], %%ymm0, %%ymm0")           // ymm0     = x1 = k*i - p
                __ASM_EMIT("vmovaps         %%ymm1, 0x60 + %[state]")                   // *i       = i'
                LANCZOS_GEN_FUNC_X8_FMA3
                __ASM_EMIT("vmovaps         0x60 + %[state], %%ymm1")                   // ymm1     = i'
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x00 + %[state], %%xmm1, %%xmm0")           // xmm0     = k*i
                __ASM_EMIT("vaddps          0x80 + %[LGEN], %%xmm1, %%xmm1")            // xmm1     = i' = i + step
                __ASM_EMIT("vsubps          0x20 + %[state], %%xmm0, %%xmm0")           // xmm0     = x1 = k*i - p
                __ASM_EMIT("vmovaps         %%xmm1, 0x60 + %[state]")                   // *i       = i'
                LANCZOS_GEN_FUNC_X4_FMA3
                __ASM_EMIT("vmovaps         0x60 + %[state], %%xmm1")                   // xmm1     = i'
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             8f")
                __ASM_EMIT("vmulps          0x00 + %[state], %%xmm1, %%xmm0")           // xmm0     = k*i
                __ASM_EMIT("vsubps          0x20 + %[state], %%xmm0, %%xmm0")           // xmm0     = x1 = k*i - p
                LANCZOS_GEN_FUNC_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("8:")

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

        #define LANCZOS_GEN_X64_FUNC_X8_FMA3 \
            /* ymm8     = k */ \
            /* ymm9     = p */ \
            /* ymm10    = t */ \
            /* ymm11    = a */ \
            /* ymm12    = x1 */ \
            /* ymm13    = d */ \
            /* ymm14    = i */ \
            /* ymm15    = step */ \
            __ASM_EMIT("vmulps          %%ymm11, %%ymm12, %%ymm4")                  /* ymm12    = x2 = x1*a */ \
            __ASM_EMIT("vmulps          %%ymm12, %%ymm4, %%ymm13")                  /* ymm13    = d = x1 * x2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm12, %%ymm0")            /* ymm0     = x1 + PI/2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm4, %%ymm4")             /* ymm4     = x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X16_FMA3                                          /* ymm0     = sinf(x1), ymm4 = sinf(x2) */ \
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%ymm12, %%ymm12")            /* ymm12    = fabsf(x1) */ \
            __ASM_EMIT("vdivps          %%ymm13, %%ymm0, %%ymm0")                   /* ymm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x20 + %[LC], %%ymm12, %%ymm2")         /* ymm2     = [ fabsf(x1) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, %%ymm10, %%ymm12, %%ymm1")              /* ymm1     = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x40 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */

        #define LANCZOS_GEN_X64_FUNC_X4_FMA3 \
            /* xmm0 = x1 */ \
            __ASM_EMIT("vmulps          %%xmm11, %%xmm12, %%xmm4")                  /* xmm12    = x2 = x1*a */ \
            __ASM_EMIT("vmulps          %%xmm12, %%xmm4, %%xmm13")                  /* xmm13    = d = x1 * x2 */ \
            __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm12, %%ymm0")               /* ymm0     = x1 x1 x1 x1 x2 x2 x2 x2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm0, %%ymm0")             /* ymm0     = x1 + PI/2 ... x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X8_FMA3                                           /* ymm0     = sinf(x1) ... sinf(x2) */ \
            __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")                        /* xmm4     = sif(x2) */ \
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%ymm12, %%ymm12")            /* ymm12    = fabsf(x1) */ \
            __ASM_EMIT("vdivps          %%ymm13, %%ymm0, %%ymm0")                   /* ymm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x20 + %[LC], %%ymm12, %%ymm2")         /* ymm2     = [ fabsf(x1) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, %%ymm10, %%ymm12, %%ymm1")              /* ymm1     = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x40 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */

        void x64_lanczos1_fma3(float *dst,  float k, float p, float t, float a, size_t count)
        {
            ARCH_X86_64_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%ymm8")                              // ymm8     = k
                __ASM_EMIT("vbroadcastss    %[p], %%ymm9")                              // ymm9     = p
                __ASM_EMIT("vbroadcastss    %[t], %%ymm10")                             // ymm10    = t
                __ASM_EMIT("vbroadcastss    %[a], %%ymm11")                             // ymm11    = a
                __ASM_EMIT("vmovaps         0x00 + %[LGEN], %%ymm14")                   // ymm14    = i = 0 1 2 3 4 5 6 7
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("vmovaps         0x60 + %[LGEN], %%ymm15")                   // ymm15    = step
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%ymm8, %%ymm14, %%ymm12")                  // ymm12    = k*i
                __ASM_EMIT("vaddps          %%ymm15, %%ymm14, %%ymm14")                 // ymm14    = i' = i + step
                __ASM_EMIT("vsubps          %%ymm9, %%ymm12, %%ymm12")                  // ymm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm14, %%xmm12")                  // xmm12    = k*i
                __ASM_EMIT("vaddps          0x80 + %[LGEN], %%xmm14, %%xmm14")          // xmm14    = i' = i + step
                __ASM_EMIT("vsubps          %%xmm9, %%xmm12, %%xmm12")                  // xmm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             8f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm14, %%xmm12")                   // xmm12    = k*i
                __ASM_EMIT("vsubps          %%xmm9, %%xmm12, %%xmm12")                   // xmm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "o" (sinf_const),
                  [LGEN] "o" (kp_gen_const),
                  [LC] "o" (lanczos_const),
                  [k] "m" (k),
                  [p] "m" (p),
                  [a] "m" (a),
                  [t] "m" (t)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

    } /* namespace avx2 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX2_PMATH_LANCZOS_H_ */
