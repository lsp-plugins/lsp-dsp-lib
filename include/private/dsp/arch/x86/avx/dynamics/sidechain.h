/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_DYNAMICS_SIDECHAIN_H_
#define PRIVATE_DSP_ARCH_X86_AVX_DYNAMICS_SIDECHAIN_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count)
        {
            IF_ARCH_X86( size_t off; )

            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                __ASM_EMIT("vbroadcastss        %[k], %%ymm7")                      /* xmm7 = k */
                __ASM_EMIT("vxorps              %%xmm6, %%xmm6, %%xmm6")            /* xmm6 = 0 */
                /* x8 blocks */
                __ASM_EMIT("vxorps              %%ymm6, %%ymm6, %%ymm6")            /* ymm6 = 0 */
                __ASM_EMIT("vbroadcastss        %[k], %%ymm7")                      /* ymm7 = k */
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x00(%[dst], %[off]), %%xmm1")      /* xmm1 = s */
                __ASM_EMIT("vmovups             0x10(%[dst], %[off]), %%xmm2")
                __ASM_EMIT("vmovups             %%xmm1, 0x00(%[head], %[off])")     /* *head = s */
                __ASM_EMIT("vmovups             %%xmm2, 0x10(%[head], %[off])")
                __ASM_EMIT("vmulps              %%xmm1, %%xmm1, %%xmm1")            /* xmm1 = s*s */
                __ASM_EMIT("vmulps              %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovups             0x00(%[tail], %[off]), %%xmm3")     /* xmm3 = t */
                __ASM_EMIT("vmovups             0x10(%[tail], %[off]), %%xmm4")
                __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm3")            /* xmm3 = t*t */
                __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm4")
                __ASM_EMIT("vsubps              %%xmm3, %%xmm1, %%xmm1")            /* xmm1 = d = s*s - t*t */
                __ASM_EMIT("vsubps              %%xmm4, %%xmm2, %%xmm2")
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = rms+d0 ? ? ? */
                __ASM_EMIT("vmovaps             %%xmm2, %%xmm5")                    /* xmm5 = d4 ? ? ? */
                __ASM_EMIT("vpermilps           $0x39, %%xmm1, %%xmm1")             /* xmm1 = d1 d2 d3 d0 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm2, %%xmm2")             /* xmm2 = d5 d6 d7 d4 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm0, %%xmm0")             /* xmm0 = rms+d0 ? ? rms+d0 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm5, %%xmm5")             /* xmm5 = d4 ? ? d4 */
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = rms+d0+d1 ? ? rms+d0 */
                __ASM_EMIT("vaddss              %%xmm2, %%xmm5, %%xmm5")            /* xmm5 = d4+d5 ? ? d4 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm1, %%xmm1")             /* xmm1 = d2 d3 d0 d1 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm2, %%xmm2")             /* xmm2 = d6 d7 d4 d5 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm0, %%xmm0")             /* xmm0 = rms+d0+d1 ? rms+d0 rms+d0+d1 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm5, %%xmm5")             /* xmm5 = d4+d5 ? d4 d4+d5 */
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = rms+d0+d1+d2 ? rms+d0 rms+d0+d1 */
                __ASM_EMIT("vaddss              %%xmm2, %%xmm5, %%xmm5")            /* xmm5 = d4+d5+d6 ? d4 d4+d5 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm1, %%xmm1")             /* xmm1 = d3 d0 d1 d2 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm2, %%xmm2")             /* xmm2 = d7 d4 d5 d6 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm0, %%xmm0")             /* xmm0 = rms+d0+d1+d2 rms+d0 rms+d0+d1 rms+d0+d1+d2 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm5, %%xmm5")             /* xmm5 = d4+d5+d6 d4 d4+d5 d4+d5+d6 */
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = rms+d0+d1+d2+d3 rms+d0 rms+d0+d1 rms+d0+d1+d2 */
                __ASM_EMIT("vaddss              %%xmm2, %%xmm5, %%xmm5")            /* xmm5 = d4+d5+d6+d7 d4 d4+d5 d4+d5+d6 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm0, %%xmm1")             /* xmm1 = ms0 ms1 ms2 ms3 */
                __ASM_EMIT("vbroadcastss        %%xmm0, %%xmm0")                    /* xmm0 = rms+d0+d1+d2+d3 rms+d0+d1+d2+d3 rms+d0+d1+d2+d3 rms+d0+d1+d2+d3 */
                __ASM_EMIT("vaddps              %%xmm5, %%xmm0, %%xmm0")            /* xmm0 = rms+d0+d1+d2+d3+d4+d5+d6+d7 rms+d0+d1+d2+d3+d4 rms+d0+d1+d2+d3+d4+d5 rms+d0+d1+d2+d3+d4+d5+d6 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm0, %%xmm2")             /* xmm2 = ms4 ms5 ms6 ms7 */
                __ASM_EMIT("vmaxps              %%xmm6, %%xmm1, %%xmm1")            /* xmm1 = R = max(ms[], 0) */
                __ASM_EMIT("vmaxps              %%xmm6, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm1")            /* xmm1 = IR = k*R */
                __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")
                __ASM_EMIT("vsqrtps             %%xmm1, %%xmm1")                    /* xmm1 = rms = sqrtf(IR) */
                __ASM_EMIT("vsqrtps             %%xmm2, %%xmm2")
                __ASM_EMIT("vmovups             %%xmm1, 0x00(%[dst], %[off])")      /* *dst = rms */
                __ASM_EMIT("vmovups             %%xmm2, 0x10(%[dst], %[off])")
                __ASM_EMIT("add                 $0x20, %[off]")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("jae                 1b")
                __ASM_EMIT("2:")
                /* x4 block */
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x00(%[dst], %[off]), %%xmm1")      /* xmm1 = s */
                __ASM_EMIT("vmovups             %%xmm1, 0x00(%[head], %[off])")     /* *head = s */
                __ASM_EMIT("vmulps              %%xmm1, %%xmm1, %%xmm1")            /* xmm1 = s*s */
                __ASM_EMIT("vmovups             0x00(%[tail], %[off]), %%xmm2")     /* xmm2 = t */
                __ASM_EMIT("vmulps              %%xmm2, %%xmm2, %%xmm2")            /* xmm2 = t*t */
                __ASM_EMIT("vsubps              %%xmm2, %%xmm1, %%xmm1")            /* xmm1 = d = s*s - t*t */
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = ms0 ? ? ? */
                __ASM_EMIT("vpermilps           $0x39, %%xmm1, %%xmm1")             /* xmm1 = d1 d2 d3 d0 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm0, %%xmm0")             /* xmm0 = ms0 ? ? ms0 */
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = ms1 ? ? ms0 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm1, %%xmm1")             /* xmm1 = d2 d3 d0 d1 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm0, %%xmm0")             /* xmm0 = ms1 ? ms0 ms1 */
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = ms2 ? ms0 ms1 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm1, %%xmm1")             /* xmm1 = d3 d0 d1 d2 */
                __ASM_EMIT("vpermilps           $0x38, %%xmm0, %%xmm0")             /* xmm0 = ms2 ms0 ms1 ms2 */
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = ms3 ms0 ms1 ms2 */
                __ASM_EMIT("vpermilps           $0x39, %%xmm0, %%xmm1")             /* xmm1 = ms0 ms1 ms2 ms3 */
                __ASM_EMIT("vmaxps              %%xmm6, %%xmm1, %%xmm1")            /* xmm1 = R = max(ms[], 0) */
                __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm1")            /* xmm1 = IR = k*R */
                __ASM_EMIT("vsqrtps             %%xmm1, %%xmm1")                    /* xmm1 = rms = sqrtf(IR) */
                __ASM_EMIT("vmovups             %%xmm1, 0x00(%[dst], %[off])")      /* *dst = rms */
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                __ASM_EMIT("4:")
                /* x1 block */
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vmovss              0x00(%[dst], %[off]), %%xmm1")      /* xmm1 = s */
                __ASM_EMIT("vmovss              %%xmm1, 0x00(%[head], %[off])")     /* *head = s */
                __ASM_EMIT("vmovss              0x00(%[tail], %[off]), %%xmm2")     /* xmm2 = t */
                __ASM_EMIT("vmulss              %%xmm1, %%xmm1, %%xmm1")            /* xmm1 = s*s */
                __ASM_EMIT("vmulss              %%xmm2, %%xmm2, %%xmm2")            /* xmm2 = t*t */
                __ASM_EMIT("vsubss              %%xmm2, %%xmm1, %%xmm1")            /* xmm1 = d = s*s - t*t */
                __ASM_EMIT("vaddss              %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = ms' = ms + d */
                __ASM_EMIT("vmaxss              %%xmm6, %%xmm0, %%xmm1")            /* xmm1 = R = max(IR, 0) */
                __ASM_EMIT("vmulss              %%xmm7, %%xmm1, %%xmm1")            /* xmm1 = IR = k * R */
                __ASM_EMIT("vsqrtss             %%xmm1, %%xmm1, %%xmm1")            /* xmm1 = sqrtf(max(IR, 0)) */
                __ASM_EMIT("vmovss              %%xmm1, 0x00(%[dst], %[off])")      /* *dst = sqrtf(max(IR, 0)) */
                __ASM_EMIT("add                 $0x04, %[off]")
                __ASM_EMIT("sub                 $1, %[count]")
                __ASM_EMIT("jge                 5b")
                __ASM_EMIT("6:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [rms] "+Yz" (rms)
                : [dst] "r" (dst), [head] "r" (head), [tail] "r" (tail),
                  [k] "m" (k)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return rms;
        }

    } /* namespace avx */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX_DYNAMICS_SIDECHAIN_H_ */
