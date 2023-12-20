/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 нояб. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_DYNAMICS_EXPANDER_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_DYNAMICS_EXPANDER_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

#include <private/dsp/arch/x86/avx2/pmath/exp.h>
#include <private/dsp/arch/x86/avx2/pmath/log.h>

namespace lsp
{
    namespace avx2
    {
    #pragma pack(push, 1)
        typedef struct expander_knee_t
        {
            float   start[8];       // +0x00
            float   end[8];         // +0x20
            float   threshold[8];   // +0x40
            float   herm[24];       // +0x60
            float   tilt[16];       // +0xc0
        } expander_knee_t;
    #pragma pack(pop)

        static const uint32_t expander_const[] __lsp_aligned32 =
        {
            LSP_DSP_VEC8(0x7fffffff),   // abs
            LSP_DSP_VEC8(0x3f800000)    // 1.0f
        };


    #define UNPACK_EXP_KNEE(DST, SRC) \
        __ASM_EMIT("vbroadcastss        0x00(%[" SRC "]), %%ymm0") \
        __ASM_EMIT("vbroadcastss        0x04(%[" SRC "]), %%ymm1") \
        __ASM_EMIT("vbroadcastss        0x08(%[" SRC "]), %%ymm2") \
        __ASM_EMIT("vbroadcastss        0x0c(%[" SRC "]), %%ymm3") \
        __ASM_EMIT("vbroadcastss        0x10(%[" SRC "]), %%ymm4") \
        __ASM_EMIT("vbroadcastss        0x14(%[" SRC "]), %%ymm5") \
        __ASM_EMIT("vbroadcastss        0x18(%[" SRC "]), %%ymm6") \
        __ASM_EMIT("vbroadcastss        0x1c(%[" SRC "]), %%ymm7") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x000 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm1, 0x020 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm2, 0x040 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm3, 0x060 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm4, 0x080 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm5, 0x0a0 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm6, 0x0c0 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm7, 0x0e0 + %[" DST "]")

    #define PROCESS_UKNEE_SINGLE_X32 \
        /* in: ymm0 = lx0, ymm4 = lx1, ymm8 = lx2, ymm12 = lx3 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm8, %%ymm10") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm12, %%ymm14") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm10, %%ymm10") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm14, %%ymm14") \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmulps              %%ymm5, %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              %%ymm9, %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps              %%ymm13, %%ymm12, %%ymm13") \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm8") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm12") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm8, %%ymm11") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm12, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm11, %%ymm10, %%ymm9, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm15, %%ymm14, %%ymm13, %%ymm12") \
        EXP_CORE_X32                                                            /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm9") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm13") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm5, %%ymm6") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm9, %%ymm10") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm13, %%ymm14") \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 <= start] ? 1.0 : EV */ \
        __ASM_EMIT("vblendvps           %%ymm6, 0x20 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm10, 0x20 + %[X2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm14, 0x20 + %[X2C], %%ymm12, %%ymm12") \
        /* out: ymm0 = g0, ymm4 = g1, ymm8 = g2, ymm12 = g3 */

    #define PROCESS_UKNEE_SINGLE_X16 \
        /* in: ymm0 = lx0, ymm4 = lx1 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmulps              %%ymm5, %%ymm4, %%ymm5") \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        EXP_CORE_X16                                                            /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm5, %%ymm6") \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 <= start] ? 1.0 : EV */ \
        __ASM_EMIT("vblendvps           %%ymm6, 0x20 + %[X2C], %%ymm4, %%ymm4") \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_UKNEE_SINGLE_X8 \
        /* in: ymm0 = lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X8                                                             /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 <= start] ? 1.0 : EV */ \
        /* out: ymm0 = g0 */

    #define PROCESS_UKNEE_SINGLE_X4 \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%xmm0, %%xmm1")        /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%xmm0, %%xmm2")        /* xmm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%xmm1, %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%xmm2, %%xmm2")        /* xmm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vmulps              %%xmm1, %%xmm0, %%xmm1")                /* xmm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%xmm1, %%xmm1")        /* xmm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm0")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%xmm0, %%xmm3")    /* xmm3 = [x0 >= end] */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm2, %%xmm1, %%xmm0")        /* xmm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X4                                                             /* xmm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm1")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%xmm1, %%xmm2")    /* xmm2 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%xmm2, 0x20 + %[X2C], %%xmm0, %%xmm0") /* xmm0 = [x0 <= start] ? 1.0 : EV */ \
        /* out: xmm0 = g0 */

