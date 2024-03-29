/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_PMATH_EXP_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_PMATH_EXP_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE2_IMPL */

namespace lsp
{
    namespace sse2
    {
        /* Code to generate constants:

            float fact(int n)
            {
                return (n > 1) ? n*fact(n-1) : 1;
            }

            uint32_t to_hex(float v)
            {
                union {
                    uint32_t i;
                    float f;
                } r;
                r.f = v;
                return r.i;
            }

            int main()
            {
                printf("LSP_DSP_VEC4(0x7fffffff), // sign\n");
                printf("LSP_DSP_VEC4(0x%08x), // ln(2)\n", to_hex(M_LN2));

                float f7 = fact(7);

                for (int i=6; i>=1; --i)
                {
                    float C = f7 / fact(i);
                    printf("LSP_DSP_VEC4(0x%08x), // C%d = %d!/%d! = %.16f \n", to_hex(C), i-1, 7, i, C);
                }
                printf("LSP_DSP_VEC4(0x%08x), // 1/7! = %.16f\n", to_hex(1.0f/f7), 1.0f/f7);
                printf("LSP_DSP_VEC4(0x%08x), // 1.0\n", to_hex(1.0f));
            }

         */

        IF_ARCH_X86(
            static const uint32_t EXP2_CONST[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7fffffff), // sign
                LSP_DSP_VEC4(0x3f317218), // ln(2)
                LSP_DSP_VEC4(0x40e00000), // C5 = 7!/6! = 7.0000000000000000
                LSP_DSP_VEC4(0x42280000), // C4 = 7!/5! = 42.0000000000000000
                LSP_DSP_VEC4(0x43520000), // C3 = 7!/4! = 210.0000000000000000
                LSP_DSP_VEC4(0x44520000), // C2 = 7!/3! = 840.0000000000000000
                LSP_DSP_VEC4(0x451d8000), // C1 = 7!/2! = 2520.0000000000000000
                LSP_DSP_VEC4(0x459d8000), // C0 = 7!/1! = 5040.0000000000000000
                LSP_DSP_VEC4(0x39500d01), // 1/7! = 0.0001984127011383
                LSP_DSP_VEC4(0x3f800000), // 1.0
                LSP_DSP_VEC4(0x7f),       // 0x7f
            };

            static const float EXP_LOG2E[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(M_LOG2E)
            };
        )

