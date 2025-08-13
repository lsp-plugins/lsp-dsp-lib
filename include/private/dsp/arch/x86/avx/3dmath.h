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

// Load matrix
// ptr = address of matrix
// x0 = row 0
// x1 = row 1
// x2 = row 2
// x3 = row 3
#define MATRIX_LOAD(ptr, x0, x1, x2, x3) \
    __ASM_EMIT("vmovups         0x00(%[" ptr "]), %" x0 ) \
    __ASM_EMIT("vmovups         0x10(%[" ptr "]), %" x1 ) \
    __ASM_EMIT("vmovups         0x20(%[" ptr "]), %" x2 ) \
    __ASM_EMIT("vmovups         0x30(%[" ptr "]), %" x3 )

// Store matrix
// ptr = address of matrix
// x0 = row 0
// x1 = row 1
// x2 = row 2
// x3 = row 3
#define MATRIX_STORE(ptr, x0, x1, x2, x3) \
    __ASM_EMIT("vmovups         %" x0 ", 0x00(%[" ptr "])") \
    __ASM_EMIT("vmovups         %" x1 ", 0x10(%[" ptr "])") \
    __ASM_EMIT("vmovups         %" x2 ", 0x20(%[" ptr "])") \
    __ASM_EMIT("vmovups         %" x3 ", 0x30(%[" ptr "])")

// Transpose matrix
// x0 = row 0
// x1 = row 1
// x2 = row 2
// x3 = row 3
// x4 = temp
#define MAT4_TRANSPOSE(x0, x1, x2, x3, x4)    \
    __ASM_EMIT("vpunpckhdq      %" x3 ", %" x2 ", %" x4)    /* x4   = c3 d3 c4 d4 */   \
    __ASM_EMIT("vpunpckldq      %" x3 ", %" x2 ", %" x2)    /* x2   = c1 d1 c2 d2 */   \
    __ASM_EMIT("vpunpckhdq      %" x1 ", %" x0 ", %" x3)    /* x3   = a3 b3 a4 b4 */   \
    __ASM_EMIT("vpunpckldq      %" x1 ", %" x0 ", %" x0)    /* x0   = a1 b1 a2 b2 */   \
    __ASM_EMIT("vpunpckhqdq     %" x2 ", %" x0 ", %" x1)    /* x1   = a2 b2 c2 d2 */   \
    __ASM_EMIT("vpunpcklqdq     %" x2 ", %" x0 ", %" x0)    /* x0   = a1 b1 c1 d1 */   \
    __ASM_EMIT("vpunpcklqdq     %" x4 ", %" x3 ", %" x2)    /* x2   = a3 b3 c3 d3 */   \
    __ASM_EMIT("vpunpckhqdq     %" x4 ", %" x3 ", %" x3)    /* x3   = a4 b4 c4 d4 */


namespace lsp
{
    namespace avx
    {
        using namespace dsp;

        IF_ARCH_X86(
            static const float IDENTITY[16] __lsp_aligned32 =
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
            static const float ONE[] __lsp_aligned32            = { LSP_DSP_VEC8(1.0f) };

            static const uint32_t X_MASK0111[] __lsp_aligned32  = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
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

        void init_matrix3d_zero(matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[m])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[m])")
                :
                : [m] "r" (m)
                : "memory",
                  "%xmm0"
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

        void transpose_matrix3d1(matrix3d_t *r)
        {
            ARCH_X86_ASM
            (
                MATRIX_LOAD("m", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
                MAT4_TRANSPOSE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4")
                MATRIX_STORE("m", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
                :
                : [m] "r" (r)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4"
            );
        }

        void transpose_matrix3d2(matrix3d_t *r, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                MATRIX_LOAD("m", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
                MAT4_TRANSPOSE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4")
                MATRIX_STORE("r", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
                :
                : [r] "r" (r), [m] "r" (m)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4"
            );
        }

    } /* namespace avx */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX_3DMATH_H_ */
