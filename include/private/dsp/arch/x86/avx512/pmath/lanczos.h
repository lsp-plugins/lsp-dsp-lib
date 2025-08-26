/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 18 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LANCZOS_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LANCZOS_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

#include <private/dsp/arch/x86/avx512/pmath/sin.h>

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t lanczos_const[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff),                  // +0x00: Mask for fabsf
                LSP_DSP_VEC16(0x38d1b717),                  // +0x40: Sinc threshold = 1e-4
                LSP_DSP_VEC16(0x3f800000),                  // +0x80: 1.0
            };
        )

        typedef struct lanczos_gen_t
        {
            float k[16];                        // +0x000: Multiplier
            float p[16];                        // +0x040: Initial phase
            float a[16];                        // +0x080: Number of lobes
            float i[16];                        // +0x0c0: Indices
            float x1[16];                       // +0x100: Computed X1
            float d[16];                        // +0x140: Denominator
            float t[16];                        // +0x180: Time range
        } lanczos_gen_t;

        #define LANCZOS_GEN_FUNC_X16 \
            /* zmm0 = x1 */ \
            __ASM_EMIT("vmulps          0x080 + %[state], %%zmm0, %%zmm4")          /* zmm4     = x2 = x1*a */ \
            __ASM_EMIT("vmovaps         %%zmm0, 0x100 + %[state]")                  /* *x1      = x1 */ \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm4, %%zmm2")                    /* zmm2     = d = x1 * x2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%zmm0, %%zmm0")             /* zmm0     = x1 + PI/2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%zmm4, %%zmm4")             /* zmm4     = x2 + PI/2 */ \
            __ASM_EMIT("vmovaps         %%zmm2, 0x140 + %[state]")                  /* *d       = d */ \
            SINF_X_PLUS_PI_2_CORE_X32                                               /* zmm0     = sinf(x1), zmm4 = sinf(x2) */ \
            __ASM_EMIT("vmovaps         0x100 + %[state], %%zmm1")                  /* zmm1     = x1 */ \
            __ASM_EMIT("vmulps          %%zmm4, %%zmm0, %%zmm0")                    /* zmm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%zmm1, %%zmm1")              /* zmm1     = fabsf(x) */ \
            __ASM_EMIT("vdivps          0x140 + %[state], %%zmm0, %%zmm0")          /* zmm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x40 + %[LC], %%zmm1, %%k1")            /* k1       = [ fabsf(x) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $5, 0x180 + %[state], %%zmm1, %%k2")        /* k2       = [ fabsf(x) >= t ] */ \
            __ASM_EMIT("vmovaps         0x80 + %[LC], %%zmm0 %{%%k1%}")             /* zmm0     = [ fabsf(x) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vxorps          %%zmm0, %%zmm0, %%zmm0 %{%%k2%}")           /* zmm0     = [ fabsf(x) < t ] ? ([ fabsf(x) >= 1e-4 ] ? f : 1.0) : 0.0 */

        #define LANCZOS_GEN_FUNC_X8 \
            /* ymm0 = x1 */ \
            __ASM_EMIT("vmulps          0x080 + %[state], %%ymm0, %%ymm4")          /* ymm4     = x2 = x1*a */ \
            __ASM_EMIT("vmovaps         %%ymm0, 0x100 + %[state]")                  /* *x1      = x1 */ \
            __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm2")                    /* ymm2     = d = x1 * x2 */ \
            __ASM_EMIT("vinsertf64x4    $1, %%ymm4, %%zmm0, %%zmm0")                /* zmm0     = x1 ... x2 ... */ \
            __ASM_EMIT("vmovaps         %%ymm2, 0x140 + %[state]")                  /* *d       = d */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%zmm0, %%zmm0")             /* zmm0     = x1 + PI/2 ... x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X16                                               /* ymm0     = sinf(x1), ymm4 = sinf(x2) */ \
            __ASM_EMIT("vextractf64x4   $1, %%zmm0, %%ymm4")                        /* ymm4     = sinf(x2) */ \
            __ASM_EMIT("vmovaps         0x100 + %[state], %%ymm1")                  /* ymm1     = x1 */ \
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%ymm1, %%ymm1")              /* ymm1     = fabsf(x) */ \
            __ASM_EMIT("vdivps          0x140 + %[state], %%ymm0, %%ymm0")          /* ymm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x40 + %[LC], %%ymm1, %%ymm2")          /* ymm2     = [ fabsf(x) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, 0x180 + %[state], %%ymm1, %%ymm1")      /* ymm1     = [ fabsf(x) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x80 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = [ fabsf(x) < t ] ? ([ fabsf(x) >= 1e-4 ] ? f : 1.0) : 0.0 */

        #define LANCZOS_GEN_FUNC_X4 \
            /* xmm0 = x1 */ \
            __ASM_EMIT("vmulps          0x080 + %[state], %%xmm0, %%xmm4")          /* xmm4     = x2 = x1*a */ \
            __ASM_EMIT("vmovaps         %%xmm0, 0x100 + %[state]")                  /* *x1      = x1 */ \
            __ASM_EMIT("vmulps          %%xmm0, %%xmm4, %%xmm2")                    /* xmm2     = d = x1 * x2 */ \
            __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm0, %%ymm0")                /* ymm0     = x1 x1 x1 x1 x2 x2 x2 x2 */ \
            __ASM_EMIT("vmovaps         %%xmm2, 0x140 + %[state]")                  /* *d       = d */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%ymm0, %%ymm0")             /* xmm0     = x1 + PI/2 ... x2 + PI/2 ... */ \
            SINF_X_PLUS_PI_2_CORE_X8                                                /* ymm0     = sinf(x1) ... sinf(x2) ... */ \
            __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")                        /* xmm4     = sinf(x2) */ \
            __ASM_EMIT("vmovaps         0x100 + %[state], %%xmm1")                  /* xmm1     = x1 */ \
            __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")                    /* xmm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%xmm1, %%xmm1")              /* xmm1     = fabsf(x) */ \
            __ASM_EMIT("vdivps          0x140 + %[state], %%xmm0, %%xmm0")          /* xmm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x40 + %[LC], %%xmm1, %%xmm2")          /* xmm2     = [ fabsf(x) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, 0x180 + %[state], %%xmm1, %%xmm1")      /* xmm1     = [ fabsf(x) < t ] */ \
            __ASM_EMIT("vblendvps       %%xmm2, 0x80 + %[LC], %%xmm0, %%xmm0")      /* xmm0     = [ fabsf(x) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%xmm1, %%xmm0, %%xmm0")                    /* xmm0     = [ fabsf(x) < t ] ? ([ fabsf(x) >= 1e-4 ] ? f : 1.0) : 0.0 */

        void lanczos1(float *dst,  float k, float p, float t, float a, size_t count)
        {
            IF_ARCH_X86(
                lanczos_gen_t state __lsp_aligned64;
                float stub[16] __lsp_aligned64;
            );

            ARCH_X86_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%zmm4")                              // zmm4     = k
                __ASM_EMIT("vbroadcastss    %[p], %%zmm5")                              // zmm5     = p
                __ASM_EMIT("vbroadcastss    %[a], %%zmm6")                              // zmm6     = a
                __ASM_EMIT("vbroadcastss    %[t], %%zmm7")                              // zmm7     = t
                __ASM_EMIT("vmovaps         %%zmm4, 0x000 + %[state]")
                __ASM_EMIT("vmovaps         %%zmm5, 0x040 + %[state]")
                __ASM_EMIT("vmovaps         %%zmm6, 0x080 + %[state]")
                __ASM_EMIT("vmovaps         %%zmm7, 0x180 + %[state]")
                __ASM_EMIT("vmovaps         0x00 + %[LGEN], %%zmm1")                    // zmm1     = i = 0 1 2 3 4 5 6 7 ...
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x000 + %[state], %%zmm1, %%zmm0")          // zmm0     = k*i
                __ASM_EMIT("vaddps          0x0c0 + %[LGEN], %%zmm1, %%zmm1")           // zmm1     = i' = i + step
                __ASM_EMIT("vsubps          0x040 + %[state], %%zmm0, %%zmm0")          // zmm0     = x1 = k*i - p
                __ASM_EMIT("vmovaps         %%zmm1, 0x0c0 + %[state]")                  // *i       = i'
                LANCZOS_GEN_FUNC_X16
                __ASM_EMIT("vmovaps         0x0c0 + %[state], %%zmm1")                  // zmm1     = i'
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x000 + %[state], %%ymm1, %%ymm0")          // ymm0     = k*i
                __ASM_EMIT("vaddps          0x100 + %[LGEN], %%ymm1, %%ymm1")           // ymm1     = i' = i + step
                __ASM_EMIT("vsubps          0x040 + %[state], %%ymm0, %%ymm0")          // ymm0     = x1 = k*i - p
                __ASM_EMIT("vmovaps         %%ymm1, 0x0c0 + %[state]")                  // *i       = i'
                LANCZOS_GEN_FUNC_X8
                __ASM_EMIT("vmovaps         0x0c0 + %[state], %%ymm1")                  // ymm1     = i'
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          0x000 + %[state], %%xmm1, %%xmm0")          // xmm0     = k*i
                __ASM_EMIT("vaddps          0x120 + %[LGEN], %%xmm1, %%xmm1")           // xmm1     = i' = i + step
                __ASM_EMIT("vsubps          0x040 + %[state], %%xmm0, %%xmm0")          // xmm0     = x1 = k*i - p
                __ASM_EMIT("vmovaps         %%xmm1, 0x0c0 + %[state]")                  // *i       = i'
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("vmovaps         0x0c0 + %[state], %%xmm1")                  // xmm1     = i'
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             10f")
                __ASM_EMIT("vmulps          0x000 + %[state], %%xmm1, %%xmm0")          // xmm0     = k*i
                __ASM_EMIT("vsubps          0x040 + %[state], %%xmm0, %%xmm0")          // xmm0     = x1 = k*i - p
                LANCZOS_GEN_FUNC_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("10:")

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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k1", "%k2"
            );
        }

        #define LANCZOS_GEN_X64_FUNC_X16 \
            /* zmm8     = k */ \
            /* zmm9     = p */ \
            /* zmm10    = t */ \
            /* zmm11    = a */ \
            /* zmm12    = x1 */ \
            /* zmm13    = d */ \
            /* zmm14    = i */ \
            /* zmm15    = step */ \
            __ASM_EMIT("vmulps          %%zmm11, %%zmm12, %%zmm4")                  /* zmm12    = x2 = x1*a */ \
            __ASM_EMIT("vmulps          %%zmm12, %%zmm4, %%zmm13")                  /* zmm13    = d = x1 * x2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%zmm12, %%zmm0")            /* zmm0     = x1 + PI/2 */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%zmm4, %%zmm4")             /* zmm4     = x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X32                                               /* zmm0     = sinf(x1), zmm4 = sinf(x2) */ \
            __ASM_EMIT("vmulps          %%zmm4, %%zmm0, %%zmm0")                    /* zmm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%zmm12, %%zmm12")            /* zmm12    = fabsf(x1) */ \
            __ASM_EMIT("vdivps          %%zmm13, %%zmm0, %%zmm0")                   /* zmm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x40 + %[LC], %%zmm12, %%k1")           /* k1       = [ fabsf(x1) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $5, %%zmm10, %%zmm1, %%k2")                 /* k2       = [ fabsf(x1) >= t ] */ \
            __ASM_EMIT("vmovaps         0x80 + %[LC], %%zmm0 %{%%k1%}")             /* zmm0     = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vxorps          %%zmm0, %%zmm0, %%zmm0 %{%%k2%}")           /* zmm0     = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */

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
            __ASM_EMIT("vinsertf64x4    $1, %%ymm4, %%zmm12, %%zmm0")               /* zmm0     = x1 ... x2 ... */ \
            __ASM_EMIT("vaddps          0x00 + %[S2C], %%zmm0, %%zmm0")             /* zmm0     = x1 + PI/2 ... x2 + PI/2 */ \
            SINF_X_PLUS_PI_2_CORE_X16                                               /* ymm0     = sinf(x1), ymm4 = sinf(x2) */ \
            __ASM_EMIT("vextractf64x4   $1, %%zmm0, %%ymm4")                        /* ymm4     = sinf(x2) */ \
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x1)*sinf(x2) */ \
            __ASM_EMIT("vandps          0x00 + %[LC], %%ymm12, %%ymm12")            /* ymm12    = fabsf(x1) */ \
            __ASM_EMIT("vdivps          %%ymm13, %%ymm0, %%ymm0")                   /* ymm0     = F = (sinf(x1)*sinf(x2)) / (x1 * x2) */ \
            __ASM_EMIT("vcmpps          $1, 0x40 + %[LC], %%ymm12, %%ymm2")         /* ymm2     = [ fabsf(x1) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, %%ymm10, %%ymm12, %%ymm1")              /* ymm1     = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x80 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
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
            __ASM_EMIT("vcmpps          $1, 0x40 + %[LC], %%ymm12, %%ymm2")         /* ymm2     = [ fabsf(x1) < 1e-4 ] */ \
            __ASM_EMIT("vcmpps          $1, %%ymm10, %%ymm12, %%ymm1")              /* ymm1     = [ fabsf(x1) < t ] */ \
            __ASM_EMIT("vblendvps       %%ymm2, 0x80 + %[LC], %%ymm0, %%ymm0")      /* ymm0     = [ fabsf(x1) >= 1e-4 ] ? f : 1.0 */ \
            __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = [ fabsf(x1) < t ] ? ([ fabsf(x1) >= 1e-4 ] ? f : 1.0) : 0.0 */

        void x64_lanczos1(float *dst,  float k, float p, float t, float a, size_t count)
        {
            ARCH_X86_64_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%zmm8")                              // zmm8     = k
                __ASM_EMIT("vbroadcastss    %[p], %%zmm9")                              // zmm9     = p
                __ASM_EMIT("vbroadcastss    %[t], %%zmm10")                             // zmm10    = t
                __ASM_EMIT("vbroadcastss    %[a], %%zmm11")                             // zmm11    = a
                __ASM_EMIT("vmovaps         0x00 + %[LGEN], %%zmm14")                   // zmm14    = i = 0 1 2 3 4 5 6 7 ...
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("vmovaps         0xc0 + %[LGEN], %%zmm15")                   // zmm15    = step
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%zmm8, %%zmm14, %%zmm12")                  // zmm12    = k*i
                __ASM_EMIT("vaddps          %%zmm15, %%zmm14, %%zmm14")                 // zmm14    = i' = i + step
                __ASM_EMIT("vsubps          %%zmm9, %%zmm12, %%zmm12")                  // zmm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%ymm8, %%ymm14, %%ymm12")                  // ymm12    = k*i
                __ASM_EMIT("vaddps          0x100 + %[LGEN], %%ymm14, %%ymm14")         // ymm14    = i' = i + step
                __ASM_EMIT("vsubps          %%ymm9, %%ymm12, %%ymm12")                  // ymm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm14, %%xmm12")                  // xmm12    = k*i
                __ASM_EMIT("vaddps          0x120 + %[LGEN], %%xmm14, %%xmm14")         // xmm14    = i' = i + step
                __ASM_EMIT("vsubps          %%xmm9, %%xmm12, %%xmm12")                  // xmm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             10f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm14, %%xmm12")                   // xmm12    = k*i
                __ASM_EMIT("vsubps          %%xmm9, %%xmm12, %%xmm12")                   // xmm12    = x1 = k*i - p
                LANCZOS_GEN_X64_FUNC_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("10:")

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

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LANCZOS_H_ */
