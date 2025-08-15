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

// Multiply martix by vector
// x0 = martix row 0 (A)
// x1 = matrix row 1 (B)
// x2 = matrix row 2 (C)
// x3 = matrix row 3 (D)
// x4 = in vector, modified
// x5 = tmp
// x6 = tmp
// x7 = tmp
// x8 = out vector
#define MATMUL_CORE(x0, x1, x2, x3, x4, x5, x6, x7, x8) \
    __ASM_EMIT("vshufps         $0xff, %" x4 ", %" x4 ", %" x7)     /* x7 = v3 v3 v3 v3 */ \
    __ASM_EMIT("vshufps         $0xaa, %" x4 ", %" x4 ", %" x6)     /* x6 = v2 v2 v2 v2 */ \
    __ASM_EMIT("vshufps         $0x55, %" x4 ", %" x4 ", %" x5)     /* x5 = v1 v1 v1 v1 */ \
    __ASM_EMIT("vshufps         $0x00, %" x4 ", %" x4 ", %" x4)     /* x4 = v0 v0 v0 v0 */ \
    __ASM_EMIT("vmulps          %" x7 ", %" x3 ", %" x7)            /* x7 = d0*v3 d1*v3 d2*v3 d3*v3 */ \
    __ASM_EMIT("vmulps          %" x6 ", %" x2 ", %" x6)            /* x6 = c0*v2 c1*v2 c2*v2 c3*v2 */ \
    __ASM_EMIT("vmulps          %" x5 ", %" x1 ", %" x5)            /* x5 = b0*v1 d1*v1 d2*v1 d3*v1 */ \
    __ASM_EMIT("vmulps          %" x4 ", %" x0 ", %" x4)            /* x4 = a0*v0 c1*v0 c2*v0 c3*v0 */ \
    __ASM_EMIT("vaddps          %" x7 ", %" x6 ", %" x6) \
    __ASM_EMIT("vaddps          %" x5 ", %" x4 ", %" x4) \
    __ASM_EMIT("vaddps          %" x6 ", %" x4 ", %" x8)

 // Multiply martix by vector (FMA3)
 // x0 = martix row 0 (A)
 // x1 = matrix row 1 (B)
 // x2 = matrix row 2 (C)
 // x3 = matrix row 3 (D)
 // x4 = in vector, modified
 // x5 = tmp
 // x6 = tmp
 // x7 = tmp
 // x8 = out vector
 #define MATMUL_CORE_FMA3(x0, x1, x2, x3, x4, x5, x6, x7, x8) \
     __ASM_EMIT("vshufps         $0xff, %" x4 ", %" x4 ", %" x7)     /* x7 = v3 v3 v3 v3 */ \
     __ASM_EMIT("vshufps         $0xaa, %" x4 ", %" x4 ", %" x6)     /* x6 = v2 v2 v2 v2 */ \
     __ASM_EMIT("vshufps         $0x55, %" x4 ", %" x4 ", %" x5)     /* x5 = v1 v1 v1 v1 */ \
     __ASM_EMIT("vshufps         $0x00, %" x4 ", %" x4 ", %" x4)     /* x4 = v0 v0 v0 v0 */ \
     __ASM_EMIT("vmulps          %" x6 ", %" x2 ", %" x6)            /* x6 = c0*v2 c1*v2 c2*v2 c3*v2 */ \
     __ASM_EMIT("vmulps          %" x4 ", %" x0 ", %" x4)            /* x4 = a0*v0 c1*v0 c2*v0 c3*v0 */ \
     __ASM_EMIT("vfmadd231ps     %" x7 ", %" x3 ", %" x6) \
     __ASM_EMIT("vfmadd231ps     %" x5 ", %" x1 ", %" x4) \
     __ASM_EMIT("vaddps          %" x6 ", %" x4 ", %" x8)

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

            static const uint32_t X_MASK0111[] __lsp_aligned16  = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
            static const uint32_t X_SMASK0001[] __lsp_aligned16 = { 0x80000000, 0, 0, 0 };
            static const uint32_t X_SMASK0010[] __lsp_aligned16 = { 0, 0x80000000, 0, 0 };
            static const uint32_t X_SMASK0100[] __lsp_aligned16 = { 0, 0, 0x80000000, 0 };
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
                __ASM_EMIT("vmovups         %%ymm0, 0x20(%[m])")
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

        void init_matrix3d_translate(matrix3d_t *m, float dx, float dy, float dz)
        {
            float x3, x4, x5;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovaps         0x00 + %[id], %[x3]")           // x3   = 1 0 0 0
                __ASM_EMIT("vmovaps         0x10 + %[id], %[x4]")           // x4   = 0 1 0 0
                __ASM_EMIT("vmovlhps        %[x1], %[x0], %[x0]")           // x0   = x ? y ?
                __ASM_EMIT("vmovaps         0x20 + %[id], %[x5]")           // x5   = 0 0 1 0
                __ASM_EMIT("vmovhps         0x00 + %[id], %[x2], %[x2]")    // x2   = z ? 1 0
                __ASM_EMIT("vshufps         $0x88, %[x2], %[x0], %[x0]")    // x0   = x y z 1
                MATRIX_STORE("m", "[x3]", "[x4]", "[x5]", "[x0]")

                : [x0] "+x" (dx), [x1] "+x" (dy), [x2] "+x"(dz),
                  [x3] "=&x" (x3), [x4] "=&x" (x4), [x5] "=&x" (x5)
                : [m] "r" (m),
                  [id] "o" (IDENTITY)
                : "memory"
            );
        }

        void init_matrix3d_scale(matrix3d_t *m, float sx, float sy, float sz)
        {
            float x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")           // x3   = 0 0 0 0
                __ASM_EMIT("vinsertps       $0x00, %[x0], %[x3], %[x0]")    // x0   = sx 0 0 0
                __ASM_EMIT("vinsertps       $0x10, %[x1], %[x3], %[x1]")    // x0   = 0 sy 0 0
                __ASM_EMIT("vinsertps       $0x20, %[x2], %[x3], %[x2]")    // x0   = 0 0 sz 0
                __ASM_EMIT("vmovaps         0x30 + %[id], %[x3]")           // x3   = 0 0 0 1

                MATRIX_STORE("m", "[x0]", "[x1]", "[x2]", "[x3]")

                : [x0] "+x" (sx), [x1] "+x" (sy), [x2] "+x"(sz),
                  [x3] "=&x" (x3)
                : [m] "r" (m),
                  [id] "o" (IDENTITY)
                : "memory"
            );
        }

        void init_matrix3d_rotate_x(matrix3d_t *m, float angle)
        {
            float s     = sinf(angle);
            float c     = cosf(angle);
            float x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")           // x3   = 0 0 0 0
                __ASM_EMIT("vmovaps         0x00 + %[id], %[x2]")           // x2   = 1 0 0 0
                __ASM_EMIT("vinsertps       $0x10, %[x0], %[x3], %[x3]")    // x3   = 0 s 0 0
                __ASM_EMIT("vinsertps       $0x20, %[x1], %[x3], %[x3]")    // x3   = 0 s c 0
                __ASM_EMIT("vmovaps         0x30 + %[id], %[x0]")           // x0   = 0 0 0 1
                __ASM_EMIT("vxorps          %[mask], %[x3], %[x1]")         // x1   = 0 -s c 0
                __ASM_EMIT("vshufps         $0xd8, %[x3], %[x3], %[x3]")    // x3   = 0 c s 0

                MATRIX_STORE("m", "[x2]", "[x3]", "[x1]", "[x0]")

                : [x0] "+x" (s), [x1] "+x" (c), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [m] "r" (m),
                  [id] "o" (IDENTITY),
                  [mask] "m" (X_SMASK0010)
                : "memory"
            );
        }

        void init_matrix3d_rotate_y(matrix3d_t *m, float angle)
        {
            float s     = sinf(angle);
            float c     = cosf(angle);
            float x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")           // x3   = 0 0 0 0
                __ASM_EMIT("vmovaps         0x10 + %[id], %[x2]")           // x2   = 0 1 0 0
                __ASM_EMIT("vinsertps       $0x20, %[x0], %[x3], %[x3]")    // x3   = 0 0 s 0
                __ASM_EMIT("vinsertps       $0x00, %[x1], %[x3], %[x3]")    // x3   = c 0 s 0
                __ASM_EMIT("vmovaps         0x30 + %[id], %[x0]")           // x0   = 0 0 0 1
                __ASM_EMIT("vxorps          %[mask], %[x3], %[x1]")         // x1   = c 0 -s 0
                __ASM_EMIT("vshufps         $0xc6, %[x3], %[x3], %[x3]")    // x3   = s 0 c 0

                MATRIX_STORE("m", "[x1]", "[x2]", "[x3]", "[x0]")

                : [x0] "+x" (s), [x1] "+x" (c), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [m] "r" (m),
                  [id] "o" (IDENTITY),
                  [mask] "m" (X_SMASK0100)
                : "memory"
            );
        }

        void init_matrix3d_rotate_z(matrix3d_t *m, float angle)
        {
            float s     = sinf(angle);
            float c     = cosf(angle);
            float x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")           // x3   = 0 0 0 0
                __ASM_EMIT("vmovaps         0x20 + %[id], %[x2]")           // x2   = 0 0 1 0
                __ASM_EMIT("vinsertps       $0x00, %[x0], %[x3], %[x3]")    // x3   = s 0 0 0
                __ASM_EMIT("vinsertps       $0x10, %[x1], %[x3], %[x3]")    // x3   = s c 0 0
                __ASM_EMIT("vmovaps         0x30 + %[id], %[x0]")           // x0   = 0 0 0 1
                __ASM_EMIT("vxorps          %[mask], %[x3], %[x1]")         // x1   = -s c 0 0
                __ASM_EMIT("vshufps         $0xe1, %[x3], %[x3], %[x3]")    // x3   = c s 0 0

                MATRIX_STORE("m", "[x3]", "[x1]", "[x2]", "[x0]")

                : [x0] "+x" (s), [x1] "+x" (c), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [m] "r" (m),
                  [id] "o" (IDENTITY),
                  [mask] "m" (X_SMASK0001)
                : "memory"
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

            float s = sinf(angle);
            float c = cosf(angle);

            float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;
            float mag = sqrtf(x*x + y*y + z*z);

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

        void apply_matrix3d_mm2(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                MATRIX_LOAD("A", "%xmm0", "%xmm1", "%xmm2", "%xmm3")

                // Multiply matrix and store
                __ASM_EMIT("vmovups     0x00(%[B]), %%xmm4")    // xmm4 = b0 b1 b2 b3
                MATMUL_CORE(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                    "%xmm4")
                __ASM_EMIT("vmovups     %%xmm4, 0x00(%[R])")

                __ASM_EMIT("vmovups     0x10(%[B]), %%xmm5")    // xmm5 = b4 b5 b6 b7
                MATMUL_CORE(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm5", "%xmm6", "%xmm7", "%xmm4",
                    "%xmm5")
                __ASM_EMIT("vmovups     %%xmm5, 0x10(%[R])")

                __ASM_EMIT("vmovups     0x20(%[B]), %%xmm6")    // xmm6 = b4 b5 b6 b7
                MATMUL_CORE(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm6", "%xmm7", "%xmm4", "%xmm5",
                    "%xmm6")
                __ASM_EMIT("vmovups     %%xmm6, 0x20(%[R])")

                __ASM_EMIT("vmovups     0x30(%[B]), %%xmm7")    // xmm7 = b4 b5 b6 b7
                MATMUL_CORE(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm7", "%xmm4", "%xmm5", "%xmm6",
                    "%xmm7")
                __ASM_EMIT("vmovups     %%xmm7, 0x30(%[R])")

                :
                : [A] "r" (s), [B] "r" (m), [R] "r" (r)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void apply_matrix3d_mm1(matrix3d_t *r, const matrix3d_t *m)
        {
            IF_ARCH_I386(
                matrix3d_t __lsp_aligned32 t;
            );

            ARCH_X86_ASM
            (
                MATRIX_LOAD("A", "%xmm0", "%xmm1", "%xmm2", "%xmm3")

                // Multiply matrix and store
                __ASM_EMIT("vmovups     0x00(%[B]), %%xmm4")    // xmm4 = b0 b1 b2 b3
                MATMUL_CORE(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                    __IF_32("%xmm4") __IF_64("%xmm8"))
                __ASM_EMIT32("vmovaps   %%xmm4, 0x00(%[T])")

                __ASM_EMIT("vmovups     0x10(%[B]), %%xmm5")    // xmm5 = b4 b5 b6 b7
                MATMUL_CORE(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm5", "%xmm6", "%xmm7", "%xmm4",
                    __IF_32("%xmm5") __IF_64("%xmm9"))
                __ASM_EMIT32("vmovaps   %%xmm5, 0x10(%[T])")

                __ASM_EMIT("vmovups     0x20(%[B]), %%xmm6")    // xmm6 = b4 b5 b6 b7
                MATMUL_CORE(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm6", "%xmm7", "%xmm4", "%xmm5",
                    __IF_32("%xmm6") __IF_64("%xmm10"))
                __ASM_EMIT32("vmovaps   %%xmm6, 0x20(%[T])")

                __ASM_EMIT("vmovups     0x30(%[B]), %%xmm7")    // xmm7 = b4 b5 b6 b7
                MATMUL_CORE(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm7", "%xmm4", "%xmm5", "%xmm6",
                    __IF_32("%xmm7") __IF_64("%xmm11"))

                // Save matrix (32-bit)
                __ASM_EMIT32("vmovaps     0x00(%[T]), %%ymm4")
                __ASM_EMIT32("vmovaps     0x20(%[T]), %%xmm6")
                __ASM_EMIT32("vmovups     %%ymm4, 0x00(%[A])")
                __ASM_EMIT32("vmovups     %%xmm6, 0x20(%[A])")
                __ASM_EMIT32("vmovups     %%xmm7, 0x30(%[A])")

                // Save matrix (64-bit)
                __ASM_EMIT64("vmovups     %%xmm8, 0x00(%[A])")
                __ASM_EMIT64("vmovups     %%xmm9, 0x10(%[A])")
                __ASM_EMIT64("vmovups     %%xmm10, 0x20(%[A])")
                __ASM_EMIT64("vmovups     %%xmm11, 0x30(%[A])")

                :
                : [A] "r" (r), [B] "r" (m)
                  __IF_32(, [T] "r" (&t))
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                  , "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9", "%xmm10", "%xmm11")
            );
        }

        void apply_matrix3d_mm2_fma3(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                MATRIX_LOAD("A", "%xmm0", "%xmm1", "%xmm2", "%xmm3")

                // Multiply matrix and store
                __ASM_EMIT("vmovups     0x00(%[B]), %%xmm4")    // xmm4 = b0 b1 b2 b3
                MATMUL_CORE_FMA3(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                    "%xmm4")
                __ASM_EMIT("vmovups     %%xmm4, 0x00(%[R])")

                __ASM_EMIT("vmovups     0x10(%[B]), %%xmm5")    // xmm5 = b4 b5 b6 b7
                MATMUL_CORE_FMA3(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm5", "%xmm6", "%xmm7", "%xmm4",
                    "%xmm5")
                __ASM_EMIT("vmovups     %%xmm5, 0x10(%[R])")

                __ASM_EMIT("vmovups     0x20(%[B]), %%xmm6")    // xmm6 = b4 b5 b6 b7
                MATMUL_CORE_FMA3(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm6", "%xmm7", "%xmm4", "%xmm5",
                    "%xmm6")
                __ASM_EMIT("vmovups     %%xmm6, 0x20(%[R])")

                __ASM_EMIT("vmovups     0x30(%[B]), %%xmm7")    // xmm7 = b4 b5 b6 b7
                MATMUL_CORE_FMA3(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm7", "%xmm4", "%xmm5", "%xmm6",
                    "%xmm7")
                __ASM_EMIT("vmovups     %%xmm7, 0x30(%[R])")

                :
                : [A] "r" (s), [B] "r" (m), [R] "r" (r)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void apply_matrix3d_mm1_fma3(matrix3d_t *r, const matrix3d_t *m)
        {
            IF_ARCH_I386(
                matrix3d_t __lsp_aligned32 t;
            );

            ARCH_X86_ASM
            (
                MATRIX_LOAD("A", "%xmm0", "%xmm1", "%xmm2", "%xmm3")

                // Multiply matrix and store
                __ASM_EMIT("vmovups     0x00(%[B]), %%xmm4")    // xmm4 = b0 b1 b2 b3
                MATMUL_CORE_FMA3(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                    __IF_32("%xmm4") __IF_64("%xmm8"))
                __ASM_EMIT32("vmovaps   %%xmm4, 0x00(%[T])")

                __ASM_EMIT("vmovups     0x10(%[B]), %%xmm5")    // xmm5 = b4 b5 b6 b7
                MATMUL_CORE_FMA3(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm5", "%xmm6", "%xmm7", "%xmm4",
                    __IF_32("%xmm5") __IF_64("%xmm9"))
                __ASM_EMIT32("vmovaps   %%xmm5, 0x10(%[T])")

                __ASM_EMIT("vmovups     0x20(%[B]), %%xmm6")    // xmm6 = b4 b5 b6 b7
                MATMUL_CORE_FMA3(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm6", "%xmm7", "%xmm4", "%xmm5",
                    __IF_32("%xmm6") __IF_64("%xmm10"))
                __ASM_EMIT32("vmovaps   %%xmm6, 0x20(%[T])")

                __ASM_EMIT("vmovups     0x30(%[B]), %%xmm7")    // xmm7 = b4 b5 b6 b7
                MATMUL_CORE_FMA3(
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm7", "%xmm4", "%xmm5", "%xmm6",
                    __IF_32("%xmm7") __IF_64("%xmm11"))

                // Save matrix (32-bit)
                __ASM_EMIT32("vmovaps     0x00(%[T]), %%ymm4")
                __ASM_EMIT32("vmovaps     0x20(%[T]), %%xmm6")
                __ASM_EMIT32("vmovups     %%ymm4, 0x00(%[A])")
                __ASM_EMIT32("vmovups     %%xmm6, 0x20(%[A])")
                __ASM_EMIT32("vmovups     %%xmm7, 0x30(%[A])")

                // Save matrix (64-bit)
                __ASM_EMIT64("vmovups     %%xmm8, 0x00(%[A])")
                __ASM_EMIT64("vmovups     %%xmm9, 0x10(%[A])")
                __ASM_EMIT64("vmovups     %%xmm10, 0x20(%[A])")
                __ASM_EMIT64("vmovups     %%xmm11, 0x30(%[A])")

                :
                : [A] "r" (r), [B] "r" (m)
                  __IF_32(, [T] "r" (&t))
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                  , "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9", "%xmm10", "%xmm11")
            );
        }

        void apply_matrix3d_mv2(vector3d_t *r, const vector3d_t *v, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    0x00(%[v]), %%xmm0")            // xmm0 = vx vx vx vx
                __ASM_EMIT("vbroadcastss    0x04(%[v]), %%xmm1")            // xmm1 = vy vy vy vy
                __ASM_EMIT("vbroadcastss    0x08(%[v]), %%xmm2")            // xmm2 = vz vz vz vz
                __ASM_EMIT("vmulps          0x00(%[m]), %%xmm0, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vmulps          0x10(%[m]), %%xmm1, %%xmm1")    // xmm1 = vy * (m4  m5  m6  m7)
                __ASM_EMIT("vmulps          0x20(%[m]), %%xmm2, %%xmm2")    // xmm2 = vz * (m8  m9  m10 m11)
                __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")
                __ASM_EMIT("vxorps          %%xmm3, %%xmm3, %%xmm3")        // xmm3 = 0 0 0 0
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")        // xmm0 = rx ry rz rw
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm1") // xmm1 = rw rw rw rw
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm3")                // xmm1 =?= xmm1
                __ASM_EMIT("je              1f")
                __ASM_EMIT("vdivps          %%xmm1, %%xmm0, %%xmm0")        // xmm0 = rx/r2 ry/r2 rz/rw rw/rw
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         %%xmm0, (%[r])")
                :
                : [r] "r" (r), [v] "r" (v), [m] "r" (m)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void apply_matrix3d_mv1(vector3d_t *r, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    0x00(%[r]), %%xmm0")            // xmm0 = vx vx vx vx
                __ASM_EMIT("vbroadcastss    0x04(%[r]), %%xmm1")            // xmm1 = vy vy vy vy
                __ASM_EMIT("vbroadcastss    0x08(%[r]), %%xmm2")            // xmm2 = vz vz vz vz
                __ASM_EMIT("vmulps          0x00(%[m]), %%xmm0, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vmulps          0x10(%[m]), %%xmm1, %%xmm1")    // xmm1 = vy * (m4  m5  m6  m7)
                __ASM_EMIT("vmulps          0x20(%[m]), %%xmm2, %%xmm2")    // xmm2 = vz * (m8  m9  m10 m11)
                __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")
                __ASM_EMIT("vxorps          %%xmm3, %%xmm3, %%xmm3")        // xmm3 = 0 0 0 0
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")        // xmm0 = rx ry rz rw
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm1") // xmm1 = rw rw rw rw
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm3")                // xmm1 =?= xmm1
                __ASM_EMIT("je              1f")
                __ASM_EMIT("vdivps          %%xmm1, %%xmm0, %%xmm0")        // xmm0 = rx/r2 ry/r2 rz/rw rw/rw
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         %%xmm0, (%[r])")
                :
                : [r] "r" (r), [m] "r" (m)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void apply_matrix3d_mv2_fma3(vector3d_t *r, const vector3d_t *v, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    0x00(%[v]), %%xmm0")            // xmm0 = vx vx vx vx
                __ASM_EMIT("vbroadcastss    0x04(%[v]), %%xmm1")            // xmm1 = vy vy vy vy
                __ASM_EMIT("vbroadcastss    0x08(%[v]), %%xmm2")            // xmm2 = vz vz vz vz
                __ASM_EMIT("vmulps          0x00(%[m]), %%xmm0, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vxorps          %%xmm3, %%xmm3, %%xmm3")        // xmm3 = 0 0 0 0
                __ASM_EMIT("vfmadd231ps     0x10(%[m]), %%xmm1, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vfmadd231ps     0x20(%[m]), %%xmm2, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm1") // xmm1 = rw rw rw rw
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm3")                // xmm1 =?= xmm1
                __ASM_EMIT("je              1f")
                __ASM_EMIT("vdivps          %%xmm1, %%xmm0, %%xmm0")        // xmm0 = rx/r2 ry/r2 rz/rw rw/rw
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         %%xmm0, (%[r])")
                :
                : [r] "r" (r), [v] "r" (v), [m] "r" (m)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void apply_matrix3d_mv1_fma3(vector3d_t *r, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    0x00(%[r]), %%xmm0")            // xmm0 = vx vx vx vx
                __ASM_EMIT("vbroadcastss    0x04(%[r]), %%xmm1")            // xmm1 = vy vy vy vy
                __ASM_EMIT("vbroadcastss    0x08(%[r]), %%xmm2")            // xmm2 = vz vz vz vz
                __ASM_EMIT("vmulps          0x00(%[m]), %%xmm0, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vxorps          %%xmm3, %%xmm3, %%xmm3")        // xmm3 = 0 0 0 0
                __ASM_EMIT("vfmadd231ps     0x10(%[m]), %%xmm1, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vfmadd231ps     0x20(%[m]), %%xmm2, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm1") // xmm1 = rw rw rw rw
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm3")                // xmm1 =?= xmm1
                __ASM_EMIT("je              1f")
                __ASM_EMIT("vdivps          %%xmm1, %%xmm0, %%xmm0")        // xmm0 = rx/r2 ry/r2 rz/rw rw/rw
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         %%xmm0, (%[r])")
                :
                : [r] "r" (r), [m] "r" (m)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void apply_matrix3d_mp2(point3d_t *p, const point3d_t *s, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    0x00(%[s]), %%xmm0")            // xmm0 = vx vx vx vx
                __ASM_EMIT("vbroadcastss    0x04(%[s]), %%xmm1")            // xmm1 = vy vy vy vy
                __ASM_EMIT("vbroadcastss    0x08(%[s]), %%xmm2")            // xmm2 = vz vz vz vz
                __ASM_EMIT("vmulps          0x00(%[m]), %%xmm0, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vmulps          0x10(%[m]), %%xmm1, %%xmm1")    // xmm1 = vy * (m4  m5  m6  m7)
                __ASM_EMIT("vmulps          0x20(%[m]), %%xmm2, %%xmm2")    // xmm2 = vz * (m8  m9  m10 m11)
                __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")
                __ASM_EMIT("vaddps          0x30(%[m]), %%xmm2, %%xmm2")
                __ASM_EMIT("vxorps          %%xmm3, %%xmm3, %%xmm3")        // xmm3 = 0 0 0 0
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")        // xmm0 = rx ry rz rw
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm1") // xmm1 = rw rw rw rw
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm3")                // xmm1 =?= xmm1
                __ASM_EMIT("je              1f")
                __ASM_EMIT("vdivps          %%xmm1, %%xmm0, %%xmm0")        // xmm0 = rx/r2 ry/r2 rz/rw rw/rw
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         %%xmm0, (%[p])")
                :
                : [p] "r" (p), [s] "r" (s), [m] "r" (m)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void apply_matrix3d_mp1(point3d_t *p, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    0x00(%[p]), %%xmm0")            // xmm0 = vx vx vx vx
                __ASM_EMIT("vbroadcastss    0x04(%[p]), %%xmm1")            // xmm1 = vy vy vy vy
                __ASM_EMIT("vbroadcastss    0x08(%[p]), %%xmm2")            // xmm2 = vz vz vz vz
                __ASM_EMIT("vmulps          0x00(%[m]), %%xmm0, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vmulps          0x10(%[m]), %%xmm1, %%xmm1")    // xmm1 = vy * (m4  m5  m6  m7)
                __ASM_EMIT("vmulps          0x20(%[m]), %%xmm2, %%xmm2")    // xmm2 = vz * (m8  m9  m10 m11)
                __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")
                __ASM_EMIT("vaddps          0x30(%[m]), %%xmm2, %%xmm2")
                __ASM_EMIT("vxorps          %%xmm3, %%xmm3, %%xmm3")        // xmm3 = 0 0 0 0
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")        // xmm0 = rx ry rz rw
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm1") // xmm1 = rw rw rw rw
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm3")                // xmm1 =?= xmm1
                __ASM_EMIT("je              1f")
                __ASM_EMIT("vdivps          %%xmm1, %%xmm0, %%xmm0")        // xmm0 = rx/r2 ry/r2 rz/rw rw/rw
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         %%xmm0, (%[p])")
                :
                : [p] "r" (p), [m] "r" (m)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void apply_matrix3d_mp2_fma3(point3d_t *p, const point3d_t *s, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    0x00(%[s]), %%xmm0")            // xmm0 = vx vx vx vx
                __ASM_EMIT("vbroadcastss    0x04(%[s]), %%xmm1")            // xmm1 = vy vy vy vy
                __ASM_EMIT("vbroadcastss    0x08(%[s]), %%xmm2")            // xmm2 = vz vz vz vz
                __ASM_EMIT("vmulps          0x00(%[m]), %%xmm0, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vmovaps         0x30(%[m]), %%xmm3")            // xmm3 = (m12 m13 m14 m15)
                __ASM_EMIT("vfmadd231ps     0x10(%[m]), %%xmm1, %%xmm0")
                __ASM_EMIT("vfmadd231ps     0x20(%[m]), %%xmm2, %%xmm3")
                __ASM_EMIT("vxorps          %%xmm1, %%xmm1, %%xmm1")        // xmm1 = 0 0 0 0
                __ASM_EMIT("vaddps          %%xmm3, %%xmm0, %%xmm0")        // xmm0 = rx ry rz rw
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm2") // xmm1 = rw rw rw rw
                __ASM_EMIT("vucomiss        %%xmm2, %%xmm1")                // xmm2 =?= xmm2
                __ASM_EMIT("je              1f")
                __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")        // xmm0 = rx/r2 ry/r2 rz/rw rw/rw
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         %%xmm0, (%[p])")
                :
                : [p] "r" (p), [s] "r" (s), [m] "r" (m)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void apply_matrix3d_mp1_fma3(point3d_t *p, const matrix3d_t *m)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    0x00(%[p]), %%xmm0")            // xmm0 = vx vx vx vx
                __ASM_EMIT("vbroadcastss    0x04(%[p]), %%xmm1")            // xmm1 = vy vy vy vy
                __ASM_EMIT("vbroadcastss    0x08(%[p]), %%xmm2")            // xmm2 = vz vz vz vz
                __ASM_EMIT("vmulps          0x00(%[p]), %%xmm0, %%xmm0")    // xmm0 = vx * (m0  m1  m2  m3)
                __ASM_EMIT("vmovaps         0x30(%[m]), %%xmm3")            // xmm3 = (m12 m13 m14 m15)
                __ASM_EMIT("vfmadd231ps     0x10(%[m]), %%xmm1, %%xmm0")
                __ASM_EMIT("vfmadd231ps     0x20(%[m]), %%xmm2, %%xmm3")
                __ASM_EMIT("vxorps          %%xmm1, %%xmm1, %%xmm1")        // xmm1 = 0 0 0 0
                __ASM_EMIT("vaddps          %%xmm3, %%xmm0, %%xmm0")        // xmm0 = rx ry rz rw
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm2") // xmm1 = rw rw rw rw
                __ASM_EMIT("vucomiss        %%xmm2, %%xmm1")                // xmm2 =?= xmm2
                __ASM_EMIT("je              1f")
                __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")        // xmm0 = rx/r2 ry/r2 rz/rw rw/rw
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         %%xmm0, (%[p])")
                :
                : [p] "r" (p), [m] "r" (m)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

    } /* namespace avx */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX_3DMATH_H_ */
