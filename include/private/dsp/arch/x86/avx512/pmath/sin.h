/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 12 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_SIN_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_SIN_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const float kp_gen_const[] __lsp_aligned64 =
            {
                0.0f,  1.0f,  2.0f,   3.0f,  4.0f,  5.0f,  6.0f,  7.0f,     // +0x000: Initial values register 1
                8.0f,  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f,
                16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f,     // +0x040: Initial values register 2
                24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f, 30.0f, 31.0f,
                LSP_DSP_VEC16(32.0),                                        // +0x080: Step x32
                LSP_DSP_VEC16(16.0),                                        // +0x0c0: Step x16
                LSP_DSP_VEC8(8.0),                                          // +0x100: Step x8
                LSP_DSP_VEC4(4.0),                                          // +0x120: Step x4
            };
        )

        IF_ARCH_X86(
            static const uint32_t sinf_const[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x3fc90fdb),      // +0x000:  PI/2
                LSP_DSP_VEC16(0x40490fdb),      // +0x040:  PI
                LSP_DSP_VEC16(0x3e22f983),      // +0x080:  1/(2*PI)
                LSP_DSP_VEC16(0x40c90fdb),      // +0x0c0:  2*PI
                LSP_DSP_VEC16(0x4096cbe4),      // +0x100:  3*PI/2
                LSP_DSP_VEC16(0xb2d7322b),      // +0x140:  C0 = -1/11! = -2.50521083854e-08
                LSP_DSP_VEC16(0x3638ef1d),      // +0x180:  C1 = 1/9! = 2.7557319224e-06
                LSP_DSP_VEC16(0xb9500d01),      // +0x1c0:  C2 = -1/7! = -0.000198412698413
                LSP_DSP_VEC16(0x3c088889),      // +0x200:  C3 = 1/5! = 0.00833333333333
                LSP_DSP_VEC16(0xbe2aaaab),      // +0x240:  C4 = -1/3! = -0.166666666667
                LSP_DSP_VEC16(0x3f800000),      // +0x280:  1.0
            };
        )

        #define SINF_X_PLUS_PI_2_CORE_X32            \
            /* zmm0 = X = x + PI/2 */ \
            __ASM_EMIT("vmulps          0x080 + %[S2C], %%zmm0, %%zmm1")            /* zmm1     = X / (2*PI) */ \
            __ASM_EMIT("vmulps          0x080 + %[S2C], %%zmm4, %%zmm5") \
            __ASM_EMIT("vpsrad          $31, %%zmm0, %%zmm3")                       /* zmm3     = [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vpsrad          $31, %%zmm4, %%zmm7") \
            __ASM_EMIT("vcvttps2dq      %%zmm1, %%zmm1")                            /* zmm1     = int(X / (2*PI)) */ \
            __ASM_EMIT("vcvttps2dq      %%zmm5, %%zmm5") \
            __ASM_EMIT("vpaddd          %%zmm3, %%zmm1, %%zmm1")                    /* zmm1     = int(X / (2*PI)) + [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vpaddd          %%zmm7, %%zmm5, %%zmm5") \
            __ASM_EMIT("vcvtdq2ps       %%zmm1, %%zmm1")                            /* zmm1     = period = int(X / (2*PI)) [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvtdq2ps       %%zmm5, %%zmm5") \
            __ASM_EMIT("vfnmadd231ps    0x0c0 + %[S2C], %%zmm1, %%zmm0")            /* zmm0     = Y = X - period * 2 * PI */ \
            __ASM_EMIT("vfnmadd231ps    0x0c0 + %[S2C], %%zmm5, %%zmm4") \
            __ASM_EMIT("vmovaps         0x100 + %[S2C], %%zmm1")                    /* zmm1     = 3*PI/2 */ \
            __ASM_EMIT("vmovaps         0x100 + %[S2C], %%zmm5") \
            __ASM_EMIT("vcmpps          $1, 0x040 + %[S2C], %%zmm0, %%k1")          /* k1       = [ Y < PI ] */ \
            __ASM_EMIT("vcmpps          $1, 0x040 + %[S2C], %%zmm4, %%k2") \
            __ASM_EMIT("vsubps          0x000 + %[S2C], %%zmm0, %%zmm2")            /* zmm2     = Y - PI/2 */ \
            __ASM_EMIT("vsubps          0x000 + %[S2C], %%zmm4, %%zmm6") \
            __ASM_EMIT("vsubps          %%zmm0, %%zmm1, %%zmm0")                    /* zmm0     = 3*PI/2 - Y */ \
            __ASM_EMIT("vsubps          %%zmm4, %%zmm5, %%zmm4") \
            __ASM_EMIT("vmovaps         %%zmm2, %%zmm0 %{%%k1%}")                   /* zmm0     = XX = [ Y < PI ] ? (Y - PI/2) : (3*PI/2 - Y) */ \
            __ASM_EMIT("vmovaps         %%zmm6, %%zmm4 %{%%k2%}") \
            /* zmm0     = XX */ \
            __ASM_EMIT("vmovaps         0x140 + %[S2C], %%zmm1")                    /* zmm3     = C0 */ \
            __ASM_EMIT("vmovaps         0x140 + %[S2C], %%zmm5") \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm2")                    /* zmm2     = X2 = XX*XX */ \
            __ASM_EMIT("vmulps          %%zmm4, %%zmm4, %%zmm6") \
            __ASM_EMIT("vfmadd213ps     0x180 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = C1 + X2*C0 */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[S2C], %%zmm6, %%zmm5") \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = C2 + X2*(C1 + X2*C0) */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[S2C], %%zmm6, %%zmm5") \
            __ASM_EMIT("vfmadd213ps     0x200 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = C3 + X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[S2C], %%zmm6, %%zmm5") \
            __ASM_EMIT("vfmadd213ps     0x240 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("vfmadd213ps     0x240 + %[S2C], %%zmm6, %%zmm5") \
            __ASM_EMIT("vfmadd213ps     0x280 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = 1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("vfmadd213ps     0x280 + %[S2C], %%zmm6, %%zmm5") \
            __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm0")                    /* zmm0     = sinf(x) = XX * (1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))))) */ \
            __ASM_EMIT("vmulps          %%zmm5, %%zmm4, %%zmm4")

        #define SINF_X_PLUS_PI_2_CORE_X16            \
            /* zmm0 = X = x + PI/2 */ \
            __ASM_EMIT("vmulps          0x080 + %[S2C], %%zmm0, %%zmm1")            /* zmm1     = X / (2*PI) */ \
            __ASM_EMIT("vpsrad          $31, %%zmm0, %%zmm3")                       /* zmm3     = [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvttps2dq      %%zmm1, %%zmm1")                            /* zmm1     = int(X / (2*PI)) */ \
            __ASM_EMIT("vpaddd          %%zmm3, %%zmm1, %%zmm1")                    /* zmm1     = int(X / (2*PI)) + [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvtdq2ps       %%zmm1, %%zmm1")                            /* zmm1     = period = int(X / (2*PI)) [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vfnmadd231ps    0x0c0 + %[S2C], %%zmm1, %%zmm0")            /* zmm0     = Y = X - period * 2 * PI */ \
            __ASM_EMIT("vmovaps         0x100 + %[S2C], %%zmm1")                    /* zmm1     = 3*PI/2 */ \
            __ASM_EMIT("vcmpps          $1, 0x040 + %[S2C], %%zmm0, %%k1")          /* k1       = [ Y < PI ] */ \
            __ASM_EMIT("vsubps          0x000 + %[S2C], %%zmm0, %%zmm2")            /* zmm2     = Y - PI/2 */ \
            __ASM_EMIT("vsubps          %%zmm0, %%zmm1, %%zmm0")                    /* zmm0     = 3*PI/2 - Y */ \
            __ASM_EMIT("vmovaps         %%zmm2, %%zmm0 %{%%k1%}")                   /* zmm0     = XX = [ Y < PI ] ? (Y - PI/2) : (3*PI/2 - Y) */ \
            /* zmm0     = XX */ \
            __ASM_EMIT("vmovaps         0x140 + %[S2C], %%zmm1")                    /* zmm3     = C0 */ \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm2")                    /* zmm2     = X2 = XX*XX */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = C1 + X2*C0 */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = C2 + X2*(C1 + X2*C0) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = C3 + X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("vfmadd213ps     0x240 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("vfmadd213ps     0x280 + %[S2C], %%zmm2, %%zmm1")            /* zmm1     = 1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm0")                    /* zmm0     = sinf(x) = XX * (1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))))) */

        #define SINF_X_PLUS_PI_2_CORE_X8            \
            /* ymm0 = X = x + PI/2 */ \
            __ASM_EMIT("vmulps          0x080 + %[S2C], %%ymm0, %%ymm1")            /* ymm1     = X / (2*PI) */ \
            __ASM_EMIT("vpsrad          $31, %%ymm0, %%ymm3")                       /* ymm3     = [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvttps2dq      %%ymm1, %%ymm1")                            /* ymm1     = int(X / (2*PI)) */ \
            __ASM_EMIT("vpaddd          %%ymm3, %%ymm1, %%ymm1")                    /* ymm1     = int(X / (2*PI)) + [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm1")                            /* ymm1     = period = int(X / (2*PI)) [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vfnmadd231ps    0x0c0 + %[S2C], %%ymm1, %%ymm0")            /* ymm0     = Y = X - period * 2 * PI */ \
            __ASM_EMIT("vmovaps         0x100 + %[S2C], %%ymm1")                    /* ymm1     = 3*PI/2 */ \
            __ASM_EMIT("vcmpps          $1, 0x040 + %[S2C], %%ymm0, %%ymm2")        /* ymm2     = [ Y < PI ] */ \
            __ASM_EMIT("vsubps          %%ymm0, %%ymm1, %%ymm1")                    /* ymm1     = 3*PI/2 - Y */ \
            __ASM_EMIT("vsubps          0x000 + %[S2C], %%ymm0, %%ymm0")            /* ymm0     = Y - PI/2 */ \
            __ASM_EMIT("vblendvps       %%ymm2, %%ymm0, %%ymm1, %%ymm0")            /* ymm0     = XX = [ Y < PI ] ? (Y - PI/2) : (3*PI/2 - Y) */ \
            /* ymm0     = XX */ \
            __ASM_EMIT("vmovaps         0x140 + %[S2C], %%ymm1")                    /* ymm3     = C0 */ \
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm2")                    /* ymm2     = X2 = XX*XX */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[S2C], %%ymm2, %%ymm1")            /* ymm1     = C1 + X2*C0 */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[S2C], %%ymm2, %%ymm1")            /* ymm1     = C2 + X2*(C1 + X2*C0) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[S2C], %%ymm2, %%ymm1")            /* ymm1     = C3 + X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("vfmadd213ps     0x240 + %[S2C], %%ymm2, %%ymm1")            /* ymm1     = C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("vfmadd213ps     0x280 + %[S2C], %%ymm2, %%ymm1")            /* ymm1     = 1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("vmulps          %%ymm1, %%ymm0, %%ymm0")                    /* ymm0     = sinf(x) = XX * (1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))))) */

        #define SINF_X_PLUS_PI_2_CORE_X4            \
            /* xmm0 = X = x + PI/2 */ \
            __ASM_EMIT("vmulps          0x080 + %[S2C], %%xmm0, %%xmm1")            /* xmm1     = X / (2*PI) */ \
            __ASM_EMIT("vpsrad          $31, %%xmm0, %%xmm3")                       /* xmm3     = [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvttps2dq      %%xmm1, %%xmm1")                            /* xmm1     = int(X / (2*PI)) */ \
            __ASM_EMIT("vpaddd          %%xmm3, %%xmm1, %%xmm1")                    /* xmm1     = int(X / (2*PI)) + [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vcvtdq2ps       %%xmm1, %%xmm1")                            /* xmm1     = period = int(X / (2*PI)) [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("vfnmadd231ps    0x0c0 + %[S2C], %%xmm1, %%xmm0")            /* xmm0     = Y = X - period * 2 * PI */ \
            __ASM_EMIT("vmovaps         0x100 + %[S2C], %%xmm1")                    /* xmm1     = 3*PI/2 */ \
            __ASM_EMIT("vcmpps          $1, 0x040 + %[S2C], %%xmm0, %%xmm2")        /* xmm2     = [ Y < PI ] */ \
            __ASM_EMIT("vsubps          %%xmm0, %%xmm1, %%xmm1")                    /* xmm1     = 3*PI/2 - Y */ \
            __ASM_EMIT("vsubps          0x000 + %[S2C], %%xmm0, %%xmm0")            /* xmm0     = Y - PI/2 */ \
            __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm1, %%xmm0")            /* xmm0     = XX = [ Y < PI ] ? (Y - PI/2) : (3*PI/2 - Y) */ \
            /* xmm0     = XX */ \
            __ASM_EMIT("vmovaps         0x140 + %[S2C], %%xmm1")                    /* xmm3     = C0 */ \
            __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm2")                    /* xmm2     = X2 = XX*XX */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[S2C], %%xmm2, %%xmm1")            /* xmm1     = C1 + X2*C0 */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[S2C], %%xmm2, %%xmm1")            /* xmm1     = C2 + X2*(C1 + X2*C0) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[S2C], %%xmm2, %%xmm1")            /* xmm1     = C3 + X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("vfmadd213ps     0x240 + %[S2C], %%xmm2, %%xmm1")            /* xmm1     = C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("vfmadd213ps     0x280 + %[S2C], %%xmm2, %%xmm1")            /* xmm1     = 1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("vmulps          %%xmm1, %%xmm0, %%xmm0")                    /* xmm0     = sinf(x) = XX * (1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))))) */

        void sinf1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%zmm1")        // zmm1     = PI/2
                __ASM_EMIT("vaddps          0x00(%[dst]), %%zmm1, %%zmm0")  // zmm0     = x1 + PI/2
                __ASM_EMIT("vaddps          0x40(%[dst]), %%zmm1, %%zmm4")  // zmm4     = x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%zmm1")        // zmm1     = PI/2
                __ASM_EMIT("vaddps          0x00(%[dst]), %%zmm1, %%zmm0")  // zmm0     = x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%ymm1")        // ymm1     = PI/2
                __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm1, %%ymm0")  // ymm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%xmm1")        // xmm1     = PI/2
                __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm1, %%xmm0")  // xmm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("8:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             16f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("12:")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%xmm1")        // xmm1     = PI/2
                __ASM_EMIT("vaddps          %%xmm0, %%xmm1, %%xmm0")        // xmm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("14:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("16:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [S2C] "o" (sinf_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k1", "%k2"
            );
        }

        void sinf2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%zmm1")        // zmm1     = PI/2
                __ASM_EMIT("vaddps          0x00(%[src]), %%zmm1, %%zmm0")  // zmm0     = x1 + PI/2
                __ASM_EMIT("vaddps          0x40(%[src]), %%zmm1, %%zmm4")  // zmm4     = x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%zmm1")        // zmm1     = PI/2
                __ASM_EMIT("vaddps          0x00(%[src]), %%zmm1, %%zmm0")  // zmm0     = x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%ymm1")        // ymm1     = PI/2
                __ASM_EMIT("vaddps          0x00(%[src]), %%ymm1, %%ymm0")  // ymm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%xmm1")        // xmm1     = PI/2
                __ASM_EMIT("vaddps          0x00(%[src]), %%xmm1, %%xmm0")  // xmm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("8:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             16f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("12:")
                __ASM_EMIT("vmovaps         0x000 + %[S2C], %%xmm1")        // xmm1     = PI/2
                __ASM_EMIT("vaddps          %%xmm0, %%xmm1, %%xmm0")        // xmm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("14:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("16:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [S2C] "o" (sinf_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k1", "%k2"
            );
        }

        void sinf_kp1(float *dst, float k, float p, size_t count)
        {
            ARCH_X86_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%zmm6")                      // zmm6     = k
                __ASM_EMIT("vbroadcastss    %[p], %%zmm7")                      // zmm7     = p
                __ASM_EMIT("vmovaps         0x000 + %[S2KP], %%zmm4")           // zmm4     = i = 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
                __ASM_EMIT("vmovaps         0x0c0 + %[S2KP], %%zmm5")           // zmm5     = step = 16 ...
                __ASM_EMIT("vaddps          0x000 + %[S2C], %%zmm7, %%zmm7")    // zmm7     = p+PI/2 ...
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%zmm4, %%zmm6, %%zmm0")            // zmm0     = k*i
                __ASM_EMIT("vaddps          %%zmm5, %%zmm4, %%zmm4")            // zmm4     = i + step
                __ASM_EMIT("vaddps          %%zmm7, %%zmm0, %%zmm0")            // zmm0     = k*i + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%ymm4, %%ymm6, %%ymm0")            // ymm0     = k*i
                __ASM_EMIT("vaddps          0x100 + %[S2KP], %%ymm4, %%ymm4")   // ymm4     = i + step
                __ASM_EMIT("vaddps          %%ymm7, %%ymm0, %%ymm0")            // ymm0     = k*i + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm6, %%xmm0")            // xmm0     = k*i
                __ASM_EMIT("vaddps          0x120 + %[S2KP], %%xmm4, %%xmm4")   // xmm4     = i + step
                __ASM_EMIT("vaddps          %%xmm7, %%xmm0, %%xmm0")            // xmm0     = k*i + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             10f")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm6, %%xmm0")            // xmm0     = k*i
                __ASM_EMIT("vaddps          %%xmm7, %%xmm0, %%xmm0")            // xmm0     = k*i + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
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
                  [S2KP] "o" (kp_gen_const),
                  [k] "m" (k),
                  [p] "m" (p)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k1", "%k2"
            );
        }

        void x64_sinf_kp1(float *dst, float k, float p, size_t count)
        {
            ARCH_X86_64_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%zmm11")                         // zmm11     = k
                __ASM_EMIT("vbroadcastss    %[p], %%zmm12")                         // zmm12     = p
                __ASM_EMIT("vmovaps         0x000 + %[S2KP], %%zmm8")               // zmm8      = i0 = 0 1 2 3 ...
                __ASM_EMIT("vmovaps         0x040 + %[S2KP], %%zmm9")               // zmm9      = i1 = 16 17 18 19 ...
                __ASM_EMIT("vmovaps         0x080 + %[S2KP], %%zmm10")              // zmm10     = step = 32 ...
                __ASM_EMIT("vaddps          0x000 + %[S2C], %%zmm12, %%zmm12")      // zmm12     = p+PI/2 ...
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%zmm8, %%zmm11, %%zmm0")               // zmm0     = k*i0
                __ASM_EMIT("vmulps          %%zmm9, %%zmm11, %%zmm4")               // zmm4     = k*i1
                __ASM_EMIT("vaddps          %%zmm10, %%zmm8, %%zmm8")               // zmm8     = i0 + step
                __ASM_EMIT("vaddps          %%zmm10, %%zmm9, %%zmm9")               // zmm9     = i1 + step
                __ASM_EMIT("vaddps          %%zmm12, %%zmm0, %%zmm0")               // zmm0     = k*i0 + p + PI/2
                __ASM_EMIT("vaddps          %%zmm12, %%zmm4, %%zmm4")               // zmm4     = k*i1 + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%zmm8, %%zmm11, %%zmm0")               // zmm0     = k*i0
                __ASM_EMIT("vaddps          0x0c0 + %[S2KP], %%zmm8, %%zmm8")       // zmm8     = i0 + step
                __ASM_EMIT("vaddps          %%zmm12, %%zmm0, %%zmm0")               // zmm0     = k*i0 + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          %%ymm8, %%ymm11, %%ymm0")               // ymm0     = k*i0
                __ASM_EMIT("vaddps          0x100 + %[S2KP], %%ymm8, %%ymm8")       // ymm8     = i0 + step
                __ASM_EMIT("vaddps          %%ymm12, %%ymm0, %%ymm0")               // ymm0     = k*i0 + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm11, %%xmm0")               // xmm0     = k*i0
                __ASM_EMIT("vaddps          0x120 + %[S2KP], %%xmm8, %%xmm8")       // xmm8     = i0 + step
                __ASM_EMIT("vaddps          %%xmm12, %%xmm0, %%xmm0")               // xmm0     = k*i0 + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("8:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm11, %%xmm0")               // xmm0     = k*i0
                __ASM_EMIT("vaddps          %%xmm12, %%xmm0, %%xmm0")               // xmm0     = k*i0 + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "o" (sinf_const),
                  [S2KP] "o" (kp_gen_const),
                  [k] "m" (k),
                  [p] "m" (p)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12",
                  "%k1", "%k2"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_SIN_H_ */
