/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 5 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_DYNAMICS_COMPRESSOR_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_DYNAMICS_COMPRESSOR_H_

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
        typedef struct comp_knee_t
        {
            float   start[4];   // +0x00
            float   end[4];     // +0x10
            float   gain[4];    // +0x20
            float   herm[12];   // +0x30
            float   tilt[8];    // +0x60
        } comp_knee_t;
    #pragma pack(pop)

    #define UNPACK_COMP_KNEE(DST, DOFF, SRC, SOFF) \
        __ASM_EMIT("movss    " SOFF " + 0x00(%[" SRC "]), %%xmm0") \
        __ASM_EMIT("movss    " SOFF " + 0x04(%[" SRC "]), %%xmm1") \
        __ASM_EMIT("movss    " SOFF " + 0x08(%[" SRC "]), %%xmm2") \
        __ASM_EMIT("movss    " SOFF " + 0x0c(%[" SRC "]), %%xmm3") \
        __ASM_EMIT("movss    " SOFF " + 0x10(%[" SRC "]), %%xmm4") \
        __ASM_EMIT("movss    " SOFF " + 0x14(%[" SRC "]), %%xmm5") \
        __ASM_EMIT("movss    " SOFF " + 0x18(%[" SRC "]), %%xmm6") \
        __ASM_EMIT("movss    " SOFF " + 0x1c(%[" SRC "]), %%xmm7") \
        __ASM_EMIT("shufps              $0x00, %%xmm0, %%xmm0") \
        __ASM_EMIT("shufps              $0x00, %%xmm1, %%xmm1") \
        __ASM_EMIT("shufps              $0x00, %%xmm2, %%xmm2") \
        __ASM_EMIT("shufps              $0x00, %%xmm3, %%xmm3") \
        __ASM_EMIT("shufps              $0x00, %%xmm4, %%xmm4") \
        __ASM_EMIT("shufps              $0x00, %%xmm5, %%xmm5") \
        __ASM_EMIT("shufps              $0x00, %%xmm6, %%xmm6") \
        __ASM_EMIT("shufps              $0x00, %%xmm7, %%xmm7") \
        __ASM_EMIT("movaps              %%xmm0, " DOFF "+ 0x00 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm1, " DOFF " + 0x10 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm2, " DOFF " + 0x20 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm3, " DOFF " + 0x30 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm4, " DOFF " + 0x40 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm5, " DOFF " + 0x50 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm6, " DOFF " + 0x60 + %[" DST "]") \
        __ASM_EMIT("movaps              %%xmm7, " DOFF " + 0x70 + %[" DST "]")

    #define PROCESS_KNEE_SINGLE_X8(OFF) \
        /* in: xmm0 = lx0, xmm4 = lx1 */ \
        __ASM_EMIT("movaps    " OFF " + 0x30 + %[knee], %%xmm1")        /* xmm1 = herm[0] */ \
        __ASM_EMIT("movaps    " OFF " + 0x30 + %[knee], %%xmm5") \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("mulps               %%xmm4, %%xmm5") \
        __ASM_EMIT("addps     " OFF " + 0x40 + %[knee], %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("addps     " OFF " + 0x40 + %[knee], %%xmm5") \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("mulps               %%xmm4, %%xmm5") \
        __ASM_EMIT("addps     " OFF " + 0x50 + %[knee], %%xmm1")        /* xmm1 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("addps     " OFF " + 0x50 + %[knee], %%xmm5") \
        __ASM_EMIT("movaps    " OFF " + 0x60 + %[knee], %%xmm2")        /* xmm2 = tilt[0] */ \
        __ASM_EMIT("movaps    " OFF " + 0x60 + %[knee], %%xmm6") \
        __ASM_EMIT("mulps               %%xmm0, %%xmm2")                /* xmm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("mulps               %%xmm4, %%xmm6") \
        __ASM_EMIT("addps     " OFF " + 0x70 + %[knee], %%xmm2")        /* xmm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("addps     " OFF " + 0x70 + %[knee], %%xmm6") \
        __ASM_EMIT("movaps              0x00 + %[mem], %%xmm0")         /* xmm0 = x0 */ \
        __ASM_EMIT("movaps              0x10 + %[mem], %%xmm4") \
        __ASM_EMIT("cmpps               $5, " OFF " + 0x10 + %[knee], %%xmm0")    /* xmm0 = [x0 >= end] */ \
        __ASM_EMIT("cmpps               $5, " OFF " + 0x10 + %[knee], %%xmm4") \
        __ASM_EMIT("andps               %%xmm0, %%xmm2")                /* xmm2 = [x0 >= end] & TV */ \
        __ASM_EMIT("andps               %%xmm4, %%xmm6") \
        __ASM_EMIT("andnps              %%xmm1, %%xmm0")                /* xmm0 = [x0 < end] & KV */ \
        __ASM_EMIT("andnps              %%xmm5, %%xmm4") \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                /* xmm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("orps                %%xmm6, %%xmm4") \
        EXP_CORE_X8                                                     /* xmm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("movaps              0x00 + %[mem], %%xmm2")         /* xmm2 = x0 */ \
        __ASM_EMIT("movaps              0x10 + %[mem], %%xmm6") \
        __ASM_EMIT("cmpps               $6, " OFF " + 0x00 + %[knee], %%xmm2")    /* xmm2 = [x0 > start] */ \
        __ASM_EMIT("cmpps               $6, " OFF " + 0x00 + %[knee], %%xmm6") \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                /* xmm4 = [x0 > start] & EV */ \
        __ASM_EMIT("andps               %%xmm6, %%xmm4") \
        __ASM_EMIT("andnps    " OFF " + 0x20 + %[knee], %%xmm2")        /* xmm2 = [x0 <= start] & GV */ \
        __ASM_EMIT("andnps    " OFF " + 0x20 + %[knee], %%xmm6") \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                /* xmm0 = ([x0 <= start]) ? GV : EV */ \
        __ASM_EMIT("orps                %%xmm6, %%xmm4") \
        /* out: xmm0 = g0, xmm4 = g1 */

    #define PROCESS_KNEE_SINGLE_X4(OFF) \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("movaps    " OFF " + 0x30 + %[knee], %%xmm1")        /* xmm1 = herm[0] */ \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("addps     " OFF " + 0x40 + %[knee], %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("mulps               %%xmm0, %%xmm1")                /* xmm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("addps     " OFF " + 0x50 + %[knee], %%xmm1")        /* xmm1 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("movaps    " OFF " + 0x60 + %[knee], %%xmm2")        /* xmm2 = tilt[0] */ \
        __ASM_EMIT("mulps               %%xmm0, %%xmm2")                /* xmm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("addps     " OFF " + 0x70 + %[knee], %%xmm2")        /* xmm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("movaps              0x00 + %[mem], %%xmm0")         /* xmm0 = x0 */ \
        __ASM_EMIT("cmpps               $5, " OFF " + 0x10 + %[knee], %%xmm0")    /* xmm0 = [x0 >= end] */ \
        __ASM_EMIT("andps               %%xmm0, %%xmm2")                /* xmm2 = [x0 >= end] & TV */ \
        __ASM_EMIT("andnps              %%xmm1, %%xmm0")                /* xmm0 = [x0 < end] & KV */ \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                /* xmm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X4                                                     /* xmm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("movaps              0x00 + %[mem], %%xmm2")         /* xmm2 = x0 */ \
        __ASM_EMIT("cmpps               $6, " OFF " + 0x00 + %[knee], %%xmm2")    /* xmm2 = [x0 > start] */ \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                /* xmm4 = [x0 > start] & EV */ \
        __ASM_EMIT("andnps    " OFF " + 0x20 + %[knee], %%xmm2")        /* xmm2 = [x0 <= start] & GV */ \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                /* xmm0 = ([x0 <= start]) ? GV : EV */ \
        /* out: xmm0 = g0 */

    #define PROCESS_COMP_FULL_X8 \
        /* in: xmm0 = x0, xmm4 = x1 */ \
        __ASM_EMIT("andps               0x00 + %[C2C], %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("andps               0x00 + %[C2C], %%xmm4") \
        __ASM_EMIT("movaps              %%xmm0, %%xmm1") \
        __ASM_EMIT("movaps              %%xmm4, %%xmm5") \
        __ASM_EMIT("movaps              %%xmm0, %%xmm2") \
        __ASM_EMIT("movaps              %%xmm4, %%xmm6") \
        __ASM_EMIT("cmpps               $6, 0x00 + 0x00 + %[knee], %%xmm1")    /* xmm1 = [x0 > k0.start] */ \
        __ASM_EMIT("cmpps               $6, 0x00 + 0x00 + %[knee], %%xmm5") \
        __ASM_EMIT("cmpps               $6, 0x80 + 0x00 + %[knee], %%xmm2")    /* xmm2 = [x0 > k1.start] */ \
        __ASM_EMIT("cmpps               $6, 0x80 + 0x00 + %[knee], %%xmm6") \
        __ASM_EMIT("orps                %%xmm2, %%xmm1") \
        __ASM_EMIT("orps                %%xmm6, %%xmm5") \
        __ASM_EMIT("orps                %%xmm5, %%xmm1") \
        __ASM_EMIT("movmskps            %%xmm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("movaps       0x00 + 0x20 + %[knee], %%xmm0")        /* xmm0 = g1 */ \
        __ASM_EMIT("movaps       0x00 + 0x20 + %[knee], %%xmm4") \
        __ASM_EMIT("mulps        0x80 + 0x20 + %[knee], %%xmm0")        /* xmm0 = G = g0*g1 */ \
        __ASM_EMIT("mulps        0x80 + 0x20 + %[knee], %%xmm4") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("movaps              %%xmm0, 0x00 + %[mem]")         /* store fabsf(x0) */ \
        __ASM_EMIT("movaps              %%xmm4, 0x10 + %[mem]") \
        LOGE_CORE_X8                                                    /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("movaps              %%xmm0, 0x20 + %[mem]")         /* *mem = lx0 */ \
        __ASM_EMIT("movaps              %%xmm4, 0x30 + %[mem]") \
        PROCESS_KNEE_SINGLE_X8("0x00")                                  /* apply knee 0 */ \
        __ASM_EMIT("movaps              %%xmm0, 0x40 + %[mem]")         /* *mem = g1 */ \
        __ASM_EMIT("movaps              %%xmm4, 0x50 + %[mem]") \
        __ASM_EMIT("movaps              0x20 + %[mem], %%xmm0")         /* xmm0 = lx0 */ \
        __ASM_EMIT("movaps              0x30 + %[mem], %%xmm4") \
        PROCESS_KNEE_SINGLE_X8("0x80")                                  /* apply knee 1 */ \
        __ASM_EMIT("mulps               0x40 + %[mem], %%xmm0")         /* xmm0 = G = g0*g1 */ \
        __ASM_EMIT("mulps               0x50 + %[mem], %%xmm4") \
        __ASM_EMIT("200:") \
        /* out: xmm0 = G0, xmm4 = G1 */

    #define PROCESS_COMP_FULL_X4 \
        /* in: xmm0 = x0 */ \
        __ASM_EMIT("andps               0x00 + %[C2C], %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("movaps              %%xmm0, 0x00 + %[mem]")         /* store fabsf(x0) */ \
        LOGE_CORE_X4                                                    /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("movaps              %%xmm0, 0x20 + %[mem]")         /* *mem = lx0 */ \
        PROCESS_KNEE_SINGLE_X4("0x00")                                  /* apply knee 0 */ \
        __ASM_EMIT("movaps              %%xmm0, 0x40 + %[mem]")         /* *mem = g1 */ \
        __ASM_EMIT("movaps              0x20 + %[mem], %%xmm0")         /* xmm0 = lx0 */ \
        PROCESS_KNEE_SINGLE_X4("0x80")                                  /* apply knee 1 */ \
        __ASM_EMIT("mulps               0x40 + %[mem], %%xmm0")         /* xmm0 = G = g0*g1 */ \
        /* out: xmm0 = G0 */

        static const uint32_t compressor_const[] __lsp_aligned16 =
        {
            LSP_DSP_VEC4(0x7fffffff)
        };

        void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_X86(
                comp_knee_t knee[2] __lsp_aligned16;
                float mem[24] __lsp_aligned16;
                float stub[4] __lsp_aligned16;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_COMP_KNEE("knee", "0x00", "comp", "0x00")
                UNPACK_COMP_KNEE("knee", "0x80", "comp", "0x20")

                // 8x blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
                PROCESS_COMP_FULL_X8
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
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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
                : [comp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub] "o" (stub),
                  [C2C] "o" (compressor_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_X86(
                comp_knee_t knee[2] __lsp_aligned16;
                float mem[24] __lsp_aligned16;
                float stub[4] __lsp_aligned16;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_COMP_KNEE("knee", "0x00", "comp", "0x00")
                UNPACK_COMP_KNEE("knee", "0x80", "comp", "0x20")

                // 8x blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
                PROCESS_COMP_FULL_X8
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
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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
                : [comp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub] "o" (stub),
                  [C2C] "o" (compressor_const),
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
    #undef PROCESS_COMP_FULL_X4
    #undef PROCESS_COMP_FULL_X8
    #undef UNPACK_COMP_KNEE

    } /* namespace sse2 */
} /* namespace lsp */


#endif /* DSP_ARCH_X86_SSE2_DYNAMICS_COMPRESSOR_H_ */
