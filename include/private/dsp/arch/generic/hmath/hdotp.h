/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_HMATH_HDOTP_H_
#define PRIVATE_DSP_ARCH_GENERIC_HMATH_HDOTP_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        float h_dotp(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
                result += *(a++) * *(b++);
            return result;
        }

        float h_abs_dotp(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
                result += ::fabs(*(a++)) * ::fabs(*(b++));
            return result;
        }

        float h_sqr_dotp(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
            {
                float xa = *a++, xb = *b++;
                result += xa*xa * xb*xb;
            }
            return result;
        }
    }
}

#endif /* PRIVATE_DSP_ARCH_GENERIC_HMATH_HDOTP_H_ */
