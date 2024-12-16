/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 нояб. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PAN_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PAN_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        IF_ARCH_X86(
            static const uint32_t depan_lin_const_i[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7fffffff)
            };
            static const float depan_lin_const_f[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1e-18f)
            };
        );

        void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count)
        {
            /*
                const float sl  = fabsf(l[i]);
                const float sr  = fabsf(r[i]);
                const float den = sl + sr;
                dst[i]          = (den >= 1e-18f) ? sr / den : dfl;
            */
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         /* xmm0 = dfl */
                __ASM_EMIT("movaps      %[CI], %%xmm7")                 /* xmm7 = mask */
                __ASM_EMIT("movaps      %[CF], %%xmm5")                 /* xmm5 = thresh */
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movaps      %%xmm0, %%xmm6")                /* xmm6 = dfl */
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                /* 8x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[l],%[off]), %%xmm2")     /* xmm2 = l */
                __ASM_EMIT("movups      0x10(%[l],%[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[r],%[off]), %%xmm0")     /* xmm0 = r */
                __ASM_EMIT("movups      0x10(%[r],%[off]), %%xmm1")
                __ASM_EMIT("andps       %%xmm7, %%xmm2")                /* xmm2 = fabsf(l) */
                __ASM_EMIT("andps       %%xmm7, %%xmm3")
                __ASM_EMIT("andps       %%xmm7, %%xmm0")                /* xmm0 = fabsf(r) */
                __ASM_EMIT("andps       %%xmm7, %%xmm1")
                __ASM_EMIT("addps       %%xmm0, %%xmm2")                /* xmm2 = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("addps       %%xmm1, %%xmm3")
                __ASM_EMIT("divps       %%xmm2, %%xmm0")                /* xmm0 = pan = fabsf(r) / den */
                __ASM_EMIT("divps       %%xmm3, %%xmm1")
                __ASM_EMIT("cmpps       $5, %%xmm5, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("cmpps       $5, %%xmm5, %%xmm3")
                __ASM_EMIT("andps       %%xmm2, %%xmm0")                /* xmm0 = [den >= thresh] & pan */
                __ASM_EMIT("andps       %%xmm3, %%xmm1")
                __ASM_EMIT("andnps      %%xmm6, %%xmm2")                /* xmm2 = ![den >= thresh] & dfl */
                __ASM_EMIT("andnps      %%xmm6, %%xmm3")
                __ASM_EMIT("orps        %%xmm2, %%xmm0")                /* xmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("orps        %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                /* 4x block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[l],%[off]), %%xmm2")     /* xmm2 = l */
                __ASM_EMIT("movups      0x00(%[r],%[off]), %%xmm0")     /* xmm0 = r */
                __ASM_EMIT("andps       %%xmm7, %%xmm2")                /* xmm2 = fabsf(l) */
                __ASM_EMIT("andps       %%xmm7, %%xmm0")                /* xmm0 = fabsf(r) */
                __ASM_EMIT("addps       %%xmm0, %%xmm2")                /* xmm2 = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("divps       %%xmm2, %%xmm0")                /* xmm0 = pan = fabsf(r) / den */
                __ASM_EMIT("cmpps       $5, %%xmm5, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("andps       %%xmm2, %%xmm0")                /* xmm0 = [den >= thresh] & pan */
                __ASM_EMIT("andnps      %%xmm6, %%xmm2")                /* xmm2 = ![den >= thresh] & dfl */
                __ASM_EMIT("orps        %%xmm2, %%xmm0")                /* xmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                /* 1x blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[l],%[off]), %%xmm2")
                __ASM_EMIT("movss       0x00(%[r],%[off]), %%xmm0")
                __ASM_EMIT("andps       %%xmm7, %%xmm2")                /* xmm2 = fabsf(l) */
                __ASM_EMIT("andps       %%xmm7, %%xmm0")                /* xmm0 = fabsf(r) */
                __ASM_EMIT("addps       %%xmm0, %%xmm2")                /* xmm2 = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("divps       %%xmm2, %%xmm0")                /* xmm0 = pan = fabsf(r) / den */
                __ASM_EMIT("cmpps       $5, %%xmm5, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("andps       %%xmm2, %%xmm0")                /* xmm0 = [den >= thresh] & pan */
                __ASM_EMIT("andnps      %%xmm6, %%xmm2")                /* xmm2 = ![den >= thresh] & dfl */
                __ASM_EMIT("orps        %%xmm2, %%xmm0")                /* xmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                /* End */
                __ASM_EMIT("6:")
                : [count] "+r" (count), [off] "=&r" (off),
                  [dfl] "+Yz" (dfl)
                : [dst] "r" (dst), [l] "r" (l), [r] "r" (r),
                  [CI] "m" (depan_lin_const_i),
                  [CF] "m" (depan_lin_const_f)
                : "%xmm1", "%xmm2", "%xmm3",
                  "%xmm5", "%xmm6", "%xmm7"
            );
        }

        IF_ARCH_X86(
            static const float depan_eqpow_const_f[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1e-36f)
            };
        );

        void depan_eqpow(float *dst, const float *l, const float *r, float dfl, size_t count)
        {
            /*
                const float sl  = l[i] * l[i];
                const float sr  = r[i] * r[i];
                const float den = sl + sr;
                dst[i]          = (den >= 1e-36f) ? sr / den : dfl;
            */
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         /* xmm0 = dfl */
                __ASM_EMIT("movaps      %[CF], %%xmm6")                 /* xmm6 = thresh */
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movaps      %%xmm0, %%xmm7")                /* xmm7 = dfl */
                __ASM_EMIT("sub         $12, %[count]")
                __ASM_EMIT("jb          2f")
                /* 12x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[l],%[off]), %%xmm3")     /* xmm3 = l */
                __ASM_EMIT("movups      0x10(%[l],%[off]), %%xmm4")
                __ASM_EMIT("movups      0x20(%[l],%[off]), %%xmm5")
                __ASM_EMIT("movups      0x00(%[r],%[off]), %%xmm0")     /* xmm0 = r */
                __ASM_EMIT("movups      0x10(%[r],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[r],%[off]), %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                /* xmm3 = l*l */
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")                /* xmm0 = r*r */
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("addps       %%xmm0, %%xmm3")                /* xmm3 = den = l*l + r*r */
                __ASM_EMIT("addps       %%xmm1, %%xmm4")
                __ASM_EMIT("addps       %%xmm2, %%xmm5")
                __ASM_EMIT("divps       %%xmm3, %%xmm0")                /* xmm0 = pan = r*r / den */
                __ASM_EMIT("divps       %%xmm4, %%xmm1")
                __ASM_EMIT("divps       %%xmm5, %%xmm2")
                __ASM_EMIT("cmpps       $5, %%xmm6, %%xmm3")            /* xmm3 = [den >= thresh] */
                __ASM_EMIT("cmpps       $5, %%xmm6, %%xmm4")
                __ASM_EMIT("cmpps       $5, %%xmm6, %%xmm5")
                __ASM_EMIT("andps       %%xmm3, %%xmm0")                /* xmm0 = [den >= thresh] & pan */
                __ASM_EMIT("andps       %%xmm4, %%xmm1")
                __ASM_EMIT("andps       %%xmm5, %%xmm2")
                __ASM_EMIT("andnps      %%xmm7, %%xmm3")                /* xmm3 = ![den >= thresh] & dfl */
                __ASM_EMIT("andnps      %%xmm7, %%xmm4")
                __ASM_EMIT("andnps      %%xmm7, %%xmm5")
                __ASM_EMIT("orps        %%xmm3, %%xmm0")                /* xmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("orps        %%xmm4, %%xmm1")
                __ASM_EMIT("orps        %%xmm5, %%xmm2")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst],%[off])")
                __ASM_EMIT("add         $0x30, %[off]")
                __ASM_EMIT("sub         $12, %[count]")
                __ASM_EMIT("jae         1b")
                /* 8x block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[l],%[off]), %%xmm3")     /* xmm3 = l */
                __ASM_EMIT("movups      0x10(%[l],%[off]), %%xmm4")
                __ASM_EMIT("movups      0x00(%[r],%[off]), %%xmm0")     /* xmm0 = r */
                __ASM_EMIT("movups      0x10(%[r],%[off]), %%xmm1")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                /* xmm3 = l*l */
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")                /* xmm0 = r*r */
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")
                __ASM_EMIT("addps       %%xmm0, %%xmm3")                /* xmm3 = den = l*l + r*r */
                __ASM_EMIT("addps       %%xmm1, %%xmm4")
                __ASM_EMIT("divps       %%xmm3, %%xmm0")                /* xmm0 = pan = r*r / den */
                __ASM_EMIT("divps       %%xmm4, %%xmm1")
                __ASM_EMIT("cmpps       $5, %%xmm6, %%xmm3")            /* xmm3 = [den >= thresh] */
                __ASM_EMIT("cmpps       $5, %%xmm6, %%xmm4")
                __ASM_EMIT("andps       %%xmm3, %%xmm0")                /* xmm0 = [den >= thresh] & pan */
                __ASM_EMIT("andps       %%xmm4, %%xmm1")
                __ASM_EMIT("andnps      %%xmm7, %%xmm3")                /* xmm3 = ![den >= thresh] & dfl */
                __ASM_EMIT("andnps      %%xmm7, %%xmm4")
                __ASM_EMIT("orps        %%xmm3, %%xmm0")                /* xmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("orps        %%xmm4, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                /* 4x block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[l],%[off]), %%xmm3")     /* xmm3 = l */
                __ASM_EMIT("movups      0x00(%[r],%[off]), %%xmm0")     /* xmm0 = r */
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                /* xmm3 = l*l */
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")                /* xmm0 = r*r */
                __ASM_EMIT("addps       %%xmm0, %%xmm3")                /* xmm3 = den = l*l + r*r */
                __ASM_EMIT("divps       %%xmm3, %%xmm0")                /* xmm0 = pan = r*r / den */
                __ASM_EMIT("cmpps       $5, %%xmm6, %%xmm3")            /* xmm3 = [den >= thresh] */
                __ASM_EMIT("andps       %%xmm3, %%xmm0")                /* xmm0 = [den >= thresh] & pan */
                __ASM_EMIT("andnps      %%xmm7, %%xmm3")                /* xmm3 = ![den >= thresh] & dfl */
                __ASM_EMIT("orps        %%xmm3, %%xmm0")                /* xmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                /* 1x blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[l],%[off]), %%xmm3")
                __ASM_EMIT("movss       0x00(%[r],%[off]), %%xmm0")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                /* xmm3 = l*l */
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")                /* xmm0 = r*r */
                __ASM_EMIT("addps       %%xmm0, %%xmm3")                /* xmm3 = den = l*l + r*r */
                __ASM_EMIT("divps       %%xmm3, %%xmm0")                /* xmm0 = pan = r*r / den */
                __ASM_EMIT("cmpps       $5, %%xmm6, %%xmm3")            /* xmm3 = [den >= thresh] */
                __ASM_EMIT("andps       %%xmm3, %%xmm0")                /* xmm0 = [den >= thresh] & pan */
                __ASM_EMIT("andnps      %%xmm7, %%xmm3")                /* xmm3 = ![den >= thresh] & dfl */
                __ASM_EMIT("orps        %%xmm3, %%xmm0")                /* xmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                /* End */
                __ASM_EMIT("8:")
                : [count] "+r" (count), [off] "=&r" (off),
                  [dfl] "+Yz" (dfl)
                : [dst] "r" (dst), [l] "r" (l), [r] "r" (r),
                  [CF] "m" (depan_eqpow_const_f)
                : "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }


    } /* namespace sse */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_SSE_PAN_H_ */
