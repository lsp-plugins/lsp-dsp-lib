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

    #define SANITIZE_BODY(DST, SRC) \
        __ASM_EMIT("xor             %[off], %[off]") \
        /* 64x blocks */ \
        __ASM_EMIT("sub             $64, %[count]") \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%zmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovups         0x40(%[" SRC "], %[off]), %%zmm1") \
        __ASM_EMIT("vmovups         0x80(%[" SRC "], %[off]), %%zmm2") \
        __ASM_EMIT("vmovups         0xc0(%[" SRC "], %[off]), %%zmm3") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm0, %%zmm4")         /* xmm4 = abs(s) */ \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm1, %%zmm5") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm2, %%zmm6") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm3, %%zmm7") \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%zmm4, %%k4")           /* k4   = [abs(s) > X_MAX] */ \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%zmm5, %%k5") \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%zmm6, %%k6") \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%zmm7, %%k7") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm4, %%k0")           /* k0   = [abs(s) > X_MIN] */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm5, %%k1") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm6, %%k2") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm7, %%k3") \
        __ASM_EMIT("kandnw          %%k0, %%k4, %%k4")                      /* k4   = [(abs(s) > X_MAX) & (abs(s) <= X_MIN)] */ \
        __ASM_EMIT("kandnw          %%k1, %%k5, %%k5") \
        __ASM_EMIT("kandnw          %%k2, %%k6, %%k6") \
        __ASM_EMIT("kandnw          %%k3, %%k7, %%k7") \
        __ASM_EMIT("vpandd          0xc0(%[CVAL]), %%zmm0, %%zmm4")         /* zmm4 = sign(s) */ \
        __ASM_EMIT("vpandd          0xc0(%[CVAL]), %%zmm1, %%zmm5") \
        __ASM_EMIT("vpandd          0xc0(%[CVAL]), %%zmm2, %%zmm6") \
        __ASM_EMIT("vpandd          0xc0(%[CVAL]), %%zmm3, %%zmm7") \
        __ASM_EMIT("vblendmps       %%zmm0, %%zmm4, %%zmm0%{%%k4%}")        /* zmm0 = ([(abs(s) > X_MAX) & (abs(s) <= X_MIN)]) ? s : sign(s) */ \
        __ASM_EMIT("vblendmps       %%zmm1, %%zmm5, %%zmm1%{%%k5%}") \
        __ASM_EMIT("vblendmps       %%zmm2, %%zmm6, %%zmm2%{%%k6%}") \
        __ASM_EMIT("vblendmps       %%zmm3, %%zmm7, %%zmm3%{%%k7%}") \
        __ASM_EMIT("vmovups         %%zmm0, 0x00(%[" DST "], %[off])")      /* zmm0 = s' */ \
        __ASM_EMIT("vmovups         %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%zmm2, 0x80(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%zmm3, 0xc0(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x100, %[off]") \
        __ASM_EMIT("sub             $64, %[count]") \
        __ASM_EMIT("jae             1b") \
        __ASM_EMIT("2:") \
        /* 32x block */ \
        __ASM_EMIT("add             $32, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%zmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovups         0x40(%[" SRC "], %[off]), %%zmm1") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm0, %%zmm4")         /* xmm4 = abs(s) */ \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm1, %%zmm5") \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%zmm4, %%k4")           /* k4   = [abs(s) > X_MAX] */ \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%zmm5, %%k5") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm4, %%k0")           /* k0   = [abs(s) > X_MIN] */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm5, %%k1") \
        __ASM_EMIT("kandnw          %%k0, %%k4, %%k4")                      /* k4   = [(abs(s) > X_MAX) & (abs(s) <= X_MIN)] */ \
        __ASM_EMIT("kandnw          %%k1, %%k5, %%k5") \
        __ASM_EMIT("vpandd          0xc0(%[CVAL]), %%zmm0, %%zmm4")         /* zmm4 = sign(s) */ \
        __ASM_EMIT("vpandd          0xc0(%[CVAL]), %%zmm1, %%zmm5") \
        __ASM_EMIT("vblendmps       %%zmm0, %%zmm4, %%zmm0%{%%k4%}")        /* zmm0 = ([(abs(s) > X_MAX) & (abs(s) <= X_MIN)]) ? s : sign(s) */ \
        __ASM_EMIT("vblendmps       %%zmm1, %%zmm5, %%zmm1%{%%k5%}") \
        __ASM_EMIT("vmovups         %%zmm0, 0x00(%[" DST "], %[off])")      /* zmm0 = s' */ \
        __ASM_EMIT("vmovups         %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x80, %[off]") \
        __ASM_EMIT("sub             $32, %[count]") \
        __ASM_EMIT("4:") \
        /* 8x blocks */ \
        __ASM_EMIT("add             $24, %[count]") \
        __ASM_EMIT("jl              6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("vmovdqu         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovdqu         0x10(%[" SRC "], %[off]), %%xmm4") \
        __ASM_EMIT("vpand           0x00(%[CVAL]), %%xmm0, %%xmm1")         /* xmm1 = abs(s) */ \
        __ASM_EMIT("vpand           0x00(%[CVAL]), %%xmm4, %%xmm5") \
        __ASM_EMIT("vpand           0xc0(%[CVAL]), %%xmm0, %%xmm2")         /* xmm2 = sign(s) */ \
        __ASM_EMIT("vpand           0xc0(%[CVAL]), %%xmm4, %%xmm6") \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%xmm1, %%xmm3")         /* xmm3 = abs(s) > X_MAX  */ \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%xmm5, %%xmm7") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%xmm1, %%xmm1")         /* xmm1 = abs(s) > X_MIN  */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%xmm5, %%xmm5") \
        __ASM_EMIT("vpandn          %%xmm1, %%xmm3, %%xmm1")                /* xmm1 = (abs(s) > X_MIN) & (abs(s) <= X_MAX) */ \
        __ASM_EMIT("vpandn          %%xmm5, %%xmm7, %%xmm5") \
        __ASM_EMIT("vblendvps       %%xmm1, %%xmm0, %%xmm2, %%xmm0")        /* xmm0 = ((abs(s) > X_MIN) & (abs(s) <= X_MAX))) ? s : sign(s) */ \
        __ASM_EMIT("vblendvps       %%xmm5, %%xmm4, %%xmm6, %%xmm4") \
        __ASM_EMIT("vmovdqu         %%xmm0, 0x00(%[" DST "], %[off])")      /* xmm0 = s */ \
        __ASM_EMIT("vmovdqu         %%xmm4, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x20, %[off]") \
        __ASM_EMIT("sub             $8, %[count]") \
        __ASM_EMIT("jge             5b") \
        __ASM_EMIT("6:") \
        /* 4x block */ \
        __ASM_EMIT("add             $4, %[count]") \
        __ASM_EMIT("jl              8f") \
        __ASM_EMIT("vmovdqu         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vpand           0x00(%[CVAL]), %%xmm0, %%xmm1")         /* xmm1 = abs(s) */ \
        __ASM_EMIT("vpand           0xc0(%[CVAL]), %%xmm0, %%xmm2")         /* xmm2 = sign(s) */ \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%xmm1, %%xmm3")         /* xmm3 = abs(s) > X_MAX  */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%xmm1, %%xmm1")         /* xmm1 = abs(s) > X_MIN  */ \
        __ASM_EMIT("vpandn          %%xmm1, %%xmm3, %%xmm1")                /* xmm1 = (abs(s) > X_MIN) & (abs(s) <= X_MAX) */ \
        __ASM_EMIT("vblendvps       %%xmm1, %%xmm0, %%xmm2, %%xmm0")        /* xmm0 = ((abs(s) > X_MIN) & (abs(s) <= X_MAX))) ? s : sign(s) */ \
        __ASM_EMIT("vmovdqu         %%xmm0, 0x00(%[" DST "], %[off])")      /* xmm0 = s */ \
        __ASM_EMIT("add             $0x10, %[off]") \
        __ASM_EMIT("sub             $4, %[count]") \
        __ASM_EMIT("8:") \
        /* 1x blocks */ \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovd           0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vpand           0x00(%[CVAL]), %%xmm0, %%xmm1")         /* xmm1 = abs(s) */ \
        __ASM_EMIT("vpand           0xc0(%[CVAL]), %%xmm0, %%xmm2")         /* xmm2 = sign(s) */ \
        __ASM_EMIT("vpcmpgtd        0x40(%[CVAL]), %%xmm1, %%xmm3")         /* xmm3 = abs(s) > X_MAX  */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%xmm1, %%xmm1")         /* xmm1 = abs(s) > X_MIN  */ \
        __ASM_EMIT("vpandn          %%xmm1, %%xmm3, %%xmm1")                /* xmm1 = (abs(s) > X_MIN) & (abs(s) <= X_MAX) */ \
        __ASM_EMIT("vblendvps       %%xmm1, %%xmm0, %%xmm2, %%xmm0")        /* xmm0 = ((abs(s) > X_MIN) & (abs(s) <= X_MAX))) ? s : sign(s) */ \
        __ASM_EMIT("vmovd           %%xmm0, 0x00(%[" DST "], %[off])")      /* xmm0 = s */ \
        __ASM_EMIT("add             $0x04, %[off]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             9b") \
        __ASM_EMIT("10:") \
        /* end */

        IF_ARCH_X86(
            static uint32_t SANITIZE_CVAL[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff),              // X_ABS
                LSP_DSP_VEC16(0x7f7fffff),              // X_MAX
                LSP_DSP_VEC16(0x007fffff),              // X_MIN
                LSP_DSP_VEC16(0x80000000),              // X_SIGN
            };
        )

        void sanitize1(float *dst, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                SANITIZE_BODY("dst", "dst")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [CVAL] "r" (SANITIZE_CVAL)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k0", "%k1", "%k2", "%k3",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }

        void sanitize2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                SANITIZE_BODY("dst", "src")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CVAL] "r" (SANITIZE_CVAL)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k0", "%k1", "%k2", "%k3",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }

    #undef SANITIZE_BODY

        IF_ARCH_X86(
            static uint32_t XC_SATURATE[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff),                  // X_ABS
                LSP_DSP_VEC16(0x80000000),                  // X_SIGN
                LSP_DSP_VEC16(0x7f800000),                  // X_P_INF
                LSP_DSP_VEC16(LSP_DSP_FLOAT_SAT_P_NAN_I),   // SX_P_NAN
                LSP_DSP_VEC16(LSP_DSP_FLOAT_SAT_P_INF_I)    // SX_P_INF
            };
        )

    #define SAT_BODY(DST, SRC) \
        __ASM_EMIT("xor             %[off], %[off]") \
        /* x64 blocks */ \
        __ASM_EMIT("sub             $64, %[count]") \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%zmm0")      /* zmm0 = s */ \
        __ASM_EMIT("vmovups         0x40(%[" SRC "], %[off]), %%zmm1") \
        __ASM_EMIT("vmovups         0x80(%[" SRC "], %[off]), %%zmm2") \
        __ASM_EMIT("vmovups         0xc0(%[" SRC "], %[off]), %%zmm3") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm0, %%zmm4")         /* zmm4 = abs(s) */ \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm1, %%zmm5") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm2, %%zmm6") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm3, %%zmm7") \
        __ASM_EMIT("vpandd          0x40(%[CVAL]), %%zmm0, %%zmm0")         /* zmm0 = sign(s) */ \
        __ASM_EMIT("vpandd          0x40(%[CVAL]), %%zmm1, %%zmm1") \
        __ASM_EMIT("vpandd          0x40(%[CVAL]), %%zmm2, %%zmm2") \
        __ASM_EMIT("vpandd          0x40(%[CVAL]), %%zmm3, %%zmm3") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm4, %%k4")           /* k4   = [abs(s) > +Inf] */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm5, %%k5") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm6, %%k6") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm7, %%k7") \
        __ASM_EMIT("vmovaps         0xc0(%[CVAL]), %%zmm4%{%%k4%}")         /* zmm4 = [abs(s) > +Inf] ? PNAN: s */ \
        __ASM_EMIT("vmovaps         0xc0(%[CVAL]), %%zmm5%{%%k5%}") \
        __ASM_EMIT("vmovaps         0xc0(%[CVAL]), %%zmm6%{%%k6%}") \
        __ASM_EMIT("vmovaps         0xc0(%[CVAL]), %%zmm7%{%%k7%}") \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%zmm4, %%k4")           /* k4   = [abs(s) == +Inf] */ \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%zmm5, %%k5") \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%zmm6, %%k6") \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%zmm7, %%k7") \
        __ASM_EMIT("vmovaps         0x100(%[CVAL]), %%zmm4%{%%k4%}")        /* zmm4 = S = [abs(s) == +Inf] ? PINF : s */ \
        __ASM_EMIT("vmovaps         0x100(%[CVAL]), %%zmm5%{%%k5%}") \
        __ASM_EMIT("vmovaps         0x100(%[CVAL]), %%zmm6%{%%k6%}") \
        __ASM_EMIT("vmovaps         0x100(%[CVAL]), %%zmm7%{%%k7%}") \
        __ASM_EMIT("vpord           %%zmm4, %%zmm0, %%zmm0")                /* zmm0 = sign(s) * S */ \
        __ASM_EMIT("vpord           %%zmm5, %%zmm1, %%zmm1") \
        __ASM_EMIT("vpord           %%zmm6, %%zmm2, %%zmm2") \
        __ASM_EMIT("vpord           %%zmm7, %%zmm3, %%zmm3") \
        __ASM_EMIT("vmovups         %%zmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%zmm2, 0x80(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%zmm3, 0xc0(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x100, %[off]") \
        __ASM_EMIT("sub             $64, %[count]") \
        __ASM_EMIT("jae             1b") \
        __ASM_EMIT("2:") \
        /* x32 block */ \
        __ASM_EMIT("add             $32, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%zmm0")      /* zmm0 = s */ \
        __ASM_EMIT("vmovups         0x40(%[" SRC "], %[off]), %%zmm1") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm0, %%zmm4")         /* zmm4 = abs(s) */ \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm1, %%zmm5") \
        __ASM_EMIT("vpandd          0x40(%[CVAL]), %%zmm0, %%zmm0")         /* zmm0 = sign(s) */ \
        __ASM_EMIT("vpandd          0x40(%[CVAL]), %%zmm1, %%zmm1") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm4, %%k4")           /* k4   = [abs(s) > +Inf] */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%zmm5, %%k5") \
        __ASM_EMIT("vmovaps         0xc0(%[CVAL]), %%zmm4%{%%k4%}")         /* zmm4 = [abs(s) > +Inf] ? PNAN: s */ \
        __ASM_EMIT("vmovaps         0xc0(%[CVAL]), %%zmm5%{%%k5%}") \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%zmm4, %%k4")           /* k4   = [abs(s) == +Inf] */ \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%zmm5, %%k5") \
        __ASM_EMIT("vmovaps         0x100(%[CVAL]), %%zmm4%{%%k4%}")        /* zmm4 = S = [abs(s) == +Inf] ? PINF : s */ \
        __ASM_EMIT("vmovaps         0x100(%[CVAL]), %%zmm5%{%%k5%}") \
        __ASM_EMIT("vpord           %%zmm4, %%zmm0, %%zmm0")                /* zmm0 = sign(s) * S */ \
        __ASM_EMIT("vpord           %%zmm5, %%zmm1, %%zmm1") \
        __ASM_EMIT("vmovups         %%zmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x80, %[off]") \
        __ASM_EMIT("sub             $32, %[count]") \
        __ASM_EMIT("4:") \
        /* x8 blocks */ \
        __ASM_EMIT("add             $24, %[count]") \
        __ASM_EMIT("jl              6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovups         0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("vandps          0x00(%[CVAL]), %%xmm0, %%xmm2")         /* xmm2 = abs(s) */ \
        __ASM_EMIT("vandps          0x00(%[CVAL]), %%xmm1, %%xmm3") \
        __ASM_EMIT("vandps          0x40(%[CVAL]), %%xmm0, %%xmm4")         /* xmm4 = sign(s) */ \
        __ASM_EMIT("vandps          0x40(%[CVAL]), %%xmm1, %%xmm5") \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%xmm2, %%xmm6")         /* xmm6 = abs(s) > +Inf */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%xmm3, %%xmm7") \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%xmm2, %%xmm2")         /* xmm2 = abs(s) == +Inf */ \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%xmm3, %%xmm3") \
        __ASM_EMIT("vblendvps       %%xmm6, 0xc0(%[CVAL]), %%xmm0, %%xmm0") /* xmm0 = S = s & (abs(s) <= +Inf) | PNAN & (abs(s) > +Inf) */ \
        __ASM_EMIT("vblendvps       %%xmm7, 0xc0(%[CVAL]), %%xmm1, %%xmm1") \
        __ASM_EMIT("vblendvps       %%xmm2, 0x100(%[CVAL]), %%xmm0, %%xmm0")/* xmm0 = S & (abs(S) != +Inf) | PINF & (abs(s) == +Inf) */ \
        __ASM_EMIT("vblendvps       %%xmm3, 0x100(%[CVAL]), %%xmm1, %%xmm1") \
        __ASM_EMIT("vorps           %%xmm4, %%xmm0, %%xmm0") \
        __ASM_EMIT("vorps           %%xmm5, %%xmm1, %%xmm1") \
        __ASM_EMIT("vmovups         %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x20, %[off]") \
        __ASM_EMIT("sub             $8, %[count]") \
        __ASM_EMIT("jge             5b") \
        __ASM_EMIT("6:") \
        /* x4 block */ \
        __ASM_EMIT("add             $4, %[count]") \
        __ASM_EMIT("jl              8f") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vandps          0x00(%[CVAL]), %%xmm0, %%xmm2")         /* xmm2 = abs(s) */ \
        __ASM_EMIT("vandps          0x40(%[CVAL]), %%xmm0, %%xmm4")         /* xmm4 = sign(s) */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%xmm2, %%xmm6")         /* xmm6 = abs(s) > +Inf */ \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%xmm2, %%xmm2")         /* xmm2 = abs(s) == +Inf */ \
        __ASM_EMIT("vblendvps       %%xmm6, 0xc0(%[CVAL]), %%xmm0, %%xmm0") /* xmm0 = S = s & (abs(s) <= +Inf) | PNAN & (abs(s) > +Inf) */ \
        __ASM_EMIT("vblendvps       %%xmm2, 0x100(%[CVAL]), %%xmm0, %%xmm0")/* xmm0 = S & (abs(S) != +Inf) | PINF & (abs(s) == +Inf) */ \
        __ASM_EMIT("vorps           %%xmm4, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovups         %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub             $4, %[count]") \
        __ASM_EMIT("add             $0x10, %[off]") \
        __ASM_EMIT("8:") \
        /* x1 block */ \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss          0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vandps          0x00(%[CVAL]), %%xmm0, %%xmm2")         /* xmm2 = abs(s) */ \
        __ASM_EMIT("vandps          0x40(%[CVAL]), %%xmm0, %%xmm4")         /* xmm4 = sign(s) */ \
        __ASM_EMIT("vpcmpgtd        0x80(%[CVAL]), %%xmm2, %%xmm6")         /* xmm6 = abs(s) > +Inf */ \
        __ASM_EMIT("vpcmpeqd        0x80(%[CVAL]), %%xmm2, %%xmm2")         /* xmm2 = abs(s) == +Inf */ \
        __ASM_EMIT("vblendvps       %%xmm6, 0xc0(%[CVAL]), %%xmm0, %%xmm0") /* xmm0 = S = s & (abs(s) <= +Inf) | PNAN & (abs(s) > +Inf) */ \
        __ASM_EMIT("vblendvps       %%xmm2, 0x100(%[CVAL]), %%xmm0, %%xmm0")/* xmm0 = S & (abs(S) != +Inf) | PINF & (abs(s) == +Inf) */ \
        __ASM_EMIT("vorps           %%xmm4, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovss          %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x04, %[off]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             9b") \
        __ASM_EMIT("10:")

        void saturate(float *dst, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM(
                SAT_BODY("dst", "dst")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [CVAL] "r" (XC_SATURATE)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }

        void copy_saturated(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM(
                SAT_BODY("dst", "src")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CVAL] "r" (XC_SATURATE)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }

    #undef SAT_BODY

    #define LIMIT_SAT_BODY(DST, SRC) \
        __ASM_EMIT("xor             %[off], %[off]") \
        __ASM_EMIT("vmovaps         0x80(%[CVAL]), %%zmm4")                 /* xmm4 = +1 */ \
        __ASM_EMIT("vmovaps         0x80(%[CVAL]), %%zmm5") \
        /* x32 blocks */ \
        __ASM_EMIT("sub             $32, %[count]") \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%zmm0")      /* zmm0 = s */ \
        __ASM_EMIT("vmovups         0x40(%[" SRC "], %[off]), %%zmm1") \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm0, %%zmm2")         /* zmm2 = abs(s) */ \
        __ASM_EMIT("vpandd          0x00(%[CVAL]), %%zmm1, %%zmm3") \
        __ASM_EMIT("vpandd          0x40(%[CVAL]), %%zmm0, %%zmm0")         /* zmm0 = sign(s) */ \
        __ASM_EMIT("vpandd          0x40(%[CVAL]), %%zmm1, %%zmm1") \
        __ASM_EMIT("vcmpps          $2, %%zmm4, %%zmm2, %%k4")              /* k4   = [ (abs(s) <= +1) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps          $2, %%zmm5, %%zmm3, %%k5") \
        __ASM_EMIT("vpcmpgtd        0xc0(%[CVAL]), %%zmm2, %%k6")           /* k6   = [abs(s) > +Inf] */ \
        __ASM_EMIT("vpcmpgtd        0xc0(%[CVAL]), %%zmm3, %%k7") \
        __ASM_EMIT("vblendmps       %%zmm2, %%zmm4, %%zmm2%{%%k4%}")        /* zmm2 = ([ (abs(s) <= +1) & !isnan(s) ]) ? abs(s) : 1 */ \
        __ASM_EMIT("vblendmps       %%zmm3, %%zmm5, %%zmm3%{%%k5%}") \
        __ASM_EMIT("vpord           %%zmm2, %%zmm0, %%zmm0")                /* zmm0 = ([ (abs(s) <= +1) & !isnan(s) ]) ? s : sign(s)*1 */ \
        __ASM_EMIT("vpord           %%zmm3, %%zmm1, %%zmm1") \
        __ASM_EMIT("vpxord          %%zmm0, %%zmm0, %%zmm0%{%%k6%}")        /* zmm0 = [abs(s) > +Inf] ? 0 : ([ (abs(s) <= +1) & !isnan(s) ]) ? s : sign(s)*1 */ \
        __ASM_EMIT("vpxord          %%zmm1, %%zmm1, %%zmm1%{%%k7%}") \
        __ASM_EMIT("vmovups         %%zmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%zmm1, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x80, %[off]") \
        __ASM_EMIT("sub             $32, %[count]") \
        __ASM_EMIT("jae             1b") \
        __ASM_EMIT("2:") \
        /* x8 blocks */ \
        __ASM_EMIT("add             $24, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT("3:") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovups         0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("vmovaps         0x80(%[CVAL]), %%xmm2")                 /* xmm2 = +1 */ \
        __ASM_EMIT("vandps          0x00(%[CVAL]), %%xmm0, %%xmm6")         /* xmm6 = abs(s) */ \
        __ASM_EMIT("vmovaps         %%xmm2, %%xmm3") \
        __ASM_EMIT("vandps          0x00(%[CVAL]), %%xmm1, %%xmm7") \
        __ASM_EMIT("vcmpps          $2, %%xmm2, %%xmm6, %%xmm2")            /* xmm2 = c = [ (abs(s) <= +1) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps          $2, %%xmm3, %%xmm7, %%xmm3") \
        __ASM_EMIT("vandps          0x40(%[CVAL]), %%xmm0, %%xmm4")         /* xmm4 = sign(s) */ \
        __ASM_EMIT("vandps          0x40(%[CVAL]), %%xmm1, %%xmm5") \
        __ASM_EMIT("vpcmpgtd        0xc0(%[CVAL]), %%xmm6, %%xmm6")         /* xmm6 = [ abs(s) > +Inf ] */ \
        __ASM_EMIT("vpcmpgtd        0xc0(%[CVAL]), %%xmm7, %%xmm7") \
        __ASM_EMIT("vorps           0x80(%[CVAL]), %%xmm4, %%xmm4")         /* xmm4 = +1 * sign(s) */ \
        __ASM_EMIT("vorps           0x80(%[CVAL]), %%xmm5, %%xmm5") \
        __ASM_EMIT("vandnps         %%xmm4, %%xmm6, %%xmm4")                /* xmm4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("vandnps         %%xmm5, %%xmm7, %%xmm5") \
        __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm4, %%xmm0")        /* xmm0 = (s & c) | (r & !c) */ \
        __ASM_EMIT("vblendvps       %%xmm3, %%xmm1, %%xmm5, %%xmm1") \
        __ASM_EMIT("vmovups         %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups         %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x20, %[off]") \
        __ASM_EMIT("sub             $8, %[count]") \
        __ASM_EMIT("jge             3b") \
        __ASM_EMIT("4:") \
        /* x4 block */ \
        __ASM_EMIT("add             $4, %[count]") \
        __ASM_EMIT("jl              6f") \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovaps         0x80(%[CVAL]), %%xmm2")                 /* xmm2 = +1 */ \
        __ASM_EMIT("vandps          0x00(%[CVAL]), %%xmm0, %%xmm6")         /* xmm6 = abs(s) */ \
        __ASM_EMIT("vcmpps          $2, %%xmm2, %%xmm6, %%xmm2")            /* xmm2 = c = [ (abs(s) <= +1) & !isnan(s) ] */ \
        __ASM_EMIT("vandps          0x40(%[CVAL]), %%xmm0, %%xmm4")         /* xmm4 = sign(s) */ \
        __ASM_EMIT("vpcmpgtd        0xc0(%[CVAL]), %%xmm6, %%xmm6")         /* xmm6 = [ abs(s) > +Inf ] */ \
        __ASM_EMIT("vorps           0x80(%[CVAL]), %%xmm4, %%xmm4")         /* xmm4 = +1 * sign(s) */ \
        __ASM_EMIT("vandnps         %%xmm4, %%xmm6, %%xmm4")                /* xmm4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm4, %%xmm0")        /* xmm0 = (s & c) | (r & !c) */ \
        __ASM_EMIT("vmovups         %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub             $4, %[count]") \
        __ASM_EMIT("add             $0x10, %[off]") \
        __ASM_EMIT("6:") \
        /* x1 block */ \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vmovss          0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovups         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovaps         0x80(%[CVAL]), %%xmm2")                 /* xmm2 = +1 */ \
        __ASM_EMIT("vandps          0x00(%[CVAL]), %%xmm0, %%xmm6")         /* xmm6 = abs(s) */ \
        __ASM_EMIT("vcmpps          $2, %%xmm2, %%xmm6, %%xmm2")            /* xmm2 = c = [ (abs(s) <= +1) & !isnan(s) ] */ \
        __ASM_EMIT("vandps          0x40(%[CVAL]), %%xmm0, %%xmm4")         /* xmm4 = sign(s) */ \
        __ASM_EMIT("vpcmpgtd        0xc0(%[CVAL]), %%xmm6, %%xmm6")         /* xmm6 = [ abs(s) > +Inf ] */ \
        __ASM_EMIT("vorps           0x80(%[CVAL]), %%xmm4, %%xmm4")         /* xmm4 = +1 * sign(s) */ \
        __ASM_EMIT("vandnps         %%xmm4, %%xmm6, %%xmm4")                /* xmm4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("vblendvps       %%xmm2, %%xmm0, %%xmm4, %%xmm0")        /* xmm0 = (s & c) | (r & !c) */ \
        __ASM_EMIT("vmovss          %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add             $0x04, %[off]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             7b") \
        __ASM_EMIT("8:")

        IF_ARCH_X86(
            static uint32_t XLIM_SAT[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff),      // abs
                LSP_DSP_VEC16(0x80000000),      // sign
                LSP_DSP_VEC16(0x3f800000),      // +1
                LSP_DSP_VEC16(0x7f800000)       // +Inf
            };
        )

        void limit_saturate1(float *dst, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM(
                LIMIT_SAT_BODY("dst", "dst")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [CVAL] "r" (XLIM_SAT)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }

        void limit_saturate2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM(
                LIMIT_SAT_BODY("dst", "src")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CVAL] "r" (XLIM_SAT)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }

    #undef LIMIT_SAT_BODY

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* INCLUDE_PRIVATE_DSP_ARCH_X86_AVX512_FLOAT_H_ */