        #define POW2_CORE_X8 \
            /* xmm0 = x, xmm4 = y */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")                /* xmm2 = x */ \
            __ASM_EMIT("movaps          %%xmm4, %%xmm6") \
            __ASM_EMIT("andps           0x00 + %[E2C], %%xmm2")         /* xmm2 = XP = fabs(x) */ \
            __ASM_EMIT("andps           0x00 + %[E2C], %%xmm6") \
            __ASM_EMIT("psrad           $31, %%xmm0")                   /* xmm0 = [ x < 0 ] */ \
            __ASM_EMIT("psrad           $31, %%xmm4") \
            __ASM_EMIT("cvttps2dq       %%xmm2, %%xmm3")                /* xmm3 = R = int(XP) */ \
            __ASM_EMIT("cvttps2dq       %%xmm6, %%xmm7") \
            __ASM_EMIT("movaps          %%xmm3, %%xmm1")                /* xmm1 = R */ \
            __ASM_EMIT("movaps          %%xmm7, %%xmm5") \
            __ASM_EMIT("cvtdq2ps        %%xmm3, %%xmm3")                /* xmm3 = float(R) */ \
            __ASM_EMIT("cvtdq2ps        %%xmm7, %%xmm7") \
            __ASM_EMIT("paddd           0xa0 + %[E2C], %%xmm1")         /* xmm1 = R + 127 */ \
            __ASM_EMIT("paddd           0xa0 + %[E2C], %%xmm5") \
            __ASM_EMIT("subps           %%xmm3, %%xmm2")                /* xmm2 = XP - float(R) */ \
            __ASM_EMIT("subps           %%xmm7, %%xmm6") \
            __ASM_EMIT("pslld           $23, %%xmm1")                   /* xmm1 = 1 << (R+127) */ \
            __ASM_EMIT("pslld           $23, %%xmm5") \
            __ASM_EMIT("mulps           0x10 + %[E2C], %%xmm2")         /* xmm2 = X = ln(2) * (XP - float(R)) */ \
            __ASM_EMIT("mulps           0x10 + %[E2C], %%xmm6") \
            __ASM_EMIT("movaps          %%xmm2, %%xmm3")                /* xmm3 = X */ \
            __ASM_EMIT("movaps          %%xmm6, %%xmm7") \
            /* xmm0 = [ x < 0 ], xmm1 = 1 << R, xmm2 = X, xmm3 = X */ \
            __ASM_EMIT("addps           0x20 + %[E2C], %%xmm2")         /* xmm2 = C5+X */ \
            __ASM_EMIT("addps           0x20 + %[E2C], %%xmm6") \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C5+X) */ \
            __ASM_EMIT("mulps           %%xmm7, %%xmm6") \
            __ASM_EMIT("addps           0x30 + %[E2C], %%xmm2")         /* xmm2 = C4+(X*(C5+X)) */ \
            __ASM_EMIT("addps           0x30 + %[E2C], %%xmm6") \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C4+(X*(C5+X))) */ \
            __ASM_EMIT("mulps           %%xmm7, %%xmm6") \
            __ASM_EMIT("addps           0x40 + %[E2C], %%xmm2")         /* xmm2 = C3+X*(C4+(X*(C5+X))) */ \
            __ASM_EMIT("addps           0x40 + %[E2C], %%xmm6") \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C3+X*(C4+(X*(C5+X)))) */ \
            __ASM_EMIT("mulps           %%xmm7, %%xmm6") \
            __ASM_EMIT("addps           0x50 + %[E2C], %%xmm2")         /* xmm2 = C2+X*(C3+X*(C4+(X*(C5+X)))) */ \
            __ASM_EMIT("addps           0x50 + %[E2C], %%xmm6") \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C2+X*(C3+X*(C4+(X*(C5+X))))) */ \
            __ASM_EMIT("mulps           %%xmm7, %%xmm6") \
            __ASM_EMIT("addps           0x60 + %[E2C], %%xmm2")         /* xmm2 = C1+X*(C2+(X*C3+X*(C4+(X*(C5+X))))) */ \
            __ASM_EMIT("addps           0x60 + %[E2C], %%xmm6") \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X)))))) */ \
            __ASM_EMIT("mulps           %%xmm7, %%xmm6") \
            __ASM_EMIT("addps           0x70 + %[E2C], %%xmm2")         /* xmm2 = C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X)))))) */ \
            __ASM_EMIT("addps           0x70 + %[E2C], %%xmm6") \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X))))))) */ \
            __ASM_EMIT("mulps           %%xmm7, %%xmm6") \
            __ASM_EMIT("mulps           0x80 + %[E2C], %%xmm2")         /* xmm2 = 1/7! * X*(C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X))))))) */ \
            __ASM_EMIT("mulps           0x80 + %[E2C], %%xmm6") \
            __ASM_EMIT("movaps          0x90 + %[E2C], %%xmm3")         /* xmm3 = 1 */ \
            __ASM_EMIT("movaps          %%xmm3, %%xmm7") \
            __ASM_EMIT("addps           %%xmm3, %%xmm2")                /* xmm2 = 1 + 1/7! * X*(C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X))))))) */ \
            __ASM_EMIT("addps           %%xmm7, %%xmm6") \
            __ASM_EMIT("mulps           %%xmm1, %%xmm2")                /* xmm2 = p = (1 << R) * (1 + 1/7! * X*(C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X)))))))) */ \
            __ASM_EMIT("mulps           %%xmm5, %%xmm6") \
            __ASM_EMIT("divps           %%xmm2, %%xmm3")                /* xmm3 = 1/p */ \
            __ASM_EMIT("divps           %%xmm6, %%xmm7") \
            /* xmm0 = [ x < 0 ], xmm2 = p, xmm3 = 1/p */ \
            __ASM_EMIT("andps           %%xmm0, %%xmm3")                /* xmm3 = 1/p & [ x < 0] */ \
            __ASM_EMIT("andps           %%xmm4, %%xmm7") \
            __ASM_EMIT("andnps          %%xmm2, %%xmm0")                /* xmm0 = p & [ x >= 0] */ \
            __ASM_EMIT("andnps          %%xmm6, %%xmm4") \
            __ASM_EMIT("orps            %%xmm3, %%xmm0")                /* xmm0 = (1/p & [ x < 0]) | (p & [ x >= 0]) */ \
            __ASM_EMIT("orps            %%xmm7, %%xmm4")

