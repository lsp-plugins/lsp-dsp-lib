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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_COLORS_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_COLORS_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        #define FILL4_CORE \
            __ASM_EMIT("ldr         q0, [%[c4]]")       /* v0 = c0 c1 c2 c3 */ \
            __ASM_EMIT("mov         v1.16b, v0.16b")    /* v1 = c0 c1 c2 c3 */ \
            \
            /* 16x blocks */ \
            __ASM_EMIT("subs        %[count], %[count], #16") \
            __ASM_EMIT("b.lo        2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x20]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x40]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x60]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x80]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0xa0]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0xc0]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0xe0]") \
            __ASM_EMIT("subs        %[count], %[count], #16") \
            __ASM_EMIT("add         %[dst], %[dst], #0x100") \
            __ASM_EMIT("b.hs         1b") \
            /* 8x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], %[count], #8") \
            __ASM_EMIT("b.lt        4f") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x20]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x40]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x60]") \
            __ASM_EMIT("sub         %[count], %[count], #8") \
            __ASM_EMIT("add         %[dst], %[dst], #0x80") \
            /* 4x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], %[count], #4") \
            __ASM_EMIT("b.lt        6f") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x20]") \
            __ASM_EMIT("sub         %[count], %[count], #4") \
            __ASM_EMIT("add         %[dst], %[dst], #0x40") \
            /* 2x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], %[count], #2") \
            __ASM_EMIT("b.lt        8f") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]") \
            __ASM_EMIT("sub         %[count], %[count], #2") \
            __ASM_EMIT("add         %[dst], %[dst], #0x20") \
            /* 1x block */ \
            __ASM_EMIT("8:") \
            __ASM_EMIT("adds        %[count], %[count], #1") \
            __ASM_EMIT("b.lt        10f") \
            __ASM_EMIT("str         q0, [%[dst], 0x00]") \
            __ASM_EMIT("10:")

        void fill_rgba(float *dst, float r, float g, float b, float a, size_t count)
        {
            IF_ARCH_AARCH64(
                float c4[4] __lsp_aligned16;
                c4[0] = r;
                c4[1] = g;
                c4[2] = b;
                c4[3] = a;
            )

            ARCH_AARCH64_ASM
            (
                FILL4_CORE
                : [dst] "+r" (dst), [count] "+r" (count)
                : [c4] "r" (&c4[0])
                : "cc", "memory",
                  "v0", "v1"
            );
        }

        void fill_hsla(float *dst, float h, float s, float l, float a, size_t count)
        {
            IF_ARCH_AARCH64(
                float c4[4] __lsp_aligned16;
                c4[0] = h;
                c4[1] = s;
                c4[2] = l;
                c4[3] = a;
            );

            ARCH_AARCH64_ASM
            (
                FILL4_CORE
                : [dst] "+r" (dst), [count] "+r" (count)
                : [c4] "r" (&c4[0])
                : "cc", "memory",
                  "v0", "v1"
            );
        }

        #undef FILL4_CORE

        #define HSLA_TO_RGBA_CORE   \
            /*  v10 = 1/2, v11 = 1/3, v12 = 1, v13 = 6, v14 = 1/6, v15 = 2/3 */ \
            /*  v0 = h0 h1 h2 h3 = H */ \
            /*  v1 = s0 s1 s2 s3 = S */ \
            /*  v2 = l0 l1 l2 l3 = L */ \
            /*  v3 = a0 a1 a2 a3 = A */ \
            /*  Calc temp1 (T1) and temp2 (T2) */ \
            __ASM_EMIT("fadd            v4.4s, v2.4s, v1.4s")       /* v4 = L+S */ \
            __ASM_EMIT("fadd            v9.4s, v2.4s, v2.4s")       /* v9 = L+L */ \
            __ASM_EMIT("fmul            v5.4s, v2.4s, v1.4s")       /* v5 = L*S */ \
            __ASM_EMIT("fsub            v4.4s, v4.4s, v5.4s")       /* v4 = L+S - L*S */ \
            __ASM_EMIT("fadd            v7.4s, v2.4s, v5.4s")       /* v7 = L + L*S */ \
            __ASM_EMIT("fcmgt           v8.4s, v10.4s, v2.4s")      /* v8 = [L < 0.5] */ \
            __ASM_EMIT("bit             v4.16b, v7.16b, v8.16b")    /* v4 = T2 = ((L+L*S) & [L<0.5]) | ((L+S-L*S) & [L>=0.5]) */ \
            __ASM_EMIT("mov             v1.16b, v0.16b")            /* v1 = TG = H */ \
            __ASM_EMIT("fsub            v5.4s, v9.4s, v4.4s")       /* v5 = T1 = L+L-T2 */ \
            __ASM_EMIT("fadd            v0.4s, v0.4s, v11.4s")      /* v0 = H + 1/3 */ \
            __ASM_EMIT("fsub            v2.4s, v1.4s, v11.4s")      /* v2 = H - 1/3 */ \
            __ASM_EMIT("fcmgt           v9.4s, v0.4s, v12.4s")      /* v9 = [H+1/3 > 1] */ \
            __ASM_EMIT("fsub            v6.4s, v0.4s, v12.4s")      /* v6 = H + 1/3 - 1 */ \
            __ASM_EMIT("fadd            v7.4s, v2.4s, v12.4s")      /* v7 = H - 1/3 + 1 */ \
            __ASM_EMIT("fcmlt           v8.4s, v2.4s, #0.0")        /* v8 = [H - 1/3 < 0] */ \
            __ASM_EMIT("bit             v0.16b, v6.16b, v9.16b")    /* v0 = TR = ((H+1/3) & [H+1/3 <= 1]) | ((H+1/3-1) & [H+1/3 > 1]) */ \
            __ASM_EMIT("bit             v2.16b, v7.16b, v8.16b")    /* v2 = TB = ((H-1/3) & [H-1/3 >= 0]) | ((H-1/3+1) & [H-1/3 < 0]) */ \
            __ASM_EMIT("fsub            v6.4s, v4.4s, v5.4s")       /* v6 = T2 - T1 */ \
            __ASM_EMIT("fmul            v6.4s, v6.4s, v13.4s")      /* v6 = K = (T2 - T1)*6 */ \
            /* v0 = TR */ \
            /* v1 = TG */ \
            /* v2 = TB */ \
            /* v3 = A */ \
            /* v4 = T2 */ \
            /* v5 = T1 */ \
            /* v6 = K */ \
            /* Process red color */ \
            __ASM_EMIT("mov             v7.16b, v5.16b")            /* v7 = T1 */ \
            __ASM_EMIT("mov             v8.16b, v5.16b")            /* v8 = T1 */ \
            __ASM_EMIT("fsub            v9.4s, v15.4s, v0.4s")      /* v9 = 2/3 - TR */ \
            __ASM_EMIT("fmla            v7.4s, v6.4s, v0.4s")       /* v7 = KTR = T1 + K*TR */ \
            __ASM_EMIT("fmla            v8.4s, v6.4s, v9.4s")       /* v8 = RTR = T1 + K*(2/3-TR) */ \
            __ASM_EMIT("fcmgt           v9.4s, v14.4s, v0.4s")      /* v9 = [TR < 1/6] */ \
            __ASM_EMIT("bif             v7.16b, v4.16b, v9.16b")    /* v7 = (KTR & [TR < 1/6]) | ((T2) & (TR >= 1/6)) */ \
            __ASM_EMIT("fcmgt           v9.4s, v15.4s, v0.4s")      /* v9 = [TR < 2/3] */ \
            __ASM_EMIT("bif             v8.16b, v5.16b, v9.16b")    /* v8 = (RTR & [TR < 2/3]) | ((T1) & (TR >= 2/3)) */ \
            __ASM_EMIT("fcmgt           v9.4s, v10.4s, v0.4s")      /* v9 = [TR < 1/2] */ \
            __ASM_EMIT("and             v0.16b, v7.16b, v9.16b")    /* v0 = (KTR & [TR < 1/6] & [TR < 1/2]) | ((T2) & (TR >= 1/6) & [TR < 1/2]) */ \
            __ASM_EMIT("bif             v0.16b, v8.16b, v9.16b")    /* v0 = (KTR & [TR < 1/6] & [TR < 1/2]) | ((T2) & (TR >= 1/6) & [TR < 1/2]) | (RTR & [TR < 2/3] & [TR >= 1/2]) | ((T1) & (TR >= 2/3) & [TR >= 1/2]) */ \
            /* Process green color */ \
            __ASM_EMIT("mov             v7.16b, v5.16b")            /* v7 = T1 */ \
            __ASM_EMIT("mov             v8.16b, v5.16b")            /* v8 = T1 */ \
            __ASM_EMIT("fsub            v9.4s, v15.4s, v1.4s")      /* v9 = 2/3 - TG */ \
            __ASM_EMIT("fmla            v7.4s, v6.4s, v1.4s")       /* v7 = KTG = T1 + K*TG */ \
            __ASM_EMIT("fmla            v8.4s, v6.4s, v9.4s")       /* v8 = RTG = T1 + K*(2/3-TG) */ \
            __ASM_EMIT("fcmgt           v9.4s, v14.4s, v1.4s")      /* v9 = [TG < 1/6] */ \
            __ASM_EMIT("bif             v7.16b, v4.16b, v9.16b")    /* v7 = (KTG & [TG < 1/6]) | ((T2) & (TG >= 1/6)) */ \
            __ASM_EMIT("fcmgt           v9.4s, v15.4s, v1.4s")      /* v9 = [TG < 2/3] */ \
            __ASM_EMIT("bif             v8.16b, v5.16b, v9.16b")    /* v8 = (RTG & [TG < 2/3]) | ((T1) & (TG >= 2/3)) */ \
            __ASM_EMIT("fcmgt           v9.4s, v10.4s, v1.4s")      /* v9 = [TG < 1/2] */ \
            __ASM_EMIT("and             v1.16b, v7.16b, v9.16b")    /* v1 = (KTG & [TG < 1/6] & [TG < 1/2]) | ((T2) & (TG >= 1/6) & [TG < 1/2]) */ \
            __ASM_EMIT("bif             v1.16b, v8.16b, v9.16b")    /* v1 = (KTG & [TG < 1/6] & [TG < 1/2]) | ((T2) & (TG >= 1/6) & [TG < 1/2]) | (RTG & [TG < 2/3] & [TG >= 1/2]) | ((T1) & (TG >= 2/3) & [TG >= 1/2]) */ \
            /* Process blue color */ \
            __ASM_EMIT("mov             v7.16b, v5.16b")            /* v7 = T1 */ \
            __ASM_EMIT("mov             v8.16b, v5.16b")            /* v8 = T1 */ \
            __ASM_EMIT("fsub            v9.4s, v15.4s, v2.4s")      /* v9 = 2/3 - TB */ \
            __ASM_EMIT("fmla            v7.4s, v6.4s, v2.4s")       /* v7 = KTB = T1 + K*TB */ \
            __ASM_EMIT("fmla            v8.4s, v6.4s, v9.4s")       /* v8 = RTB = T1 + K*(2/3-TB) */ \
            __ASM_EMIT("fcmgt           v9.4s, v14.4s, v2.4s")      /* v9 = [TB < 1/6] */ \
            __ASM_EMIT("bif             v7.16b, v4.16b, v9.16b")    /* v7 = (KTB & [TB < 1/6]) | ((T2) & (TB >= 1/6)) */ \
            __ASM_EMIT("fcmgt           v9.4s, v15.4s, v2.4s")      /* v9 = [TB < 2/3] */ \
            __ASM_EMIT("bif             v8.16b, v5.16b, v9.16b")    /* v8 = (RTB & [TB < 2/3]) | ((T1) & (TB >= 2/3)) */ \
            __ASM_EMIT("fcmgt           v9.4s, v10.4s, v2.4s")      /* v9 = [TB < 1/2] */ \
            __ASM_EMIT("and             v2.16b, v7.16b, v9.16b")    /* v2 = (KTB & [TB < 1/6] & [TB < 1/2]) | ((T2) & (TB >= 1/6) & [TB < 1/2]) */ \
            __ASM_EMIT("bif             v2.16b, v8.16b, v9.16b")    /* v2 = (KTB & [TB < 1/6] & [TB < 1/2]) | ((T2) & (TB >= 1/6) & [TB < 1/2]) | (RTB & [TB < 2/3] & [TB >= 1/2]) | ((T1) & (TB >= 2/3) & [TB >= 1/2]) */ \
            /*  v0 = R */ \
            /*  v1 = G */ \
            /*  v2 = B */ \
            /*  v3 = A */ \

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

        IF_ARCH_AARCH64(
            static const float HSL_RGB[] =
            {
                LSP_DSP_VEC4(0.5f),                // 1/2
                LSP_DSP_VEC4(0.333333333333f),     // 1/3
                LSP_DSP_VEC4(1.0f),                // 1
                LSP_DSP_VEC4(6.0f),                // 6
                LSP_DSP_VEC4(0.166666666667f),     // 1/6
                LSP_DSP_VEC4(0.666666666667f)      // 2/3
            };
        )

        void hsla_to_rgba(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ldp             q10, q11, [%[XC], 0x00]")   // v10 = 1/2, v11 = 1/3
                __ASM_EMIT("ldp             q12, q13, [%[XC], 0x20]")   // v12 = 1, v13 = 6
                __ASM_EMIT("ldp             q14, q15, [%[XC], 0x40]")   // v14 = 1/6, v15 = 2/3
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")

                //-----------------------------------------------------------------
                // 4x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[src]]")    // v0 = h0 h1 h2 h3, v1 = s0 s1 s2 s3, v2 = l0 l1 l2 l3, v3 = a0 a1 a2 a3

                HSLA_TO_RGBA_CORE

                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x40")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")

                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            10f")

                //-----------------------------------------------------------------
                // 1x-3x block
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            4f")
                __ASM_EMIT("ld4             {v0.2s, v1.2s, v2.2s, v3.2s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("4:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld4             {v0.s, v1.s, v2.s, v3.s}[2], [%[src]]")
                __ASM_EMIT("6:")

                HSLA_TO_RGBA_CORE

                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("st4             {v0.2s, v1.2s, v2.2s, v3.2s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("8:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("st4             {v0.s, v1.s, v2.s, v3.s}[2], [%[dst]]")

                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [XC] "r" (&HSL_RGB[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15"
            );
        }

        #undef HSLA_TO_RGBA_CORE

        #define RGBA_TO_HSLA_CORE \
            /*  v0   = R */ \
            /*  v1   = G */ \
            /*  v2   = B */ \
            /*  v3   = A */ \
            /*  v10  = 4, v11 = 2, v12 = 6, v13 = 1, v14 = 1/2, v15 = 1/6 */ \
            __ASM_EMIT("fmax            v4.4s, v0.4s, v1.4s")       /* v4   = max(R, G) */ \
            __ASM_EMIT("fmin            v5.4s, v0.4s, v1.4s")       /* v5   = min(R, G) */ \
            __ASM_EMIT("fmax            v4.4s, v4.4s, v2.4s")       /* v4   = CMAX = max(R, G, B) */ \
            __ASM_EMIT("fmin            v5.4s, v5.4s, v2.4s")       /* v5   = CMIN = min(R, G, B) */ \
            __ASM_EMIT("fsub            v6.4s, v4.4s, v5.4s")       /* v6   = D = CMAX - CMIN */ \
            __ASM_EMIT("fadd            v5.4s, v4.4s, v5.4s")       /* v5   = CMAX + CMIN */ \
            \
            __ASM_EMIT("frecpe          v9.4s, v6.4s")              /* v9   = RD */ \
            __ASM_EMIT("frecps          v8.4s, v9.4s, v6.4s")       /* v8   = (2 - RD*D) */ \
            __ASM_EMIT("fmul            v9.4s, v8.4s, v9.4s")       /* v9   = d' = RD * (2 - RD*D) */ \
            __ASM_EMIT("frecps          v8.4s, v9.4s, v6.4s")       /* v8   = (2 - RD*d') */ \
            __ASM_EMIT("fmul            v9.4s, v8.4s, v9.4s")       /* v9   = 1/D = d' * (2 - RD*d') */  \
            \
            /* v0  = R */ \
            /* v1  = G */ \
            /* v2  = B */ \
            /* v3  = A */ \
            /* v4  = CMAX */ \
            /* v5  = CMAX + CMIN */ \
            /* v6  = D */ \
            /* v9  = 1/D */ \
            __ASM_EMIT("fsub            v7.4s, v2.4s, v0.4s")       /* v7   = B-R */ \
            __ASM_EMIT("fsub            v8.4s, v0.4s, v1.4s")       /* v8   = R-G */ \
            __ASM_EMIT("fsub            v16.4s, v1.4s, v2.4s")      /* v16  = G-B */ \
            __ASM_EMIT("fmul            v7.4s, v7.4s, v9.4s")       /* v7   = (B-R)/D */ \
            __ASM_EMIT("fmul            v16.4s, v16.4s, v9.4s")     /* v16  = (G-B)/D */ \
            __ASM_EMIT("fmul            v8.4s, v8.4s, v9.4s")       /* v8   = (R-G)/D */ \
            __ASM_EMIT("fcmlt           v17.4s, v16.4s, #0.0")      /* v17  = [(G-B)/D < 0] */ \
            __ASM_EMIT("fadd            v7.4s, v7.4s, v11.4s")      /* v7   = HG = (B-R)/D + 2 */ \
            __ASM_EMIT("and             v17.16b, v17.16b, v12.16b") /* v17  = [(G-B)/D < 0] & 6 */ \
            __ASM_EMIT("fadd            v8.4s, v8.4s, v10.4s")      /* v8   = HB = (R-G)/D + 4 */ \
            __ASM_EMIT("fadd            v9.4s, v16.4s, v17.4s")     /* v9   = HR = (G-B)/D + [(G-B)/D < 0] & 6 */ \
            /* v0  = R */ \
            /* v1  = G */ \
            /* v2  = B */ \
            /* v3  = A */ \
            /* v4  = CMAX */ \
            /* v5  = CMAX + CMIN */ \
            /* v6  = D */ \
            /* v7  = HG */ \
            /* v8  = HB */ \
            /* v9  = HR */ \
            __ASM_EMIT("fcmeq           v1.4s, v1.4s, v4.4s")       /* v1 = [G == CMAX] */ \
            __ASM_EMIT("fcmeq           v0.4s, v0.4s, v4.4s")       /* v0 = [R == CMAX] */ \
            __ASM_EMIT("bsl             v1.16b, v7.16b, v8.16b")    /* v1 = (HG & [G == CMAX]) | (HB & [G != CMAX]) */ \
            __ASM_EMIT("bsl             v0.16b, v9.16b, v1.16b")    /* v0 = (HR & [R == CMAX]) | (HG & [G == CMAX] & [R != CMAX]) | (HB & [G != CMAX] & [R != CMAX]) */ \
            __ASM_EMIT("fcmeq           v4.4s, v6.4s, #0.0")        /* v4 = [ D == 0 ] */ \
            __ASM_EMIT("fmul            v2.4s, v5.4s, v14.4s")      /* v2 = L = (CMAX + CMIN) * 0.5 */ \
            __ASM_EMIT("fsub            v1.4s, v13.4s, v2.4s")      /* v1 = X = 1 - L */ \
            __ASM_EMIT("bic             v0.16b, v0.16b, v4.16b")    /* v0 = H = [D != 0] & ((HR & [R == CMAX]) | (HG & [G == CMAX] & [R != CMAX]) | (HB & [G != CMAX] & [R != CMAX])) */ \
            \
            __ASM_EMIT("frecpe          v5.4s, v2.4s")              /* v5 = RL */ \
            __ASM_EMIT("frecpe          v9.4s, v1.4s")              /* v9 = RX */ \
            __ASM_EMIT("frecps          v4.4s, v5.4s, v2.4s")       /* v4 = (2 - RL*L) */ \
            __ASM_EMIT("frecps          v8.4s, v9.4s, v1.4s")       /* v8 = (2 - RX*X) */ \
            __ASM_EMIT("fmul            v5.4s, v4.4s, v5.4s")       /* v5 = l' = L * (2 - RL*L) */ \
            __ASM_EMIT("fmul            v9.4s, v8.4s, v9.4s")       /* v9 = x' = X * (2 - RX*X) */ \
            __ASM_EMIT("frecps          v4.4s, v5.4s, v2.4s")       /* v4 = (2 - RL*l') */ \
            __ASM_EMIT("frecps          v8.4s, v9.4s, v1.4s")       /* v8 = (2 - RX*X') */ \
            __ASM_EMIT("fmul            v5.4s, v4.4s, v5.4s")       /* v5 = 1/L = l' * (2 - RL*l') */  \
            __ASM_EMIT("fmul            v9.4s, v8.4s, v9.4s")       /* v9 = 1/X = x' * (2 - RX*x') */  \
            __ASM_EMIT("fmul            v1.4s, v6.4s, v5.4s")       /* v1 = D/L */ \
            __ASM_EMIT("fmul            v4.4s, v6.4s, v9.4s")       /* v4 = D/X */ \
            __ASM_EMIT("eor             v5.16b, v5.16b, v5.16b")    /* v5 = 0 */ \
            \
            __ASM_EMIT("fcmge           v6.4s, v14.4s, v2.4s")      /* v6 = [L <= 0.5] */ \
            __ASM_EMIT("fcmeq           v7.4s, v1.4s, v5.4s")       /* v7 = [L == 0] */ \
            __ASM_EMIT("fcmgt           v8.4s, v2.4s, v14.4s")      /* v8 = [L > 0.5] */ \
            __ASM_EMIT("bit             v6.16b, v5.16b, v7.16b")    /* v6 = [L <= 0.5] & [L != 0] */ \
            __ASM_EMIT("and             v8.16b, v8.16b, v4.16b")    /* v8 = D/X & [L > 0.5] */ \
            __ASM_EMIT("and             v6.16b, v6.16b, v1.16b")    /* v6 = D/L & [L <= 0.5] & [L != 0] */ \
            __ASM_EMIT("orr             v1.16b, v8.16b, v6.16b")    /* v1 = S = (D/L & [L <= 0.5] & [L != 0]) | (D/X & [L > 0.5]) */ \
            __ASM_EMIT("fmul            v0.4s, v0.4s, v15.4s")      /* v0 = H * 1/6 */ \
            __ASM_EMIT("fmul            v1.4s, v1.4s, v14.4s")      /* v1 = S * 1/2 */ \

        /*
            float cmax = (R < G) ? ((B < G) ? G : B) : ((B < R) ? R : B);
            float cmin = (R < G) ? ((B < R) ? B : R) : ((B < G) ? B : G);
            float d = cmax - cmin;

            H = 0.0f;
            S = 0.0f;
            L = HSL_RGB_0_5 * (cmax + cmin);

            // Calculate hue
            HR = ((G - B) / d) + ((H < 0.0f) ? 6.0f : 0.0f);
            HG = (B - R) / d + 2.0f;
            HB = (R - G) / d + 4.0f;
            if (D == 0)
                H = 0;
            else if (R == cmax)
                H = HR;
            else if (G == cmax)
                H = HG;
            else
                H = HB;

            // Calculate saturation
            if (L < 0.5f)
                S = (L != 0.0f) ? d / L : 0.0f;
            else
                S = (L != 1.0f) ? d / (1.0f - L) : 0.0f;

            // Normalize hue and saturation
            H  *= HSL_RGB_1_6;
            S  *= HSL_RGB_0_5;
         */

        IF_ARCH_AARCH64(
            static const float RGB_HSL[] =
            {
                LSP_DSP_VEC4(4.0f),
                LSP_DSP_VEC4(2.0f),
                LSP_DSP_VEC4(6.0f),
                LSP_DSP_VEC4(1.0f),
                LSP_DSP_VEC4(0.5f),
                LSP_DSP_VEC4(0.166666666667f)      // 1/6
            };
        )

        void rgba_to_hsla(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ldp             q10, q11, [%[XC], 0x00]")   // v10 = 4, v11 = 2
                __ASM_EMIT("ldp             q12, q13, [%[XC], 0x20]")   // v12 = 6, v13 = 1
                __ASM_EMIT("ldp             q14, q15, [%[XC], 0x40]")   // v14 = 1/2, v15 = 1/6
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")

                //-----------------------------------------------------------------
                // 4x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[src]]")    // v0 = r0 r1 r2 r3, v1 = g0 g1 g2 g3, v2 = b0 b1 b2 b3, v3 = a0 a1 a2 a3

                RGBA_TO_HSLA_CORE

                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x40")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")

                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            10f")

                //-----------------------------------------------------------------
                // 1x-3x block
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            4f")
                __ASM_EMIT("ld4             {v0.2s, v1.2s, v2.2s, v3.2s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("4:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld4             {v0.s, v1.s, v2.s, v3.s}[2], [%[src]]")
                __ASM_EMIT("6:")

                RGBA_TO_HSLA_CORE

                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("st4             {v0.2s, v1.2s, v2.2s, v3.2s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("8:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("st4             {v0.s, v1.s, v2.s, v3.s}[2], [%[dst]]")

                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [XC] "r" (&RGB_HSL[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17"
            );
        }

        #undef RGBA_TO_HSLA_CORE

        #define RGBA_TO_BGRA32_CORE_X8 \
            /* v0   = r1    */ \
            /* v1   = g1    */ \
            /* v2   = b1    */ \
            /* v3   = a1    */ \
            /* v4   = r2    */ \
            /* v5   = g2    */ \
            /* v6   = b2    */ \
            /* v7   = a2    */ \
            /* v14  = 255.0 */ \
            /* v15  = 255.0 */ \
            __ASM_EMIT("fmul            v3.4s, v3.4s, v14.4s")          /* v3   = a * 255 */ \
            __ASM_EMIT("fmul            v7.4s, v7.4s, v15.4s") \
            __ASM_EMIT("fsub            v3.4s, v14.4s, v3.4s")          /* v3   = A = 255 - a*255 */ \
            __ASM_EMIT("fsub            v7.4s, v15.4s, v7.4s") \
            __ASM_EMIT("fmul            v8.4s, v2.4s, v3.4s")           /* v8   = B = b * A */ \
            __ASM_EMIT("fmul            v11.4s, v6.4s, v7.4s") \
            __ASM_EMIT("fmul            v9.4s, v1.4s, v3.4s")           /* v9   = G = g * A */ \
            __ASM_EMIT("fmul            v12.4s, v5.4s, v7.4s") \
            __ASM_EMIT("fmul            v10.4s, v0.4s, v3.4s")          /* v10  = R = r * A */ \
            __ASM_EMIT("fmul            v13.4s, v4.4s, v7.4s") \
            __ASM_EMIT("fcmge           v0.4s, v8.4s, #0.0")            /* v0   = [ B >= 0 ] */ \
            __ASM_EMIT("fcmge           v4.4s, v11.4s, #0.0") \
            __ASM_EMIT("fcmge           v1.4s, v9.4s, #0.0")            /* v1   = [ G >= 0 ] */ \
            __ASM_EMIT("fcmge           v5.4s, v12.4s, #0.0") \
            __ASM_EMIT("fcmge           v2.4s, v10.4s, #0.0")           /* v2   = [ R >= 0 ] */ \
            __ASM_EMIT("fcmge           v6.4s, v13.4s, #0.0") \
            __ASM_EMIT("and             v0.16b, v0.16b, v8.16b")        /* v0   = B & [ B >= 0 ] */ \
            __ASM_EMIT("and             v4.16b, v4.16b, v11.16b") \
            __ASM_EMIT("and             v1.16b, v1.16b, v9.16b")        /* v1   = G & [ G >= 0 ] */ \
            __ASM_EMIT("and             v5.16b, v5.16b, v12.16b") \
            __ASM_EMIT("and             v2.16b, v2.16b, v10.16b")       /* v2   = R & [ R >= 0 ] */ \
            __ASM_EMIT("and             v6.16b, v6.16b, v13.16b") \
            \
            /* Transpose back (first part) */ \
            __ASM_EMIT("trn1            v8.4s, v0.4s, v1.4s") \
            __ASM_EMIT("trn1            v10.4s, v2.4s, v3.4s") \
            __ASM_EMIT("trn2            v9.4s, v0.4s, v1.4s") \
            __ASM_EMIT("trn2            v11.4s, v2.4s, v3.4s") \
            __ASM_EMIT("trn1            v0.2d, v8.2d, v10.2d") \
            __ASM_EMIT("trn1            v1.2d, v9.2d, v11.2d") \
            __ASM_EMIT("trn2            v2.2d, v8.2d, v10.2d") \
            __ASM_EMIT("trn2            v3.2d, v9.2d, v11.2d") \
            /* Transpose back (second part) */ \
            __ASM_EMIT("trn1            v8.4s, v4.4s, v5.4s") \
            __ASM_EMIT("trn1            v10.4s, v6.4s, v7.4s") \
            __ASM_EMIT("trn2            v9.4s, v4.4s, v5.4s") \
            __ASM_EMIT("trn2            v11.4s, v6.4s, v7.4s") \
            __ASM_EMIT("trn1            v4.2d, v8.2d, v10.2d") \
            __ASM_EMIT("trn1            v5.2d, v9.2d, v11.2d") \
            __ASM_EMIT("trn2            v6.2d, v8.2d, v10.2d") \
            __ASM_EMIT("trn2            v7.2d, v9.2d, v11.2d") \
            /* v0   = b0 g0 r0 a0   */ \
            /* v1   = b1 g1 r1 a1   */ \
            /* v2   = b2 g2 r2 a2   */ \
            /* v3   = b3 g3 r3 a3   */ \
            /* v4   = b4 g4 r4 a4   */ \
            /* v5   = b5 g5 r5 a5   */ \
            /* v6   = b6 g6 r6 a6   */ \
            /* v7   = b7 g7 r7 a7   */ \
            \
            __ASM_EMIT("fcvtzu          v0.4s, v0.4s")                  /* v0 = i32(b0) i32(g0) i32(r0) i32(a0) */ \
            __ASM_EMIT("fcvtzu          v1.4s, v1.4s") \
            __ASM_EMIT("fcvtzu          v2.4s, v2.4s") \
            __ASM_EMIT("fcvtzu          v3.4s, v3.4s") \
            __ASM_EMIT("fcvtzu          v4.4s, v4.4s")                  /* v4 = i32(b4) i32(g4) i32(r4) i32(a4) */ \
            __ASM_EMIT("fcvtzu          v5.4s, v5.4s") \
            __ASM_EMIT("fcvtzu          v6.4s, v6.4s") \
            __ASM_EMIT("fcvtzu          v7.4s, v7.4s") \
            __ASM_EMIT("sqxtun          v8.4h, v0.4s")                  /* v8 = i16(b0) i16(g0) i16(r0) i16(a0) 0 0 0 0 */ \
            __ASM_EMIT("sqxtun          v9.4h, v2.4s")                  /* v9 = i16(b2) i16(g2) i16(r2) i16(a2) 0 0 0 0 */ \
            __ASM_EMIT("sqxtun2         v8.8h, v1.4s")                  /* v8 = i16(b0) i16(g0) i16(r0) i16(a0) i16(b1) i16(g1) i16(r1) i16(a1) */ \
            __ASM_EMIT("sqxtun2         v9.8h, v3.4s")                  /* v8 = i16(b2) i16(g2) i16(r2) i16(a2) i16(b3) i16(g3) i16(r3) i16(a3) */ \
            __ASM_EMIT("sqxtun          v10.4h, v4.4s") \
            __ASM_EMIT("sqxtun          v11.4h, v6.4s") \
            __ASM_EMIT("sqxtun2         v10.8h, v5.4s") \
            __ASM_EMIT("sqxtun2         v11.8h, v7.4s") \
            __ASM_EMIT("sqxtun          v0.8b, v8.8h")                  /* v0 = b0 g0 r0 a0 b1 g1 r1 a1 0 0 0 0 0 0 0 0 */ \
            __ASM_EMIT("sqxtun          v1.8b, v10.8h") \
            __ASM_EMIT("sqxtun2         v0.16b, v9.8h")                 /* v0 = b0 g0 r0 a0 b1 g1 r1 a1 b2 g2 r2 a2 b3 g3 r3 a3 */ \
            __ASM_EMIT("sqxtun2         v1.16b, v11.8h")

        #define RGBA_TO_BGRA32_CORE_X4 \
            /* v0   = r     */ \
            /* v1   = g     */ \
            /* v2   = b     */ \
            /* v3   = a     */ \
            /* v14  = 255.0 */ \
            /* v15  = 255.0 */ \
            __ASM_EMIT("fmul            v3.4s, v3.4s, v14.4s")          /* v3   = a * 255 */ \
            __ASM_EMIT("fsub            v3.4s, v15.4s, v3.4s")          /* v3   = A = 255 - a*255 */ \
            __ASM_EMIT("fmul            v8.4s, v2.4s, v3.4s")           /* v8   = B = b * A */ \
            __ASM_EMIT("fmul            v9.4s, v1.4s, v3.4s")           /* v9   = G = g * A */ \
            __ASM_EMIT("fmul            v10.4s, v0.4s, v3.4s")          /* v10  = R = r * A */ \
            __ASM_EMIT("fcmge           v0.4s, v8.4s, #0.0")            /* v0   = [ B >= 0 ] */ \
            __ASM_EMIT("fcmge           v1.4s, v9.4s, #0.0")            /* v1   = [ G >= 0 ] */ \
            __ASM_EMIT("fcmge           v2.4s, v10.4s, #0.0")           /* v2   = [ R >= 0 ] */ \
            __ASM_EMIT("and             v0.16b, v0.16b, v8.16b")        /* v0   = B & [ B >= 0 ] */ \
            __ASM_EMIT("and             v1.16b, v1.16b, v9.16b")        /* v1   = G & [ G >= 0 ] */ \
            __ASM_EMIT("and             v2.16b, v2.16b, v10.16b")       /* v2   = R & [ R >= 0 ] */ \
            \
            /* v0   = b0 b1 b2 b3   */ \
            /* v1   = g0 g1 g2 g3   */ \
            /* v2   = r0 r1 r2 r3   */ \
            /* v3   = a0 a1 a2 a3   */ \
            __ASM_EMIT("trn1            v8.4s, v0.4s, v1.4s") \
            __ASM_EMIT("trn1            v10.4s, v2.4s, v3.4s") \
            __ASM_EMIT("trn2            v9.4s, v0.4s, v1.4s") \
            __ASM_EMIT("trn2            v11.4s, v2.4s, v3.4s") \
            /* v8   = b0 g0 b2 g2   */ \
            /* v9   = b1 g1 b3 g3   */ \
            /* v10  = r0 a0 r2 a2   */ \
            /* v11  = r1 a1 r3 a3   */ \
            __ASM_EMIT("trn1            v0.2d, v8.2d, v10.2d") \
            __ASM_EMIT("trn1            v1.2d, v9.2d, v11.2d") \
            __ASM_EMIT("trn2            v2.2d, v8.2d, v10.2d") \
            __ASM_EMIT("trn2            v3.2d, v9.2d, v11.2d") \
            /* v0   = b0 g0 r0 a0   */ \
            /* v1   = b1 g1 r1 a1   */ \
            /* v2   = b2 g2 r2 a2   */ \
            /* v3   = b3 g3 r3 a3   */ \
            \
            __ASM_EMIT("fcvtzu          v0.4s, v0.4s")                  /* v0 = i32(b0) i32(g0) i32(r0) i32(a0) */ \
            __ASM_EMIT("fcvtzu          v1.4s, v1.4s") \
            __ASM_EMIT("fcvtzu          v2.4s, v2.4s") \
            __ASM_EMIT("fcvtzu          v3.4s, v3.4s") \
            __ASM_EMIT("sqxtun          v8.4h, v0.4s")                  /* v8 = i16(b0) i16(g0) i16(r0) i16(a0) 0 0 0 0 */ \
            __ASM_EMIT("sqxtun          v9.4h, v2.4s")                  /* v9 = i16(b2) i16(g2) i16(r2) i16(a2) 0 0 0 0 */ \
            __ASM_EMIT("sqxtun2         v8.8h, v1.4s")                  /* v8 = i16(b0) i16(g0) i16(r0) i16(a0) i16(b1) i16(g1) i16(r1) i16(a1) */ \
            __ASM_EMIT("sqxtun2         v9.8h, v3.4s")                  /* v8 = i16(b2) i16(g2) i16(r2) i16(a2) i16(b3) i16(g3) i16(r3) i16(a3) */ \
            __ASM_EMIT("sqxtun          v0.8b, v8.8h")                  /* v0 = b0 g0 r0 a0 b1 g1 r1 a1 0 0 0 0 0 0 0 0 */ \
            __ASM_EMIT("sqxtun2         v0.16b, v9.8h")                 /* v0 = b0 g0 r0 a0 b1 g1 r1 a1 b2 g2 r2 a2 b3 g3 r3 a3 */

        IF_ARCH_AARCH64(
            static const float RGBA_TO_BGRA32[] =
            {
                LSP_DSP_VEC4(255.0f),
                LSP_DSP_VEC4(255.0f)
            };
        )

        void rgba_to_bgra32(void *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ldp             q14, q15, [%[XC]]")         // v14 = 255.0, v15 = 255.0
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")

                //-----------------------------------------------------------------
                // 8x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x40")
                __ASM_EMIT("ld4             {v4.4s, v5.4s, v6.4s, v7.4s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x40")
                RGBA_TO_BGRA32_CORE_X8
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")

                //-----------------------------------------------------------------
                // 4x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ld4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[src]]")
                RGBA_TO_BGRA32_CORE_X4
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x40")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")

                //-----------------------------------------------------------------
                // 1x-3x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.ls            12f")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld4             {v0.2s, v1.2s, v2.2s, v3.2s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld4             {v0.s, v1.s, v2.s, v3.s}[2], [%[src]]")
                __ASM_EMIT("8:")
                RGBA_TO_BGRA32_CORE_X4
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("beq             10f")
                __ASM_EMIT("st1             {v0.d}[0], [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x08")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("beq             12f")
                __ASM_EMIT("st1             {v0.s}[2], [%[dst]]")
                // End
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [XC] "r" (&RGBA_TO_BGRA32[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15"
            );
        }

        #undef RGBA_TO_BGRA32_CORE_X8
        #undef RGBA_TO_BGRA32_CORE_X4
    } /* namespace asimd */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_COLORS_H_ */
