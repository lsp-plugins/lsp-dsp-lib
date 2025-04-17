/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_PMATH_SIN_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_PMATH_SIN_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE2_IMPL */

namespace lsp
{
    namespace sse2
    {
        /*
            Time-optimized sinf implementation:

            float sinf(float x)
            {
                // Limit x to [-pi/2 .. pi/2]
                x              += PI/2;
                int32_t period  = (x / 2*PI) - (x < 0.0f);
                x               = x - period * 2*PI;
                x               = (x < PI) ? x - PI/2 : 3*PI/2 - x;

                const float x2 = x*x;
                return x * (1.0f + x2*(-0.166666666667f + x2*(0.00833333333333f + x2 * (-0.000198412698413f + x2 * (2.7557319224e-06f + x2 * -2.50521083854e-08f)))));
            }
        */

        IF_ARCH_X86(
            static const float kp_gen_const[] __lsp_aligned16 =
            {
                0.0f, 1.0f, 2.0f, 3.0f,         // +0x00: Initial values 0..3
                LSP_DSP_VEC4(4.0f)              // +0x10: Step
            };
        )

        IF_ARCH_X86(
            static const uint32_t sinf_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x3fc90fdb),       // +0x00:   PI/2
                LSP_DSP_VEC4(0x40490fdb),       // +0x10:   PI
                LSP_DSP_VEC4(0x3e22f983),       // +0x20:   1/(2*PI)
                LSP_DSP_VEC4(0x40c90fdb),       // +0x30:   2*PI
                LSP_DSP_VEC4(0x4096cbe4),       // +0x40:   3*PI/2
                LSP_DSP_VEC4(0xb2d7322b),       // +0x50:   C0 = -1/11! = -2.50521083854e-08
                LSP_DSP_VEC4(0x3638ef1d),       // +0x60:   C1 = 1/9! = 2.7557319224e-06
                LSP_DSP_VEC4(0xb9500d01),       // +0x70:   C2 = -1/7! = -0.000198412698413
                LSP_DSP_VEC4(0x3c088889),       // +0x80:   C3 = 1/5! = 0.00833333333333
                LSP_DSP_VEC4(0xbe2aaaab),       // +0x90:   C4 = -1/3! = -0.166666666667
                LSP_DSP_VEC4(0x3f800000),       // +0xa0:   1.0
            };
        )

        #define SINF_X_PLUS_PI_2_CORE_X8              \
            /* xmm0 = X1 = x1 + PI/2 */               \
            /* xmm4 = X2 = x2 + PI/2 */               \
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")                /* xmm1     = X */ \
            __ASM_EMIT("movaps          %%xmm4, %%xmm5") \
            __ASM_EMIT("movaps          %%xmm0, %%xmm3")                /* xmm3     = X */ \
            __ASM_EMIT("movaps          %%xmm4, %%xmm7") \
            __ASM_EMIT("mulps           0x20 + %[S2C], %%xmm1")         /* xmm1     = X / (2*PI) */ \
            __ASM_EMIT("mulps           0x20 + %[S2C], %%xmm5") \
            __ASM_EMIT("psrad           $31, %%xmm3")                   /* xmm3     = [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("psrad           $31, %%xmm7") \
            __ASM_EMIT("cvttps2dq       %%xmm1, %%xmm1")                /* xmm1     = int(X / (2*PI)) */ \
            __ASM_EMIT("cvttps2dq       %%xmm5, %%xmm5") \
            __ASM_EMIT("paddd           %%xmm3, %%xmm1")                /* xmm1     = int(X / (2*PI)) + [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("paddd           %%xmm7, %%xmm5") \
            __ASM_EMIT("cvtdq2ps        %%xmm1, %%xmm1")                /* xmm1     = period = int(X / (2*PI)) [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("cvtdq2ps        %%xmm5, %%xmm5") \
            __ASM_EMIT("mulps           0x30 + %[S2C], %%xmm1")         /* xmm1     = period * 2 * PI */ \
            __ASM_EMIT("mulps           0x30 + %[S2C], %%xmm5") \
            __ASM_EMIT("subps           %%xmm1, %%xmm0")                /* xmm0     = Y = X - period * 2 * PI */ \
            __ASM_EMIT("subps           %%xmm5, %%xmm4") \
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")                /* xmm2     = Y */ \
            __ASM_EMIT("movaps          %%xmm4, %%xmm6") \
            __ASM_EMIT("movaps          0x40 + %[S2C], %%xmm1")         /* xmm1     = 3*PI/2 */ \
            __ASM_EMIT("movaps          0x40 + %[S2C], %%xmm5") \
            __ASM_EMIT("cmpps           $1, 0x10 + %[S2C], %%xmm2")     /* xmm2     = [ Y < PI ] */ \
            __ASM_EMIT("cmpps           $1, 0x10 + %[S2C], %%xmm6") \
            __ASM_EMIT("subps           %%xmm0, %%xmm1")                /* xmm1     = 3*PI/2 - Y */ \
            __ASM_EMIT("subps           %%xmm4, %%xmm5") \
            __ASM_EMIT("subps           0x00 + %[S2C], %%xmm0")         /* xmm0     = Y - PI/2 */ \
            __ASM_EMIT("subps           0x00 + %[S2C], %%xmm4") \
            __ASM_EMIT("andps           %%xmm2, %%xmm0")                /* xmm0     = (Y - PI/2) & [ Y < PI ] */ \
            __ASM_EMIT("andps           %%xmm6, %%xmm4") \
            __ASM_EMIT("andnps          %%xmm1, %%xmm2")                /* xmm2     = (3*PI/2 - Y) & [ Y >= PI ] */ \
            __ASM_EMIT("andnps          %%xmm5, %%xmm6") \
            __ASM_EMIT("orps            %%xmm2, %%xmm0")                /* xmm0     = XX = [ Y < PI ] ? (Y - PI/2) : (3*PI/2 - Y) */ \
            __ASM_EMIT("orps            %%xmm6, %%xmm4") \
            /* xmm0     = XX1 */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")                /* xmm1     = XX */ \
            __ASM_EMIT("movaps          %%xmm4, %%xmm5") \
            __ASM_EMIT("mulps           %%xmm0, %%xmm0")                /* xmm0     = X2 = XX*XX */ \
            __ASM_EMIT("mulps           %%xmm4, %%xmm4") \
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")                /* xmm2     = X2 */ \
            __ASM_EMIT("movaps          %%xmm4, %%xmm6") \
            __ASM_EMIT("mulps           0x50 + %[S2C], %%xmm0")         /* xmm0     = X2*C0 */ \
            __ASM_EMIT("mulps           0x50 + %[S2C], %%xmm4") \
            __ASM_EMIT("addps           0x60 + %[S2C], %%xmm0")         /* xmm0     = C1 + X2*C0 */ \
            __ASM_EMIT("addps           0x60 + %[S2C], %%xmm4") \
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                /* xmm0     = X2*(C1 + X2*C0) */ \
            __ASM_EMIT("mulps           %%xmm6, %%xmm4") \
            __ASM_EMIT("addps           0x70 + %[S2C], %%xmm0")         /* xmm0     = C2 + X2*(C1 + X2*C0) */ \
            __ASM_EMIT("addps           0x70 + %[S2C], %%xmm4") \
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                /* xmm0     = X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("mulps           %%xmm6, %%xmm4") \
            __ASM_EMIT("addps           0x80 + %[S2C], %%xmm0")         /* xmm0     = C3 + X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("addps           0x80 + %[S2C], %%xmm4") \
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                /* xmm0     = X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("mulps           %%xmm6, %%xmm4") \
            __ASM_EMIT("addps           0x90 + %[S2C], %%xmm0")         /* xmm0     = C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("addps           0x90 + %[S2C], %%xmm4") \
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                /* xmm0     = X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("mulps           %%xmm6, %%xmm4") \
            __ASM_EMIT("addps           0xa0 + %[S2C], %%xmm0")         /* xmm0     = 1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("addps           0xa0 + %[S2C], %%xmm4") \
            __ASM_EMIT("mulps           %%xmm1, %%xmm0")                /* xmm0     = sinf(x) = XX * (1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))))) */ \
            __ASM_EMIT("mulps           %%xmm5, %%xmm4")

        #define SINF_X_PLUS_PI_2_CORE_X4              \
            /* xmm0 = X = x + PI/2 */               \
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")                /* xmm1     = X */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm3")                /* xmm3     = X */ \
            __ASM_EMIT("mulps           0x20 + %[S2C], %%xmm1")         /* xmm1     = X / (2*PI) */ \
            __ASM_EMIT("psrad           $31, %%xmm3")                   /* xmm3     = [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("cvttps2dq       %%xmm1, %%xmm1")                /* xmm1     = int(X / (2*PI)) */ \
            __ASM_EMIT("paddd           %%xmm3, %%xmm1")                /* xmm1     = int(X / (2*PI)) + [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("cvtdq2ps        %%xmm1, %%xmm1")                /* xmm1     = period = int(X / (2*PI)) [ X < 0.0 ] ? -1 : 0 */ \
            __ASM_EMIT("mulps           0x30 + %[S2C], %%xmm1")         /* xmm1     = period * 2 * PI */ \
            __ASM_EMIT("subps           %%xmm1, %%xmm0")                /* xmm0     = Y = X - period * 2 * PI */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")                /* xmm2     = Y */ \
            __ASM_EMIT("movaps          0x40 + %[S2C], %%xmm1")         /* xmm1     = 3*PI/2 */ \
            __ASM_EMIT("cmpps           $1, 0x10 + %[S2C], %%xmm2")     /* xmm2     = [ Y < PI ] */ \
            __ASM_EMIT("subps           %%xmm0, %%xmm1")                /* xmm1     = 3*PI/2 - Y */ \
            __ASM_EMIT("subps           0x00 + %[S2C], %%xmm0")         /* xmm0     = Y - PI/2 */ \
            __ASM_EMIT("andps           %%xmm2, %%xmm0")                /* xmm0     = (Y - PI/2) & [ Y < PI ] */ \
            __ASM_EMIT("andnps          %%xmm1, %%xmm2")                /* xmm2     = (3*PI/2 - Y) & [ Y >= PI ] */ \
            __ASM_EMIT("orps            %%xmm2, %%xmm0")                /* xmm0     = XX = [ Y < PI ] ? (Y - PI/2) : (3*PI/2 - Y) */ \
            /* xmm0     = XX1 */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")                /* xmm1     = XX */ \
            __ASM_EMIT("mulps           %%xmm0, %%xmm0")                /* xmm0     = X2 = XX*XX */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")                /* xmm2     = X2 */ \
            __ASM_EMIT("mulps           0x50 + %[S2C], %%xmm0")         /* xmm0     = X2*C0 */ \
            __ASM_EMIT("addps           0x60 + %[S2C], %%xmm0")         /* xmm0     = C1 + X2*C0 */ \
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                /* xmm0     = X2*(C1 + X2*C0) */ \
            __ASM_EMIT("addps           0x70 + %[S2C], %%xmm0")         /* xmm0     = C2 + X2*(C1 + X2*C0) */ \
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                /* xmm0     = X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("addps           0x80 + %[S2C], %%xmm0")         /* xmm0     = C3 + X2*(C2 + X2*(C1 + X2*C0)) */ \
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                /* xmm0     = X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("addps           0x90 + %[S2C], %%xmm0")         /* xmm0     = C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))) */ \
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                /* xmm0     = X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("addps           0xa0 + %[S2C], %%xmm0")         /* xmm0     = 1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0)))) */ \
            __ASM_EMIT("mulps           %%xmm1, %%xmm0")                /* xmm0     = sinf(x) = XX * (1.0 + X2*(C4 + X2*(C3 + X2*(C2 + X2*(C1 + X2*C0))))) */

        void sinf1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movaps          0x00 + %[S2C], %%xmm1")         // xmm1     = PI/2
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")          // xmm0     = x1
                __ASM_EMIT("movups          0x10(%[dst]), %%xmm4")          // xmm4     = x2
                __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0     = x1 + PI/2
                __ASM_EMIT("addps           %%xmm1, %%xmm4")                // xmm4     = x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")          // xmm0     = x
                __ASM_EMIT("addps           0x00 + %[S2C], %%xmm0")         // xmm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
                __ASM_EMIT("8:")
                __ASM_EMIT("addps           0x00 + %[S2C], %%xmm0")         // xmm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [S2C] "o" (sinf_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sinf2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movaps          0x00 + %[S2C], %%xmm1")         // xmm1     = PI/2
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")          // xmm0     = x1
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")          // xmm4     = x2
                __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0     = x1 + PI/2
                __ASM_EMIT("addps           %%xmm1, %%xmm4")                // xmm4     = x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")          // xmm0     = x
                __ASM_EMIT("addps           0x00 + %[S2C], %%xmm0")         // xmm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
                __ASM_EMIT("8:")
                __ASM_EMIT("addps           0x00 + %[S2C], %%xmm0")         // xmm0     = x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [S2C] "o" (sinf_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sinf_kp1(float *dst, float k, float p, size_t count)
        {
            ARCH_X86_ASM(
                // Prepare
                __ASM_EMIT("movss           %[k], %%xmm6")                  // xmm6     = k
                __ASM_EMIT("movss           %[p], %%xmm7")                  // xmm7     = p
                __ASM_EMIT("shufps          $0x00, %%xmm6, %%xmm6")         // xmm6     = k k k k
                __ASM_EMIT("shufps          $0x00, %%xmm7, %%xmm7")         // xmm7     = p p p p
                __ASM_EMIT("movaps          0x00 + %[S2KP], %%xmm4")        // xmm4     = i = 0 1 2 3
                __ASM_EMIT("movaps          0x10 + %[S2KP], %%xmm5")        // xmm5     = step = 4 4 4 4
                __ASM_EMIT("addps           0x00 + %[S2C], %%xmm7")         // xmm7     = p+PI/2 p+PI/2 p+PI/2 p+PI/2
                // x4 blocks
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movaps          %%xmm4, %%xmm0")                // xmm0     = i
                __ASM_EMIT("mulps           %%xmm6, %%xmm0")                // xmm0     = k*i
                __ASM_EMIT("addps           %%xmm5, %%xmm4")                // xmm4     = i + step
                __ASM_EMIT("addps           %%xmm7, %%xmm0")                // xmm0     = k*i + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             6f")
                __ASM_EMIT("movaps          %%xmm4, %%xmm0")                // xmm0     = i
                __ASM_EMIT("mulps           %%xmm6, %%xmm0")                // xmm0     = k*i
                __ASM_EMIT("addps           %%xmm7, %%xmm0")                // xmm0     = k*i + p + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
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
                : [S2C] "o" (sinf_const),
                  [S2KP] "o" (kp_gen_const),
                  [k] "m" (k),
                  [p] "m" (p)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace sse2 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_SSE2_PMATH_SIN_H_ */
