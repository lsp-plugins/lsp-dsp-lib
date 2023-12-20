/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_DYNAMICS_GATE_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_DYNAMICS_GATE_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

#include <private/dsp/arch/x86/avx512/pmath/exp.h>
#include <private/dsp/arch/x86/avx512/pmath/log.h>

namespace lsp
{
    namespace avx512
    {
    #pragma pack(push, 1)
        typedef struct gate_knee_t
        {
            float   start[16];          // +0x000
            float   end[16];            // +0x040
            float   gain_start[16];     // +0x080
            float   gain_end[16];       // +0x0c0
            float   herm[64];           // +0x100
        } gate_knee_t;
    #pragma pack(pop)

        static const uint32_t gate_const[] __lsp_aligned64 =
        {
            LSP_DSP_VEC16(0x7fffffff)
        };


    #define UNPACK_GATE_KNEE(DST, SRC) \
        __ASM_EMIT("vbroadcastss        0x00(%[" SRC "]), %%zmm0") \
        __ASM_EMIT("vbroadcastss        0x04(%[" SRC "]), %%zmm1") \
        __ASM_EMIT("vbroadcastss        0x08(%[" SRC "]), %%zmm2") \
        __ASM_EMIT("vbroadcastss        0x0c(%[" SRC "]), %%zmm3") \
        __ASM_EMIT("vbroadcastss        0x10(%[" SRC "]), %%zmm4") \
        __ASM_EMIT("vbroadcastss        0x14(%[" SRC "]), %%zmm5") \
        __ASM_EMIT("vbroadcastss        0x18(%[" SRC "]), %%zmm6") \
        __ASM_EMIT("vbroadcastss        0x1c(%[" SRC "]), %%zmm7") \
        __ASM_EMIT("vmovaps             %%zmm0, 0x000 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%zmm1, 0x040 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%zmm2, 0x080 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%zmm3, 0x0c0 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%zmm4, 0x100 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%zmm5, 0x140 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%zmm6, 0x180 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%zmm7, 0x1c0 + %[" DST "]")

    #define PROCESS_KNEE_SINGLE_X32 \
        /* in: zmm0 = lx0, zmm4 = lx1 */ \
        __ASM_EMIT("vmovaps             0x100 + %[knee], %%zmm1")               /* zmm1 = herm[0] */ \
        __ASM_EMIT("vmovaps             0x100 + %[knee], %%zmm5") \
        __ASM_EMIT("vfmadd213ps         0x140 + %[knee], %%zmm0, %%zmm1")       /* zmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vfmadd213ps         0x140 + %[knee], %%zmm4, %%zmm5") \
        __ASM_EMIT("vfmadd213ps         0x180 + %[knee], %%zmm0, %%zmm1")       /* zmm1 = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps         0x180 + %[knee], %%zmm4, %%zmm5") \
        __ASM_EMIT("vfmadd213ps         0x1c0 + %[knee], %%zmm1, %%zmm0")       /* zmm0 = KV = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0+herm[3] */ \
        __ASM_EMIT("vfmadd213ps         0x1c0 + %[knee], %%zmm5, %%zmm4") \
        EXP_CORE_X32                                                            /* zmm0 = EV = expf(KV) */ \
        __ASM_EMIT("vmovaps             0x000 + %[mem], %%zmm1")                /* zmm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x040 + %[mem], %%zmm5") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%zmm1, %%k4")      /* k4   = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%zmm5, %%k6") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%zmm1, %%k5")      /* k5   = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%zmm5, %%k7") \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%zmm0 %{%%k4%}")      /* zmm0 = [x0 <= start] ? gain_start : expf(KV) */ \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%zmm4 %{%%k6%}") \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%zmm0 %{%%k5%}")      /* zmm0 = [x0 >= end] ? gain_end : [x0 <= start] ? gain_start : expf(KV) */ \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%zmm4 %{%%k7%}") \
        /* out: zmm0 = g0, zmm4 = g1 */

