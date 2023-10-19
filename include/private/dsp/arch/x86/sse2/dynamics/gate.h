/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 19 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_DYNAMICS_GATE_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_DYNAMICS_GATE_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE2_IMPL */

#include <private/dsp/arch/x86/sse2/pmath/exp.h>
#include <private/dsp/arch/x86/sse2/pmath/log.h>

namespace lsp
{
    namespace sse2
    {
    #pragma pack(push, 1)
        typedef struct gate_knee_t
        {
            float   start[4];       // +0x00
            float   end[4];         // +0x10
            float   gain_start[4];  // +0x20
            float   gain_end[4];    // +0x30
            float   herm[16];       // +0x40
        } gate_knee_t;
    #pragma pack(pop)

    #define UNPACK_GATE_KNEE(DST, SRC) \
        __ASM_EMIT("movss               0x00(%[" SRC "]), %%xmm0") \
        __ASM_EMIT("movss               0x04(%[" SRC "]), %%xmm1") \
        __ASM_EMIT("movss               0x08(%[" SRC "]), %%xmm2") \
        __ASM_EMIT("movss               0x0c(%[" SRC "]), %%xmm3") \
        __ASM_EMIT("movss               0x10(%[" SRC "]), %%xmm4") \
        __ASM_EMIT("movss               0x14(%[" SRC "]), %%xmm5") \
        __ASM_EMIT("movss               0x18(%[" SRC "]), %%xmm6") \
        __ASM_EMIT("movss               0x1c(%[" SRC "]), %%xmm7") \
        __ASM_EMIT("shufps              $0x00, %%xmm0, %%xmm0") \
        __ASM_EMIT("shufps              $0x00, %%xmm1, %%xmm1") \
        __ASM_EMIT("shufps              $0x00, %%xmm2, %%xmm2") \
        __ASM_EMIT("shufps              $0x00, %%xmm3, %%xmm3") \
        __ASM_EMIT("shufps              $0x00, %%xmm4, %%xmm4") \
        __ASM_EMIT("shufps              $0x00, %%xmm5, %%xmm5") \
        __ASM_EMIT("shufps              $0x00, %%xmm6, %%xmm6") \
        __ASM_EMIT("shufps              $0x00, %%xmm7, %%xmm7") \
        __ASM_EMIT("movaps              %%xmm0, 0x00 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm1, 0x10 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm2, 0x20 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm3, 0x30 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm4, 0x40 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm5, 0x50 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm6, 0x60 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm7, 0x70 + %[" DST "]")

