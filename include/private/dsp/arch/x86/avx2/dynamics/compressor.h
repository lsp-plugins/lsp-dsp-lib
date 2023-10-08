/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_DYNAMICS_COMPRESSOR_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_DYNAMICS_COMPRESSOR_H_

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
        typedef struct comp_knee_t
        {
            float   start[8];   // +0x00
            float   end[8];     // +0x20
            float   gain[8];    // +0x40
            float   herm[24];   // +0x60
            float   tilt[16];   // +0xc0
        } comp_knee_t;
    #pragma pack(pop)

        static const uint32_t compressor_const[] __lsp_aligned32 =
        {
            LSP_DSP_VEC8(0x7fffffff)
        };


    #define UNPACK_COMP_KNEE(DST, DOFF, SRC, SOFF) \
        __ASM_EMIT("vbroadcastss " SOFF " + 0x00(%[" SRC "]), %%ymm0") \
        __ASM_EMIT("vbroadcastss " SOFF " + 0x04(%[" SRC "]), %%ymm1") \
        __ASM_EMIT("vbroadcastss " SOFF " + 0x08(%[" SRC "]), %%ymm2") \
        __ASM_EMIT("vbroadcastss " SOFF " + 0x0c(%[" SRC "]), %%ymm3") \
        __ASM_EMIT("vbroadcastss " SOFF " + 0x10(%[" SRC "]), %%ymm4") \
        __ASM_EMIT("vbroadcastss " SOFF " + 0x14(%[" SRC "]), %%ymm5") \
        __ASM_EMIT("vbroadcastss " SOFF " + 0x18(%[" SRC "]), %%ymm6") \
        __ASM_EMIT("vbroadcastss " SOFF " + 0x1c(%[" SRC "]), %%ymm7") \
        __ASM_EMIT("vmovaps             %%ymm0, " DOFF " + 0x000 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm1, " DOFF " + 0x020 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm2, " DOFF " + 0x040 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm3, " DOFF " + 0x060 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm4, " DOFF " + 0x080 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm5, " DOFF " + 0x0a0 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm6, " DOFF " + 0x0c0 + %[" DST "]") \
        __ASM_EMIT("vmovaps             %%ymm7, " DOFF " + 0x0e0 + %[" DST "]")

    #define PROCESS_KNEE_SINGLE_X32(OFF) \
        /* in: ymm0 = lx0, ymm4 = lx1 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm8, %%ymm10") \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm12, %%ymm14") \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm10, %%ymm10") \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm14, %%ymm14") \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmulps              %%ymm5, %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps              %%ymm9, %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps              %%ymm13, %%ymm12, %%ymm13") \
        __ASM_EMIT("vaddps    " OFF " + 0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xa0 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps    " OFF " + 0xa0 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps    " OFF " + 0xa0 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm8") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm12") \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm0, %%ymm3")  /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm8, %%ymm11") \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm12, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm11, %%ymm10, %%ymm9, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm15, %%ymm14, %%ymm13, %%ymm12") \
        EXP_CORE_X32                                                            /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm9") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm13") \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm1, %%ymm3")      /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm5, %%ymm7") \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm9, %%ymm11") \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm13, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm3, " OFF " + 0x40 + %[knee], %%ymm0, %%ymm0")  /* ymm0 = [x0 <= start] ? GV : EV */ \
        __ASM_EMIT("vblendvps           %%ymm7, " OFF " + 0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm11, " OFF " + 0x40 + %[knee], %%ymm8, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm15, " OFF " + 0x40 + %[knee], %%ymm12, %%ymm12") \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_KNEE_SINGLE_X16(OFF) \
        /* in: ymm0 = lx0, ymm4 = lx1 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vmulps              %%ymm5, %%ymm4, %%ymm5") \
        __ASM_EMIT("vaddps    " OFF " + 0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xa0 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm0, %%ymm3")  /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        EXP_CORE_X16                                                            /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm1, %%ymm3")      /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm5, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm3, " OFF " + 0x40 + %[knee], %%ymm0, %%ymm0")  /* ymm0 = [x0 <= start] ? GV : EV */ \
        __ASM_EMIT("vblendvps           %%ymm7, " OFF " + 0x40 + %[knee], %%ymm4, %%ymm4") \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_KNEE_SINGLE_X8(OFF) \
        /* in: ymm0 = lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vmulps              %%ymm1, %%ymm0, %%ymm1")                /* ymm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vaddps    " OFF " + 0xa0 + %[knee], %%ymm1, %%ymm1")        /* ymm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm0, %%ymm3")  /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X8                                                             /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm1, %%ymm3")      /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%ymm3, " OFF " + 0x40 + %[knee], %%ymm0, %%ymm0")  /* ymm0 = [x0 <= start] ? GV : EV */ \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_KNEE_SINGLE_X4(OFF) \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%xmm0, %%xmm1")        /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%xmm0, %%xmm2")        /* xmm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%xmm1, %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%xmm2, %%xmm2")        /* xmm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vmulps              %%xmm1, %%xmm0, %%xmm1")                /* xmm1 = (herm[0]*lx0+herm[1])*lx0 */ \
        __ASM_EMIT("vaddps    " OFF " + 0xa0 + %[knee], %%xmm1, %%xmm1")        /* xmm5 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm0")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%xmm0, %%xmm3")  /* xmm3 = [x0 >= end] */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm2, %%xmm1, %%xmm0")        /* xmm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X4                                                             /* xmm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm1")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%xmm1, %%xmm3")      /* xmm3 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%xmm3, " OFF " + 0x40 + %[knee], %%xmm0, %%xmm0")  /* xmm0 = [x0 <= start] ? GV : EV */ \
        /* out: xmm0 = g0, xmm4 = g1 */

    #define PROCESS_COMP_FULL_X32 \
        /* in: ymm0 = x0, ymm4 = x1, ymm8 = x2, ymm12 = x4 */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm12, %%ymm12") \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 > k0.start] */ \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm0, %%ymm2")    /* ymm2 = [x0 > k0.start] */ \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm8, %%ymm10") \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm12, %%ymm14") \
        __ASM_EMIT("vorps               %%ymm2, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm6, %%ymm5, %%ymm5") \
        __ASM_EMIT("vorps               %%ymm10, %%ymm9, %%ymm9") \
        __ASM_EMIT("vorps               %%ymm14, %%ymm13, %%ymm13") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm13, %%ymm9, %%ymm9") \
        __ASM_EMIT("vorps               %%ymm9, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps     0x000 + 0x40 + %[knee], %%ymm0")                /* ymm0 = g1 */ \
        __ASM_EMIT("vmovaps     0x000 + 0x40 + %[knee], %%ymm4") \
        __ASM_EMIT("vmulps      0x100 + 0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = G = g0*g1 */ \
        __ASM_EMIT("vmulps      0x100 + 0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("vmovaps             %%ymm0, %%ymm8") \
        __ASM_EMIT("vmovaps             %%ymm4, %%ymm12") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0x40 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0x60 + %[mem]") \
        LOGE_CORE_X32                                                           /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x80 + %[mem]")                 /* *mem = lx0 */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0xa0 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0xc0 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0xe0 + %[mem]") \
        PROCESS_KNEE_SINGLE_X32("0x000")                                        /* apply knee 0 */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x100 + %[mem]")                 /* *mem = g1 */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x120 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0x140 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0x160 + %[mem]") \
        __ASM_EMIT("vmovaps             0x80 + %[mem], %%ymm0")                 /* ymm0 = lx0 */ \
        __ASM_EMIT("vmovaps             0xa0 + %[mem], %%ymm4") \
        __ASM_EMIT("vmovaps             0xc0 + %[mem], %%ymm8") \
        __ASM_EMIT("vmovaps             0xe0 + %[mem], %%ymm12") \
        PROCESS_KNEE_SINGLE_X32("0x100")                                        /* apply knee 1 */ \
        __ASM_EMIT("vmulps              0x100 + %[mem], %%ymm0, %%ymm0")         /* ymm0 = G = g0*g1 */ \
        __ASM_EMIT("vmulps              0x120 + %[mem], %%ymm4, %%ymm4") \
        __ASM_EMIT("vmulps              0x140 + %[mem], %%ymm8, %%ymm8") \
        __ASM_EMIT("vmulps              0x160 + %[mem], %%ymm12, %%ymm12") \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1, ymm8 = G2, ymm12 = G3 */

    #define PROCESS_COMP_FULL_X16 \
        /* in: ymm0 = x0, ymm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 > k0.start] */ \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm0, %%ymm2")    /* ymm2 = [x0 > k0.start] */ \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vorps               %%ymm2, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm6, %%ymm5, %%ymm5") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps     0x000 + 0x40 + %[knee], %%ymm0")                /* ymm0 = g1 */ \
        __ASM_EMIT("vmovaps     0x000 + 0x40 + %[knee], %%ymm4") \
        __ASM_EMIT("vmulps      0x100 + 0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = G = g0*g1 */ \
        __ASM_EMIT("vmulps      0x100 + 0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        LOGE_CORE_X16                                                           /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x40 + %[mem]")                 /* *mem = lx0 */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x60 + %[mem]") \
        PROCESS_KNEE_SINGLE_X16("0x000")                                        /* apply knee 0 */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x80 + %[mem]")                 /* *mem = g1 */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0xa0 + %[mem]") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm0")                 /* ymm0 = lx0 */ \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm4") \
        PROCESS_KNEE_SINGLE_X16("0x100")                                        /* apply knee 1 */ \
        __ASM_EMIT("vmulps              0x80 + %[mem], %%ymm0, %%ymm0")         /* ymm0 = G = g0*g1 */ \
        __ASM_EMIT("vmulps              0xa0 + %[mem], %%ymm4, %%ymm4") \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1 */

    #define PROCESS_COMP_FULL_X8 \
        /* in: ymm0 = x0, ymm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X8                                                            /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x40 + %[mem]")                 /* *mem = lx0 */ \
        PROCESS_KNEE_SINGLE_X8("0x000")                                         /* apply knee 0 */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x80 + %[mem]")                 /* *mem = g1 */ \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm0")                 /* ymm0 = lx0 */ \
        PROCESS_KNEE_SINGLE_X8("0x100")                                         /* apply knee 1 */ \
        __ASM_EMIT("vmulps              0x80 + %[mem], %%ymm0, %%ymm0")         /* ymm0 = G = g0*g1 */ \
        /* out: ymm0 = G0 */

    #define PROCESS_COMP_FULL_X4 \
        /* in: xmm0 = x0, xmm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%xmm0, %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X4                                                            /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x40 + %[mem]")                 /* *mem = lx0 */ \
        PROCESS_KNEE_SINGLE_X4("0x000")                                         /* apply knee 0 */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x80 + %[mem]")                 /* *mem = g1 */ \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%xmm0")                 /* xmm0 = lx0 */ \
        PROCESS_KNEE_SINGLE_X4("0x100")                                         /* apply knee 1 */ \
        __ASM_EMIT("vmulps              0x80 + %[mem], %%xmm0, %%xmm0")         /* xmm0 = G = g0*g1 */ \
        /* out: xmm0 = G0 */

        void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_X86(
                comp_knee_t knee[2] __lsp_aligned32;
                float mem[48] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_COMP_KNEE("knee", "0x000", "comp", "0x00")
                UNPACK_COMP_KNEE("knee", "0x100", "comp", "0x20")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_COMP_FULL_X16
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
                PROCESS_COMP_FULL_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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

    IF_ARCH_X86_64(
        void x64_compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_X86(
                comp_knee_t knee[2] __lsp_aligned32;
                float mem[96] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_COMP_KNEE("knee", "0x000", "comp", "0x00")
                UNPACK_COMP_KNEE("knee", "0x100", "comp", "0x20")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_COMP_FULL_X32
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
                PROCESS_COMP_FULL_X16
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
                PROCESS_COMP_FULL_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

        void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_X86(
                comp_knee_t knee[2] __lsp_aligned32;
                float mem[48] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_COMP_KNEE("knee", "0x000", "comp", "0x00")
                UNPACK_COMP_KNEE("knee", "0x100", "comp", "0x20")

                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                PROCESS_COMP_FULL_X16
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
                PROCESS_COMP_FULL_X8
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
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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

    IF_ARCH_X86_64(
        void x64_compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_X86(
                comp_knee_t knee[2] __lsp_aligned32;
                float mem[96] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_COMP_KNEE("knee", "0x000", "comp", "0x00")
                UNPACK_COMP_KNEE("knee", "0x100", "comp", "0x20")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_COMP_FULL_X32
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
                PROCESS_COMP_FULL_X16
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
                PROCESS_COMP_FULL_X8
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
                PROCESS_COMP_FULL_X4
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
                PROCESS_COMP_FULL_X4
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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

    #undef PROCESS_COMP_FULL_X4
    #undef PROCESS_COMP_FULL_X8
    #undef PROCESS_COMP_FULL_X16
    #undef PROCESS_COMP_FULL_X32
    #undef PROCESS_KNEE_SINGLE_X4
    #undef PROCESS_KNEE_SINGLE_X8
    #undef PROCESS_KNEE_SINGLE_X16
    #undef PROCESS_KNEE_SINGLE_X32

    #define PROCESS_KNEE_SINGLE_X32_FMA3(OFF) \
        /* in: ymm0 = lx0, ymm4 = lx1 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm8, %%ymm10") \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm12, %%ymm14") \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm9, %%ymm9") \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm13, %%ymm13") \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm10, %%ymm10") \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm14, %%ymm14") \
        __ASM_EMIT("vfmadd213ps " OFF " + 0xa0 + %[knee], %%ymm0, %%ymm1")      /* ymm1 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps " OFF " + 0xa0 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vfmadd213ps " OFF " + 0xa0 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vfmadd213ps " OFF " + 0xa0 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm8") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm12") \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm0, %%ymm3")  /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm8, %%ymm11") \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm12, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm11, %%ymm10, %%ymm9, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm15, %%ymm14, %%ymm13, %%ymm12") \
        EXP_CORE_X32_FMA3                                                       /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm9") \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm13") \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm1, %%ymm3")      /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm5, %%ymm7") \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm9, %%ymm11") \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm13, %%ymm15") \
        __ASM_EMIT("vblendvps           %%ymm3, " OFF " + 0x40 + %[knee], %%ymm0, %%ymm0")  /* ymm0 = [x0 <= start] ? GV : EV */ \
        __ASM_EMIT("vblendvps           %%ymm7, " OFF " + 0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("vblendvps           %%ymm11, " OFF " + 0x40 + %[knee], %%ymm8, %%ymm8") \
        __ASM_EMIT("vblendvps           %%ymm15, " OFF " + 0x40 + %[knee], %%ymm12, %%ymm12") \

    #define PROCESS_KNEE_SINGLE_X16_FMA3(OFF) \
        /* in: ymm0 = lx0, ymm4 = lx1 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm5, %%ymm5") \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm6, %%ymm6") \
        __ASM_EMIT("vfmadd213ps " OFF " + 0xa0 + %[knee], %%ymm0, %%ymm1")      /* ymm1 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vfmadd213ps " OFF " + 0xa0 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm4") \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm0, %%ymm3")  /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm4, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        __ASM_EMIT("vblendvps           %%ymm7, %%ymm6, %%ymm5, %%ymm4") \
        EXP_CORE_X16_FMA3                                                       /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vmovaps             0x20 + %[mem], %%ymm5") \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm1, %%ymm3")      /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm5, %%ymm7") \
        __ASM_EMIT("vblendvps           %%ymm3, " OFF " + 0x40 + %[knee], %%ymm0, %%ymm0")  /* ymm0 = [x0 <= start] ? GV : EV */ \
        __ASM_EMIT("vblendvps           %%ymm7, " OFF " + 0x40 + %[knee], %%ymm4, %%ymm4") \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_KNEE_SINGLE_X8_FMA3(OFF) \
        /* in: ymm0 = lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%ymm0, %%ymm1")        /* ymm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%ymm0, %%ymm2")        /* ymm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%ymm1, %%ymm1")        /* ymm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%ymm2, %%ymm2")        /* ymm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vfmadd213ps " OFF " + 0xa0 + %[knee], %%ymm0, %%ymm1")      /* ymm1 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm0")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%ymm0, %%ymm3")  /* ymm3 = [x0 >= end] */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm2, %%ymm1, %%ymm0")        /* ymm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X8_FMA3                                                        /* ymm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%ymm1")                 /* ymm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%ymm1, %%ymm3")      /* ymm3 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%ymm3, " OFF " + 0x40 + %[knee], %%ymm0, %%ymm0")  /* ymm0 = [x0 <= start] ? GV : EV */ \
        /* out: ymm0 = g0, ymm4 = g1 */

    #define PROCESS_KNEE_SINGLE_X4_FMA3(OFF) \
        /* in: xmm0 = lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0x60 + %[knee], %%xmm0, %%xmm1")        /* xmm1 = herm[0]*lx0 */ \
        __ASM_EMIT("vmulps    " OFF " + 0xc0 + %[knee], %%xmm0, %%xmm2")        /* xmm2 = tilt[0]*lx0 */ \
        __ASM_EMIT("vaddps    " OFF " + 0x80 + %[knee], %%xmm1, %%xmm1")        /* xmm1 = herm[0]*lx0+herm[1] */ \
        __ASM_EMIT("vaddps    " OFF " + 0xe0 + %[knee], %%xmm2, %%xmm2")        /* xmm2 = TV = tilt[0]*lx0+tilt[1] */ \
        __ASM_EMIT("vfmadd213ps " OFF " + 0xa0 + %[knee], %%xmm0, %%xmm1")      /* xmm1 = KV = (herm[0]*lx0+herm[1])*lx0+herm[2] */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm0")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $5, " OFF " + 0x20 + %[knee], %%xmm0, %%xmm3")  /* xmm3 = [x0 >= end] */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm2, %%xmm1, %%xmm0")        /* xmm0 = [x0 >= end] ? TV : KV */ \
        EXP_CORE_X4_FMA3                                                        /* xmm0 = EV = expf([x0 >= end] ? TV : KV) */ \
        __ASM_EMIT("vmovaps             0x00 + %[mem], %%xmm1")                 /* xmm1 = x0 */ \
        __ASM_EMIT("vcmpps              $2, " OFF " + 0x00 + %[knee], %%xmm1, %%xmm3")      /* xmm3 = [x0 <= start] */ \
        __ASM_EMIT("vblendvps           %%xmm3, " OFF " + 0x40 + %[knee], %%xmm0, %%xmm0")  /* xmm0 = [x0 <= start] ? GV : EV */ \
        /* out: xmm0 = g0, xmm4 = g1 */

    #define PROCESS_COMP_FULL_X32_FMA3 \
        /* in: ymm0 = x0, ymm4 = x1, ymm8 = x2, ymm12 = x4 */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm8, %%ymm8") \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm12, %%ymm12") \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 > k0.start] */ \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm8, %%ymm9") \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm12, %%ymm13") \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm0, %%ymm2")    /* ymm2 = [x0 > k0.start] */ \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm8, %%ymm10") \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm12, %%ymm14") \
        __ASM_EMIT("vorps               %%ymm2, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm6, %%ymm5, %%ymm5") \
        __ASM_EMIT("vorps               %%ymm10, %%ymm9, %%ymm9") \
        __ASM_EMIT("vorps               %%ymm14, %%ymm13, %%ymm13") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm13, %%ymm9, %%ymm9") \
        __ASM_EMIT("vorps               %%ymm9, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps     0x000 + 0x40 + %[knee], %%ymm0")                /* ymm0 = g1 */ \
        __ASM_EMIT("vmovaps     0x000 + 0x40 + %[knee], %%ymm4") \
        __ASM_EMIT("vmulps      0x100 + 0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = G = g0*g1 */ \
        __ASM_EMIT("vmulps      0x100 + 0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("vmovaps             %%ymm0, %%ymm8") \
        __ASM_EMIT("vmovaps             %%ymm4, %%ymm12") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0x40 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0x60 + %[mem]") \
        LOGE_CORE_X32_FMA3                                                      /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x80 + %[mem]")                 /* *mem = lx0 */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0xa0 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0xc0 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0xe0 + %[mem]") \
        PROCESS_KNEE_SINGLE_X32_FMA3("0x000")                                    /* apply knee 0 */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x100 + %[mem]")                 /* *mem = g1 */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x120 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm8, 0x140 + %[mem]") \
        __ASM_EMIT("vmovaps             %%ymm12, 0x160 + %[mem]") \
        __ASM_EMIT("vmovaps             0x80 + %[mem], %%ymm0")                 /* ymm0 = lx0 */ \
        __ASM_EMIT("vmovaps             0xa0 + %[mem], %%ymm4") \
        __ASM_EMIT("vmovaps             0xc0 + %[mem], %%ymm8") \
        __ASM_EMIT("vmovaps             0xe0 + %[mem], %%ymm12") \
        PROCESS_KNEE_SINGLE_X32_FMA3("0x100")                                    /* apply knee 1 */ \
        __ASM_EMIT("vmulps              0x100 + %[mem], %%ymm0, %%ymm0")         /* ymm0 = G = g0*g1 */ \
        __ASM_EMIT("vmulps              0x120 + %[mem], %%ymm4, %%ymm4") \
        __ASM_EMIT("vmulps              0x140 + %[mem], %%ymm8, %%ymm8") \
        __ASM_EMIT("vmulps              0x160 + %[mem], %%ymm12, %%ymm12") \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1, ymm8 = G2, ymm12 = G3 */

    #define PROCESS_COMP_FULL_X16_FMA3 \
        /* in: ymm0 = x0, ymm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm4, %%ymm4") \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm0, %%ymm1")    /* ymm1 = [x0 > k0.start] */ \
        __ASM_EMIT("vcmpps              $6, 0x000 + 0x00 + %[knee], %%ymm4, %%ymm5") \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm0, %%ymm2")    /* ymm2 = [x0 > k0.start] */ \
        __ASM_EMIT("vcmpps              $6, 0x100 + 0x00 + %[knee], %%ymm4, %%ymm6") \
        __ASM_EMIT("vorps               %%ymm2, %%ymm1, %%ymm1") \
        __ASM_EMIT("vorps               %%ymm6, %%ymm5, %%ymm5") \
        __ASM_EMIT("vorps               %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovmskps           %%ymm1, %[mask]") \
        __ASM_EMIT("test                %[mask], %[mask]") \
        __ASM_EMIT("jnz                 100f") \
        __ASM_EMIT("vmovaps     0x000 + 0x40 + %[knee], %%ymm0")                /* ymm0 = g1 */ \
        __ASM_EMIT("vmovaps     0x000 + 0x40 + %[knee], %%ymm4") \
        __ASM_EMIT("vmulps      0x100 + 0x40 + %[knee], %%ymm0, %%ymm0")        /* ymm0 = G = g0*g1 */ \
        __ASM_EMIT("vmulps      0x100 + 0x40 + %[knee], %%ymm4, %%ymm4") \
        __ASM_EMIT("jmp                 200f") \
        __ASM_EMIT("100:") \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x20 + %[mem]") \
        LOGE_CORE_X16_FMA3                                                      /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x40 + %[mem]")                 /* *mem = lx0 */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0x60 + %[mem]") \
        PROCESS_KNEE_SINGLE_X16_FMA3("0x000")                                   /* apply knee 0 */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x80 + %[mem]")                 /* *mem = g1 */ \
        __ASM_EMIT("vmovaps             %%ymm4, 0xa0 + %[mem]") \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm0")                 /* ymm0 = lx0 */ \
        __ASM_EMIT("vmovaps             0x60 + %[mem], %%ymm4") \
        PROCESS_KNEE_SINGLE_X16_FMA3("0x100")                                   /* apply knee 1 */ \
        __ASM_EMIT("vmulps              0x80 + %[mem], %%ymm0, %%ymm0")         /* ymm0 = G = g0*g1 */ \
        __ASM_EMIT("vmulps              0xa0 + %[mem], %%ymm4, %%ymm4") \
        __ASM_EMIT("200:") \
        /* out: ymm0 = G0, ymm4 = G1 */

    #define PROCESS_COMP_FULL_X8_FMA3 \
        /* in: ymm0 = x0, ymm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%ymm0, %%ymm0")         /* ymm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X8_FMA3                                                       /* ymm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x40 + %[mem]")                 /* *mem = lx0 */ \
        PROCESS_KNEE_SINGLE_X8_FMA3("0x000")                                    /* apply knee 0 */ \
        __ASM_EMIT("vmovaps             %%ymm0, 0x80 + %[mem]")                 /* *mem = g1 */ \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%ymm0")                 /* ymm0 = lx0 */ \
        PROCESS_KNEE_SINGLE_X8_FMA3("0x100")                                    /* apply knee 1 */ \
        __ASM_EMIT("vmulps              0x80 + %[mem], %%ymm0, %%ymm0")         /* ymm0 = G = g0*g1 */ \
        /* out: ymm0 = G0 */

    #define PROCESS_COMP_FULL_X4_FMA3 \
        /* in: xmm0 = x0, xmm4 = x1 */ \
        __ASM_EMIT("vandps              0x00 + %[C2C], %%xmm0, %%xmm0")         /* xmm0 = fabsf(x0) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x00 + %[mem]")                 /* store fabsf(x0) */ \
        LOGE_CORE_X4_FMA3                                                       /* xmm0 = lx0 = logf(fabsf(x0)) */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x40 + %[mem]")                 /* *mem = lx0 */ \
        PROCESS_KNEE_SINGLE_X4_FMA3("0x000")                                    /* apply knee 0 */ \
        __ASM_EMIT("vmovaps             %%xmm0, 0x80 + %[mem]")                 /* *mem = g1 */ \
        __ASM_EMIT("vmovaps             0x40 + %[mem], %%xmm0")                 /* xmm0 = lx0 */ \
        PROCESS_KNEE_SINGLE_X4_FMA3("0x100")                                    /* apply knee 1 */ \
        __ASM_EMIT("vmulps              0x80 + %[mem], %%xmm0, %%xmm0")         /* xmm0 = G = g0*g1 */ \
        /* out: xmm0 = G0 */

    void compressor_x2_gain_fma3(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
    {
        IF_ARCH_X86(
            comp_knee_t knee[2] __lsp_aligned32;
            float mem[48] __lsp_aligned32;
            float stub[16] __lsp_aligned32;
            size_t mask;
        );

        ARCH_X86_ASM
        (
            // Prepare stuff
            UNPACK_COMP_KNEE("knee", "0x000", "comp", "0x00")
            UNPACK_COMP_KNEE("knee", "0x100", "comp", "0x20")

            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            PROCESS_COMP_FULL_X16_FMA3
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
            PROCESS_COMP_FULL_X8_FMA3
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("4:")
            // 4x blocks
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
            PROCESS_COMP_FULL_X4_FMA3
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
            PROCESS_COMP_FULL_X4_FMA3
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

    IF_ARCH_X86_64(
        void x64_compressor_x2_gain_fma3(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_X86(
                comp_knee_t knee[2] __lsp_aligned32;
                float mem[96] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_COMP_KNEE("knee", "0x000", "comp", "0x00")
                UNPACK_COMP_KNEE("knee", "0x100", "comp", "0x20")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_COMP_FULL_X32_FMA3
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
                PROCESS_COMP_FULL_X16_FMA3
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
                PROCESS_COMP_FULL_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // 4x blocks
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                PROCESS_COMP_FULL_X4_FMA3
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
                PROCESS_COMP_FULL_X4_FMA3
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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

    void compressor_x2_curve_fma3(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
    {
        IF_ARCH_X86(
            comp_knee_t knee[2] __lsp_aligned32;
            float mem[48] __lsp_aligned32;
            float stub[16] __lsp_aligned32;
            size_t mask;
        );

        ARCH_X86_ASM
        (
            // Prepare stuff
            UNPACK_COMP_KNEE("knee", "0x000", "comp", "0x00")
            UNPACK_COMP_KNEE("knee", "0x100", "comp", "0x20")

            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            PROCESS_COMP_FULL_X16_FMA3
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
            PROCESS_COMP_FULL_X8_FMA3
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
            PROCESS_COMP_FULL_X4_FMA3
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
            PROCESS_COMP_FULL_X4_FMA3
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

    IF_ARCH_X86_64(
        void x64_compressor_x2_curve_fma3(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            IF_ARCH_X86(
                comp_knee_t knee[2] __lsp_aligned32;
                float mem[96] __lsp_aligned32;
                float stub[16] __lsp_aligned32;
                size_t mask;
            );

            ARCH_X86_ASM
            (
                // Prepare stuff
                UNPACK_COMP_KNEE("knee", "0x000", "comp", "0x00")
                UNPACK_COMP_KNEE("knee", "0x100", "comp", "0x20")

                // 32x blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                PROCESS_COMP_FULL_X32_FMA3
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
                PROCESS_COMP_FULL_X16_FMA3
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
                PROCESS_COMP_FULL_X8_FMA3
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
                PROCESS_COMP_FULL_X4_FMA3
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
                PROCESS_COMP_FULL_X4_FMA3
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
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }
    )

    #undef PROCESS_COMP_FULL_X4_FMA3
    #undef PROCESS_COMP_FULL_X8_FMA3
    #undef PROCESS_COMP_FULL_X16_FMA3
    #undef PROCESS_COMP_FULL_X32_FMA3
    #undef PROCESS_KNEE_SINGLE_X4_FMA3
    #undef PROCESS_KNEE_SINGLE_X8_FMA3
    #undef PROCESS_KNEE_SINGLE_X16_FMA3
    #undef PROCESS_KNEE_SINGLE_X32_FMA3

    #undef UNPACK_COMP_KNEE

    } /* namespace avx2 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX2_DYNAMICS_COMPRESSOR_H_ */