    #define PROCESS_KNEE_SINGLE_X16 \
        /* in: ymm0 = lx0 */ \
        __ASM_EMIT("vmovaps             0x100 + %[knee], %%zmm1")               /* zmm1 = herm[0] */ \
        __ASM_EMIT("vfmadd213ps         0x140 + %[knee], %%zmm0, %%zmm1")       /* zmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vfmadd213ps         0x180 + %[knee], %%zmm0, %%zmm1")       /* zmm1 = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps         0x1c0 + %[knee], %%zmm1, %%zmm0")       /* zmm0 = KV = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0+herm[3] */ \
        EXP_CORE_X16                                                            /* zmm0 = EV = expf(KV) */ \
        __ASM_EMIT("vmovaps             0x000 + %[mem], %%zmm1")                /* zmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%zmm1, %%k4")      /* k4   = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%zmm1, %%k5")      /* k5   = [x0 >= end] */ \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%zmm0 %{%%k4%}")      /* zmm0 = [x0 <= start] ? gain_start : expf(KV) */ \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%zmm0 %{%%k5%}")      /* zmm0 = [x0 >= end] ? gain_end : [x0 <= start] ? gain_start : expf(KV) */ \
        /* out: ymm0 = g0 */

    #define PROCESS_KNEE_SINGLE_X8 \
        /* in: ymm0 = lx0 */ \
        __ASM_EMIT("vmovaps             0x100 + %[knee], %%ymm1")               /* ymm1 = herm[0] */ \
        __ASM_EMIT("vfmadd213ps         0x140 + %[knee], %%ymm0, %%ymm1")       /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vfmadd213ps         0x180 + %[knee], %%ymm0, %%ymm1")       /* ymm1 = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps         0x1c0 + %[knee], %%ymm1, %%ymm0")       /* ymm0 = KV = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0+herm[3] */ \
        EXP_CORE_X8                                                             /* ymm0 = EV = expf(KV) */ \
        __ASM_EMIT("vmovaps             0x000 + %[mem], %%ymm1")                /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm1, %%k4")      /* k4   = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm1, %%k5")      /* k5   = [x0 >= end] */ \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%ymm0 %{%%k4%}")      /* ymm0 = [x0 <= start] ? gain_start : expf(KV) */ \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%ymm0 %{%%k5%}")      /* ymm0 = [x0 >= end] ? gain_end : [x0 <= start] ? gain_start : expf(KV) */ \
        /* out: ymm0 = g0 */

    #define PROCESS_KNEE_SINGLE_X4 \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("vmovaps             0x100 + %[knee], %%xmm1")               /* xmm1 = herm[0] */ \
        __ASM_EMIT("vfmadd213ps         0x140 + %[knee], %%xmm0, %%xmm1")       /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vfmadd213ps         0x180 + %[knee], %%xmm0, %%xmm1")       /* xmm1 = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps         0x1c0 + %[knee], %%xmm1, %%xmm0")       /* xmm0 = KV = ((herm[0]*lx0+herm[1])*lx0+herm[2])*lx0+herm[3] */ \
        EXP_CORE_X4                                                             /* xmm0 = EV = expf(KV) */ \
        __ASM_EMIT("vmovaps             0x000 + %[mem], %%xmm1")                /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%xmm1, %%k4")      /* k4   = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%xmm1, %%k5")      /* k5   = [x0 >= end] */ \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%xmm0 %{%%k4%}")      /* xmm0 = [x0 <= start] ? gain_start : expf(KV) */ \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%xmm0 %{%%k5%}")      /* xmm0 = [x0 >= end] ? gain_end : [x0 <= start] ? gain_start : expf(KV) */ \
        /* out: xmm0 = g0 */

    #define PROCESS_GATE_FULL_X32 \
        /* in: zmm0 = x0, zmm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[G2C], %%zmm0, %%zmm0")             /* zmm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[G2C], %%zmm4, %%zmm4") \
        __ASM_EMIT("vcmpps              $6, 0x000 + %[knee], %%zmm0, %%k4")         /* k4   = [x0 > start] */ \
        __ASM_EMIT("vcmpps              $6, 0x000 + %[knee], %%zmm4, %%k6") \
        __ASM_EMIT("vcmpps              $1, 0x040 + %[knee], %%zmm0, %%k5 %{%%k4%}") /* k5 = [x0 > start] && [x0 < end] */ \
        __ASM_EMIT("vcmpps              $1, 0x040 + %[knee], %%zmm4, %%k7 %{%%k6%}") \
        __ASM_EMIT("korw                %%k7, %%k5, %%k5") \
        __ASM_EMIT("kmovw               %%k5, %k[mask]") \
        __ASM_EMIT("test                %k[mask], %k[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%zmm0")                   /* zmm0 = gain_start */ \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%zmm4") \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%zmm0 %{%%k4%}")          /* zmm0 = [x0 > start] ? gain_end : gain_start */ \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%zmm4 %{%%k6%}") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%zmm0, 0x00 + %[mem]")                     /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%zmm4, 0x40 + %[mem]") \
        LOGE_CORE_X32                                                               /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_KNEE_SINGLE_X32                                                     /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: zmm0 = g0, zmm4 = g1 */

    #define PROCESS_GATE_FULL_X16 \
        /* in: zmm0 = x */ \
        __ASM_EMIT("vandps              0x00 + %[G2C], %%zmm0, %%zmm0")             /* zmm0 = fabsf(x0) */ \
        __ASM_EMIT("vcmpps              $6, 0x000 + %[knee], %%zmm0, %%k4")         /* k4   = [x0 > start] */ \
        __ASM_EMIT("vcmpps              $1, 0x040 + %[knee], %%zmm0, %%k5 %{%%k4%}")/* k5 = [x0 > start] && [x0 < end] */ \
        __ASM_EMIT("kmovw               %%k5, %k[mask]") \
        __ASM_EMIT("test                %k[mask], %k[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%zmm0")                   /* zmm0 = gain_start */ \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%zmm0 %{%%k4%}")          /* zmm0 = [x0 > start] ? gain_end : gain_start */ \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%zmm0, 0x00 + %[mem]")                     /* store fabsf(x0) */ \
        LOGE_CORE_X16                                                               /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_KNEE_SINGLE_X16                                                     /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: zmm0 = g */

    #define PROCESS_GATE_FULL_X8 \
        /* in: ymm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[G2C], %%ymm0, %%ymm0")             /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vcmpps              $6, 0x000 + %[knee], %%ymm0, %%k4")         /* k4   = [x0 > start] */ \
        __ASM_EMIT("vcmpps              $1, 0x040 + %[knee], %%ymm0, %%k5 %{%%k4%}")/* k5 = [x0 > start] && [x0 < end] */ \
        __ASM_EMIT("kmovw               %%k5, %k[mask]") \
        __ASM_EMIT("test                %k[mask], %k[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x080 + %[knee], %%ymm0")                   /* ymm0 = gain_start */ \
        __ASM_EMIT("vmovaps             0x0c0 + %[knee], %%ymm0 %{%%k4%}")          /* ymm0 = [x0 > start] ? gain_end : gain_start */ \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vandps              0x00 + %[G2C], %%ymm0, %%ymm0")             /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                     /* store fabsf(x0) */ \
        LOGE_CORE_X8                                                                /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_KNEE_SINGLE_X8                                                      /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = g */

    #define PROCESS_GATE_FULL_X4 \
        /* in: xmm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[G2C], %%xmm0, %%xmm0")             /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x00 + %[mem]")                     /* store fabsf(x0) */ \
        LOGE_CORE_X4                                                                /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_KNEE_SINGLE_X4                                                      /* apply knee */ \
        /* out: xmm0 = g */


        void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                gate_knee_t knee __lsp_aligned64;
                float mem[32] __lsp_aligned64;
                float stub[16] __lsp_aligned64;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_GATE_KNEE("knee", "gate")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                PROCESS_GATE_FULL_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                PROCESS_GATE_FULL_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_GATE_FULL_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
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
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("14:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("16:")

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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }

        void gate_x1_curve(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                gate_knee_t knee __lsp_aligned64;
                float mem[32] __lsp_aligned64;
                float stub[16] __lsp_aligned64;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_GATE_KNEE("knee", "gate")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                PROCESS_GATE_FULL_X32
                __ASM_EMIT("vmulps          0x00(%[src]), %%zmm0, %%zmm0")
                __ASM_EMIT("vmulps          0x40(%[src]), %%zmm4, %%zmm4")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                PROCESS_GATE_FULL_X16
                __ASM_EMIT("vmulps          0x00(%[src]), %%zmm0, %%zmm0")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_GATE_FULL_X8
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("vmulps          0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
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
                __ASM_EMIT("vmovaps         %%xmm0, %%xmm4")
                PROCESS_GATE_FULL_X4
                __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("14:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("16:")

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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }

    #undef PROCESS_GATE_FULL_X4
    #undef PROCESS_GATE_FULL_X8
    #undef PROCESS_GATE_FULL_X16
    #undef PROCESS_GATE_FULL_X32
    #undef PROCESS_KNEE_SINGLE_X4
    #undef PROCESS_KNEE_SINGLE_X8
    #undef PROCESS_KNEE_SINGLE_X16
    #undef PROCESS_KNEE_SINGLE_X32

    #undef UNPACK_GATE_KNEE

    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_DYNAMICS_GATE_H_ */
