/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 7 авг. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_3DMATH_3DMATH_H_
#define PRIVATE_DSP_ARCH_X86_AVX_3DMATH_3DMATH_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

#include <private/dsp/arch/x86/avx/3dmath/common.h>

namespace lsp
{
    namespace avx
    {
        using namespace dsp;

        void init_matrix3d(matrix3d_t *dst, const matrix3d_t *src)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[s]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[s]), %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[d])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[d])")
                :
                : [s] "r" (src), [d] "r" (dst)
                : "memory",
                  "%xmm0", "%xmm1"
            );
        }

        void init_matrix3d_one(matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovaps         %[one], %%ymm0")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[m])")
                __ASM_EMIT("vmovups         %%ymm0, 0x20(%[m])")
                :
                : [m] "r" (m),
                  [one] "m" (ONE)
                : "memory",
                  "%xmm0"
            );
        }

        void init_matrix3d_identity(matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovaps         0x00 + %[id], %%ymm0")
                __ASM_EMIT("vmovaps         0x20 + %[id], %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[m])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[m])")
                :
                : [m] "r" (m),
                  [id] "o" (IDENTITY)
                : "memory",
                  "%xmm0", "%xmm1"
            );
        }

        void init_matrix3d_rotate_xyz(matrix3d_t *m, float x, float y, float z, float angle)
        {
            if (x == 0.0f)
            {
                if (y == 0.0f)
                {
                    if (z > 0.0f)
                        init_matrix3d_rotate_z(m, angle);
                    else if (z < 0.0f)
                        init_matrix3d_rotate_z(m, -angle);
                    else // (z == 0.0)
                        init_matrix3d_identity(m);
                    return;
                }
                else if (z == 0.0f)
                {
                    if (y > 0.0f)
                        init_matrix3d_rotate_y(m, angle);
                    else if (y < 0.0f)
                        init_matrix3d_rotate_y(m, -angle);
                    return;
                }
            }
            else if ((y == 0.0) && (z == 0.0))
            {
                if (x > 0.0f)
                    init_matrix3d_rotate_x(m, angle);
                else
                    init_matrix3d_rotate_x(m, -angle);
                return;
            }

            const float s = sinf(angle);
            const float c = cosf(angle);

            float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;
            const float mag = sqrtf(x*x + y*y + z*z);

            x          /= mag;
            y          /= mag;
            z          /= mag;

            xx          = x * x;
            yy          = y * y;
            zz          = z * z;
            xy          = x * y;
            yz          = y * z;
            zx          = z * x;
            xs          = x * s;
            ys          = y * s;
            zs          = z * s;
            one_c       = 1.0f - c;

            float *M    = m->m;
            M[0]        = (one_c * xx) + c;
            M[1]        = (one_c * xy) + zs;
            M[2]        = (one_c * zx) - ys;
            M[3]        = 0.0f;
            M[4]        = (one_c * xy) - zs;
            M[5]        = (one_c * yy) + c;
            M[6]        = (one_c * yz) + xs;
            M[7]        = 0.0f;
            M[8]        = (one_c * zx) + ys;
            M[9]        = (one_c * yz) - xs;
            M[10]       = (one_c * zz) + c;
            M[11]       = 0.0f;
            M[12]       = 0.0f;
            M[13]       = 0.0f;
            M[14]       = 0.0f;
            M[15]       = 0.0f;
        }

    } /* namespace avx */
} /* namespace lsp */

#endif /* INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_3DMATH_3DMATH_H_ */
