/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 8 мар. 2024 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-dsp-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more deheads.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef PRIVATE_DSP_ARCH_GENERIC_CORRELATION_H_
#define PRIVATE_DSP_ARCH_GENERIC_CORRELATION_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
        {
            float xv = 0.0f;
            float xa = 0.0f;
            float xb = 0.0f;

            if (count >= 4)
            {
                float T[4], A[4], B[4];

                T[0]    = 0.0f;
                T[1]    = 0.0f;
                T[2]    = 0.0f;
                T[3]    = 0.0f;

                A[0]    = 0.0f;
                A[1]    = 0.0f;
                A[2]    = 0.0f;
                A[3]    = 0.0f;

                B[0]    = 0.0f;
                B[1]    = 0.0f;
                B[2]    = 0.0f;
                B[3]    = 0.0f;

                for ( ; count >= 4; count -= 4)
                {
                    T[0]       += a[0] * b[0];
                    T[1]       += a[1] * b[1];
                    T[2]       += a[2] * b[2];
                    T[3]       += a[3] * b[3];

                    A[0]       += a[0] * a[0];
                    A[1]       += a[1] * a[1];
                    A[2]       += a[2] * a[2];
                    A[3]       += a[3] * a[3];

                    B[0]       += b[0] * b[0];
                    B[1]       += b[1] * b[1];
                    B[2]       += b[2] * b[2];
                    B[3]       += b[3] * b[3];

                    a          += 4;
                    b          += 4;
                }

                xv          = T[0] + T[1] + T[2] + T[3];
                xa          = A[0] + A[1] + A[2] + A[3];
                xb          = B[0] + B[1] + B[2] + B[3];
            }

            for ( ; count > 0; --count)
            {
                xv         += a[0] * b[0];
                xa         += a[0] * a[0];
                xb         += b[0] * b[0];

                a          += 1;
                b          += 1;
            }

            corr->v    += xv;
            corr->a    += xa;
            corr->b    += xb;
        }

        void corr_incr(dsp::correlation_t *corr, float *dst,
            const float *a_head, const float *b_head,
            const float *a_tail, const float *b_tail,
            size_t count)
        {
            float T[4], BA[4], BB[4], B[4], DV[4], DA[4], DB[4];

            float vv    = corr->v;
            float va    = corr->a;
            float vb    = corr->b;

            for ( ; count >= 4; count -= 4)
            {
                DV[0]       = a_head[0]*b_head[0] - a_tail[0]*b_tail[0];
                DV[1]       = a_head[1]*b_head[1] - a_tail[1]*b_tail[1];
                DV[2]       = a_head[2]*b_head[2] - a_tail[2]*b_tail[2];
                DV[3]       = a_head[3]*b_head[3] - a_tail[3]*b_tail[3];

                DA[0]       = a_head[0]*a_head[0] - a_tail[0]*a_tail[0];
                DA[1]       = a_head[1]*a_head[1] - a_tail[1]*a_tail[1];
                DA[2]       = a_head[2]*a_head[2] - a_tail[2]*a_tail[2];
                DA[3]       = a_head[3]*a_head[3] - a_tail[3]*a_tail[3];

                DB[0]       = b_head[0]*b_head[0] - b_tail[0]*b_tail[0];
                DB[1]       = b_head[1]*b_head[1] - b_tail[1]*b_tail[1];
                DB[2]       = b_head[2]*b_head[2] - b_tail[2]*b_tail[2];
                DB[3]       = b_head[3]*b_head[3] - b_tail[3]*b_tail[3];

                T[0]        = vv + DV[0];
                T[1]        = T[0] + DV[1];
                T[2]        = T[1] + DV[2];
                T[3]        = T[2] + DV[3];

                BA[0]       = va + DA[0];
                BA[1]       = BA[0] + DA[1];
                BA[2]       = BA[1] + DA[2];
                BA[3]       = BA[2] + DA[3];

                BB[0]       = vb + DB[0];
                BB[1]       = BB[0] + DB[1];
                BB[2]       = BB[1] + DB[2];
                BB[3]       = BB[2] + DB[3];

                B[0]        = BA[0] * BB[0];
                B[1]        = BA[1] * BB[1];
                B[2]        = BA[2] * BB[2];
                B[3]        = BA[3] * BB[3];

                dst[0]      = (B[0] >= 1e-18f) ? T[0] / sqrtf(B[0]) : 0.0f;
                dst[1]      = (B[1] >= 1e-18f) ? T[1] / sqrtf(B[1]) : 0.0f;
                dst[2]      = (B[2] >= 1e-18f) ? T[2] / sqrtf(B[2]) : 0.0f;
                dst[3]      = (B[3] >= 1e-18f) ? T[3] / sqrtf(B[3]) : 0.0f;

                vv          = T[3];
                va          = BA[3];
                vb          = BB[3];

                a_head     += 4;
                b_head     += 4;
                a_tail     += 4;
                b_tail     += 4;
                dst        += 4;
            }

            for (; count > 0; --count)
            {
                DV[0]       = a_head[0]*b_head[0] - a_tail[0]*b_tail[0];
                DA[0]       = a_head[0]*a_head[0] - a_tail[0]*a_tail[0];
                DB[0]       = b_head[0]*b_head[0] - b_tail[0]*b_tail[0];

                T[0]        = vv + DV[0];
                BA[0]       = va + DA[0];
                BB[0]       = vb + DB[0];
                B[0]        = BA[0] * BB[0];

                dst[0]      = (B[0] >= 1e-18f) ? T[0] / sqrtf(B[0]) : 0.0f;

                vv          = T[0];
                va          = BA[0];
                vb          = BB[0];

                a_head     += 1;
                b_head     += 1;
                a_tail     += 1;
                b_tail     += 1;
                dst        += 1;
            }

            corr->v     = vv;
            corr->a     = va;
            corr->b     = vb;
        }

    } /* namespace generic */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_GENERIC_CORRELATION_H_ */
