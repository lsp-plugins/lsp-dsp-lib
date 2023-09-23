/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_H_
#define PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

#include <private/dsp/arch/generic/graphics/axis.h>
#include <private/dsp/arch/generic/graphics/pixelfmt.h>

namespace lsp
{
    namespace generic
    {
        void fill_rgba(float *dst, float r, float g, float b, float a, size_t count)
        {
            while (count--)
            {
                dst[0]  = r;
                dst[1]  = g;
                dst[2]  = b;
                dst[3]  = a;
                dst    += 4;
            }
        }

        void fill_hsla(float *dst, float h, float s, float l, float a, size_t count)
        {
            while (count--)
            {
                dst[0]  = h;
                dst[1]  = s;
                dst[2]  = l;
                dst[3]  = a;
                dst    += 4;
            }
        }

        static const float HSL_RGB_0_5          = 0.5f;
        static const float HSL_RGB_1_3          = 1.0f / 3.0f;
        static const float HSL_RGB_1_6          = 1.0f / 6.0f;
        static const float HSL_RGB_2_3          = 2.0f / 3.0f;

        void rgba_to_hsla(float *dst, const float *src, size_t count)
        {
            float R, G, B, H, S, L;

            for (size_t i=0; i<count; ++i, dst += 4, src += 4)
            {
                R = src[0];
                G = src[1];
                B = src[2];

                float cmax = (R < G) ? ((B < G) ? G : B) : ((B < R) ? R : B);
                float cmin = (R < G) ? ((B < R) ? B : R) : ((B < G) ? B : G);
                float d = cmax - cmin;

                L = HSL_RGB_0_5 * (cmax + cmin);

                // Calculate hue
                if (d == 0.0f)
                    H = 0.0f;
                else if (R == cmax)
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
                if (L <= 0.5f)
                    S = (L != 0.0f) ? d / L : 0.0f;
                else
                    S = (L != 1.0f) ? d / (1.0f - L) : 0.0f;

                // Normalize hue and saturation
                H  *= HSL_RGB_1_6;
                S  *= HSL_RGB_0_5;

                dst[0]  = H;
                dst[1]  = S;
                dst[2]  = L;
                dst[3]  = src[3];
            }
        }

        void hsla_to_rgba(float *dst, const float *src, size_t count)
        {
            float R, G, B, H, S, L;
            float temp1, temp2, tempr, tempg, tempb, k;

            for (size_t i=0; i<count; ++i, dst += 4, src += 4)
            {
                H   = src[0];
                S   = src[1];
                L   = src[2];

    //            if (S <= 0.0f)
    //            {
    //                dst[0] = L;
    //                dst[1] = L;
    //                dst[2] = L;
    //                dst[3] = src[3];
    //                continue;
    //            }

                //Set the temporary values
                if  (L < HSL_RGB_0_5)
                    temp2 = L + (L * S);
                else
                    temp2 = (L + S) - (L * S);

                temp1 = L + L - temp2;
                tempr = H + HSL_RGB_1_3;
                if (tempr > 1.0f)
                    tempr   -= 1.0f;

                tempg = H;
                tempb = H - HSL_RGB_1_3;

                if (tempb < 0.0f)
                    tempb   += 1.0f;

                k = (temp2 - temp1) * 6.0f;

                /*
                float ktr = temp1 + k * tempr;
                float rtr = temp1 + k * (HSL_RGB_2_3 - tempr);
                float ktg = temp1 + k * tempg;
                float rtg = temp1 + k * (HSL_RGB_2_3 - tempg);
                float ktb = temp1 + k * tempb;
                float rtb = temp1 + k * (HSL_RGB_2_3 - tempb);

                printf("ktr = %.2f, ktg=%.2f, ktb=%.2f\n", ktr, ktg, ktb);
                printf("rtr = %.2f, rtg=%.2f, rtb=%.2f\n", rtr, rtg, rtb);*/

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

                dst[0]  = R;
                dst[1]  = G;
                dst[2]  = B;
                dst[3]  = src[3];
            }
        }
    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_H_ */
