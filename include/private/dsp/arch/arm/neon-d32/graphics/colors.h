/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 сент. 2022 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_GRAPHICS_COLORS_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_GRAPHICS_COLORS_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        #define FILL4_CORE \
            __ASM_EMIT("vld1.32     {q0}, [%[c4]]")     /* q0 = c0 c1 c2 c3 */ \
            __ASM_EMIT("vmov        q1, q0")            /* q1 = c0 c1 c2 c3 */ \
            __ASM_EMIT("vmov        q2, q0")            /* q2 = c0 c1 c2 c3 */ \
            __ASM_EMIT("vmov        q3, q1")            /* q3 = c0 c1 c2 c3 */ \
            \
            /* 8x blocks */ \
            __ASM_EMIT("subs        %[count], #8") \
            __ASM_EMIT("blo         2f") \
            __ASM_EMIT("vmov        q4, q0")            /* q4 = c0 c1 c2 c3 */ \
            __ASM_EMIT("vmov        q5, q1")            /* q5 = c0 c1 c2 c3 */ \
            __ASM_EMIT("vmov        q6, q2")            /* q6 = c0 c1 c2 c3 */ \
            __ASM_EMIT("vmov        q7, q3")            /* q7 = c0 c1 c2 c3 */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vstm        %[dst], {q0-q7}") \
            __ASM_EMIT("subs        %[count], #8") \
            __ASM_EMIT("add         %[dst], #0x80") \
            __ASM_EMIT("bhs         1b") \
            /* 4x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], #4") \
            __ASM_EMIT("blt         4f") \
            __ASM_EMIT("vstm        %[dst], {q0-q3}") \
            __ASM_EMIT("sub         %[count], #4") \
            __ASM_EMIT("add         %[dst], #0x40") \
            /* 2x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], #2") \
            __ASM_EMIT("blt         6f") \
            __ASM_EMIT("vst1.32     {q0-q1}, [%[dst]]") \
            __ASM_EMIT("sub         %[count], #2") \
            __ASM_EMIT("add         %[dst], #0x20") \
            /* 1x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], #1") \
            __ASM_EMIT("blt         8f") \
            __ASM_EMIT("vst1.32     {q0}, [%[dst]]") \
            __ASM_EMIT("8:")

            void fill_rgba(float *dst, float r, float g, float b, float a, size_t count)
            {
                IF_ARCH_ARM(
                    float c4[4] __lsp_aligned16;
                    c4[0] = r;
                    c4[1] = g;
                    c4[2] = b;
                    c4[3] = a;
                )

                ARCH_ARM_ASM
                (
                    FILL4_CORE
                    : [dst] "+r" (dst), [count] "+r" (count)
                    : [c4] "r" (&c4[0])
                    : "cc", "memory",
                      "q0", "q1", "q2", "q3",
                      "q4", "q5", "q6", "q7"
                );
            }

            void fill_hsla(float *dst, float h, float s, float l, float a, size_t count)
            {
                IF_ARCH_ARM(
                    float c4[4] __lsp_aligned16;
                    c4[0] = h;
                    c4[1] = s;
                    c4[2] = l;
                    c4[3] = a;
                );

                ARCH_ARM_ASM
                (
                    FILL4_CORE
                    : [dst] "+r" (dst), [count] "+r" (count)
                    : [c4] "r" (&c4[0])
                    : "cc", "memory",
                      "q0", "q1", "q2", "q3",
                      "q4", "q5", "q6", "q7"
                );
            }

        #undef FILL4_CORE

        IF_ARCH_ARM(
            static const float HSL_RGB[] =
            {
                LSP_DSP_VEC4(0.5f),                // 1/2
                LSP_DSP_VEC4(0.333333333333f),     // 1/3
                LSP_DSP_VEC4(1.0f),                // 1
                LSP_DSP_VEC4(6.0f),                // 6
                LSP_DSP_VEC4(0.166666666667f),     // 1/6
                LSP_DSP_VEC4(0.666666666667f)      // 2/3
            };

            static const float RGB_HSL[] =
            {
                LSP_DSP_VEC4(4.0f),
                LSP_DSP_VEC4(2.0f),
                LSP_DSP_VEC4(6.0f),
                LSP_DSP_VEC4(1.0f),
                LSP_DSP_VEC4(0.5f),
                LSP_DSP_VEC4(0.166666666667f)      // 1/6
            };

            static const float RGBA_TO_BGRA32[] =
            {
                LSP_DSP_VEC4(255.0f),
                LSP_DSP_VEC4(255.0f)
            };
        )

        #define HSLA_TO_RGBA_CORE   \
            /*  q10 = 1/2, q11 = 1/3, q12 = 1, q13 = 6, q14 = 1/6, q15 = 2/3 */ \
            /*  q0 = h0 h1 h2 h3 = H */ \
            /*  q1 = l0 l1 l2 l3 = L */ \
            /*  q2 = s0 s1 s2 s3 = S */ \
            /*  q3 = a0 a1 a2 a3 = A */ \
            /*  Calc temp1 (T1) and temp2 (T2) */ \
            __ASM_EMIT("vadd.f32        q4, q2, q1")                /* q4 = L+S */ \
            __ASM_EMIT("vadd.f32        q9, q1, q1")                /* q9 = L+L */ \
            __ASM_EMIT("vmul.f32        q5, q2, q1")                /* q5 = L*S */ \
            __ASM_EMIT("vsub.f32        q4, q4, q5")                /* q4 = L+S - L*S */ \
            __ASM_EMIT("vadd.f32        q7, q1, q5")                /* q7 = L + L*S */ \
            __ASM_EMIT("vclt.f32        q8, q1, q10")               /* q8 = [L < 0.5] */ \
            __ASM_EMIT("vbit            q4, q7, q8")                /* q4 = T2 = (L+L*S) & [L<0.5]) | (((L+S-L*S) & [L>=0.5]) */ \
            __ASM_EMIT("vmov            q2, q0")                    /* q2 = TG = H */ \
            __ASM_EMIT("vsub.f32        q5, q9, q4")                /* q5 = T1 = L+L-T2 */ \
            __ASM_EMIT("vadd.f32        q0, q0, q11")               /* q0 = H + 1/3 */ \
            __ASM_EMIT("vsub.f32        q1, q2, q11")               /* q1 = H - 1/3 */ \
            __ASM_EMIT("vcgt.f32        q9, q0, q12")               /* q9 = [H+1/3 > 1] */ \
            __ASM_EMIT("veor            q8, q8")                    /* q8 = 0 */ \
            __ASM_EMIT("vsub.f32        q6, q0, q12")               /* q6 = H + 1/3 - 1 */ \
            __ASM_EMIT("vadd.f32        q7, q2, q12")               /* q7 = H - 1/3 + 1 */ \
            __ASM_EMIT("vclt.f32        q8, q1, q8")                /* q8 = [H-1/3 < 0] */ \
            __ASM_EMIT("vbit            q0, q6, q9")                /* q0 = TR = ((H+1/3) & [H+1/3 <= 1]) | ((H+1/3-1) & [H+1/3 > 1]) */ \
            __ASM_EMIT("vbit            q1, q7, q8")                /* q1 = TB = ((H-1/3) & [H-1/3 >= 0]) | ((H-1/3+1) & [H-1/3 < 0]) */ \
            __ASM_EMIT("vsub.f32        q6, q4, q5")                /* q6 = T2 - T1 */ \
            __ASM_EMIT("vmul.f32        q6, q6, q13")               /* q6 = K = (T2 - T1)*6 */ \
            /* q0 = TR */ \
            /* q2 = TG */ \
            /* q1 = TB */ \
            /* q3 = A */ \
            /* q4 = T2 */ \
            /* q5 = T1 */ \
            /* q6 = K */ \
            /* Process red color */ \
            __ASM_EMIT("vmov            q7, q5")                    /* q7 = T1 */ \
            __ASM_EMIT("vmov            q8, q5")                    /* q8 = T1 */ \
            __ASM_EMIT("vsub.f32        q9, q15, q0")               /* q9 = 2/3 - TR */ \
            __ASM_EMIT("vmla.f32        q7, q6, q0")                /* q7 = KTR = T1 + K*TR */ \
            __ASM_EMIT("vmla.f32        q8, q6, q9")                /* q8 = RTR = T1 + K*(2/3-TR) */ \
            __ASM_EMIT("vclt.f32        q9, q0, q14")               /* q9 = [TR < 1/6] */ \
            __ASM_EMIT("vbif            q7, q4, q9")                /* q7 = (KTR & [TR < 1/6]) | ((T2) & (TR >= 1/6)) */ \
            __ASM_EMIT("vclt.f32        q9, q0, q15")               /* q9 = [TR < 2/3] */ \
            __ASM_EMIT("vbif            q8, q5, q9")                /* q8 = (RTR & [TR < 2/3]) | ((T1) & (TR >= 2/3)) */ \
            __ASM_EMIT("vclt.f32        q9, q0, q10")               /* q9 = [TR < 1/2] */ \
            __ASM_EMIT("vand            q0, q7, q9")                /* q0 = (KTR & [TR < 1/6] & [TR < 1/2]) | ((T2) & (TR >= 1/6) & [TR < 1/2]) */ \
            __ASM_EMIT("vbif            q0, q8, q9")                /* q0 = (KTR & [TR < 1/6] & [TR < 1/2]) | ((T2) & (TR >= 1/6) & [TR < 1/2]) | (RTR & [TR < 2/3] & [TR >= 1/2]) | ((T1) & (TR >= 2/3) & [TR >= 1/2]) */ \
            /* Process green color */ \
            __ASM_EMIT("vmov            q7, q5")                    /* q7 = T1 */ \
            __ASM_EMIT("vmov            q8, q5")                    /* q8 = T1 */ \
            __ASM_EMIT("vsub.f32        q9, q15, q2")               /* q9 = 2/3 - TG */ \
            __ASM_EMIT("vmla.f32        q7, q6, q2")                /* q7 = KTG = T1 + K*TG */ \
            __ASM_EMIT("vmla.f32        q8, q6, q9")                /* q8 = RTG = T1 + K*(2/3-TG) */ \
            __ASM_EMIT("vclt.f32        q9, q2, q14")               /* q9 = [TG < 1/6] */ \
            __ASM_EMIT("vbif            q7, q4, q9")                /* q7 = (KTG & [TG < 1/6]) | ((T2) & (TG >= 1/6)) */ \
            __ASM_EMIT("vclt.f32        q9, q2, q15")               /* q9 = [TG < 2/3] */ \
            __ASM_EMIT("vbif            q8, q5, q9")                /* q8 = (RTG & [TG < 2/3]) | ((T1) & (TG >= 2/3)) */ \
            __ASM_EMIT("vclt.f32        q9, q2, q10")               /* q9 = [TG < 1/2] */ \
            __ASM_EMIT("vand            q2, q7, q9")                /* q2 = (KTG & [TG < 1/6] & [TG < 1/2]) | ((T2) & (TG >= 1/6) & [TG < 1/2]) */ \
            __ASM_EMIT("vbif            q2, q8, q9")                /* q2 = (KTG & [TG < 1/6] & [TG < 1/2]) | ((T2) & (TG >= 1/6) & [TG < 1/2]) | (RTG & [TG < 2/3] & [TG >= 1/2]) | ((T1) & (TG >= 2/3) & [TG >= 1/2]) */ \
            /* Process blue color */ \
            __ASM_EMIT("vmov            q7, q5")                    /* q7 = T1 */ \
            __ASM_EMIT("vmov            q8, q5")                    /* q8 = T1 */ \
            __ASM_EMIT("vsub.f32        q9, q15, q1")               /* q9 = 2/3 - TB */ \
            __ASM_EMIT("vmla.f32        q7, q6, q1")                /* q7 = KTB = T1 + K*TB */ \
            __ASM_EMIT("vmla.f32        q8, q6, q9")                /* q8 = RTB = T1 + K*(2/3-TB) */ \
            __ASM_EMIT("vclt.f32        q9, q1, q14")               /* q9 = [TB < 1/6] */ \
            __ASM_EMIT("vbif            q7, q4, q9")                /* q7 = (KTB & [TB < 1/6]) | ((T2) & (TB >= 1/6)) */ \
            __ASM_EMIT("vclt.f32        q9, q1, q15")               /* q9 = [TB < 2/3] */ \
            __ASM_EMIT("vbif            q8, q5, q9")                /* q8 = (RTB & [TB < 2/3]) | ((T1) & (TB >= 2/3)) */ \
            __ASM_EMIT("vclt.f32        q9, q1, q10")               /* q9 = [TB < 1/2] */ \
            __ASM_EMIT("vand            q1, q7, q9")                /* q1 = (KTB & [TB < 1/6] & [TB < 1/2]) | ((T2) & (TB >= 1/6) & [TB < 1/2]) */ \
            __ASM_EMIT("vbif            q1, q8, q9")                /* q1 = (KTB & [TB < 1/6] & [TB < 1/2]) | ((T2) & (TB >= 1/6) & [TB < 1/2]) | (RTB & [TB < 2/3] & [TB >= 1/2]) | ((T1) & (TB >= 2/3) & [TB >= 1/2]) */ \
            /*  q0 = R */ \
            /*  q1 = B */ \
            /*  q2 = G */ \
            /*  q3 = A */ \

        /*
                //Set the temporary values
                if  (L < HSL_RGB_0_5)
                    temp2 = L + (L * S);
                else
                    temp2 = (L + S) - (L * S);

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



            void hsla_to_rgba(float *dst, const float *src, size_t count)
            {
                ARCH_ARM_ASM
                (
                    __ASM_EMIT("vldm            %[XC], {q10-q15}")          // q10 = 1/2, q11 = 1/3, q12 = 1, q13 = 6, q14 = 1/6, q15 = 2/3
                    __ASM_EMIT("subs            %[count], #4")
                    __ASM_EMIT("blo             2f")

                    //-----------------------------------------------------------------
                    // 4x blocks
                    __ASM_EMIT("1:")
                    __ASM_EMIT("vld4.32         {q0-q1}, [%[src]]!")        // q0 = h0 h1 s0 s1, q1 = l0 l1 a0 a1
                    __ASM_EMIT("vld4.32         {q2-q3}, [%[src]]!")        // q2 = h2 h3 s2 s3, q3 = l2 l3 a2 a3
                    __ASM_EMIT("vswp            d1, d4")                    // q0 = h0 h1 h2 h3, q2 = s0 s1 s2 s3
                    __ASM_EMIT("vswp            d3, d6")                    // q1 = l0 l1 l2 l3, q3 = a0 a1 a2 a3

                    HSLA_TO_RGBA_CORE

                    __ASM_EMIT("vswp            d1, d4")                    // q0 = r0 r1 g0 g1, q2 = r2 r3 g2 g3
                    __ASM_EMIT("vswp            d3, d6")                    // q1 = b0 b1 a0 a1, q3 = b2 b3 a2 a3
                    __ASM_EMIT("vst4.32         {q0-q1}, [%[dst]]!")
                    __ASM_EMIT("vst4.32         {q2-q3}, [%[dst]]!")
                    __ASM_EMIT("subs            %[count], #4")
                    __ASM_EMIT("bhs             1b")

                    __ASM_EMIT("2:")
                    __ASM_EMIT("adds            %[count], #4")
                    __ASM_EMIT("bls             10f")

                    //-----------------------------------------------------------------
                    // 1x-3x block
                    __ASM_EMIT("tst             %[count], #2")
                    __ASM_EMIT("beq             4f")
                    __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")        // q0 = h0 s0 l0 a0, q1 = h1 s1 l1 a1
                    __ASM_EMIT("4:")
                    __ASM_EMIT("tst             %[count], #1")
                    __ASM_EMIT("beq             6f")
                    __ASM_EMIT("vld1.32         {q2}, [%[src]]")            // q2 = h2 s2 l2 a2, q3 = ? ? ? ?
                    __ASM_EMIT("6:")

                    __ASM_EMIT("vtrn.32         q0, q1")                    // q0 = h0 h1 l0 l1, q1 = s0 s1 a0 a1
                    __ASM_EMIT("vtrn.32         q2, q3")                    // q2 = h2  ? l2  ?, q3 = s2  ? a2  ?
                    __ASM_EMIT("vswp            d1, d4")                    // q0 = h0 h1 h2  ?, q2 = l0 l1 l2  ?
                    __ASM_EMIT("vswp            d3, d6")                    // q1 = s0 s1 s2  ?, q3 = a0 a1 a2  ?

                    __ASM_EMIT("vswp            q1, q2")                    // q1 = l0 l1 l2  ?, q2 = s0 s1 s2  ?
                    HSLA_TO_RGBA_CORE
                    __ASM_EMIT("vswp            q1, q2")

                    __ASM_EMIT("vtrn.32         q0, q1")
                    __ASM_EMIT("vtrn.32         q2, q3")
                    __ASM_EMIT("vswp            d1, d4")
                    __ASM_EMIT("vswp            d3, d6")

                    __ASM_EMIT("tst             %[count], #2")
                    __ASM_EMIT("beq             8f")
                    __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                    __ASM_EMIT("8:")
                    __ASM_EMIT("tst             %[count], #1")
                    __ASM_EMIT("beq             10f")
                    __ASM_EMIT("vst1.32         {q2}, [%[dst]]")

                    __ASM_EMIT("10:")

                    : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                    : [XC] "r" (&HSL_RGB[0])
                    : "cc", "memory",
                      "q0", "q1", "q2", "q3",
                      "q4", "q5", "q6", "q7",
                      "q8", "q9", "q10", "q11",
                      "q12", "q13", "q14", "q15"
                );
            }

        #undef HSLA_TO_RGBA_CORE

        #define RGBA_TO_HSLA_CORE \
            /*  q0   = R */ \
            /*  q1   = B */ \
            /*  q2   = G */ \
            /*  q3   = A */ \
            /*  q10  = 4, q11 = 2, q12 = 6, q13 = 1, q14 = 1/2, q15 = 1/6 */ \
            __ASM_EMIT("vmax.f32        q4, q0, q1") \
            __ASM_EMIT("vmin.f32        q5, q0, q1") \
            __ASM_EMIT("vmax.f32        q4, q4, q2")                /* q4 = CMAX */ \
            __ASM_EMIT("vmin.f32        q5, q5, q2")                /* q5 = CMIN */ \
            __ASM_EMIT("vsub.f32        q6, q4, q5")                /* q6 = D = CMAX - CMIN */ \
            __ASM_EMIT("vadd.f32        q5, q4, q5")                /* q5 = CMAX + CMIN */ \
            __ASM_EMIT("vstm            %[RGBA], {q0-q3}")          /* store R, B, G, A */ \
            \
            __ASM_EMIT("vrecpe.f32      q9, q6")                    /* q9 = RD */ \
            __ASM_EMIT("vrecps.f32      q8, q9, q6")                /* q8 = (2 - RD*D) */ \
            __ASM_EMIT("vmul.f32        q9, q8, q9")                /* q9 = d' = RD * (2 - RD*D) */ \
            __ASM_EMIT("vrecps.f32      q8, q9, q6")                /* q8 = (2 - RD*d') */ \
            __ASM_EMIT("vmul.f32        q9, q8, q9")                /* q9 = 1/D = d' * (2 - RD*d') */  \
            \
            /* q0  = R */ \
            /* q1  = B */ \
            /* q2  = G */ \
            /* q3  = A */ \
            /* q4  = CMAX */ \
            /* q5  = CMAX + CMIN */ \
            /* q6  = D */ \
            /* q9  = 1/D */ \
            __ASM_EMIT("vsub.f32        q7, q1, q0")                /* q7 = B-R */ \
            __ASM_EMIT("vsub.f32        q8, q0, q2")                /* q8 = R-G */ \
            __ASM_EMIT("vsub.f32        q0, q2, q1")                /* q0 = G-B */ \
            __ASM_EMIT("vmul.f32        q7, q7, q9")                /* q7 = (B-R)/D */ \
            __ASM_EMIT("veor            q1, q1")                    /* q1 = 0 */ \
            __ASM_EMIT("vmul.f32        q0, q0, q9")                /* q0 = (G-B)/D */ \
            __ASM_EMIT("vmul.f32        q8, q8, q9")                /* q8 = (R-G)/D */ \
            __ASM_EMIT("vclt.f32        q1, q0, q1")                /* q1 = [(G-B)/D < 0] */ \
            __ASM_EMIT("vadd.f32        q7, q7, q11")               /* q7 = HG = (B-R)/D + 2 */ \
            __ASM_EMIT("vand            q1, q12")                   /* q1 = [(G-B)/D < 0] & 6 */ \
            __ASM_EMIT("vadd.f32        q8, q8, q10")               /* q8 = HB = (R-G)/D + 4 */ \
            __ASM_EMIT("vadd.f32        q9, q0, q1")                /* q9 = HR = (G-B)/D + [(G-B)/D < 0] & 6 */ \
            __ASM_EMIT("vldm            %[RGBA], {q0-q3}")          /* load R, B, G, A */ \
            /* q0  = R */ \
            /* q1  = B */ \
            /* q2  = G */ \
            /* q3  = A */ \
            /* q4  = CMAX */ \
            /* q5  = CMAX + CMIN */ \
            /* q6  = D */ \
            /* q7  = HG */ \
            /* q8  = HB */ \
            /* q9  = HR */ \
            __ASM_EMIT("vceq.f32        q2, q2, q4")                /* q2 = [G == CMAX] */ \
            __ASM_EMIT("vceq.f32        q0, q0, q4")                /* q0 = [R == CMAX] */ \
            __ASM_EMIT("vbsl            q2, q7, q8")                /* q2 = (HG & [G == CMAX]) | (HB & [G != CMAX]) */ \
            __ASM_EMIT("veor            q4, q4")                    /* q4 = 0 */ \
            __ASM_EMIT("vbsl            q0, q9, q2")                /* q0 = (HR & [R == CMAX]) | (HG & [G == CMAX] & [R != CMAX]) | (HB & [G != CMAX] & [R != CMAX]) */ \
            __ASM_EMIT("vceq.f32        q4, q4, q6")                /* q4 = [ D == 0 ] */ \
            __ASM_EMIT("vmul.f32        q1, q5, q14")               /* q1 = L = (CMAX + CMIN) * 0.5 */ \
            __ASM_EMIT("vsub.f32        q2, q13, q1")               /* q2 = X = 1 - L */ \
            __ASM_EMIT("vbic            q0, q0, q4")                /* q0 = H = [D != 0] & ((HR & [R == CMAX]) | (HG & [G == CMAX] & [R != CMAX]) | (HB & [G != CMAX] & [R != CMAX])) */ \
            \
            __ASM_EMIT("vrecpe.f32      q5, q1")                    /* q5 = RL */ \
            __ASM_EMIT("vrecpe.f32      q9, q2")                    /* q9 = RX */ \
            __ASM_EMIT("vrecps.f32      q4, q5, q1")                /* q4 = (2 - RL*L) */ \
            __ASM_EMIT("vrecps.f32      q8, q9, q2")                /* q8 = (2 - RX*X) */ \
            __ASM_EMIT("vmul.f32        q5, q4, q5")                /* q5 = l' = L * (2 - RL*L) */ \
            __ASM_EMIT("vmul.f32        q9, q8, q9")                /* q9 = x' = X * (2 - RX*X) */ \
            __ASM_EMIT("vrecps.f32      q4, q5, q1")                /* q4 = (2 - RL*l') */ \
            __ASM_EMIT("vrecps.f32      q8, q9, q2")                /* q8 = (2 - RX*X') */ \
            __ASM_EMIT("vmul.f32        q5, q4, q5")                /* q5 = 1/L = l' * (2 - RL*l') */  \
            __ASM_EMIT("vmul.f32        q9, q8, q9")                /* q9 = 1/X = x' * (2 - RX*x') */  \
            __ASM_EMIT("vmul.f32        q2, q6, q5")                /* q2 = D/L */ \
            __ASM_EMIT("vmul.f32        q4, q6, q9")                /* q4 = D/X */ \
            __ASM_EMIT("veor            q5, q5")                    /* q5 = 0 */ \
            \
            __ASM_EMIT("vcle.f32        q6, q1, q14")               /* q6 = [L <= 0.5] */ \
            __ASM_EMIT("vceq.f32        q7, q1, q5")                /* q7 = [L == 0] */ \
            __ASM_EMIT("vcgt.f32        q8, q1, q14")               /* q8 = [L > 0.5] */ \
            __ASM_EMIT("vbit            q6, q5, q7")                /* q6 = [L <= 0.5] & [L != 0] */ \
            __ASM_EMIT("vand            q8, q8, q4")                /* q8 = D/X & [L > 0.5] */ \
            __ASM_EMIT("vand            q6, q6, q2")                /* q6 = D/L & [L <= 0.5] & [L != 0] */ \
            __ASM_EMIT("vorr            q2, q8, q6")                /* q2 = S = (D/L & [L <= 0.5] & [L != 0]) | (D/X & [L > 0.5]) */ \
            __ASM_EMIT("vmul.f32        q0, q0, q15")               /* q0 = H * 1/6 */ \
            __ASM_EMIT("vmul.f32        q2, q2, q14")               /* q2 = S * 1/2 */ \

        /*
            float cmax = (R < G) ? ((B < G) ? G : B) : ((B < R) ? R : B);
            float cmin = (R < G) ? ((B < R) ? B : R) : ((B < G) ? B : G);
            float d = cmax - cmin;

            H = 0.0f;
            S = 0.0f;
            L = HSL_RGB_0_5 * (cmax + cmin);

            // Calculate hue
            if (R == cmax)
            {
                H = (G - B) / d;
                if (H < 0.0f)
                    H += 6.0f;
            }
            else if (G == cmax)
                H = (B - R) / d + 2.0f;
            else
                H = (R - G) / d + 4.0f;

            // Calculate saturation
            if (L < 0.5f)
                S = (L != 0.0f) ? d / L : 0.0f;
            else
                S = (L != 1.0f) ? d / (1.0f - L) : 0.0f;

            // Normalize hue and saturation
            H  *= HSL_RGB_1_6;
            S  *= HSL_RGB_0_5;
         */

            void rgba_to_hsla(float *dst, const float *src, size_t count)
            {
        #pragma pack(push, 1)
                IF_ARCH_ARM(
                    struct
                    {
                        float r[4], g[4], b[4], a[4];
                    } rgba __lsp_aligned16;
                );
        #pragma pack(pop)

                ARCH_ARM_ASM
                (
                    __ASM_EMIT("vldm            %[XC], {q10-q15}")
                    __ASM_EMIT("subs            %[count], #4")
                    __ASM_EMIT("blo             2f")

                    //-----------------------------------------------------------------
                    // 4x blocks
                    __ASM_EMIT("1:")
                    __ASM_EMIT("vld4.32         {q0-q1}, [%[src]]!")
                    __ASM_EMIT("vld4.32         {q2-q3}, [%[src]]!")
                    __ASM_EMIT("vswp            d1, d4")
                    __ASM_EMIT("vswp            d3, d6")

                    RGBA_TO_HSLA_CORE

                    __ASM_EMIT("vswp            d1, d4")
                    __ASM_EMIT("vswp            d3, d6")
                    __ASM_EMIT("vst4.32         {q0-q1}, [%[dst]]!")
                    __ASM_EMIT("vst4.32         {q2-q3}, [%[dst]]!")
                    __ASM_EMIT("subs            %[count], #4")
                    __ASM_EMIT("bhs             1b")

                    __ASM_EMIT("2:")
                    __ASM_EMIT("adds            %[count], #4")
                    __ASM_EMIT("bls             10f")

                    //-----------------------------------------------------------------
                    // 1x-3x block
                    __ASM_EMIT("tst             %[count], #2")
                    __ASM_EMIT("beq             4f")
                    __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
                    __ASM_EMIT("4:")
                    __ASM_EMIT("tst             %[count], #1")
                    __ASM_EMIT("beq             6f")
                    __ASM_EMIT("vld1.32         {q2}, [%[src]]")
                    __ASM_EMIT("6:")

                    __ASM_EMIT("vtrn.32         q0, q1")
                    __ASM_EMIT("vtrn.32         q2, q3")
                    __ASM_EMIT("vswp            d1, d4")
                    __ASM_EMIT("vswp            d3, d6")

                    __ASM_EMIT("vswp            q1, q2")
                    RGBA_TO_HSLA_CORE
                    __ASM_EMIT("vswp            q1, q2")

                    __ASM_EMIT("vtrn.32         q0, q1")
                    __ASM_EMIT("vtrn.32         q2, q3")
                    __ASM_EMIT("vswp            d1, d4")
                    __ASM_EMIT("vswp            d3, d6")

                    __ASM_EMIT("tst             %[count], #2")
                    __ASM_EMIT("beq             8f")
                    __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
                    __ASM_EMIT("8:")
                    __ASM_EMIT("tst             %[count], #1")
                    __ASM_EMIT("beq             10f")
                    __ASM_EMIT("vst1.32         {q2}, [%[dst]]")

                    __ASM_EMIT("10:")

                    : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                    : [XC] "r" (&RGB_HSL[0]), [RGBA] "r" (&rgba)
                    : "cc", "memory",
                      "q0", "q1", "q2", "q3",
                      "q4", "q5", "q6", "q7",
                      "q8", "q9", "q10", "q11",
                      "q12", "q13", "q14", "q15"
                );
            }

        #undef RGBA_TO_HSLA_CORE

        #define RGBA_TO_BGRA32_CORE_X8 \
            /* Transpose */ \
            __ASM_EMIT("vtrn.32         q0, q1") \
            __ASM_EMIT("vtrn.32         q2, q3") \
            __ASM_EMIT("vtrn.32         q4, q5") \
            __ASM_EMIT("vtrn.32         q6, q7") \
            __ASM_EMIT("vswp            d4, d1") \
            __ASM_EMIT("vswp            d12, d9") \
            __ASM_EMIT("vswp            d6, d3") \
            __ASM_EMIT("vswp            d14, d11") \
            \
            /* q0 = r, q1 = g, q2 = b, q3 = a */ \
            __ASM_EMIT("vmul.f32        q3, q3, q14")                   /* q3   = a * 255 */ \
            __ASM_EMIT("vmul.f32        q7, q7, q15") \
            __ASM_EMIT("vsub.f32        q3, q14, q3")                   /* q3   = A = 255 - a*255 * */ \
            __ASM_EMIT("vsub.f32        q7, q15, q7") \
            __ASM_EMIT("vmul.f32        q8, q2, q3")                    /* q8   = B = b * A */ \
            __ASM_EMIT("vmul.f32        q11, q6, q7") \
            __ASM_EMIT("vmul.f32        q9, q1, q3")                    /* q9   = G = g * A */ \
            __ASM_EMIT("vmul.f32        q12, q5, q7") \
            __ASM_EMIT("veor            q6, q6")                        /* q6   = 0 */ \
            __ASM_EMIT("vmul.f32        q10, q0, q3")                   /* q10  = R = r * A */ \
            __ASM_EMIT("vmul.f32        q13, q4, q7") \
            __ASM_EMIT("vcge.f32        q0, q8, q6")                    /* q0   = [B >= 0] */ \
            __ASM_EMIT("vcge.f32        q4, q11, q6") \
            __ASM_EMIT("vcge.f32        q1, q9, q6")                    /* q1   = [G >= 0] */ \
            __ASM_EMIT("vcge.f32        q5, q12, q6") \
            __ASM_EMIT("vcge.f32        q2, q10, q6")                   /* q2   = [R >= 0] */ \
            __ASM_EMIT("vcge.f32        q6, q13, q6") \
            __ASM_EMIT("vand            q0, q0, q8")                    /* q0   = B & [B >= 0] */ \
            __ASM_EMIT("vand            q4, q4, q11") \
            __ASM_EMIT("vand            q1, q1, q9")                    /* q1   = G & [G >= 0] */ \
            __ASM_EMIT("vand            q5, q5, q12") \
            __ASM_EMIT("vand            q2, q2, q10")                   /* q2   = R & [R >= 0] */ \
            __ASM_EMIT("vand            q6, q6, q13") \
            \
            /* Transpose back */ \
            __ASM_EMIT("vtrn.32         q0, q1") \
            __ASM_EMIT("vtrn.32         q2, q3") \
            __ASM_EMIT("vtrn.32         q4, q5") \
            __ASM_EMIT("vtrn.32         q6, q7") \
            __ASM_EMIT("vswp            d4, d1") \
            __ASM_EMIT("vswp            d12, d9") \
            __ASM_EMIT("vswp            d6, d3") \
            __ASM_EMIT("vswp            d14, d11") \
            /* q0     = b0 g0 r0 a0 */ \
            /* q1     = b1 g1 r1 a1 */ \
            /* q2     = b2 g2 r2 a2 */ \
            /* q3     = b3 g3 r3 a3 */ \
            \
            __ASM_EMIT("vcvt.u32.f32    q0, q0")                        /* q0 = int(b0 g0 r0 a0) */ \
            __ASM_EMIT("vcvt.u32.f32    q4, q4") \
            __ASM_EMIT("vcvt.u32.f32    q1, q1")                        /* q1 = int(b1 g1 r1 a1) */ \
            __ASM_EMIT("vcvt.u32.f32    q5, q5") \
            __ASM_EMIT("vcvt.u32.f32    q2, q2")                        /* q2 = int(b2 g2 r2 a2) */ \
            __ASM_EMIT("vcvt.u32.f32    q6, q6") \
            __ASM_EMIT("vcvt.u32.f32    q3, q3")                        /* q3 = int(b3 g3 r3 a3) */ \
            __ASM_EMIT("vcvt.u32.f32    q7, q7") \
            __ASM_EMIT("vqmovn.u32      d0, q0") \
            __ASM_EMIT("vqmovn.u32      d1, q1") \
            __ASM_EMIT("vqmovn.u32      d2, q2") \
            __ASM_EMIT("vqmovn.u32      d3, q3") \
            __ASM_EMIT("vqmovn.u32      d4, q4") \
            __ASM_EMIT("vqmovn.u32      d5, q5") \
            __ASM_EMIT("vqmovn.u32      d6, q6") \
            __ASM_EMIT("vqmovn.u32      d7, q7") \
            __ASM_EMIT("vqmovn.u16      d0, q0") \
            __ASM_EMIT("vqmovn.u16      d1, q1") \
            __ASM_EMIT("vqmovn.u16      d2, q2") \
            __ASM_EMIT("vqmovn.u16      d3, q3")

        #define RGBA_TO_BGRA32_CORE_X4 \
            /* Transpose */ \
            __ASM_EMIT("vtrn.32         q0, q1") \
            __ASM_EMIT("vtrn.32         q2, q3") \
            __ASM_EMIT("vswp            d4, d1") \
            __ASM_EMIT("vswp            d6, d3") \
            \
            /* q0 = r, q1 = g, q2 = b, q3 = a */ \
            __ASM_EMIT("vmul.f32        q3, q3, q14")                   /* q3   = a * 255 */ \
            __ASM_EMIT("vsub.f32        q3, q14, q3")                   /* q3   = A = 255 - a*255 * */ \
            __ASM_EMIT("vmul.f32        q8, q2, q3")                    /* q8   = B = b * A */ \
            __ASM_EMIT("vmul.f32        q9, q1, q3")                    /* q9   = G = g * A */ \
            __ASM_EMIT("vmul.f32        q10, q0, q3")                   /* q10  = R = r * A */ \
            __ASM_EMIT("veor            q6, q6")                        /* q6   = 0 */ \
            __ASM_EMIT("vcge.f32        q0, q8, q6")                    /* q0   = [B >= 0] */ \
            __ASM_EMIT("vcge.f32        q1, q9, q6")                    /* q1   = [G >= 0] */ \
            __ASM_EMIT("vcge.f32        q2, q10, q6")                   /* q2   = [R >= 0] */ \
            __ASM_EMIT("vand            q0, q0, q8")                    /* q0   = B & [B >= 0] */ \
            __ASM_EMIT("vand            q1, q1, q9")                    /* q1   = G & [G >= 0] */ \
            __ASM_EMIT("vand            q2, q2, q10")                   /* q2   = R & [R >= 0] */ \
            \
            /* Transpose back */ \
            __ASM_EMIT("vtrn.32         q0, q1") \
            __ASM_EMIT("vtrn.32         q2, q3") \
            __ASM_EMIT("vswp            d4, d1") \
            __ASM_EMIT("vswp            d6, d3") \
            /* q0     = b0 g0 r0 a0 */ \
            /* q1     = b1 g1 r1 a1 */ \
            /* q2     = b2 g2 r2 a2 */ \
            /* q3     = b3 g3 r3 a3 */ \
            \
            __ASM_EMIT("vcvt.u32.f32    q0, q0")                        /* q0 = int(b0 g0 r0 a0) */ \
            __ASM_EMIT("vcvt.u32.f32    q1, q1")                        /* q1 = int(b1 g1 r1 a1) */ \
            __ASM_EMIT("vcvt.u32.f32    q2, q2")                        /* q2 = int(b2 g2 r2 a2) */ \
            __ASM_EMIT("vcvt.u32.f32    q3, q3")                        /* q3 = int(b3 g3 r3 a3) */ \
            __ASM_EMIT("vqmovn.u32      d0, q0") \
            __ASM_EMIT("vqmovn.u32      d1, q1") \
            __ASM_EMIT("vqmovn.u32      d2, q2") \
            __ASM_EMIT("vqmovn.u32      d3, q3") \
            __ASM_EMIT("vqmovn.u16      d0, q0") \
            __ASM_EMIT("vqmovn.u16      d1, q1")

            void rgba_to_bgra32(void *dst, const float *src, size_t count)
            {
                IF_ARCH_ARM( uint32_t fpscr, b_fpscr; )

                ARCH_ARM_ASM
                (
                    // Set-up rounding mode
                    __ASM_EMIT("vmsr            FPSCR, %[bfp]")
                    __ASM_EMIT("orr             %[fp], %[bfp], #0xc00000")
                    __ASM_EMIT("vmrs            %[fp], FPSCR")

                    __ASM_EMIT("vld1.32         {q14-q15}, [%[XC]]")
                    __ASM_EMIT("subs            %[count], #8")
                    __ASM_EMIT("blo             2f")

                    //-----------------------------------------------------------------
                    // 8x blocks
                    __ASM_EMIT("1:")
                    __ASM_EMIT("vldm            %[src]!, {q0-q7}")
                    RGBA_TO_BGRA32_CORE_X8
                    __ASM_EMIT("subs            %[count], #8")
                    __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
                    __ASM_EMIT("bhs             1b")

                    __ASM_EMIT("2:")
                    __ASM_EMIT("adds            %[count], #4")
                    __ASM_EMIT("blt             4f")

                    //-----------------------------------------------------------------
                    // 4x blocks
                    __ASM_EMIT("vldm            %[src]!, {q0-q3}")
                    RGBA_TO_BGRA32_CORE_X4
                    __ASM_EMIT("sub             %[count], #4")
                    __ASM_EMIT("vstm            %[dst]!, {q0}")

                    __ASM_EMIT("4:")
                    __ASM_EMIT("adds            %[count], #4")
                    __ASM_EMIT("bls             12f")

                    //-----------------------------------------------------------------
                    // 1x-3x block
                    __ASM_EMIT("tst             %[count], #2")
                    __ASM_EMIT("beq             6f")
                    __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
                    __ASM_EMIT("6:")
                    __ASM_EMIT("tst             %[count], #1")
                    __ASM_EMIT("beq             8f")
                    __ASM_EMIT("vld1.32         {q2}, [%[src]]")
                    __ASM_EMIT("8:")
                    RGBA_TO_BGRA32_CORE_X4
                    __ASM_EMIT("tst             %[count], #2")
                    __ASM_EMIT("beq             10f")
                    __ASM_EMIT("vst1.32         {d0}, [%[dst]]!")
                    __ASM_EMIT("10:")
                    __ASM_EMIT("tst             %[count], #1")
                    __ASM_EMIT("beq             12f")
                    __ASM_EMIT("vstm            %[dst], {s2}")

                    __ASM_EMIT("12:")

                    // Restore rounding mode
                    __ASM_EMIT("vmrs            %[bfp], FPSCR")

                    : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
                      [fp] "=&r" (fpscr), [bfp] "=&r" (b_fpscr)
                    : [XC] "r" (&RGBA_TO_BGRA32[0])
                    : "cc", "memory",
                      "q0", "q1", "q2", "q3",
                      "q4", "q5", "q6", "q7",
                      "q8", "q9", "q10", "q11",
                      "q12", "q13", "q14", "q15"
                );
            }

        #undef RGBA_TO_BGRA32_CORE_X8
        #undef RGBA_TO_BGRA32_CORE_X4
    } /* namespace neon_d32 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_GRAPHICS_COLORS_H_ */