    #define PROCESS_UEXP_FULL_X32 \
        /* in: ymm0 = x0, ymm4 = x1, ymm8 = x2, ymm12 = x4 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm12, %%ymm12") \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = min(fabsf(x0), threshold) */ \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm8, %%ymm8") \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm12, %%ymm12") \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 > start] */ \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm13, %%ymm9, %%ymm9") \
        __ASM_EMIT("vorps               %%ymm9, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm0")                 /* ymm0 = 1.0f */ \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm4") \
        __ASM_EMIT("vmovaps             %%ymm0, %%ymm8") \
        __ASM_EMIT("vmovaps             %%ymm4, %%ymm12") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0x40 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0x60 + %[mem]") \
        LOGE_CORE_X32                                                           /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X32                                                /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1, ymm8 = G2, ymm12 = G3 */

    #define PROCESS_UEXP_FULL_X16 \
        /* in: ymm0 = x0, ymm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = min(fabsf(x0), threshold) */ \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 > start] */ \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm0")                 /* ymm0 = 1.0f */ \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm4") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        LOGE_CORE_X16                                                           /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X16                                                /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1 */

    #define PROCESS_UEXP_FULL_X8 \
        /* in: ymm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = min(fabsf(x0), threshold) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X8                                                            /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X8                                                 /* apply knee */ \
        /* out: ymm0 = G0 */

    #define PROCESS_UEXP_FULL_X4 \
        /* in: xmm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%xmm0, %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("vminps              0x40 + %[knee], %%xmm0, %%xmm0")        /* xmm0 = min(fabsf(x0), threshold) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X4                                                            /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X4                                                 /* apply knee */ \
        /* out: xmm0 = G0 */

        void uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[16] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_UEXP_FULL_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_UEXP_FULL_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_UEXP_FULL_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                PROCESS_UEXP_FULL_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    IF_ARCH_X86_64(
        void x64_uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[32] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_UEXP_FULL_X32
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_UEXP_FULL_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_UEXP_FULL_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_UEXP_FULL_X4
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
                PROCESS_UEXP_FULL_X4
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
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

        void uexpander_x1_curve(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[16] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_UEXP_FULL_X16
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_UEXP_FULL_X8
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_UEXP_FULL_X4
                __ASM_EMIT("vmulps          0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                __ASM_EMIT("vmovaps         %%xmm0, %%xmm4")
                PROCESS_UEXP_FULL_X4
                __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    IF_ARCH_X86_64(
        void x64_uexpander_x1_curve(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[32] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_UEXP_FULL_X32
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          0x40(%[src]), %%ymm8, %%ymm8")
                __ASM_EMIT("vmulps          0x60(%[src]), %%ymm12, %%ymm12")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_UEXP_FULL_X16
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_UEXP_FULL_X8
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
                PROCESS_UEXP_FULL_X4
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
                PROCESS_UEXP_FULL_X4
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
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

    #undef PROCESS_UEXP_FULL_X4
    #undef PROCESS_UEXP_FULL_X8
    #undef PROCESS_UEXP_FULL_X16
    #undef PROCESS_UEXP_FULL_X32
    #undef PROCESS_UKNEE_SINGLE_X4
    #undef PROCESS_UKNEE_SINGLE_X8
    #undef PROCESS_UKNEE_SINGLE_X16
    #undef PROCESS_UKNEE_SINGLE_X32


    #define PROCESS_DKNEE_SINGLE_X32 \
        /* in: ymm0 = lx0, ymm4 = lx1, ymm8 = lx2, ymm12 = lx3 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm8, %%ymm10") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm12, %%ymm14") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm10, %%ymm10") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm14, %%ymm14") \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmulps              %%ymm5, %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              %%ymm9, %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps              %%ymm13, %%ymm12, %%ymm13") \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm8") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm12") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm8, %%ymm11") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm12, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 <= start] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm11, %%ymm10, %%ymm9, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm15, %%ymm14, %%ymm13, %%ymm12") \
        EXP_CORE_X32                                                            /* ymm0 = EV = expf([x0 <= start] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm9") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm13") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm5, %%ymm6") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm9, %%ymm10") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm13, %%ymm14") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm1, %%ymm3")    /* ymm3 = [x0 >= threshold] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm5, %%ymm7") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm9, %%ymm11") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm13, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vblendvps           %%ymm6, 0x20 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm10, 0x20 + %[X2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm14, 0x20 + %[X2C], %%ymm12, %%ymm12") \
        __ASM_EMIT("vandps              %%ymm3, %%ymm0, %%ymm0")                /* ymm0 = [x0 < threshold] ? 0 : [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vandps              %%ymm7, %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              %%ymm11, %%ymm8, %%ymm8") \
        __ASM_EMIT("vandps              %%ymm15, %%ymm12, %%ymm12") \
        /* out: ymm0 = g0, ymm4 = g1, ymm8 = g2, ymm12 = g3 */

    #define PROCESS_DKNEE_SINGLE_X16 \
        /* in: ymm0 = lx0, ymm4 = lx1 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmulps              %%ymm5, %%ymm4, %%ymm5") \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 <= start] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        EXP_CORE_X16                                                            /* ymm0 = EV = expf([x0 <= start] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm5, %%ymm6") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm1, %%ymm3")    /* ymm3 = [x0 >= threshold] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm5, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vblendvps           %%ymm6, 0x20 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              %%ymm3, %%ymm0, %%ymm0")                /* ymm0 = [x0 < threshold] ? 0 : [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vandps              %%ymm7, %%ymm4, %%ymm4") \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_DKNEE_SINGLE_X8 \
        /* in: ymm0 = lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 <= start] ? TV : KV */ \
        EXP_CORE_X8                                                             /* ymm0 = EV = expf([x0 <= start] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm1, %%ymm3")    /* ymm3 = [x0 >= threshold] */ \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vandps              %%ymm3, %%ymm0, %%ymm0")                /* ymm0 = [x0 < threshold] ? 0 : [x0 >= end] ? 1.0 : EV */ \
        /* out: ymm0 = g0 */

    #define PROCESS_DKNEE_SINGLE_X4 \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%xmm0, %%xmm1")        /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%xmm0, %%xmm2")        /* xmm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%xmm1, %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%xmm2, %%xmm2")        /* xmm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vmulps              %%xmm1, %%xmm0, %%xmm1")                /* xmm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vaddps              0xa0 + %[knee], %%xmm1, %%xmm1")        /* xmm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm0")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%xmm0, %%xmm3")    /* xmm3 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm2, %%xmm1, %%xmm0")        /* xmm0 = [x0 <= start] ? TV : KV */ \
        EXP_CORE_X4                                                             /* xmm0 = EV = expf([x0 <= start] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm1")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%xmm1, %%xmm2")    /* xmm2 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%xmm1, %%xmm3")    /* ymm3 = [x0 >= threshold] */ \
        __ASM_EMIT("vblendvps           %%xmm2, 0x20 + %[X2C], %%xmm0, %%xmm0") /* xmm0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vandps              %%xmm3, %%xmm0, %%xmm0")                /* xmm0 = [x0 < threshold] ? 0 : [x0 >= end] ? 1.0 : EV */ \
        /* out: xmm0 = g0 */

    #define PROCESS_DEXP_FULL_X32 \
        /* in: ymm0 = x0, ymm4 = x1, ymm8 = x2, ymm12 = x4 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm12, %%ymm12") \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 < end] */ \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm13, %%ymm9, %%ymm9") \
        __ASM_EMIT("vorps               %%ymm9, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm0")                 /* ymm0 = 1.0f */ \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm4") \
        __ASM_EMIT("vmovaps             %%ymm0, %%ymm8") \
        __ASM_EMIT("vmovaps             %%ymm4, %%ymm12") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0x40 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0x60 + %[mem]") \
        LOGE_CORE_X32                                                           /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X32                                                /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1, ymm8 = G2, ymm12 = G3 */

    #define PROCESS_DEXP_FULL_X16 \
        /* in: ymm0 = x0, ymm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 < end] */ \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm0")                 /* ymm0 = 1.0f */ \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm4") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        LOGE_CORE_X16                                                           /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X16                                                /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1 */

    #define PROCESS_DEXP_FULL_X8 \
        /* in: ymm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X8                                                            /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X8                                                 /* apply knee */ \
        /* out: ymm0 = G0 */

    #define PROCESS_DEXP_FULL_X4 \
        /* in: xmm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%xmm0, %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X4                                                            /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X4                                                 /* apply knee */ \
        /* out: xmm0 = G0 */

        void dexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[16] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_DEXP_FULL_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_DEXP_FULL_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_DEXP_FULL_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                PROCESS_DEXP_FULL_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    IF_ARCH_X86_64(
        void x64_dexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[32] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_DEXP_FULL_X32
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_DEXP_FULL_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_DEXP_FULL_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_DEXP_FULL_X4
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
                PROCESS_DEXP_FULL_X4
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
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

        void dexpander_x1_curve(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[16] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_DEXP_FULL_X16
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_DEXP_FULL_X8
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_DEXP_FULL_X4
                __ASM_EMIT("vmulps          0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                __ASM_EMIT("vmovaps         %%xmm0, %%xmm4")
                PROCESS_DEXP_FULL_X4
                __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    IF_ARCH_X86_64(
        void x64_dexpander_x1_curve(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[32] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_DEXP_FULL_X32
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          0x40(%[src]), %%ymm8, %%ymm8")
                __ASM_EMIT("vmulps          0x60(%[src]), %%ymm12, %%ymm12")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_DEXP_FULL_X16
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_DEXP_FULL_X8
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
                PROCESS_DEXP_FULL_X4
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
                PROCESS_DEXP_FULL_X4
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
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

    #undef PROCESS_DEXP_FULL_X4
    #undef PROCESS_DEXP_FULL_X8
    #undef PROCESS_DEXP_FULL_X16
    #undef PROCESS_DEXP_FULL_X32
    #undef PROCESS_DKNEE_SINGLE_X4
    #undef PROCESS_DKNEE_SINGLE_X8
    #undef PROCESS_DKNEE_SINGLE_X16
    #undef PROCESS_DKNEE_SINGLE_X32


    #define PROCESS_UKNEE_SINGLE_X32_FMA3 \
        /* in: ymm0 = lx0, ymm4 = lx1, ymm8 = lx2, ymm12 = lx3 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm8, %%ymm10") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm12, %%ymm14") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm10, %%ymm10") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm14, %%ymm14") \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm0, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm8") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm12") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm8, %%ymm11") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm12, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm11, %%ymm10, %%ymm9, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm15, %%ymm14, %%ymm13, %%ymm12") \
        EXP_CORE_X32_FMA3                                                       /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm9") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm13") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm5, %%ymm6") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm9, %%ymm10") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm13, %%ymm14") \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 <= start] ? 1.0 : EV */ \
        __ASM_EMIT("vblendvps           %%ymm6, 0x20 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm10, 0x20 + %[X2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm14, 0x20 + %[X2C], %%ymm12, %%ymm12") \
        /* out: ymm0 = g0, ymm4 = g1, ymm8 = g2, ymm12 = g3 */

    #define PROCESS_UKNEE_SINGLE_X16_FMA3 \
        /* in: ymm0 = lx0, ymm4 = lx1 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm0, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        EXP_CORE_X16_FMA3                                                       /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm5, %%ymm6") \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 <= start] ? 1.0 : EV */ \
        __ASM_EMIT("vblendvps           %%ymm6, 0x20 + %[X2C], %%ymm4, %%ymm4") \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_UKNEE_SINGLE_X8_FMA3 \
        /* in: ymm0 = lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm0, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X8_FMA3                                                        /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 <= start] ? 1.0 : EV */ \
        /* out: ymm0 = g0 */

    #define PROCESS_UKNEE_SINGLE_X4_FMA3 \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%xmm0, %%xmm1")        /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%xmm0, %%xmm2")        /* xmm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%xmm1, %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%xmm2, %%xmm2")        /* xmm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%xmm0, %%xmm1")        /* xmm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm0")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%xmm0, %%xmm3")    /* xmm3 = [x0 >= end] */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm2, %%xmm1, %%xmm0")        /* xmm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X4_FMA3                                                        /* xmm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm1")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%xmm1, %%xmm2")    /* xmm2 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%xmm2, 0x20 + %[X2C], %%xmm0, %%xmm0") /* xmm0 = [x0 <= start] ? 1.0 : EV */ \
        /* out: xmm0 = g0 */

    #define PROCESS_UEXP_FULL_X32_FMA3 \
        /* in: ymm0 = x0, ymm4 = x1, ymm8 = x2, ymm12 = x4 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm12, %%ymm12") \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = min(fabsf(x0), threshold) */ \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm8, %%ymm8") \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm12, %%ymm12") \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 > start] */ \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm13, %%ymm9, %%ymm9") \
        __ASM_EMIT("vorps               %%ymm9, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm0")                 /* ymm0 = 1.0f */ \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm4") \
        __ASM_EMIT("vmovaps             %%ymm0, %%ymm8") \
        __ASM_EMIT("vmovaps             %%ymm4, %%ymm12") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0x40 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0x60 + %[mem]") \
        LOGE_CORE_X32_FMA3                                                      /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X32_FMA3                                           /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1, ymm8 = G2, ymm12 = G3 */

    #define PROCESS_UEXP_FULL_X16_FMA3 \
        /* in: ymm0 = x0, ymm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = min(fabsf(x0), threshold) */ \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 > start] */ \
        __ASM_EMIT("vcmpps              $6, 0x00 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm0")                 /* ymm0 = 1.0f */ \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm4") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        LOGE_CORE_X16_FMA3                                                      /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X16_FMA3                                           /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1 */

    #define PROCESS_UEXP_FULL_X8_FMA3 \
        /* in: ymm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vminps              0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = min(fabsf(x0), threshold) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X8_FMA3                                                       /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X8_FMA3                                            /* apply knee */ \
        /* out: ymm0 = G0 */

    #define PROCESS_UEXP_FULL_X4_FMA3 \
        /* in: xmm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%xmm0, %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("vminps              0x40 + %[knee], %%xmm0, %%xmm0")        /* xmm0 = min(fabsf(x0), threshold) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X4_FMA3                                                       /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_UKNEE_SINGLE_X4_FMA3                                            /* apply knee */ \
        /* out: xmm0 = G0 */

        void uexpander_x1_gain_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[16] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_UEXP_FULL_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_UEXP_FULL_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_UEXP_FULL_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                PROCESS_UEXP_FULL_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    IF_ARCH_X86_64(
        void x64_uexpander_x1_gain_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[32] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_UEXP_FULL_X32_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_UEXP_FULL_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_UEXP_FULL_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_UEXP_FULL_X4_FMA3
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
                PROCESS_UEXP_FULL_X4_FMA3
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
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

        void uexpander_x1_curve_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[16] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_UEXP_FULL_X16_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_UEXP_FULL_X8_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_UEXP_FULL_X4_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                __ASM_EMIT("vmovaps         %%xmm0, %%xmm4")
                PROCESS_UEXP_FULL_X4_FMA3
                __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    IF_ARCH_X86_64(
        void x64_uexpander_x1_curve_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[32] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_UEXP_FULL_X32_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          0x40(%[src]), %%ymm8, %%ymm8")
                __ASM_EMIT("vmulps          0x60(%[src]), %%ymm12, %%ymm12")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_UEXP_FULL_X16_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_UEXP_FULL_X8_FMA3
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
                PROCESS_UEXP_FULL_X4_FMA3
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
                PROCESS_UEXP_FULL_X4_FMA3
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
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

    #undef PROCESS_UEXP_FULL_X4_FMA3
    #undef PROCESS_UEXP_FULL_X8_FMA3
    #undef PROCESS_UEXP_FULL_X16_FMA3
    #undef PROCESS_UEXP_FULL_X32_FMA3
    #undef PROCESS_UKNEE_SINGLE_X4_FMA3
    #undef PROCESS_UKNEE_SINGLE_X8_FMA3
    #undef PROCESS_UKNEE_SINGLE_X16_FMA3
    #undef PROCESS_UKNEE_SINGLE_X32_FMA3


    #define PROCESS_DKNEE_SINGLE_X32_FMA3 \
        /* in: ymm0 = lx0, ymm4 = lx1, ymm8 = lx2, ymm12 = lx3 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm8, %%ymm10") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm12, %%ymm14") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm10, %%ymm10") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm14, %%ymm14") \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm0, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm8") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm12") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm8, %%ymm11") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm12, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 <= start] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm11, %%ymm10, %%ymm9, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm15, %%ymm14, %%ymm13, %%ymm12") \
        EXP_CORE_X32_FMA3                                                       /* ymm0 = EV = expf([x0 <= start] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm9") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm13") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm5, %%ymm6") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm9, %%ymm10") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm13, %%ymm14") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm1, %%ymm3")    /* ymm3 = [x0 >= threshold] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm5, %%ymm7") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm9, %%ymm11") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm13, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vblendvps           %%ymm6, 0x20 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm10, 0x20 + %[X2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm14, 0x20 + %[X2C], %%ymm12, %%ymm12") \
        __ASM_EMIT("vandps              %%ymm3, %%ymm0, %%ymm0")                /* ymm0 = [x0 < threshold] ? 0 : [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vandps              %%ymm7, %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              %%ymm11, %%ymm8, %%ymm8") \
        __ASM_EMIT("vandps              %%ymm15, %%ymm12, %%ymm12") \
        /* out: ymm0 = g0, ymm4 = g1, ymm8 = g2, ymm12 = g3 */

    #define PROCESS_DKNEE_SINGLE_X16_FMA3 \
        /* in: ymm0 = lx0, ymm4 = lx1 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm0, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 <= start] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        EXP_CORE_X16_FMA3                                                       /* ymm0 = EV = expf([x0 <= start] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm5, %%ymm6") \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm1, %%ymm3")    /* ymm3 = [x0 >= threshold] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm5, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vblendvps           %%ymm6, 0x20 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              %%ymm3, %%ymm0, %%ymm0")                /* ymm0 = [x0 < threshold] ? 0 : [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vandps              %%ymm7, %%ymm4, %%ymm4") \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_DKNEE_SINGLE_X8_FMA3 \
        /* in: ymm0 = lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%ymm0, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%ymm0, %%ymm3")    /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 <= start] ? TV : KV */ \
        EXP_CORE_X8_FMA3                                                        /* ymm0 = EV = expf([x0 <= start] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%ymm1, %%ymm2")    /* ymm2 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%ymm1, %%ymm3")    /* ymm3 = [x0 >= threshold] */ \
        __ASM_EMIT("vblendvps           %%ymm2, 0x20 + %[X2C], %%ymm0, %%ymm0") /* ymm0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vandps              %%ymm3, %%ymm0, %%ymm0")                /* ymm0 = [x0 < threshold] ? 0 : [x0 >= end] ? 1.0 : EV */ \
        /* out: ymm0 = g0 */

    #define PROCESS_DKNEE_SINGLE_X4_FMA3 \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("vmulps              0x60 + %[knee], %%xmm0, %%xmm1")        /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps              0xc0 + %[knee], %%xmm0, %%xmm2")        /* xmm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps              0x80 + %[knee], %%xmm1, %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps              0xe0 + %[knee], %%xmm2, %%xmm2")        /* xmm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vfmadd213ps         0xa0 + %[knee], %%xmm0, %%xmm1")        /* xmm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm0")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, 0x00 + %[knee], %%xmm0, %%xmm3")    /* xmm3 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm2, %%xmm1, %%xmm0")        /* xmm0 = [x0 <= start] ? TV : KV */ \
        EXP_CORE_X4_FMA3                                                        /* xmm0 = EV = expf([x0 <= start] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm1")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, 0x20 + %[knee], %%xmm1, %%xmm2")    /* xmm2 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, 0x40 + %[knee], %%xmm1, %%xmm3")    /* xmm3 = [x0 >= threshold] */ \
        __ASM_EMIT("vblendvps           %%xmm2, 0x20 + %[X2C], %%xmm0, %%xmm0") /* xmm0 = [x0 >= end] ? 1.0 : EV */ \
        __ASM_EMIT("vandps              %%xmm3, %%xmm0, %%xmm0")                /* xmm0 = [x0 < threshold] ? 0 : [x0 >= end] ? 1.0 : EV */ \
        /* out: xmm0 = g0 */

    #define PROCESS_DEXP_FULL_X32_FMA3 \
        /* in: ymm0 = x0, ymm4 = x1, ymm8 = x2, ymm12 = x4 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm12, %%ymm12") \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 < end] */ \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm13, %%ymm9, %%ymm9") \
        __ASM_EMIT("vorps               %%ymm9, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm0")                 /* ymm0 = 1.0f */ \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm4") \
        __ASM_EMIT("vmovaps             %%ymm0, %%ymm8") \
        __ASM_EMIT("vmovaps             %%ymm4, %%ymm12") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0x40 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0x60 + %[mem]") \
        LOGE_CORE_X32_FMA3                                                      /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X32_FMA3                                           /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1, ymm8 = G2, ymm12 = G3 */

    #define PROCESS_DEXP_FULL_X16_FMA3 \
        /* in: ymm0 = x0, ymm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 < end] */ \
        __ASM_EMIT("vcmpps              $1, 0x20 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm0")                 /* ymm0 = 1.0f */ \
        __ASM_EMIT("vmovaps             0x20 + %[X2C], %%ymm4") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        LOGE_CORE_X16_FMA3                                                      /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X16_FMA3                                           /* apply knee */ \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1 */

    #define PROCESS_DEXP_FULL_X8_FMA3 \
        /* in: ymm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X8_FMA3                                                       /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X8_FMA3                                            /* apply knee */ \
        /* out: ymm0 = G0 */

    #define PROCESS_DEXP_FULL_X4_FMA3 \
        /* in: xmm0 = x0 */ \
        __ASM_EMIT("vandps              0x00 + %[X2C], %%xmm0, %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X4_FMA3                                                       /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        PROCESS_DKNEE_SINGLE_X4_FMA3                                            /* apply knee */ \
        /* out: xmm0 = G0 */

        void dexpander_x1_gain_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[16] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_DEXP_FULL_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_DEXP_FULL_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_DEXP_FULL_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                PROCESS_DEXP_FULL_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    IF_ARCH_X86_64(
        void x64_dexpander_x1_gain_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[32] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_DEXP_FULL_X32_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_DEXP_FULL_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_DEXP_FULL_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_DEXP_FULL_X4_FMA3
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
                PROCESS_DEXP_FULL_X4_FMA3
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
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

        void dexpander_x1_curve_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[16] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_DEXP_FULL_X16_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_DEXP_FULL_X8_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_DEXP_FULL_X4_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                __ASM_EMIT("vmovaps         %%xmm0, %%xmm4")
                PROCESS_DEXP_FULL_X4_FMA3
                __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count),
                  [mask] "=&r" (mask)
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    IF_ARCH_X86_64(
        void x64_dexpander_x1_curve_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count)
        {
            IF_ARCH_X86(
                float stub0[16] __lsp_aligned32;
                expander_knee_t knee __lsp_aligned32;
                float mem[32] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_EXP_KNEE("knee", "exp")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_DEXP_FULL_X32_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          0x40(%[src]), %%ymm8, %%ymm8")
                __ASM_EMIT("vmulps          0x60(%[src]), %%ymm12, %%ymm12")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_DEXP_FULL_X16_FMA3
                __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          0x20(%[src]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // 8x blocks
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                PROCESS_DEXP_FULL_X8_FMA3
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
                PROCESS_DEXP_FULL_X4_FMA3
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
                PROCESS_DEXP_FULL_X4_FMA3
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
                : [exp] "r" (c),
                  [knee] "o" (knee),
                  [mem] "o" (mem),
                  [stub0] "o" (stub0),
                  [stub] "o" (stub),
                  [X2C] "o" (expander_const),
                  [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C),
                  [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

    #undef PROCESS_DEXP_FULL_X4_FMA3
    #undef PROCESS_DEXP_FULL_X8_FMA3
    #undef PROCESS_DEXP_FULL_X16_FMA3
    #undef PROCESS_DEXP_FULL_X32_FMA3
    #undef PROCESS_DKNEE_SINGLE_X4_FMA3
    #undef PROCESS_DKNEE_SINGLE_X8_FMA3
    #undef PROCESS_DKNEE_SINGLE_X16_FMA3
    #undef PROCESS_DKNEE_SINGLE_X32_FMA3

    #undef UNPACK_EXP_KNEE

    } /* namespace avx2 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX2_DYNAMICS_EXPANDER_H_ */
