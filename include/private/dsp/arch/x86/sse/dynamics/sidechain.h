/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 23 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_DYNAMICS_SIDECHAIN_H_
#define PRIVATE_DSP_ARCH_X86_SSE_DYNAMICS_SIDECHAIN_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count)
        {
            IF_ARCH_X86( size_t off; )

            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[k], %%xmm6")                      /* xmm6 = k */
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")                    /* xmm7 = 0 */
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             /* xmm6 = k */
                /* x8 blocks */
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm1")      /* xmm1 = s */
                __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm2")
                __ASM_EMIT("movups      %%xmm1, 0x00(%[head], %[off])")     /* *head = s */
                __ASM_EMIT("movups      %%xmm2, 0x10(%[head], %[off])")
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")                    /* xmm1 = s*s */
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("movups      0x00(%[tail], %[off]), %%xmm3")     /* xmm3 = t */
                __ASM_EMIT("movups      0x10(%[tail], %[off]), %%xmm4")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                    /* xmm3 = t*t */
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("subps       %%xmm3, %%xmm1")                    /* xmm1 = d = s*s - t*t */
                __ASM_EMIT("subps       %%xmm4, %%xmm2")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = rms+d0 ? ? ? */
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")                    /* xmm5 = d4 ? ? ? */
                __ASM_EMIT("shufps      $0x39, %%xmm1, %%xmm1")             /* xmm1 = d1 d2 d3 d0 */
                __ASM_EMIT("shufps      $0x39, %%xmm2, %%xmm2")             /* xmm2 = d5 d6 d7 d4 */
                __ASM_EMIT("shufps      $0x38, %%xmm0, %%xmm0")             /* xmm0 = rms+d0 ? ? rms+d0 */
                __ASM_EMIT("shufps      $0x38, %%xmm5, %%xmm5")             /* xmm5 = d4 ? ? d4 */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = rms+d0+d1 ? ? rms+d0 */
                __ASM_EMIT("addss       %%xmm2, %%xmm5")                    /* xmm5 = d4+d5 ? ? d4 */
                __ASM_EMIT("shufps      $0x39, %%xmm1, %%xmm1")             /* xmm1 = d2 d3 d0 d1 */
                __ASM_EMIT("shufps      $0x39, %%xmm2, %%xmm2")             /* xmm2 = d6 d7 d4 d5 */
                __ASM_EMIT("shufps      $0x38, %%xmm0, %%xmm0")             /* xmm0 = rms+d0+d1 ? rms+d0 rms+d0+d1 */
                __ASM_EMIT("shufps      $0x38, %%xmm5, %%xmm5")             /* xmm5 = d4+d5 ? d4 d4+d5 */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = rms+d0+d1+d2 ? rms+d0 rms+d0+d1 */
                __ASM_EMIT("addss       %%xmm2, %%xmm5")                    /* xmm5 = d4+d5+d6 ? d4 d4+d5 */
                __ASM_EMIT("shufps      $0x39, %%xmm1, %%xmm1")             /* xmm1 = d3 d0 d1 d2 */
                __ASM_EMIT("shufps      $0x39, %%xmm2, %%xmm2")             /* xmm2 = d7 d4 d5 d6 */
                __ASM_EMIT("shufps      $0x38, %%xmm0, %%xmm0")             /* xmm0 = rms+d0+d1+d2 rms+d0 rms+d0+d1 rms+d0+d1+d2 */
                __ASM_EMIT("shufps      $0x38, %%xmm5, %%xmm5")             /* xmm5 = d4+d5+d6 d4 d4+d5 d4+d5+d6 */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = rms+d0+d1+d2+d3 rms+d0 rms+d0+d1 rms+d0+d1+d2 */
                __ASM_EMIT("addss       %%xmm2, %%xmm5")                    /* xmm5 = d4+d5+d6+d7 d4 d4+d5 d4+d5+d6 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    /* xmm1 = ms3 ms0 ms1 ms2 */
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")             /* xmm0 = rms+d0+d1+d2+d3 rms+d0+d1+d2+d3 rms+d0+d1+d2+d3 rms+d0+d1+d2+d3 */
                __ASM_EMIT("shufps      $0x39, %%xmm1, %%xmm1")             /* xmm1 = ms0 ms1 ms2 ms3 */
                __ASM_EMIT("addps       %%xmm5, %%xmm0")                    /* xmm0 = rms+d0+d1+d2+d3+d4+d5+d6+d7 rms+d0+d1+d2+d3+d4 rms+d0+d1+d2+d3+d4+d5 rms+d0+d1+d2+d3+d4+d5+d6 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm1 = ms7 ms4 ms5 ms6 */
                __ASM_EMIT("shufps      $0x39, %%xmm2, %%xmm2")             /* xmm2 = ms4 ms5 ms6 ms7 */
                __ASM_EMIT("maxps       %%xmm7, %%xmm1")                    /* xmm1 = R = max(ms[], 0) */
                __ASM_EMIT("maxps       %%xmm7, %%xmm2")
                __ASM_EMIT("mulps       %%xmm6, %%xmm1")                    /* xmm1 = IR = k*R */
                __ASM_EMIT("mulps       %%xmm6, %%xmm2")
                __ASM_EMIT("sqrtps      %%xmm1, %%xmm1")                    /* xmm1 = rms = sqrtf(IR) */
                __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
                __ASM_EMIT("movups      %%xmm1, 0x00(%[dst], %[off])")      /* *dst = rms */
                __ASM_EMIT("movups      %%xmm2, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                __ASM_EMIT("2:")
                /* x4 block */
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm1")      /* xmm1 = s */
                __ASM_EMIT("movups      %%xmm1, 0x00(%[head], %[off])")     /* *head = s */
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")                    /* xmm1 = s*s */
                __ASM_EMIT("movups      0x00(%[tail], %[off]), %%xmm2")     /* xmm2 = t */
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")                    /* xmm2 = t*t */
                __ASM_EMIT("subps       %%xmm2, %%xmm1")                    /* xmm1 = d = s*s - t*t */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = ms0 ? ? ? */
                __ASM_EMIT("shufps      $0x39, %%xmm1, %%xmm1")             /* xmm1 = d1 d2 d3 d0 */
                __ASM_EMIT("shufps      $0x38, %%xmm0, %%xmm0")             /* xmm0 = ms0 ? ? ms0 */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = ms1 ? ? ms0 */
                __ASM_EMIT("shufps      $0x39, %%xmm1, %%xmm1")             /* xmm1 = d2 d3 d0 d1 */
                __ASM_EMIT("shufps      $0x38, %%xmm0, %%xmm0")             /* xmm0 = ms1 ? ms0 ms1 */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = ms2 ? ms0 ms1 */
                __ASM_EMIT("shufps      $0x39, %%xmm1, %%xmm1")             /* xmm1 = d3 d0 d1 d2 */
                __ASM_EMIT("shufps      $0x38, %%xmm0, %%xmm0")             /* xmm0 = ms2 ms0 ms1 ms2 */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = ms3 ms0 ms1 ms2 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    /* xmm1 = ms3 ms0 ms1 ms2 */
                __ASM_EMIT("shufps      $0x39, %%xmm1, %%xmm1")             /* xmm1 = ms0 ms1 ms2 ms3 */
                __ASM_EMIT("maxps       %%xmm7, %%xmm1")                    /* xmm1 = R = max(ms[], 0) */
                __ASM_EMIT("mulps       %%xmm6, %%xmm1")                    /* xmm1 = IR = k*R */
                __ASM_EMIT("sqrtps      %%xmm1, %%xmm1")                    /* xmm1 = rms = sqrtf(IR) */
                __ASM_EMIT("movups      %%xmm1, 0x00(%[dst], %[off])")      /* *dst = rms */
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("4:")
                /* x1 block */
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[dst], %[off]), %%xmm1")      /* xmm1 = s */
                __ASM_EMIT("movss       %%xmm1, 0x00(%[head], %[off])")     /* *head = s */
                __ASM_EMIT("movss       0x00(%[tail], %[off]), %%xmm2")     /* xmm2 = t */
                __ASM_EMIT("mulss       %%xmm1, %%xmm1")                    /* xmm1 = s*s */
                __ASM_EMIT("mulss       %%xmm2, %%xmm2")                    /* xmm2 = t*t */
                __ASM_EMIT("subss       %%xmm2, %%xmm1")                    /* xmm1 = d = s*s - t*t */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                    /* xmm0 = ms' = ms + d */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")
                __ASM_EMIT("maxss       %%xmm7, %%xmm1")                    /* xmm1 = R = max(IR, 0) */
                __ASM_EMIT("mulss       %%xmm6, %%xmm1")                    /* xmm1 = IR = k * R */
                __ASM_EMIT("sqrtss      %%xmm1, %%xmm1")                    /* xmm1 = sqrtf(max(IR, 0)) */
                __ASM_EMIT("movss       %%xmm1, 0x00(%[dst], %[off])")      /* *dst = sqrtf(max(IR, 0)) */
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("sub         $1, %[count]")
                __ASM_EMIT("jge         5b")
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
    } /* namespace sse */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_SSE_DYNAMICS_SIDECHAIN_H_ */
