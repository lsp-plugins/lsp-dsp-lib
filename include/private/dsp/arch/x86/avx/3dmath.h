/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 13 авг. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_3DMATH_H_
#define PRIVATE_DSP_ARCH_X86_AVX_3DMATH_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

// Normalize vector/point
// x0 = vector/point to normalize, w = 0
// x1 = temp
// x2 = temp
#define NORMALIZE(x0, x1, x2) /* x0 = vector/point to normalize */ \
    __ASM_EMIT("vmulps          %" x0 ", %" x0 ", %" x2)    /* x2   = x*x y*y z*z 0 */ \
    __ASM_EMIT("vhaddps         %" x2 ", %" x2 ", %" x1)    /* x1   = x*x+y*y z*z x*x+y*y z*z */ \
    __ASM_EMIT("vhaddps         %" x1 ", %" x1 ", %" x1)    /* x1   = w2 w2 w2 w2 = x*x + y*y + z*z ... */ \
    __ASM_EMIT("vxorps          %" x2 ", %" x2 ", %" x2)    /* x2   = 0 */ \
    __ASM_EMIT("vsqrtps         %" x1 ", %" x1)             /* x1   = w = sqrt(w2) */ \
    __ASM_EMIT("vucomiss        %" x2 ", %" x0)             /* x2   =?= 0 */ \
    __ASM_EMIT("jle             1000000f") \
    __ASM_EMIT("vdivps          %" x1 ", %" x0 ", %" x0)    /* x0   = x/w y/w z/w w/w */ \
    __ASM_EMIT("1000000:")

namespace lsp
{
    namespace avx
    {
        using namespace dsp;

        IF_ARCH_X86(
            static const float IDENTITY[16] __lsp_aligned16 =
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            static const uint32_t X_MASK0111[] __lsp_aligned16  = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
        )

        void init_point_xyz(point3d_t *p, float x, float y, float z)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovlhps        %[x1], %[x0], %[x0]")           // x0   = x ? y ?
                __ASM_EMIT("vmovhps         %[id], %[x2], %[x2]")           // x2   = z ? 1 0
                __ASM_EMIT("vshufps         $0x88, %[x2], %[x0], %[x0]")    // x0   = x y z 1
                __ASM_EMIT("vmovups         %[x0], (%[p])")

                : [x0] "+x" (x), [x1] "+x" (y), [x2] "+x"(z)
                : [p] "r" (p),
                  [id] "m" (IDENTITY)
                : "memory"
            );
        }

        void init_point(point3d_t *p, const point3d_t *s)
        {
            float x0;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[s]), %[x0]")
                __ASM_EMIT("vmovups         %[x0], (%[p])")
                : [x0] "=&x" (x0)
                : [s] "r" (s), [p] "r" (p)
                : "memory"
            );
        }

        void normalize_point(point3d_t *p)
        {
            float x0, x1, x2;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p]), %[x0]")                 // x0   = x y z w
                __ASM_EMIT("vandps          %[mask], %[x0], %[x0]")         // x0   = x y z 0
                NORMALIZE("[x0]", "[x1]", "[x2]")
                __ASM_EMIT("vinsertps       $0x30, %[id], %[x0], %[x0]")    // x0   = x y z 1
                __ASM_EMIT("vmovups         %[x0], (%[p])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
                : [p] "r" (p),
                  [id] "m" (IDENTITY),
                  [mask] "m" (X_MASK0111)
                : "cc", "memory"
            );
        }

        void init_vector_dxyz(vector3d_t *p, float dx, float dy, float dz)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovlhps        %[x1], %[x0], %[x0]")           // x0   = dx ? dy ?
                __ASM_EMIT("vmovhps         %[id], %[x2], %[x2]")           // x2   = dz ? 1 0
                __ASM_EMIT("vshufps         $0xc8, %[x2], %[x0], %[x0]")    // x0   = dx dy dz 0
                __ASM_EMIT("vmovups         %[x0], (%[p])")

                : [x0] "+x" (dx), [x1] "+x" (dy), [x2] "+x"(dz)
                : [p] "r" (p),
                  [id] "m" (IDENTITY)
                : "memory"
            );
        }

        void init_vector(vector3d_t *p, const vector3d_t *s)
        {
            float x0;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[s]), %[x0]")
                __ASM_EMIT("vmovups         %[x0], (%[p])")
                : [x0] "=&x" (x0)
                : [s] "r" (s), [p] "r" (p)
                : "memory"
            );
        }

        void normalize_vector(vector3d_t *v)
        {
            float x0, x1, x2;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[v]), %[x0]")             // x0   = dx dy dz dw
                __ASM_EMIT("vandps          %[mask], %[x0], %[x0]")     // x0   = dx dy dz 0
                NORMALIZE("[x0]", "[x1]", "[x2]")
                __ASM_EMIT("vmovups         %[x0], (%[v])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
                : [v] "r" (v),
                  [id] "m" (IDENTITY),
                  [mask] "m" (X_MASK0111)
                : "cc", "memory"
            );
        }

        void normalize_vector2(vector3d_t *v, const vector3d_t *sv)
        {
            float x0, x1, x2;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[sv]), %[x0]")            // x0   = dx dy dz dw
                __ASM_EMIT("vandps          %[mask], %[x0], %[x0]")     // x0   = dx dy dz 0
                NORMALIZE("[x0]", "[x1]", "[x2]")
                __ASM_EMIT("vmovups         %[x0], (%[v])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
                : [v] "r" (v),
                  [sv] "r" (sv),
                  [id] "m" (IDENTITY),
                  [mask] "m" (X_MASK0111)
                : "cc", "memory"
            );
        }

    } /* namespace avx */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX_3DMATH_H_ */