    #define PROCESS_KNEE_SINGLE_X8 \
        /* in: xmm0 = lx0, xmm4 = lx1 */ \
        __ASM_EMIT("movaps              0x40 + %[knee], %%xmm1")        /* xmm1 = herm[0] */ \
        __ASM_EMIT("movaps              0x40 + %[knee], %%xmm5") \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("mulps               %%xmm4, %%xmm5") \
        __ASM_EMIT("addps               0x50 + %[knee], %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("addps               0x50 + %[knee], %%xmm5") \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("mulps               %%xmm4, %%xmm5") \
        __ASM_EMIT("addps               0x60 + %[knee], %%xmm1")        /* xmm1 = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("addps               0x60 + %[knee], %%xmm5") \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0 */ \
        __ASM_EMIT("mulps               %%xmm4, %%xmm5") \
        __ASM_EMIT("addps               0x70 + %[knee], %%xmm1")        /* xmm1 = KV = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0+herm[3] */ \
        __ASM_EMIT("addps               0x70 + %[knee], %%xmm5") \
        __ASM_EMIT("movaps              %%xmm1, %%xmm0")                /* xmm0 = KV */ \
        __ASM_EMIT("movaps              %%xmm5, %%xmm4") \
        EXP_CORE_X8                                                     /* xmm0 = expf(KV) */ \
        __ASM_EMIT("movaps              0x00 + %[mem], %%xmm1")         /* xmm1 = x0 */ \
        __ASM_EMIT("movaps              0x10 + %[mem], %%xmm5") \
        __ASM_EMIT("movaps              %%xmm1, %%xmm2")                /* xmm2 = x0 */ \
        __ASM_EMIT("movaps              %%xmm5, %%xmm6") \
        __ASM_EMIT("cmpps               $6, 0x00 + %[knee], %%xmm1")    /* xmm0 = [x0 > start] */ \
        __ASM_EMIT("cmpps               $6, 0x00 + %[knee], %%xmm5") \
        __ASM_EMIT("cmpps               $1, 0x10 + %[knee], %%xmm2")    /* xmm2 = [x0 < end] */ \
        __ASM_EMIT("cmpps               $1, 0x10 + %[knee], %%xmm6") \
        __ASM_EMIT("andps               %%xmm1, %%xmm0")                /* xmm0 = [x0 > start] & expf(KV) */ \
        __ASM_EMIT("andps               %%xmm5, %%xmm4") \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                /* xmm0 = ([x0 > start] && [x0 < end]) & expf(KV) */ \
        __ASM_EMIT("andps               %%xmm6, %%xmm4") \
        __ASM_EMIT("andnps              0x20 + %[knee], %%xmm1")        /* xmm1 = [x0 <= start] & gain_start */   \
        __ASM_EMIT("andnps              0x20 + %[knee], %%xmm5") \
        __ASM_EMIT("andnps              0x30 + %[knee], %%xmm2")        /* xmm2 = [x0 >= end] & gain_end */ \
        __ASM_EMIT("andnps              0x30 + %[knee], %%xmm6") \
        __ASM_EMIT("orps                %%xmm1, %%xmm0")                /* xmm0 = ([x0 <= start]) ? gain_start : ([x0 < end]) ? expf(KV) : 0 */ \
        __ASM_EMIT("orps                %%xmm5, %%xmm4") \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                /* xmm0 = ([x0 <= start]) ? gain_start : ([x0 < end]) ? expf(KV) : gain_end */ \
        __ASM_EMIT("orps                %%xmm6, %%xmm4") \
        /* out: xmm0 = g0, xmm4 = g1 */

    #define PROCESS_KNEE_SINGLE_X4 \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("movaps              0x40 + %[knee], %%xmm1")        /* xmm1 = herm[0] */ \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("addps               0x50 + %[knee], %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("addps               0x60 + %[knee], %%xmm1")        /* xmm1 = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0 */ \
        __ASM_EMIT("addps               0x70 + %[knee], %%xmm1")        /* xmm1 = KV = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0+herm[3] */ \
        __ASM_EMIT("movaps              %%xmm1, %%xmm0")                /* xmm0 = KV */ \
        EXP_CORE_X4                                                     /* xmm0 = expf(KV) */ \
        __ASM_EMIT("movaps              0x00 + %[mem], %%xmm1")         /* xmm1 = x0 */ \
        __ASM_EMIT("movaps              %%xmm1, %%xmm2")                /* xmm2 = x0 */ \
        __ASM_EMIT("cmpps               $6, 0x00 + %[knee], %%xmm1")    /* xmm0 = [x0 > start] */ \
        __ASM_EMIT("cmpps               $1, 0x10 + %[knee], %%xmm2")    /* xmm2 = [x0 < end] */ \
        __ASM_EMIT("andps               %%xmm1, %%xmm0")                /* xmm0 = [x0 > start] & expf(KV) */ \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                /* xmm0 = ([x0 > start] && [x0 < end]) & expf(KV) */ \
        __ASM_EMIT("andnps              0x20 + %[knee], %%xmm1")        /* xmm1 = [x0 <= start] & gain_start */   \
        __ASM_EMIT("andnps              0x30 + %[knee], %%xmm2")        /* xmm2 = [x0 >= end] & gain_end */ \
        __ASM_EMIT("orps                %%xmm1, %%xmm0")                /* xmm0 = ([x0 <= start]) ? gain_start : ([x0 < end]) ? expf(KV) : 0 */ \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                /* xmm0 = ([x0 <= start]) ? gain_start : ([x0 < end]) ? expf(KV) : gain_end */ \
        /* out: xmm0 = g0 */

    #define PROCESS_GATE_FULL_X8 \
        /* in: xmm0 = x0, xmm4 = x1 */ \
        __ASM_EMIT("andps               0x00 + %[G2C], %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("andps               0x00 + %[G2C], %%xmm4") \
        __ASM_EMIT("movaps              %%xmm0, %%xmm1") \
        __ASM_EMIT("movaps              %%xmm4, %%xmm5") \
        __ASM_EMIT("movaps              %%xmm0, %%xmm2") \
        __ASM_EMIT("movaps              %%xmm4, %%xmm6") \
        __ASM_EMIT("cmpps               $6, 0x00 + %[knee], %%xmm1")    /* xmm1 = [x0 > start] */ \
        __ASM_EMIT("cmpps               $6, 0x00 + %[knee], %%xmm5") \
        __ASM_EMIT("cmpps               $1, 0x10 + %[knee], %%xmm2")    /* xmm2 = [x0 < end] */ \
        __ASM_EMIT("cmpps               $1, 0x10 + %[knee], %%xmm6") \
        __ASM_EMIT("movaps              %%xmm1, %%xmm3")                /* xmm3 = [x0 > start] */ \
        __ASM_EMIT("movaps              %%xmm5, %%xmm7") \
        __ASM_EMIT("andps               %%xmm2, %%xmm3")                /* xmm3 = [x0 > start] && [x0 < end] */ \
        __ASM_EMIT("andps               %%xmm6, %%xmm7") \
        __ASM_EMIT("orps                %%xmm7, %%xmm3") \
        __ASM_EMIT("movmskps            %%xmm3, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        /* Case when we do not need logarithm because there is no sample at the knee */ \
        __ASM_EMIT("andnps              0x20 + %[knee], %%xmm1")        /* xmm1 = [x0 <= start] & gain_start */   \
        __ASM_EMIT("andnps              0x20 + %[knee], %%xmm5") \
        __ASM_EMIT("andnps              0x30 + %[knee], %%xmm2")        /* xmm2 = [x0 >= end] & gain_end */ \
        __ASM_EMIT("andnps              0x30 + %[knee], %%xmm6") \
        __ASM_EMIT("movaps              %%xmm1, %%xmm0")                /* xmm0 = [x0 <= start] & gain_start */ \
        __ASM_EMIT("movaps              %%xmm5, %%xmm4") \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                /* xmm0 = [x0 <= start] & gain_start | [x0 >= end] & gain_end */ \
        __ASM_EMIT("orps                %%xmm6, %%xmm4") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("movaps              %%xmm0, 0x00 + %[mem]")         /* store fabsf(x0) */ \
        __ASM_EMIT("movaps              %%xmm4, 0x10 + %[mem]") \
        LOGE_CORE_X8                                                    /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_KNEE_SINGLE_X8                                          /* apply knee 0 */ \
        __ASM_EMIT("200:") \
        /* out: xmm0 = g0, xmm4 = g1 */

    #define PROCESS_GATE_FULL_X4 \
        /* in: xmm0 = x0 */ \
        __ASM_EMIT("andps               0x00 + %[G2C], %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("movaps              %%xmm0, 0x00 + %[mem]")         /* store fabsf(x0) */ \
        LOGE_CORE_X4                                                    /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_KNEE_SINGLE_X4                                          /* apply knee 0 */ \
        /* out: xmm0 = g */

        static const uint32_t gate_const[] __lsp_aligned16 =
        {
            LSP_DSP_VEC4(0x7fffffff)
        };

        void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                gate_knee_t knee __lsp_aligned16;
                float mem[8] __lsp_aligned16;
                float stub[4] __lsp_aligned16;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_GATE_KNEE("knee", "gate")

                // 8x blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
                PROCESS_GATE_FULL_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
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
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [gate] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub] "o" (stub),
                  [G2C] "o" (gate_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void gate_x1_curve(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                gate_knee_t knee __lsp_aligned16;
                float mem[8] __lsp_aligned16;
                float stub[4] __lsp_aligned16;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_GATE_KNEE("knee", "gate")

                // 8x blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
                PROCESS_GATE_FULL_X8
                __ASM_EMIT("movups          0x00(%[src]), %%xmm1")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm5")
                __ASM_EMIT("mulps           %%xmm1, %%xmm0")
                __ASM_EMIT("mulps           %%xmm5, %%xmm4")
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("movups          0x00(%[src]), %%xmm1")
                __ASM_EMIT("mulps           %%xmm1, %%xmm0")
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
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
                __ASM_EMIT("movaps          %%xmm0, %%xmm4")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("mulps           %%xmm4, %%xmm0")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [gate] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub] "o" (stub),
                  [G2C] "o" (gate_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef PROCESS_KNEE_SINGLE_X4
    #undef PROCESS_KNEE_SINGLE_X8
    #undef PROCESS_GATE_FULL_X4
    #undef PROCESS_GATE_FULL_X8
    #undef UNPACK_GATE_KNEE

    } /* namespace sse2 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE2_DYNAMICS_GATE_H_ */
