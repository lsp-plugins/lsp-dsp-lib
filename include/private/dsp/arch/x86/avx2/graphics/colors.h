/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 23 мая 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_GRAPHICS_COLORS_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_GRAPHICS_COLORS_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

#include <private/dsp/arch/x86/avx2/graphics/transpose.h>

namespace lsp
{
    namespace avx2
    {
        IF_ARCH_X86(
            static const float HSL_RGB[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0.5f),                // 1/2
                LSP_DSP_VEC8(0.333333333333f),     // 1/3
                LSP_DSP_VEC8(1.0f),                // 1
                LSP_DSP_VEC8(6.0f),                // 6
                LSP_DSP_VEC8(0.166666666667f),     // 1/6
                LSP_DSP_VEC8(0.666666666667f)      // 2/3
            };

            static const float RGB_HSL[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(4.0f),
                LSP_DSP_VEC8(2.0f),
                LSP_DSP_VEC8(6.0f),
                LSP_DSP_VEC8(1.0f),
                LSP_DSP_VEC8(0.5f),
                LSP_DSP_VEC8(0.166666666667f)      // 1/6
            };
        )

    /*
            //Set the temporary values
            if  (HSL_RGB_0_5 > L)
                temp2 = (L + S) - (L * S)
            else
                temp2 = L + (L * S);

            temp1 = L + L - temp2;

            tempr = H + HSL_RGB_1_3;
            tempg = H;
            tempb = H - HSL_RGB_1_3;

            if (tempr > 1.0f)
                tempr   -= 1.0f;
            if (tempb < 0.0f)
                tempb   += 1.0f;

            k = (temp2 - temp1) * 6.0f;

            //Red
            if (tempr < HSL_RGB_0_5)
                R = (tempr < HSL_RGB_1_6) ? temp1 + k * tempr : temp2;
            else
                R = (tempr < HSL_RGB_2_3) ? temp1 + k * (HSL_RGB_2_3 - tempr) : temp1;

            //Green
            if (tempg < HSL_RGB_0_5)
                G = (tempg < HSL_RGB_1_6) ? temp1 + k * tempg : temp2;
            else
                G = (tempg < HSL_RGB_2_3) ? temp1 + k * (HSL_RGB_2_3 - tempg) : temp1;

            //Blue
            if (tempb < HSL_RGB_0_5)
                B = (tempb < HSL_RGB_1_6) ? temp1 + k * tempb : temp2;
            else
                B = (tempb < HSL_RGB_2_3) ? temp1 + k * (HSL_RGB_2_3 - tempb) : temp1;
    */