        #define POW2_CORE_X4 \
            /* xmm0 = x */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")                /* xmm2 = x */ \
            __ASM_EMIT("andps           0x00 + %[E2C], %%xmm2")         /* xmm2 = XP = fabs(x) */ \
            __ASM_EMIT("psrad           $31, %%xmm0")                   /* xmm0 = [ x < 0 ] */ \
            __ASM_EMIT("cvttps2dq       %%xmm2, %%xmm3")                /* xmm3 = R = int(XP) */ \
            __ASM_EMIT("movaps          %%xmm3, %%xmm1")                /* xmm1 = R */ \
            __ASM_EMIT("cvtdq2ps        %%xmm3, %%xmm3")                /* xmm3 = float(R) */ \
            __ASM_EMIT("paddd           0xa0 + %[E2C], %%xmm1")         /* xmm1 = R + 127 */ \
            __ASM_EMIT("subps           %%xmm3, %%xmm2")                /* xmm2 = XP - float(R) */ \
            __ASM_EMIT("pslld           $23, %%xmm1")                   /* xmm1 = 1 << (R+127) */ \
            __ASM_EMIT("mulps           0x10 + %[E2C], %%xmm2")         /* xmm2 = X = ln(2) * (XP - float(R)) */ \
            __ASM_EMIT("movaps          %%xmm2, %%xmm3")                /* xmm3 = X */ \
            /* xmm0 = [ x < 0 ], xmm1 = 1 << R, xmm2 = X, xmm3 = X */ \
            __ASM_EMIT("addps           0x20 + %[E2C], %%xmm2")         /* xmm2 = C5+X */ \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C5+X) */ \
            __ASM_EMIT("addps           0x30 + %[E2C], %%xmm2")         /* xmm2 = C4+(X*(C5+X)) */ \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C4+(X*(C5+X))) */ \
            __ASM_EMIT("addps           0x40 + %[E2C], %%xmm2")         /* xmm2 = C3+X*(C4+(X*(C5+X))) */ \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C3+X*(C4+(X*(C5+X)))) */ \
            __ASM_EMIT("addps           0x50 + %[E2C], %%xmm2")         /* xmm2 = C2+X*(C3+X*(C4+(X*(C5+X)))) */ \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C2+X*(C3+X*(C4+(X*(C5+X))))) */ \
            __ASM_EMIT("addps           0x60 + %[E2C], %%xmm2")         /* xmm2 = C1+X*(C2+(X*C3+X*(C4+(X*(C5+X))))) */ \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X)))))) */ \
            __ASM_EMIT("addps           0x70 + %[E2C], %%xmm2")         /* xmm2 = C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X)))))) */ \
            __ASM_EMIT("mulps           %%xmm3, %%xmm2")                /* xmm2 = X*(C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X))))))) */ \
            __ASM_EMIT("mulps           0x80 + %[E2C], %%xmm2")         /* xmm2 = 1/7! * X*(C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X))))))) */ \
            __ASM_EMIT("movaps          0x90 + %[E2C], %%xmm3")         /* xmm3 = 1 */ \
            __ASM_EMIT("addps           %%xmm3, %%xmm2")                /* xmm2 = 1 + 1/7! * X*(C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X))))))) */ \
            __ASM_EMIT("mulps           %%xmm1, %%xmm2")                /* xmm2 = p = (1 << R) * (1 + 1/7! * X*(C0+X*(C1+X*(C2+(X*C3+X*(C4+(X*(C5+X)))))))) */ \
            __ASM_EMIT("divps           %%xmm2, %%xmm3")                /* xmm3 = 1/p */ \
            /* xmm0 = [ x < 0 ], xmm2 = p, xmm3 = 1/p */ \
            __ASM_EMIT("andps           %%xmm0, %%xmm3")                /* xmm3 = 1/p & [ x < 0] */ \
            __ASM_EMIT("andnps          %%xmm2, %%xmm0")                /* xmm0 = p & [ x >= 0] */ \
            __ASM_EMIT("orps            %%xmm3, %%xmm0")                /* xmm0 = (1/p & [ x < 0]) | (p & [ x >= 0]) */

        #define EXP_CORE_X4 \
            __ASM_EMIT("mulps           %[LOG2E], %%xmm0") \
            POW2_CORE_X4

        #define EXP_CORE_X8 \
            __ASM_EMIT("mulps           %[LOG2E], %%xmm0") \
            __ASM_EMIT("mulps           %[LOG2E], %%xmm4") \
            POW2_CORE_X8

        void exp1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[dst]), %%xmm4")
                EXP_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
                EXP_CORE_X4
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
                EXP_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void exp2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
                EXP_CORE_X8
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
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                EXP_CORE_X4
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
                EXP_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        /*
            const float *E2C = reinterpret_cast<const float *>(EXP2_CONST);

            float XP = fabs(x);
            int R    = XP;
            float X  = (XP - float(R)) * E2C[1];

            float p = E2C[9] + E2C[8]*X*(
                     E2C[7] + X*(
                     E2C[6] + X*(
                     E2C[5] + X*(
                     E2C[4] + X*(
                     E2C[3] + X*(
                     E2C[2] + X
                     )))))
                );

            p *= 1 << R;

            return (x < 0.0f) ? 1.0f / p : p;
         */
    }
}

#endif /* PRIVATE_DSP_ARCH_X86_SSE2_PMATH_EXP_H_ */
