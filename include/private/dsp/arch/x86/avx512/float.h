/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 8 сент. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FLOAT_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FLOAT_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
    #define LIMIT_BODY(DST, SRC) \
        __ASM_EMIT("xor                 %[off], %[off]") \
        __ASM_EMIT("vbroadcastss        %[min], %%zmm6")                        /* zmm6 = min */ \
        __ASM_EMIT("vbroadcastss        %[max], %%zmm7")                        /* zmm7 = max */ \
        /* x64 blocks */ \
        __ASM_EMIT("sub                 $64, %[count]") \
        __ASM_EMIT("jb                  2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%zmm0")      /* zmm0 = s0 */  \
        __ASM_EMIT("vmovups             0x40(%[" SRC "], %[off]), %%zmm1") \
        __ASM_EMIT("vmovups             0x80(%[" SRC "], %[off]), %%zmm2") \
        __ASM_EMIT("vmovups             0xc0(%[" SRC "], %[off]), %%zmm3") \
        __ASM_EMIT("vcmpps              $2, %%zmm0, %%zmm6, %%k4")              /* k4   = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $2, %%zmm1, %%zmm6, %%k5") \
        __ASM_EMIT("vcmpps              $2, %%zmm2, %%zmm6, %%k6") \
        __ASM_EMIT("vcmpps              $2, %%zmm3, %%zmm6, %%k7") \
        __ASM_EMIT("vblendmps           %%zmm0, %%zmm6, %%zmm0%{%%k4%}")        /* zmm0 = (s >= min) ? s : min */ \
        __ASM_EMIT("vblendmps           %%zmm1, %%zmm6, %%zmm1%{%%k5%}") \
        __ASM_EMIT("vblendmps           %%zmm2, %%zmm6, %%zmm2%{%%k6%}") \
        __ASM_EMIT("vblendmps           %%zmm3, %%zmm6, %%zmm3%{%%k7%}") \
        __ASM_EMIT("vcmpps              $5, %%zmm0, %%zmm7, %%k4")              /* k4   = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $5, %%zmm1, %%zmm7, %%k5") \
        __ASM_EMIT("vcmpps              $5, %%zmm2, %%zmm7, %%k6") \
        __ASM_EMIT("vcmpps              $5, %%zmm3, %%zmm7, %%k7") \
        __ASM_EMIT("vblendmps           %%zmm0, %%zmm7, %%zmm0%{%%k4%}")        /* zmm0 = (s <= max) ? ((s >= min) ? s : min) : max */ \
        __ASM_EMIT("vblendmps           %%zmm1, %%zmm7, %%zmm1%{%%k5%}") \
        __ASM_EMIT("vblendmps           %%zmm2, %%zmm7, %%zmm2%{%%k6%}") \
        __ASM_EMIT("vblendmps           %%zmm3, %%zmm7, %%zmm3%{%%k7%}") \
        __ASM_EMIT("vmovups             %%zmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%zmm2, 0x80(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%zmm3, 0xc0(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x100, %[off]") \
        __ASM_EMIT("sub                 $64, %[count]") \
        __ASM_EMIT("jae                 1b") \
        __ASM_EMIT("2:") \
        /* x32 block */ \
        __ASM_EMIT("add                 $32, %[count]") \
        __ASM_EMIT("jl                  4f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%zmm0")      /* zmm0 = s0 */  \
        __ASM_EMIT("vmovups             0x40(%[" SRC "], %[off]), %%zmm1") \
        __ASM_EMIT("vcmpps              $2, %%zmm0, %%zmm6, %%k4")              /* k4   = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $2, %%zmm1, %%zmm6, %%k5") \
        __ASM_EMIT("vblendmps           %%zmm0, %%zmm6, %%zmm0%{%%k4%}")        /* zmm0 = (s >= min) ? s : min */ \
        __ASM_EMIT("vblendmps           %%zmm1, %%zmm6, %%zmm1%{%%k5%}") \
        __ASM_EMIT("vcmpps              $5, %%zmm0, %%zmm7, %%k4")              /* k4   = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $5, %%zmm1, %%zmm7, %%k5") \
        __ASM_EMIT("vblendmps           %%zmm0, %%zmm7, %%zmm0%{%%k4%}")        /* zmm0 = (s <= max) ? ((s >= min) ? s : min) : max */ \
        __ASM_EMIT("vblendmps           %%zmm1, %%zmm7, %%zmm1%{%%k5%}") \
        __ASM_EMIT("vmovups             %%zmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x80, %[off]") \
        __ASM_EMIT("sub                 $32, %[count]") \
        __ASM_EMIT("4:") \
        /* x16 blocks */ \
        __ASM_EMIT("add                 $16, %[count]") \
        __ASM_EMIT("jl                  6f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = s0 */  \
        __ASM_EMIT("vmovups             0x20(%[" SRC "], %[off]), %%ymm1") \
        __ASM_EMIT("vcmpps              $2, %%ymm0, %%ymm6, %%ymm2")            /* ymm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $2, %%ymm1, %%ymm6, %%ymm3") \
        __ASM_EMIT("vblendvps           %%ymm2, %%ymm0, %%ymm6, %%ymm0")        /* ymm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm1, %%ymm6, %%ymm1") \
        __ASM_EMIT("vcmpps              $5, %%ymm0, %%ymm7, %%ymm2")            /* ymm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $5, %%ymm1, %%ymm7, %%ymm3") \
        __ASM_EMIT("vblendvps           %%ymm2, %%ymm0, %%ymm7, %%ymm0")        /* ymm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm1, %%ymm7, %%ymm1") \
        __ASM_EMIT("vmovups             %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x40, %[off]") \
        __ASM_EMIT("sub                 $16, %[count]") \
        __ASM_EMIT("6:") \
        /* x8 block */ \
        __ASM_EMIT("add                 $8, %[count]") \
        __ASM_EMIT("jl                  8f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("vmovups             0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("vcmpps              $2, %%xmm0, %%xmm6, %%xmm2")            /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $2, %%xmm1, %%xmm6, %%xmm3") \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm1, %%xmm6, %%xmm1") \
        __ASM_EMIT("vcmpps              $5, %%xmm0, %%xmm7, %%xmm2")            /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $5, %%xmm1, %%xmm7, %%xmm3") \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm1, %%xmm7, %%xmm1") \
        __ASM_EMIT("vmovups             %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $8, %[count]") \
        __ASM_EMIT("add                 $0x20, %[off]") \
        __ASM_EMIT("8:") \
        /* x4 block */ \
        __ASM_EMIT("add                 $4, %[count]") \
        __ASM_EMIT("jl                  10f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("vcmpps              $2, %%xmm0, %%xmm6, %%xmm2")            /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vcmpps              $5, %%xmm0, %%xmm7, %%xmm2")            /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vmovups             %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $4, %[count]") \
        __ASM_EMIT("add                 $0x10, %[off]") \
        __ASM_EMIT("10:") \
        /* x1 blocks */ \
        __ASM_EMIT("add                 $3, %[count]") \
        __ASM_EMIT("jl                  12f") \
        __ASM_EMIT("11:") \
        __ASM_EMIT("vmovss              0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("vcmpps              $2, %%xmm0, %%xmm6, %%xmm2")            /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vcmpps              $5, %%xmm0, %%xmm7, %%xmm2")            /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vmovss              %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x04, %[off]") \
        __ASM_EMIT("dec                 %[count]") \
        __ASM_EMIT("jge                 11b") \
        __ASM_EMIT("12:")


    void limit1(float *dst, float min, float max, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM(
            LIMIT_BODY("dst", "dst")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst),
              [min] "m" (min), [max] "m" (max)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7",
              "%k4", "%k5", "%k6", "%k7"
        );
    }

    void limit2(float *dst, const float *src, float min, float max, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM(
            LIMIT_BODY("dst", "src")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src),
              [min] "m" (min), [max] "m" (max)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7",
              "%k4", "%k5", "%k6", "%k7"
        );
    }

    #undef LIMIT_BODY

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* INCLUDE_PRIVATE_DSP_ARCH_X86_AVX512_FLOAT_H_ */