    #define HSLA_TO_RGBA_CORE_X8   \
        /*  Transpose */\
        MAT4_TRANSPOSE("%%ymm0", "%%ymm1", "%%ymm2", "%%ymm3", "%%ymm4", "%%ymm5") \
        \
        /*  ymm0 = h0 h1 h2 h3 h4 h5 h6 h7 = H */ \
        /*  ymm1 = s0 s1 s2 s3 s4 s5 s6 s7 = S */ \
        /*  ymm2 = l0 l1 l2 l3 l4 l5 l6 l7 = L */ \
        /*  ymm3 = a0 a1 a2 a3 a4 a5 a6 a7 = A */ \
        /*  Calc temp1 (T1) and temp2 (T2) */ \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm7")                /* ymm7 = L * S */ \
        __ASM_EMIT("vaddps          %%ymm1, %%ymm2, %%ymm6")                /* ymm6 = L + S */ \
        __ASM_EMIT("vaddps          %%ymm2, %%ymm7, %%ymm4")                /* ymm4 = L + L * S */ \
        __ASM_EMIT("vsubps          %%ymm7, %%ymm6, %%ymm5")                /* ymm5 = L + S - L * S */ \
        __ASM_EMIT("vaddps          %%ymm2, %%ymm2, %%ymm6")                /* ymm6 = L + L */ \
        __ASM_EMIT("vcmpps          $1, 0x00(%[XC]), %%ymm2, %%ymm7")       /* ymm7 = [L < 0.5f] */ \
        __ASM_EMIT("vblendvps       %%ymm7, %%ymm4, %%ymm5, %%ymm4")        /* ymm4 = T2 = ([L < 0.5f] & (L + L*S)) | ([L >= 0.5f] & (L+S - L*S)) */ \
        __ASM_EMIT("vsubps          0x20(%[XC]), %%ymm0, %%ymm2")           /* ymm2 = H - 1/3 */ \
        __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm5")                /* ymm5 = T1 = L + L - T2 */ \
        __ASM_EMIT("vmovaps         %%ymm0, %%ymm1")                        /* ymm1 = TG = H */ \
        __ASM_EMIT("vaddps          0x20(%[XC]), %%ymm0, %%ymm0")           /* ymm0 = H + 1/3 */ \
        __ASM_EMIT("vsubps          0x40(%[XC]), %%ymm0, %%ymm6")           /* ymm6 = H + 1/3 - 1 */ \
        __ASM_EMIT("vcmpps          $6, 0x40(%[XC]), %%ymm0, %%ymm7")       /* ymm7 = [(H + 1/3) > 1] */ \
        __ASM_EMIT("vblendvps       %%ymm7, %%ymm6, %%ymm0, %%ymm0")        /* ymm0 = TR = ((H + 1/3) & [(H + 1/3) <= 1]) | ((H + 1/3 - 1) & [(H + 1/3) > 1]) */ \
        __ASM_EMIT("vxorps          %%ymm7, %%ymm7, %%ymm7")                /* ymm7 = 0 */ \
        __ASM_EMIT("vaddps          0x40(%[XC]), %%ymm2, %%ymm6")           /* ymm6 = H - 1/3 + 1 */ \
        __ASM_EMIT("vcmpps          $1, %%ymm7, %%ymm2, %%ymm7")            /* ymm7 = [(H - 1/3) < 0] */ \
        __ASM_EMIT("vblendvps       %%ymm7, %%ymm6, %%ymm2, %%ymm2")        /* ymm2 = TB = ((H - 1/3) & [(H - 1/3) >= 0]) | ((H - 1/3 + 1) & [(H - 1/3) < 0]) */ \
        __ASM_EMIT("vsubps          %%ymm5, %%ymm4, %%ymm6")                /* ymm6 = T2 - T1 */ \
        __ASM_EMIT("vmulps          0x60(%[XC]), %%ymm6, %%ymm6")           /* ymm6 = K = (T2 - T1)*6.0 */ \
        /*  ymm0 = TR */ \
        /*  ymm1 = TG */ \
        /*  ymm2 = TB */ \
        /*  ymm3 = A */ \
        /*  ymm4 = T2 */ \
        /*  ymm5 = T1 */ \
        /*  ymm6 = K */ \
        __ASM_EMIT("vmovaps         %%ymm0, 0x00(%[HSLM])")                 /* TR */ \
        __ASM_EMIT("vmovaps         %%ymm1, 0x20(%[HSLM])")                 /* TG */ \
        __ASM_EMIT("vmovaps         %%ymm2, 0x40(%[HSLM])")                 /* TB */ \
        __ASM_EMIT("vmovaps         %%ymm3, 0x60(%[HSLM])")                 /* A */ \
        __ASM_EMIT("vmovaps         %%ymm4, 0x80(%[HSLM])")                 /* T2 */ \
        __ASM_EMIT("vmovaps         %%ymm5, 0xa0(%[HSLM])")                 /* T1 */ \
        \
        __ASM_EMIT("vmovaps         0xa0(%[XC]), %%ymm7")                   /* ymm7 = 2/3 */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm0, %%ymm0")                /* ymm0 = K * TR */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm1, %%ymm1")                /* ymm1 = K * TG */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = K * TB */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                /* ymm7 = K * 2/3 */ \
        __ASM_EMIT("vsubps          %%ymm0, %%ymm7, %%ymm3")                /* ymm3 = K * (2/3 - TR) */ \
        __ASM_EMIT("vsubps          %%ymm1, %%ymm7, %%ymm4")                /* ymm4 = K * (2/3 - TG) */ \
        __ASM_EMIT("vsubps          %%ymm2, %%ymm7, %%ymm6")                /* ymm6 = K * (2/3 - TB) */ \
        __ASM_EMIT("vaddps          %%ymm5, %%ymm0, %%ymm0")                /* ymm0 = KTR = K * TR + T1 */ \
        __ASM_EMIT("vaddps          %%ymm5, %%ymm1, %%ymm1")                /* ymm1 = KTG = K * TG + T1 */ \
        __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = KTB = K * TG + T1 */ \
        __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                /* ymm3 = RTR = K * (2/3 - TR) + T1 */ \
        __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")                /* ymm4 = RTG = K * (2/3 - TG) + T1 */ \
        __ASM_EMIT("vaddps          %%ymm5, %%ymm6, %%ymm5")                /* ymm5 = RTB = K * (2/3 - TG) + T1 */ \
        \
        /*  Process red */ \
        __ASM_EMIT("vmovaps         0x00(%[HSLM]), %%ymm7")                 /* ymm7 = TR */ \
        __ASM_EMIT("vcmpps          $5, 0x80(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TR >= 1/6] */ \
        __ASM_EMIT("vblendvps       %%ymm6, 0x80(%[HSLM]), %%ymm0, %%ymm0") /* ymm0 = KR1 = ([TR >= 1/6] & T2) | ([TR < 1/6] & KTR)  */ \
        __ASM_EMIT("vcmpps          $5, 0xa0(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TR >= 2/3] */ \
        __ASM_EMIT("vblendvps       %%ymm6, 0xa0(%[HSLM]), %%ymm3, %%ymm3") /* ymm3 = KR2 = ([TR >= 2/3] & T1) | ([TR < 2/3] & RTR)  */ \
        __ASM_EMIT("vcmpps          $5, 0x00(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TR >= 0.5] */ \
        __ASM_EMIT("vblendvps       %%ymm6, %%ymm3, %%ymm0, %%ymm0")        /* ymm0 = R = ([TR < 0.5f] & KR1) | ([TR >= 0.5f] & KR2)  */ \
        /*  Process green */ \
        __ASM_EMIT("vmovaps         0x20(%[HSLM]), %%ymm7")                 /* ymm7 = TG */ \
        __ASM_EMIT("vcmpps          $5, 0x80(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TG >= 1/6] */ \
        __ASM_EMIT("vblendvps       %%ymm6, 0x80(%[HSLM]), %%ymm1, %%ymm1") /* ymm1 = KG1 = ([TG >= 1/6] & T2) | ([TG < 1/6] & KTG)  */ \
        __ASM_EMIT("vcmpps          $5, 0xa0(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TG >= 2/3] */ \
        __ASM_EMIT("vblendvps       %%ymm6, 0xa0(%[HSLM]), %%ymm4, %%ymm4") /* ymm4 = KG2 = ([TG >= 2/3] & T1) | ([TG < 2/3] & RTG)  */ \
        __ASM_EMIT("vcmpps          $5, 0x00(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TG >= 0.5] */ \
        __ASM_EMIT("vblendvps       %%ymm6, %%ymm4, %%ymm1, %%ymm1")        /* ymm1 = G = ([TG < 0.5f] & KG1) | ([TG >= 0.5f] & KG2)  */ \
        /*  Process blue */ \
        __ASM_EMIT("vmovaps         0x40(%[HSLM]), %%ymm7")                 /* ymm7 = TB */ \
        __ASM_EMIT("vcmpps          $5, 0x80(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TB >= 1/6] */ \
        __ASM_EMIT("vblendvps       %%ymm6, 0x80(%[HSLM]), %%ymm2, %%ymm2") /* ymm2 = KB1 = ([TB >= 1/6] & T2) | ([TB < 1/6] & KTB)  */ \
        __ASM_EMIT("vcmpps          $5, 0xa0(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TB >= 2/3] */ \
        __ASM_EMIT("vblendvps       %%ymm6, 0xa0(%[HSLM]), %%ymm5, %%ymm5") /* ymm5 = KB2 = ([TB >= 2/3] & T1) | ([TB < 2/3] & RTB)  */ \
        __ASM_EMIT("vcmpps          $5, 0x00(%[XC]), %%ymm7, %%ymm6")       /* ymm6 = [TB >= 0.5] */ \
        __ASM_EMIT("vblendvps       %%ymm6, %%ymm5, %%ymm2, %%ymm2")        /* ymm2 = B = ([TB < 0.5f] & KB1) | ([TB >= 0.5f] & KB2)  */ \
        /* Process alpha */ \
        __ASM_EMIT("vmovaps         0x60(%[HSLM]), %%ymm3")                 /* ymm3 = A */ \
        /*  Transpose final result back */ \
        MAT4_TRANSPOSE("%%ymm0", "%%ymm1", "%%ymm2", "%%ymm3", "%%ymm4", "%%ymm5") \

    #define HSLA_TO_RGBA_CORE_X4   \
        /*  Transpose */\
        MAT4_TRANSPOSE("%%xmm0", "%%xmm1", "%%xmm2", "%%xmm3", "%%xmm4", "%%xmm5") \
        \
        /*  xmm0 = h0 h1 h2 h3 = H */ \
        /*  xmm1 = s0 s1 s2 s3 = S */ \
        /*  xmm2 = l0 l1 l2 l3 = L */ \
        /*  xmm3 = a0 a1 a2 a3 = A */ \
        /*  Calc temp1 (T1) and temp2 (T2) */ \
        __ASM_EMIT("vmulps          %%xmm1, %%xmm2, %%xmm7")                /* xmm7 = L * S */ \
        __ASM_EMIT("vaddps          %%xmm1, %%xmm2, %%xmm6")                /* xmm6 = L + S */ \
        __ASM_EMIT("vaddps          %%xmm2, %%xmm7, %%xmm4")                /* xmm4 = L + L * S */ \
        __ASM_EMIT("vsubps          %%xmm7, %%xmm6, %%xmm5")                /* xmm5 = L + S - L * S */ \
        __ASM_EMIT("vaddps          %%xmm2, %%xmm2, %%xmm6")                /* xmm6 = L + L */ \
        __ASM_EMIT("vcmpps          $1, 0x00(%[XC]), %%xmm2, %%xmm7")       /* xmm7 = [L < 0.5f] */ \
        __ASM_EMIT("vblendvps       %%xmm7, %%xmm4, %%xmm5, %%xmm4")        /* xmm4 = T2 = ([L < 0.5f] & (L + L*S)) | ([L >= 0.5f] & (L+S - L*S)) */ \
        __ASM_EMIT("vsubps          0x20(%[XC]), %%xmm0, %%xmm2")           /* xmm2 = H - 1/3 */ \
        __ASM_EMIT("vsubps          %%xmm4, %%xmm6, %%xmm5")                /* xmm5 = T1 = L + L - T2 */ \
        __ASM_EMIT("vmovaps         %%xmm0, %%xmm1")                        /* xmm1 = TG = H */ \
        __ASM_EMIT("vaddps          0x20(%[XC]), %%xmm0, %%xmm0")           /* xmm0 = H + 1/3 */ \
        __ASM_EMIT("vsubps          0x40(%[XC]), %%xmm0, %%xmm6")           /* xmm6 = H + 1/3 - 1 */ \
        __ASM_EMIT("vcmpps          $6, 0x40(%[XC]), %%xmm0, %%xmm7")       /* xmm7 = [(H + 1/3) > 1] */ \
        __ASM_EMIT("vblendvps       %%xmm7, %%xmm6, %%xmm0, %%xmm0")        /* xmm0 = TR = ((H + 1/3) & [(H + 1/3) <= 1]) | ((H + 1/3 - 1) & [(H + 1/3) > 1]) */ \
        __ASM_EMIT("vxorps          %%xmm7, %%xmm7, %%xmm7")                /* xmm7 = 0 */ \
        __ASM_EMIT("vaddps          0x40(%[XC]), %%xmm2, %%xmm6")           /* xmm6 = H - 1/3 + 1 */ \
        __ASM_EMIT("vcmpps          $1, %%xmm7, %%xmm2, %%xmm7")            /* xmm7 = [(H - 1/3) < 0] */ \
        __ASM_EMIT("vblendvps       %%xmm7, %%xmm6, %%xmm2, %%xmm2")        /* xmm2 = TB = ((H - 1/3) & [(H - 1/3) >= 0]) | ((H - 1/3 + 1) & [(H - 1/3) < 0]) */ \
        __ASM_EMIT("vsubps          %%xmm5, %%xmm4, %%xmm6")                /* xmm6 = T2 - T1 */ \
        __ASM_EMIT("vmulps          0x60(%[XC]), %%xmm6, %%xmm6")           /* xmm6 = K = (T2 - T1)*6.0 */ \
        /*  xmm0 = TR */ \
        /*  xmm1 = TG */ \
        /*  xmm2 = TB */ \
        /*  xmm3 = A */ \
        /*  xmm4 = T2 */ \
        /*  xmm5 = T1 */ \
        /*  xmm6 = K */ \
        __ASM_EMIT("vmovaps         %%xmm0, 0x00(%[HSLM])")                 /* TR */ \
        __ASM_EMIT("vmovaps         %%xmm1, 0x20(%[HSLM])")                 /* TG */ \
        __ASM_EMIT("vmovaps         %%xmm2, 0x40(%[HSLM])")                 /* TB */ \
        __ASM_EMIT("vmovaps         %%xmm3, 0x60(%[HSLM])")                 /* A */ \
        __ASM_EMIT("vmovaps         %%xmm4, 0x80(%[HSLM])")                 /* T2 */ \
        __ASM_EMIT("vmovaps         %%xmm5, 0xa0(%[HSLM])")                 /* T1 */ \
        \
        __ASM_EMIT("vmovaps         0xa0(%[XC]), %%xmm7")                   /* xmm7 = 2/3 */ \
        __ASM_EMIT("vmulps          %%xmm6, %%xmm0, %%xmm0")                /* xmm0 = K * TR */ \
        __ASM_EMIT("vmulps          %%xmm6, %%xmm1, %%xmm1")                /* xmm1 = K * TG */ \
        __ASM_EMIT("vmulps          %%xmm6, %%xmm2, %%xmm2")                /* xmm2 = K * TB */ \
        __ASM_EMIT("vmulps          %%xmm6, %%xmm7, %%xmm7")                /* xmm7 = K * 2/3 */ \
        __ASM_EMIT("vsubps          %%xmm0, %%xmm7, %%xmm3")                /* xmm3 = K * (2/3 - TR) */ \
        __ASM_EMIT("vsubps          %%xmm1, %%xmm7, %%xmm4")                /* xmm4 = K * (2/3 - TG) */ \
        __ASM_EMIT("vsubps          %%xmm2, %%xmm7, %%xmm6")                /* xmm6 = K * (2/3 - TB) */ \
        __ASM_EMIT("vaddps          %%xmm5, %%xmm0, %%xmm0")                /* xmm0 = KTR = K * TR + T1 */ \
        __ASM_EMIT("vaddps          %%xmm5, %%xmm1, %%xmm1")                /* xmm1 = KTG = K * TG + T1 */ \
        __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = KTB = K * TG + T1 */ \
        __ASM_EMIT("vaddps          %%xmm5, %%xmm3, %%xmm3")                /* xmm3 = RTR = K * (2/3 - TR) + T1 */ \
        __ASM_EMIT("vaddps          %%xmm5, %%xmm4, %%xmm4")                /* xmm4 = RTG = K * (2/3 - TG) + T1 */ \
        __ASM_EMIT("vaddps          %%xmm5, %%xmm6, %%xmm5")                /* xmm5 = RTB = K * (2/3 - TG) + T1 */ \
        \
        /*  Process red */ \
        __ASM_EMIT("vmovaps         0x00(%[HSLM]), %%xmm7")                 /* xmm7 = TR */ \
        __ASM_EMIT("vcmpps          $5, 0x80(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TR >= 1/6] */ \
        __ASM_EMIT("vblendvps       %%xmm6, 0x80(%[HSLM]), %%xmm0, %%xmm0") /* xmm0 = KR1 = ([TR >= 1/6] & T2) | ([TR < 1/6] & KTR)  */ \
        __ASM_EMIT("vcmpps          $5, 0xa0(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TR >= 2/3] */ \
        __ASM_EMIT("vblendvps       %%xmm6, 0xa0(%[HSLM]), %%xmm3, %%xmm3") /* xmm3 = KR2 = ([TR >= 2/3] & T1) | ([TR < 2/3] & RTR)  */ \
        __ASM_EMIT("vcmpps          $5, 0x00(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TR >= 0.5] */ \
        __ASM_EMIT("vblendvps       %%xmm6, %%xmm3, %%xmm0, %%xmm0")        /* xmm0 = R = ([TR < 0.5f] & KR1) | ([TR >= 0.5f] & KR2)  */ \
        /*  Process green */ \
        __ASM_EMIT("vmovaps         0x20(%[HSLM]), %%xmm7")                 /* xmm7 = TG */ \
        __ASM_EMIT("vcmpps          $5, 0x80(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TG >= 1/6] */ \
        __ASM_EMIT("vblendvps       %%xmm6, 0x80(%[HSLM]), %%xmm1, %%xmm1") /* xmm1 = KG1 = ([TG >= 1/6] & T2) | ([TG < 1/6] & KTG)  */ \
        __ASM_EMIT("vcmpps          $5, 0xa0(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TG >= 2/3] */ \
        __ASM_EMIT("vblendvps       %%xmm6, 0xa0(%[HSLM]), %%xmm4, %%xmm4") /* xmm4 = KG2 = ([TG >= 2/3] & T1) | ([TG < 2/3] & RTG)  */ \
        __ASM_EMIT("vcmpps          $5, 0x00(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TG >= 0.5] */ \
        __ASM_EMIT("vblendvps       %%xmm6, %%xmm4, %%xmm1, %%xmm1")        /* xmm1 = G = ([TG < 0.5f] & KG1) | ([TG >= 0.5f] & KG2)  */ \
        /*  Process blue */ \
        __ASM_EMIT("vmovaps         0x40(%[HSLM]), %%xmm7")                 /* xmm7 = TB */ \
        __ASM_EMIT("vcmpps          $5, 0x80(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TB >= 1/6] */ \
        __ASM_EMIT("vblendvps       %%xmm6, 0x80(%[HSLM]), %%xmm2, %%xmm2") /* xmm2 = KB1 = ([TB >= 1/6] & T2) | ([TB < 1/6] & KTB)  */ \
        __ASM_EMIT("vcmpps          $5, 0xa0(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TB >= 2/3] */ \
        __ASM_EMIT("vblendvps       %%xmm6, 0xa0(%[HSLM]), %%xmm5, %%xmm5") /* xmm5 = KB2 = ([TB >= 2/3] & T1) | ([TB < 2/3] & RTB)  */ \
        __ASM_EMIT("vcmpps          $5, 0x00(%[XC]), %%xmm7, %%xmm6")       /* xmm6 = [TB >= 0.5] */ \
        __ASM_EMIT("vblendvps       %%xmm6, %%xmm5, %%xmm2, %%xmm2")        /* xmm2 = B = ([TB < 0.5f] & KB1) | ([TB >= 0.5f] & KB2)  */ \
        /* Process alpha */ \
        __ASM_EMIT("vmovaps         0x60(%[HSLM]), %%xmm3")                 /* xmm3 = A */ \
        /*  Transpose final result back */ \
        MAT4_TRANSPOSE("%%xmm0", "%%xmm1", "%%xmm2", "%%xmm3", "%%xmm4", "%%xmm5")

        void hsla_to_rgba(float *dst, const float *src, size_t count)
        {
            #pragma pack(push, 1)
            struct {
                float tr[8], tg[8], tb[8], a[8];
                float t2[8], t1[8];
            } hslm __lsp_aligned32;
            #pragma pack(pop)

            ARCH_X86_ASM
            (
                //-----------------------------------------------------------------
                // 8x blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")      // ymm0 = h0 s0 l0 a0 h1 s1 l1 a1
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm1")      // ymm1 = h2 s2 l2 a2 h3 s3 l3 a3
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm2")      // ymm2 = h4 s4 l4 a4 h5 s5 l5 a5
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm3")      // ymm3 = h6 s6 l6 a6 h7 s7 l7 a7

                HSLA_TO_RGBA_CORE_X8

                // Store result
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst])")

                // Repeat loop
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")

                //-----------------------------------------------------------------
                // 4x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                __ASM_EMIT("vmovups         0x10(%[src]), %%xmm1")
                __ASM_EMIT("vmovups         0x20(%[src]), %%xmm2")
                __ASM_EMIT("vmovups         0x30(%[src]), %%xmm3")

                HSLA_TO_RGBA_CORE_X4

                // Store result
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst])")
                __ASM_EMIT("vmovups         %%xmm2, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%xmm3, 0x30(%[dst])")

                // Update pointers
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")

                //-----------------------------------------------------------------
                // 1x - 3x block
                // Load last variable-sized chunk
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("4:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm1")
                __ASM_EMIT("vmovups         0x10(%[src]), %%xmm2")
                __ASM_EMIT("6:")

                HSLA_TO_RGBA_CORE_X4

                // Store last chunk
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovups         %%xmm1, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm2, 0x10(%[dst])")

                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [XC] "r" (HSL_RGB), [HSLM] "r" (&hslm)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef HSLA_TO_RGBA_CORE_X8
    #undef HSLA_TO_RGBA_CORE_X4


    } /* namespace avx2 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX2_GRAPHICS_COLORS_H_ */
