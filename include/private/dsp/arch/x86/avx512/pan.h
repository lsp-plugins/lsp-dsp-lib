/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 нояб. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PAN_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PAN_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t depan_lin_const_i[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff)
            };
            static const float depan_lin_const_f[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(1e-18f)
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
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("vbroadcastss    %%xmm0, %%zmm6")                        /* zmm6 = dfl */
                __ASM_EMIT("vmovaps         %[CI], %%zmm7")                         /* zmm7 = mask */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vmovaps         %[CF], %%zmm5")                         /* zmm5 = thresh */
                __ASM_EMIT("jb              2f")
                /* 32x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("vandps          0x00(%[l],%[off]), %%zmm7, %%zmm2")     /* zmm2 = fabsf(l) */
                __ASM_EMIT("vandps          0x40(%[l],%[off]), %%zmm7, %%zmm3")
                __ASM_EMIT("vandps          0x00(%[r],%[off]), %%zmm7, %%zmm0")     /* xmm0 = fabsf(r) */
                __ASM_EMIT("vandps          0x40(%[r],%[off]), %%zmm7, %%zmm1")
                __ASM_EMIT("vaddps          %%zmm0, %%zmm2, %%zmm2")                /* zmm2 = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("vaddps          %%zmm1, %%zmm3, %%zmm3")
                __ASM_EMIT("vdivps          %%zmm2, %%zmm0, %%zmm0")                /* zmm0 = pan = fabsf(r) / den */
                __ASM_EMIT("vdivps          %%zmm3, %%zmm1, %%zmm1")
                __ASM_EMIT("vcmpps          $1, %%zmm5, %%zmm2, %%k2")              /* k2   = [den < thresh] */
                __ASM_EMIT("vcmpps          $1, %%zmm5, %%zmm3, %%k3")
                __ASM_EMIT("vmovaps         %%zmm6, %%zmm0 %{%%k2%}")               /* zmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("vmovaps         %%zmm6, %%zmm1 %{%%k3%}")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                /* 16x blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vandps          0x00(%[l],%[off]), %%ymm7, %%ymm2")     /* ymm2 = fabsf(l) */
                __ASM_EMIT("vandps          0x20(%[l],%[off]), %%ymm7, %%ymm3")
                __ASM_EMIT("vandps          0x00(%[r],%[off]), %%ymm7, %%ymm0")     /* xmm0 = fabsf(r) */
                __ASM_EMIT("vandps          0x20(%[r],%[off]), %%ymm7, %%ymm1")
                __ASM_EMIT("vaddps          %%ymm0, %%ymm2, %%ymm2")                /* ymm2 = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("vaddps          %%ymm1, %%ymm3, %%ymm3")
                __ASM_EMIT("vdivps          %%ymm2, %%ymm0, %%ymm0")                /* ymm0 = pan = fabsf(r) / den */
                __ASM_EMIT("vdivps          %%ymm3, %%ymm1, %%ymm1")
                __ASM_EMIT("vcmpps          $5, %%ymm5, %%ymm2, %%ymm2")            /* ymm2 = [den >= thresh] */
                __ASM_EMIT("vcmpps          $5, %%ymm5, %%ymm3, %%ymm3")
                __ASM_EMIT("vblendvps       %%ymm2, %%ymm0, %%ymm6, %%ymm0")        /* ymm0 = den >= thresh) ? pan : dfl */
                __ASM_EMIT("vblendvps       %%ymm3, %%ymm1, %%ymm6, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                /* 8x block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vandps          0x00(%[l],%[off]), %%xmm7, %%xmm2")     /* xmm2 = fabsf(l) */
                __ASM_EMIT("vandps          0x10(%[l],%[off]), %%xmm7, %%xmm3")
                __ASM_EMIT("vandps          0x00(%[r],%[off]), %%xmm7, %%xmm0")     /* xmm0 = fabsf(r) */
                __ASM_EMIT("vandps          0x10(%[r],%[off]), %%xmm7, %%xmm1")
                __ASM_EMIT("vaddps          %%xmm0, %%xmm2, %%xmm2")                /* xmm2 = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("vaddps          %%xmm1, %%xmm3, %%xmm3")
                __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = pan = fabsf(r) / den */
                __ASM_EMIT("vdivps          %%xmm3, %%xmm1, %%xmm1")
                __ASM_EMIT("vcmpps          $5, %%xmm5, %%xmm2, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("vcmpps          $5, %%xmm5, %%xmm3, %%xmm3")
                __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = den >= thresh) ? pan : dfl */
                __ASM_EMIT("vblendvps       %%xmm3, %%xmm1, %%xmm6, %%xmm1")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                /* 4x block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vandps          0x00(%[l],%[off]), %%xmm7, %%xmm2")     /* xmm2 = fabsf(l) */
                __ASM_EMIT("vandps          0x00(%[r],%[off]), %%xmm7, %%xmm0")     /* xmm0 = fabsf(r) */
                __ASM_EMIT("vaddps          %%xmm0, %%xmm2, %%xmm2")                /* xmm2 = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = pan = fabsf(r) / den */
                __ASM_EMIT("vcmpps          $5, %%xmm5, %%xmm2, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = den >= thresh) ? pan : dfl */
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                /* 1x blocks */
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[l],%[off]), %%xmm2")
                __ASM_EMIT("vmovss          0x00(%[r],%[off]), %%xmm0")
                __ASM_EMIT("vandps          %%xmm2, %%xmm7, %%xmm2")                /* xmm2 = fabsf(l) */
                __ASM_EMIT("vandps          %%xmm0, %%xmm7, %%xmm0")                /* xmm2 = fabsf(r) */
                __ASM_EMIT("vaddps          %%xmm0, %%xmm2, %%xmm2")                /* xmm2 = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = pan = fabsf(r) / den */
                __ASM_EMIT("vcmpps          $5, %%xmm5, %%xmm2, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = den >= thresh) ? pan : dfl */
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                /* End */
                __ASM_EMIT("10:")
                : [count] "+r" (count), [off] "=&r" (off),
                  [dfl] "+Yz" (dfl)
                : [dst] "r" (dst), [l] "r" (l), [r] "r" (r),
                  [CI] "m" (depan_lin_const_i),
                  [CF] "m" (depan_lin_const_f)
                : "%xmm1", "%xmm2", "%xmm3",
                  "%xmm5", "%xmm6", "%xmm7",
                  "%k2", "%k3"
            );
        }

        IF_ARCH_X86(
            static const float depan_eqpow_const_f[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(1e-36f)
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
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("vbroadcastss    %%xmm0, %%zmm7")                        /* zmm7 = dfl */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vmovaps         %[CF], %%zmm6")                         /* zmm6 = thresh */
                __ASM_EMIT("jb              2f")
                /* 32x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[r],%[off]), %%zmm0")             /* zmm0 = r */
                __ASM_EMIT("vmovups         0x40(%[r],%[off]), %%zmm1")
                __ASM_EMIT("vmovups         0x00(%[l],%[off]), %%zmm2")             /* zmm2 = l */
                __ASM_EMIT("vmovups         0x40(%[l],%[off]), %%zmm3")
                __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm0")                /* zmm0 = r*r */
                __ASM_EMIT("vmulps          %%zmm1, %%zmm1, %%zmm1")
                __ASM_EMIT("vfmadd132ps     %%zmm2, %%zmm0, %%zmm2")                /* zmm2 = den = l*l + r*r */
                __ASM_EMIT("vfmadd132ps     %%zmm3, %%zmm1, %%zmm3")
                __ASM_EMIT("vdivps          %%zmm2, %%zmm0, %%zmm0")                /* zmm0 = pan = r*r / den */
                __ASM_EMIT("vdivps          %%zmm3, %%zmm1, %%zmm1")
                __ASM_EMIT("vcmpps          $1, %%zmm6, %%zmm2, %%k2")              /* zmm2 = [den < thresh] */
                __ASM_EMIT("vcmpps          $1, %%zmm6, %%zmm3, %%k3")
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm0 %{%%k2%}")               /* zmm0 = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm1 %{%%k3%}")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst],%[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                /* 16x block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[r],%[off]), %%ymm0")             /* ymm0 = r */
                __ASM_EMIT("vmovups         0x20(%[r],%[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x00(%[l],%[off]), %%ymm2")             /* ymm2 = l */
                __ASM_EMIT("vmovups         0x20(%[l],%[off]), %%ymm3")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")                /* ymm0 = r*r */
                __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")
                __ASM_EMIT("vfmadd132ps     %%ymm2, %%ymm0, %%ymm2")                /* ymm2 = den = l*l + r*r */
                __ASM_EMIT("vfmadd132ps     %%ymm3, %%ymm1, %%ymm3")
                __ASM_EMIT("vdivps          %%ymm2, %%ymm0, %%ymm0")                /* ymm0 = pan = r*r / den */
                __ASM_EMIT("vdivps          %%ymm3, %%ymm1, %%ymm1")
                __ASM_EMIT("vcmpps          $5, %%ymm6, %%ymm2, %%ymm2")            /* ymm2 = [den >= thresh] */
                __ASM_EMIT("vcmpps          $5, %%ymm6, %%ymm3, %%ymm3")
                __ASM_EMIT("vblendvps       %%ymm2, %%ymm0, %%ymm7, %%ymm0")        /* ymm0 = den >= thresh) ? pan : dfl */
                __ASM_EMIT("vblendvps       %%ymm3, %%ymm1, %%ymm7, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst],%[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                /* 8x block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[r],%[off]), %%xmm0")             /* xmm0 = r */
                __ASM_EMIT("vmovups         0x10(%[r],%[off]), %%xmm1")
                __ASM_EMIT("vmovups         0x00(%[l],%[off]), %%xmm2")             /* xmm2 = l */
                __ASM_EMIT("vmovups         0x10(%[l],%[off]), %%xmm3")
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")                /* xmm0 = r*r */
                __ASM_EMIT("vmulps          %%xmm1, %%xmm1, %%xmm1")
                __ASM_EMIT("vfmadd132ps     %%xmm2, %%xmm0, %%xmm2")                /* xmm2 = den = l*l + r*r */
                __ASM_EMIT("vfmadd132ps     %%xmm3, %%xmm1, %%xmm3")
                __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = pan = r*r / den */
                __ASM_EMIT("vdivps          %%xmm3, %%xmm1, %%xmm1")
                __ASM_EMIT("vcmpps          $5, %%xmm6, %%xmm2, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("vcmpps          $5, %%xmm6, %%xmm3, %%xmm3")
                __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = den >= thresh) ? pan : dfl */
                __ASM_EMIT("vblendvps       %%xmm3, %%xmm1, %%xmm7, %%xmm1")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                /* 4x block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[r],%[off]), %%xmm0")             /* xmm0 = r */
                __ASM_EMIT("vmovups         0x00(%[l],%[off]), %%xmm2")             /* xmm2 = l */
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")                /* xmm0 = r*r */
                __ASM_EMIT("vfmadd132ps     %%xmm2, %%xmm0, %%xmm2")                /* xmm2 = den = l*l + r*r */
                __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = pan = r*r / den */
                __ASM_EMIT("vcmpps          $5, %%xmm6, %%xmm2, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = den >= thresh) ? pan : dfl */
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                /* 1x blocks */
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[l],%[off]), %%xmm2")
                __ASM_EMIT("vmovss          0x00(%[r],%[off]), %%xmm0")
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")                /* xmm0 = r*r */
                __ASM_EMIT("vfmadd132ps     %%xmm2, %%xmm0, %%xmm2")                /* xmm2 = den = l*l + r*r */
                __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = pan = r*r / den */
                __ASM_EMIT("vcmpps          $5, %%xmm6, %%xmm2, %%xmm2")            /* xmm2 = [den >= thresh] */
                __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = den >= thresh) ? pan : dfl */
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                /* End */
                __ASM_EMIT("10:")
                : [count] "+r" (count), [off] "=&r" (off),
                  [dfl] "+Yz" (dfl)
                : [dst] "r" (dst), [l] "r" (l), [r] "r" (r),
                  [CF] "m" (depan_eqpow_const_f)
                : "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7",
                  "%k2", "%k3"
            );
        }

    } /* namespaec avx512 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PAN_H_ */
