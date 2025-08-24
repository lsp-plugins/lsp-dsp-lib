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
// Input:
//   x0 = vector/point to normalize, w = 0
//   x1 = temp
//   x2 = temp
// Output:
//   x0 = normalized vector/point
#define NORMALIZE(x0, x1, x2) /* x0 = vector/point to normalize */ \
    __ASM_EMIT("vmulps          %" x0 ", %" x0 ", %" x2)        /* x2   = x*x y*y z*z 0 */ \
    __ASM_EMIT("vhaddps         %" x2 ", %" x2 ", %" x1)        /* x1   = x*x+y*y z*z x*x+y*y z*z */ \
    __ASM_EMIT("vhaddps         %" x1 ", %" x1 ", %" x1)        /* x1   = w2 w2 w2 w2 = x*x + y*y + z*z ... */ \
    __ASM_EMIT("vxorps          %" x2 ", %" x2 ", %" x2)        /* x2   = 0 */ \
    __ASM_EMIT("vsqrtps         %" x1 ", %" x1)                 /* x1   = w = sqrt(w2) */ \
    __ASM_EMIT("vcmpps          $4, %" x2 ", %" x0 ", %" x2)    /* x2   = (w != 0) */ \
    __ASM_EMIT("vdivps          %" x1 ", %" x0 ", %" x0)        /* x0   = x/w y/w z/w w/w */ \
    __ASM_EMIT("vandps          %" x2 ", %" x0 ", %" x0)        /* x0   = (w != 0) ? x/w y/w z/w w/w : 0 */ \

// Load matrix
// Input:
//   ptr = address of matrix
// Output:
//   x0 = row 0
//   x1 = row 1
//   x2 = row 2
//   x3 = row 3
#define MATRIX_LOAD(ptr, x0, x1, x2, x3) \
    __ASM_EMIT("vmovups         0x00(%[" ptr "]), %" x0 ) \
    __ASM_EMIT("vmovups         0x10(%[" ptr "]), %" x1 ) \
    __ASM_EMIT("vmovups         0x20(%[" ptr "]), %" x2 ) \
    __ASM_EMIT("vmovups         0x30(%[" ptr "]), %" x3 )

// Store matrix
// Input:
//   ptr = address of matrix
//   x0 = row 0
//   x1 = row 1
//   x2 = row 2
//   x3 = row 3
#define MATRIX_STORE(ptr, x0, x1, x2, x3) \
    __ASM_EMIT("vmovups         %" x0 ", 0x00(%[" ptr "])") \
    __ASM_EMIT("vmovups         %" x1 ", 0x10(%[" ptr "])") \
    __ASM_EMIT("vmovups         %" x2 ", 0x20(%[" ptr "])") \
    __ASM_EMIT("vmovups         %" x3 ", 0x30(%[" ptr "])")

// Transpose 4x4 matrix
// Input:
//   x0 = row 0
//   x1 = row 1
//   x2 = row 2
//   x3 = row 3
//   x4 = temp
// Output:
//   x0 = row 0
//   x1 = row 1
//   x2 = row 2
//   x3 = row 3
#define MAT4_TRANSPOSE(x0, x1, x2, x3, x4)    \
    __ASM_EMIT("vpunpckhdq      %" x3 ", %" x2 ", %" x4)            /* x4   = c3 d3 c4 d4 */   \
    __ASM_EMIT("vpunpckldq      %" x3 ", %" x2 ", %" x2)            /* x2   = c1 d1 c2 d2 */   \
    __ASM_EMIT("vpunpckhdq      %" x1 ", %" x0 ", %" x3)            /* x3   = a3 b3 a4 b4 */   \
    __ASM_EMIT("vpunpckldq      %" x1 ", %" x0 ", %" x0)            /* x0   = a1 b1 a2 b2 */   \
    __ASM_EMIT("vpunpckhqdq     %" x2 ", %" x0 ", %" x1)            /* x1   = a2 b2 c2 d2 */   \
    __ASM_EMIT("vpunpcklqdq     %" x2 ", %" x0 ", %" x0)            /* x0   = a1 b1 c1 d1 */   \
    __ASM_EMIT("vpunpcklqdq     %" x4 ", %" x3 ", %" x2)            /* x2   = a3 b3 c3 d3 */   \
    __ASM_EMIT("vpunpckhqdq     %" x4 ", %" x3 ", %" x3)            /* x3   = a4 b4 c4 d4 */

// Transpose 3x3 matrix
// Input:
//   x0 = row 0 = a0 a1 a2 ?
//   x1 = row 1 = b0 b1 b2 ?
//   x2 = row 2 = c0 c1 c2 ?
//   x3 = temp
// Output:
//   x0 = row 0 = a0 b0 c0 ?
//   x1 = row 1 = a1 b1 c1 ?
//   x2 = row 2 = a2 b2 c2 ?
#define MAT3_TRANSPOSE(x0, x1, x2, x3)    \
    __ASM_EMIT("vunpckhps       %" x1 ", %" x0 ", %" x3)            /* x3   = a2 b2 ? ?     */ \
    __ASM_EMIT("vunpcklps       %" x1 ", %" x0 ", %" x0)            /* x0   = a0 b0 a1 b1   */  \
    __ASM_EMIT("vmovhlps        %" x0 ", %" x1 ", %" x1)            /* x1   = a1 b1 b2 ?    */ \
    __ASM_EMIT("vmovlhps        %" x2 ", %" x0 ", %" x0)            /* x0   = a0 b0 c0 c1   */ \
    __ASM_EMIT("vshufps         $0x54, %" x2 ", %" x1", %" x1)      /* x1   = a1 b1 c1 c1   */ \
    __ASM_EMIT("vshufps         $0xa4, %" x2 ", %" x3", %" x2)      /* x2   = a2 b2 c2 c2   */

// Multiply martix by vector
// Input:
//   x0 = martix row 0 (A)
//   x1 = matrix row 1 (B)
//   x2 = matrix row 2 (C)
//   x3 = matrix row 3 (D)
//   x4 = in vector, modified
//   x5 = tmp
//   x6 = tmp
//   x7 = tmp
// Output:
//   x8 = out vector
#define MATMUL_CORE(x0, x1, x2, x3, x4, x5, x6, x7, x8) \
    __ASM_EMIT("vshufps         $0xff, %" x4 ", %" x4 ", %" x7)     /* x7   = v3 v3 v3 v3 */ \
    __ASM_EMIT("vshufps         $0xaa, %" x4 ", %" x4 ", %" x6)     /* x6   = v2 v2 v2 v2 */ \
    __ASM_EMIT("vshufps         $0x55, %" x4 ", %" x4 ", %" x5)     /* x5   = v1 v1 v1 v1 */ \
    __ASM_EMIT("vshufps         $0x00, %" x4 ", %" x4 ", %" x4)     /* x4   = v0 v0 v0 v0 */ \
    __ASM_EMIT("vmulps          %" x7 ", %" x3 ", %" x7)            /* x7   = d0*v3 d1*v3 d2*v3 d3*v3 */ \
    __ASM_EMIT("vmulps          %" x6 ", %" x2 ", %" x6)            /* x6   = c0*v2 c1*v2 c2*v2 c3*v2 */ \
    __ASM_EMIT("vmulps          %" x5 ", %" x1 ", %" x5)            /* x5   = b0*v1 d1*v1 d2*v1 d3*v1 */ \
    __ASM_EMIT("vmulps          %" x4 ", %" x0 ", %" x4)            /* x4   = a0*v0 c1*v0 c2*v0 c3*v0 */ \
    __ASM_EMIT("vaddps          %" x7 ", %" x6 ", %" x6) \
    __ASM_EMIT("vaddps          %" x5 ", %" x4 ", %" x4) \
    __ASM_EMIT("vaddps          %" x6 ", %" x4 ", %" x8)

// Multiply martix by vector (FMA3)
// Input:
//   x0 = martix row 0 (A)
//   x1 = matrix row 1 (B)
//   x2 = matrix row 2 (C)
//   x3 = matrix row 3 (D)
//   x4 = in vector, modified
//   x5 = tmp
//   x6 = tmp
//   x7 = tmp
// Output:
//   x8 = out vector
 #define MATMUL_CORE_FMA3(x0, x1, x2, x3, x4, x5, x6, x7, x8) \
     __ASM_EMIT("vshufps         $0xff, %" x4 ", %" x4 ", %" x7)     /* x7  = v3 v3 v3 v3 */ \
     __ASM_EMIT("vshufps         $0xaa, %" x4 ", %" x4 ", %" x6)     /* x6  = v2 v2 v2 v2 */ \
     __ASM_EMIT("vshufps         $0x55, %" x4 ", %" x4 ", %" x5)     /* x5  = v1 v1 v1 v1 */ \
     __ASM_EMIT("vshufps         $0x00, %" x4 ", %" x4 ", %" x4)     /* x4  = v0 v0 v0 v0 */ \
     __ASM_EMIT("vmulps          %" x6 ", %" x2 ", %" x6)            /* x6  = c0*v2 c1*v2 c2*v2 c3*v2 */ \
     __ASM_EMIT("vmulps          %" x4 ", %" x0 ", %" x4)            /* x4  = a0*v0 c1*v0 c2*v0 c3*v0 */ \
     __ASM_EMIT("vfmadd231ps     %" x7 ", %" x3 ", %" x6) \
     __ASM_EMIT("vfmadd231ps     %" x5 ", %" x1 ", %" x4) \
     __ASM_EMIT("vaddps          %" x6 ", %" x4 ", %" x8)

// 1x vector cross-product (3 coordinates)
// Input:
//   x0 = vector1 [dx1 dy1 dz1 0 ]
//   x1 = vector2 [dx2 dy2 dz2 0 ]
//   x2 = temporary
//   x3 = temporary
//
// Output:
//   x0 = vector1 * vector2 [ vz vx vy 0 ]
#define VECTOR_CROSS3(x0, x1, x2, x3) \
    __ASM_EMIT("vshufps         $0xc9, %" x0 ", %" x0 ", %" x2)     /* x2   = dy1 dz1 dx1 0 */ \
    __ASM_EMIT("vshufps         $0xc9, %" x1 ", %" x1 ", %" x3)     /* x3   = dy2 dz2 dx2 0 */ \
    __ASM_EMIT("vmulps          %" x2 ", %" x1 ", %" x1)            /* x1   = dx2*dy1 dy2*dz1 dz2*dx1 0 */ \
    __ASM_EMIT("vmulps          %" x3 ", %" x0 ", %" x0)            /* x0   = dx1*dy2 dy1*dz2 dz1*dx2 0 */ \
    __ASM_EMIT("vsubps          %" x1 ", %" x0 ", %" x0)            /* x0   = dx1*dy2-dx2*dy1 dy1*dz2-dy2*dz1 dz1*dx2-dz2*dx1 0 = NY NZ NX 0 */ \

// 1x vector cross-product (3 coordinates), FMA3
// Input:
//   x0 = vector1 [dx1 dy1 dz1 0 ]
//   x1 = vector2 [dx2 dy2 dz2 0 ]
//   x2 = temporary
//   x3 = temporary
//
// Output:
//   x0 = vector1 * vector2 [ vz vx vy 0 ]
#define VECTOR_CROSS3_FMA3(x0, x1, x2, x3) \
    __ASM_EMIT("vshufps         $0xc9, %" x0 ", %" x0 ", %" x2)     /* x2   = dy1 dz1 dx1 0 */ \
    __ASM_EMIT("vshufps         $0xc9, %" x1 ", %" x1 ", %" x3)     /* x3   = dy2 dz2 dx2 0 */ \
    __ASM_EMIT("vmulps          %" x2 ", %" x1 ", %" x1)            /* x1   = dx2*dy1 dy2*dz1 dz2*dx1 0 */ \
    __ASM_EMIT("vfmsub213ps     %" x1 ", %" x3 ", %" x0)            /* x0   = dx1*dy2-dx2*dy1 dy1*dz2-dy2*dz1 dz1*dx2-dz2*dx1 0 = NY NZ NX 0 */

/* 1x vector dot-product (3 coordinates)
 * Input:
 *   x0 = vector1 [dx1 dy1 dz1 0 ]
 *   x1 = vector2 [dx2 dy2 dz2 0 ]
 *   x2 = temporary
 *   x3 = temporary
 *
 * Output:
 *   x0 = result (scalar)
 */
#define VECTOR_DOT3(x0, x1, x2, x3) \
    /* do not use DPPS as it is slow */ \
    /* shuffles and adds work faster than vhaddps */ \
    __ASM_EMIT("vmulps          %" x1 ", %" x0 ", %" x0)            /* x0   = dx1*dx2 dy1*dy2 dz1*dz2 ? */ \
    __ASM_EMIT("vshufps         $0x55, %" x0 ", %" x0 ", %" x3)     /* x3   = dy1*dy2  */ \
    __ASM_EMIT("vmovhlps        %" x0 ", %" x0 ", %" x2)            /* x2   = dz1*dz2 ? ? ? */ \
    __ASM_EMIT("vaddss          %" x3 ", %" x0 ", %" x0)            /* x0   = dx1*dx2 + dy1*dy2 */ \
    __ASM_EMIT("vaddss          %" x2 ", %" x0 ", %" x0)            /* x0   = dx1*dx2 + dy1*dy2 + dz1*dz2 */ \

/* Compute projection of the normal to the result of cross product of two vectors (mixed product)
 * Input:
 *   x0 = vector a [ x0 y0 z0 0 ]
 *   x1 = vector b [ x1 y1 z1 0 ]
 *   x2 = vector c [ x2 y2 z2 0 ]
 *   x3 = temporary
 *   x4 = temporary
 *
 * Output:
 *   x0 = cross product: a dot (b cross c)
 */
#define MIXED_PRODUCT3(x0, x1, x2, x3, x4) \
    __ASM_EMIT("vshufps         $0xc9, %" x1 ", %" x1 ", %" x3)     /* x3   = y1 z1 x1 0 */ \
    __ASM_EMIT("vshufps         $0xc9, %" x2 ", %" x2 ", %" x4)     /* x4   = y2 z2 x2 0 */ \
    __ASM_EMIT("vshufps         $0xd2, %" x0 ", %" x0 ", %" x0)     /* x0   = z0 x0 y0 0 */ \
    __ASM_EMIT("vmulps          %" x4 ", %" x1 ", %" x1)            /* x1   = x1*y2 y1*z2 z1*x2 w1*w2 */ \
    __ASM_EMIT("vmulps          %" x3 ", %" x2 ", %" x2)            /* x2   = y1*x2 z1*y2 x1*z2 w1*w2 */ \
    __ASM_EMIT("vsubps          %" x2 ", %" x1 ", %" x1)            /* x1   = x1*y2-y1*x2 y1*z2-z1*y2 z1*x2-x1*z2 0 = vz vx vy 0 */ \
    __ASM_EMIT("vmulps          %" x1 ", %" x0 ", %" x0)            /* x0   = z0*vz x0*vx y0*vy 0 */ \
    __ASM_EMIT("vmovhlps        %" x0 ", %" x2 ", %" x2)            /* x2   = y0*vy 0 ? ? */ \
    __ASM_EMIT("vunpcklps       %" x0 ", %" x0 ", %" x0)            /* x0   = z0*vz z0*vz x0*vx x0*vx */ \
    __ASM_EMIT("vmovhlps        %" x0 ", %" x1 ", %" x1)            /* x1   = x0*vx x0*vx ? ? */ \
    __ASM_EMIT("vaddss          %" x2 ", %" x0 ", %" x0)            /* x0   = z0*vz + y0*vy */ \
    __ASM_EMIT("vaddss          %" x1 ", %" x0 ", %" x0)            /* x0   = z0*vz + y0*vy + x0*vx */

/* Compute projection of the normal to the result of cross product of two vectors (mixed product), FMA3 implementation
 * Input:
 *   x0 = vector a [ x0 y0 z0 0 ]
 *   x1 = vector b [ x1 y1 z1 0 ]
 *   x2 = vector c [ x2 y2 z2 0 ]
 *   x3 = temporary
 *   x4 = temporary
 *
 * Output:
 *   x0 = cross product: a dot (b cross c)
 */
#define MIXED_PRODUCT3_FMA3(x0, x1, x2, x3, x4) \
    __ASM_EMIT("vshufps         $0xc9, %" x1 ", %" x1 ", %" x3)     /* x3   = y1 z1 x1 0 */ \
    __ASM_EMIT("vshufps         $0xc9, %" x2 ", %" x2 ", %" x4)     /* x4   = y2 z2 x2 0 */ \
    __ASM_EMIT("vshufps         $0xd2, %" x0 ", %" x0 ", %" x0)     /* x0   = z0 x0 y0 0 */ \
    __ASM_EMIT("vmulps          %" x4 ", %" x1 ", %" x1)            /* x1   = x1*y2 y1*z2 z1*x2 w1*w2 */ \
    __ASM_EMIT("vfnmadd231ps    %" x3 ", %" x2 ", %" x1)            /* x1   = x1*y2-y1*x2 y1*z2-z1*y2 z1*x2-x1*z2 0 = vz vx vy 0 */ \
    __ASM_EMIT("vmulps          %" x1 ", %" x0 ", %" x0)            /* x0   = z0*vz x0*vx y0*vy 0 */ \
    __ASM_EMIT("vmovhlps        %" x0 ", %" x2 ", %" x2)            /* x2   = y0*vy 0 ? ? */ \
    __ASM_EMIT("vunpcklps       %" x0 ", %" x0 ", %" x0)            /* x0   = z0*vz z0*vz x0*vx x0*vx */ \
    __ASM_EMIT("vmovhlps        %" x0 ", %" x1 ", %" x1)            /* x1   = x0*vx x0*vx ? ? */ \
    __ASM_EMIT("vaddss          %" x2 ", %" x0 ", %" x0)            /* x0   = z0*vz + y0*vy */ \
    __ASM_EMIT("vaddss          %" x1 ", %" x0 ", %" x0)            /* x0   = z0*vz + y0*vy + x0*vx */


/* 3x vector multiplication
 * Input:
 *   x0 = vector1 [dx0 dy0 dz0 0 ]
 *   x1 = vector2 [dx1 dy1 dz1 0 ]
 *   x2 = vector3 [dx2 dy2 dz2 0 ]
 *
 * Output:
 *   x0 = vector1 * vector2 [ vz vx vy 0 ]
 *   x1 = vector2 * vector3 [ vz vx vy 0 ]
 *   x2 = vector3 * vector1 [ vz vx vy 0 ]
 */
#define VECTOR_CROSS3_X3(x0, x1, x2, x3, x4, x5, x6, x7) \
    __ASM_EMIT("vshufps         $0xc9, %" x0 ", %" x0 ", %" x3) /* x3   = dy0 dz0 dx0 0 */ \
    __ASM_EMIT("vshufps         $0xc9, %" x1 ", %" x1 ", %" x4) /* x4   = dy1 dz1 dx1 0 */ \
    __ASM_EMIT("vshufps         $0xc9, %" x2 ", %" x2 ", %" x5) /* x5   = dy2 dz2 dx2 0 */ \
    __ASM_EMIT("vmulps          %" x4 ", %" x0 ", %" x6)        /* x6   = dx0*dy1 dy0*dz1 dz0*dx1 0 */ \
    __ASM_EMIT("vmulps          %" x3 ", %" x1 ", %" x7)        /* x7   = dy0*dx1 dz0*dy1 dx0*dz1 0 */ \
    __ASM_EMIT("vmulps          %" x5 ", %" x1 ", %" x1)        /* x1   = dx1*dy2 dy1*dz2 dz1*dx2 0 */ \
    __ASM_EMIT("vmulps          %" x2 ", %" x4 ", %" x4)        /* x4   = dy1*dx2 dz1*dy2 dx1*dz2 0 */ \
    __ASM_EMIT("vmulps          %" x3 ", %" x2 ", %" x2)        /* x2   = dx2*dy0 dy2*dz0 dz2*dx0 0 */ \
    __ASM_EMIT("vmulps          %" x0 ", %" x5 ", %" x5)        /* x5   = dy2*dx0 dz2*dy0 dx2*dz0 0 */ \
    __ASM_EMIT("vsubps          %" x7 ", %" x6 ", %" x0)        /* x0   = N0 = (dx0*dy1-dy0*dx1 dy0*dz1-dz0*dy1 dz0*dx1-dx0*dz1 0) */ \
    __ASM_EMIT("vsubps          %" x4 ", %" x1 ", %" x1)        /* x1   = N1 = (dx1*dy2-dy1*dx2 dy1*dz2-dz1*dy2 dz1*dx2-dx1*dz2 0) */ \
    __ASM_EMIT("vsubps          %" x5 ", %" x2 ", %" x2)        /* x2   = N2 = (dx2*dy0-dy2*dx0 dy2*dz0-dz2*dy0 dz2*dx0-dx2*dz0 0)*/

#define VECTOR_CROSS3_X3_FMA3(x0, x1, x2, x3, x4, x5, x6, x7) \
    __ASM_EMIT("vshufps         $0xc9, %" x0 ", %" x0 ", %" x3) /* x3   = dy0 dz0 dx0 0 */ \
    __ASM_EMIT("vshufps         $0xc9, %" x1 ", %" x1 ", %" x4) /* x4   = dy1 dz1 dx1 0 */ \
    __ASM_EMIT("vshufps         $0xc9, %" x2 ", %" x2 ", %" x5) /* x5   = dy2 dz2 dx2 0 */ \
    __ASM_EMIT("vmulps          %" x4 ", %" x0 ", %" x6)        /* x6   = dx0*dy1 dy0*dz1 dz0*dx1 0 */ \
    __ASM_EMIT("vmulps          %" x3 ", %" x1 ", %" x7)        /* x7   = dy0*dx1 dz0*dy1 dx0*dz1 0 */ \
    __ASM_EMIT("vmulps          %" x2 ", %" x4 ", %" x4)        /* x4   = dy1*dx2 dz1*dy2 dx1*dz2 0 */ \
    __ASM_EMIT("vmulps          %" x3 ", %" x2 ", %" x2)        /* x2   = dx2*dy0 dy2*dz0 dz2*dx0 0 */ \
    __ASM_EMIT("vfmsub213ps     %" x4 ", %" x5 ", %" x1)        /* x1   = N1 = (dx1*dy2-dy1*dx2 dy1*dz2-dz1*dy2 dz1*dx2-dx1*dz2 0) */ \
    __ASM_EMIT("vfnmadd231ps    %" x0 ", %" x5 ", %" x2)        /* x2   = N2 = (dx2*dy0-dy2*dx0 dy2*dz0-dz2*dy0 dz2*dx0-dx2*dz0 0)*/ \
    __ASM_EMIT("vsubps          %" x7 ", %" x6 ", %" x0)        /* x0   = N0 = (dx0*dy1-dy0*dx1 dy0*dz1-dz0*dy1 dz0*dx1-dx0*dz1 0) */


/* 3x scalar multiplication
 * Input:
 *   x0 = vector1 [dx0 dy0 dz0 0 ]
 *   x1 = vector2 [dx1 dy1 dz1 0 ]
 *   x2 = vector3 [dx2 dy2 dz2 0 ]
 *
 * Output:
 *   x0 = vector1 * vector2 [ a ? ? ? ]
 *   x1 = vector2 * vector3 [ b ? ? ? ]
 *   x2 = vector3 * vector1 [ c ? ? ? ]
 */
#define VECTOR_DOT3_X3(x0, x1, x2, x3) \
    __ASM_EMIT("vmulps          %" x1 ", %" x0 ", %" x3)        /* x3   = A = dx0*dx1 dy0*dy1 dz0*dz1 0 */ \
    __ASM_EMIT("vmulps          %" x2 ", %" x1 ", %" x1)        /* x1   = B = dx1*dx2 dy1*dy2 dz1*dz2 0 */ \
    __ASM_EMIT("vmulps          %" x0 ", %" x2 ", %" x2)        /* x2   = C = dx2*dx0 dy2*dy0 dz2*dz0 0 */ \
    __ASM_EMIT("vhaddps         %" x3 ", %" x3 ", %" x0)        /* x0   = ax+ay az ax+ay az */ \
    __ASM_EMIT("vhaddps         %" x1 ", %" x1 ", %" x1)        /* x1   = bx+by bz bx+by bz */ \
    __ASM_EMIT("vhaddps         %" x2 ", %" x2 ", %" x2)        /* x2   = cx+cy cz cx+cy cz */ \
    __ASM_EMIT("vhaddps         %" x0 ", %" x0 ", %" x0)        /* x0   = A = ax+ay+az ... */ \
    __ASM_EMIT("vhaddps         %" x1 ", %" x1 ", %" x1)        /* x1   = B = bx+by+bz ... */ \
    __ASM_EMIT("vhaddps         %" x2 ", %" x2 ", %" x2)        /* x2   = C = cx+cy+cz ... */

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
            static const float ONE[] __lsp_aligned32                = { LSP_DSP_VEC8(1.0f) };
            static const uint32_t IONE[] __lsp_aligned16            = { LSP_DSP_VEC4(1) };

            static const float X_3D_MTOLERANCE[] __lsp_aligned16    = { LSP_DSP_VEC4(-DSP_3D_TOLERANCE) };
            static const float X_3D_TOLERANCE[] __lsp_aligned16     = { LSP_DSP_VEC4(DSP_3D_TOLERANCE) };
            static const uint32_t X_MASK0111[] __lsp_aligned16      = { 0xffffffff, 0xffffffff, 0xffffffff, 0 };
            static const uint32_t X_SMASK0001[] __lsp_aligned16     = { 0x80000000, 0, 0, 0 };
            static const uint32_t X_SMASK0010[] __lsp_aligned16     = { 0, 0x80000000, 0, 0 };
            static const uint32_t X_SMASK0100[] __lsp_aligned16     = { 0, 0, 0x80000000, 0 };
            static const uint32_t X_SMASK0111[] __lsp_aligned16     = { 0x80000000, 0x80000000, 0x80000000, 0 };

            static const uint32_t ICULL3[] __lsp_aligned16          = { 0x01, 0x04, 0x10, 0 };
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
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm3")                // xmm1 =?= 0
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
                __ASM_EMIT("vmovups         0x30(%[m]), %%xmm3")            // xmm3 = (m12 m13 m14 m15)
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
                __ASM_EMIT("vmovups         0x30(%[m]), %%xmm3")            // xmm3 = (m12 m13 m14 m15)
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

        float calc_area_p3(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p0]), %[x2]")                /* x2   = p0 = x0 y0 z0 w0 */
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")                /* x1   = p1 = x1 y1 z1 w1 */
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")                /* x2   = p2 = x2 y2 z2 w2 */
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")           /* x0   = p1 - p0 = dx1 dy1 dz1 0 */
                __ASM_EMIT("vsubps          %[x2], %[x1], %[x1]")           /* x1   = p2 - p0 = dx2 dy2 dz2 0 */
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")               /* x0   = NZ NX NY 0 */
                VECTOR_DOT3("[x0]", "[x0]", "[x1]", "[x2]")                 /* x0   = NX*NX + NY*NY + NZ*NZ */
                __ASM_EMIT("vsqrtss         %[x0], %[x0], %[x0]")           /* x0   = sqrtf(NX*NX + NY*NY + NZ*NZ) */
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [p0] "r" (p0), [p1] "r" (p1), [p2] "r" (p2)
                : "memory"
            );

            return x0;
        }

        float calc_area_pv(const point3d_t *pv)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x2]")            /* x0   = p0 = x0 y0 z0 w0 */
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x0]")            /* x1   = p1 = x1 y1 z1 w1 */
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x1]")            /* x2   = p2 = x2 y2 z2 w2 */
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")           /* x0   = p1 - p0 = dx1 dy1 dz1 0 */
                __ASM_EMIT("vsubps          %[x2], %[x1], %[x1]")           /* x1   = p2 - p0 = dx2 dy2 dz2 0 */
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")               /* x0   = NZ NX NY 0 */
                VECTOR_DOT3("[x0]", "[x0]", "[x1]", "[x2]")                 /* x0   = NX*NX + NY*NY + NZ*NZ */
                __ASM_EMIT("vsqrtss         %[x0], %[x0], %[x0]")           /* x0   = sqrtf(NX*NX + NY*NY + NZ*NZ) */

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [pv] "r" (pv)
                : "memory"
            );

            return x0;
        }

        float calc_area_p3_fma3(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p0]), %[x2]")                /* x2   = x0 y0 z0 w0 */
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")                /* x1   = x1 y1 z1 w1 */
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")                /* x2   = x2 y2 z2 w2 */
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")           /* x0   = p1 - p0 = dx1 dy1 dz1 0 */
                __ASM_EMIT("vsubps          %[x2], %[x1], %[x1]")           /* x1   = p2 - p0 = dx2 dy2 dz2 0 */
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")          /* x0   = NZ NX NY 0 */
                VECTOR_DOT3("[x0]", "[x0]", "[x1]", "[x2]")                 /* x0   = NX*NX + NY*NY + NZ*NZ */
                __ASM_EMIT("vsqrtss         %[x0], %[x0], %[x0]")           /* x0   = sqrtf(NX*NX + NY*NY + NZ*NZ) */
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [p0] "r" (p0), [p1] "r" (p1), [p2] "r" (p2)
                : "memory"
            );

            return x0;
        }

        float calc_area_pv_fma3(const point3d_t *pv)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x2]")            /* x0   = x0 y0 z0 w0 */
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x0]")            /* x1   = x1 y1 z1 w1 */
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x1]")            /* x2   = x2 y2 z2 w2 */
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")           /* x0   = p1 - p0 = dx1 dy1 dz1 0 */
                __ASM_EMIT("vsubps          %[x2], %[x1], %[x1]")           /* x1   = p2 - p0 = dx2 dy2 dz2 0 */
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")          /* x0   = NZ NX NY 0 */
                VECTOR_DOT3("[x0]", "[x0]", "[x1]", "[x2]")                 /* x0   = NX*NX + NY*NY + NZ*NZ */
                __ASM_EMIT("vsqrtss         %[x0], %[x0], %[x0]")           /* x0   = sqrtf(NX*NX + NY*NY + NZ*NZ) */

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [pv] "r" (pv)
                : "memory"
            );

            return x0;
        }

        size_t colocation_x2_v1p2(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1)
        {
            float x0, x1, x2;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[pl]), %[x2]")                /* x2   = pl    */
                __ASM_EMIT("vmulps          (%[p0]), %[x2], %[x0]")         /* x0   = x0*px y0*py z0*pz w0*pw */
                __ASM_EMIT("vmulps          (%[p1]), %[x2], %[x1]")         /* x1   = x1*px y1*py z1*pz w1*pw */
                __ASM_EMIT("vhaddps         %[x1], %[x0], %[x0]")           /* x0   = x0*px+y0*py z0*pz+w0*pw x1*px+y1*py z1*pz+w1*pw */
                __ASM_EMIT("vhaddps         %[x0], %[x0], %[x0]")           /* x0   = x1*px+y1*py+z1*pz+w1*pw x1*px+y1*py+z1*pz+w1*pw ? ? = k0 k1 ? ? */

                __ASM_EMIT("vcmpps          $1, %[MTOL], %[x0], %[x1]")     /* x1   = k0 < -TOL  k1 < -TOL ? ? */
                __ASM_EMIT("vcmpps          $2, %[PTOL], %[x0], %[x2]")     /* x2   = k0 <= +TOL k1 <= +TOL ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x1], %[x1]")         /* x1   = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x2], %[x2]")         /* x2   = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
                __ASM_EMIT("vpaddd          %[x1], %[x2], %[x0]")
                __ASM_EMIT("vextractps      $0x00, %[x0], %[p0]")
                __ASM_EMIT("vextractps      $0x01, %[x0], %[p1]")
                __ASM_EMIT("lea             (%[p0], %[p1], 4), %[p0]")
                : [p0] "+r" (p0), [p1] "+r" (p1),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
                : [pl] "r" (pl),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc", "memory"
            );

            return size_t(p0);
        }

        size_t colocation_x2_v1pv(const vector3d_t *pl, const point3d_t *pv)
        {
            float x0, x1, x2;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[pl]), %[x2]")                /* x2   = pl    */
                __ASM_EMIT("vmulps          0x00(%[pv]), %[x2], %[x0]")     /* x0   = x0*px y0*py z0*pz w0*pw */
                __ASM_EMIT("vmulps          0x10(%[pv]), %[x2], %[x1]")     /* x1   = x1*px y1*py z1*pz w1*pw */
                __ASM_EMIT("vhaddps         %[x1], %[x0], %[x0]")           /* x0   = x0*px+y0*py z0*pz+w0*pw x1*px+y1*py z1*pz+w1*pw */
                __ASM_EMIT("vhaddps         %[x0], %[x0], %[x0]")           /* x0   = x1*px+y1*py+z1*pz+w1*pw x1*px+y1*py+z1*pz+w1*pw ? ? = k0 k1 ? ? */

                __ASM_EMIT("vcmpps          $1, %[MTOL], %[x0], %[x1]")     /* x1   = k0 < -TOL  k1 < -TOL ? ? */
                __ASM_EMIT("vcmpps          $2, %[PTOL], %[x0], %[x2]")     /* x2   = k0 <= +TOL k1 <= +TOL ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x1], %[x1]")         /* x1   = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x2], %[x2]")         /* x2   = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
                __ASM_EMIT("vpaddd          %[x1], %[x2], %[x0]")
                __ASM_EMIT("vextractps      $0x00, %[x0], %[pl]")
                __ASM_EMIT("vextractps      $0x01, %[x0], %[pv]")
                __ASM_EMIT("lea             (%[pl], %[pv], 4), %[pl]")
                : [pl] "+r" (pl), [pv] "+r" (pv),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
                : [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc", "memory"
            );

            return size_t(pl);
        }

        size_t colocation_x3_v1p3(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[pl]), %[x3]")                /* x3   = pl    */
                __ASM_EMIT("vmulps          (%[p0]), %[x3], %[x0]")         /* x0   = p0 * pl */
                __ASM_EMIT("vmulps          (%[p1]), %[x3], %[x1]")         /* x1   = p1 * pl */
                __ASM_EMIT("vmulps          (%[p2]), %[x3], %[x2]")         /* x2   = p2 * pl */
                __ASM_EMIT("vhaddps         %[x1], %[x0], %[x0]")
                __ASM_EMIT("vhaddps         %[x3], %[x2], %[x2]")
                __ASM_EMIT("vhaddps         %[x2], %[x0], %[x0]")           /* x0    = k0 k1 k2 ? */
                __ASM_EMIT("vcmpps          $1, %[MTOL], %[x0], %[x1]")     /* x1   = k0 < -TOL  k1 < -TOL ? ? */
                __ASM_EMIT("vcmpps          $2, %[PTOL], %[x0], %[x2]")     /* x2   = k0 <= +TOL k1 <= +TOL ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x1], %[x1]")         /* x1   = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x2], %[x2]")         /* x2   = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
                __ASM_EMIT("vpaddd          %[x1], %[x2], %[x0]")
                __ASM_EMIT("vextractps      $0x00, %[x0], %[p0]")
                __ASM_EMIT("vextractps      $0x01, %[x0], %[p1]")
                __ASM_EMIT("vextractps      $0x02, %[x0], %[p2]")
                __ASM_EMIT("lea             (%[p1], %[p2], 4), %[p1]")
                __ASM_EMIT("lea             (%[p0], %[p1], 4), %[p0]")
                : [p0] "+r" (p0), [p1] "+r" (p1), [p2] "+r" (p2),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [pl] "r" (pl),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc", "memory"
            );

            return size_t(p0);
        }

        size_t colocation_x3_v1pv(const vector3d_t *pl, const point3d_t *pv)
        {
            float x0, x1, x2, x3;
            size_t pt;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[pl]), %[x3]")                /* x3   = pl    */
                __ASM_EMIT("vmulps          0x00(%[pv]), %[x3], %[x0]")     /* x0   = p0 * pl */
                __ASM_EMIT("vmulps          0x10(%[pv]), %[x3], %[x1]")     /* x1   = p1 * pl */
                __ASM_EMIT("vmulps          0x20(%[pv]), %[x3], %[x2]")     /* x2   = p2 * pl */
                __ASM_EMIT("vhaddps         %[x1], %[x0], %[x0]")
                __ASM_EMIT("vhaddps         %[x3], %[x2], %[x2]")
                __ASM_EMIT("vhaddps         %[x2], %[x0], %[x0]")           /* x0   = k0 k1 k2 ? */
                __ASM_EMIT("vcmpps          $1, %[MTOL], %[x0], %[x1]")     /* x1   = k0 < -TOL  k1 < -TOL ? ? */
                __ASM_EMIT("vcmpps          $2, %[PTOL], %[x0], %[x2]")     /* x2   = k0 <= +TOL k1 <= +TOL ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x1], %[x1]")         /* x1   = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x2], %[x2]")         /* x2   = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
                __ASM_EMIT("vpaddd          %[x1], %[x2], %[x0]")
                __ASM_EMIT("vextractps      $0x00, %[x0], %[pt]")
                __ASM_EMIT("vextractps      $0x01, %[x0], %[pl]")
                __ASM_EMIT("vextractps      $0x02, %[x0], %[pv]")
                __ASM_EMIT("lea             (%[pl], %[pv], 4), %[pl]")
                __ASM_EMIT("lea             (%[pt], %[pl], 4), %[pt]")
                : [pl] "+r" (pl), [pv] "+r" (pv), [pt] "=&r" (pt),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc", "memory"
            );

            return pt;
        }

        size_t colocation_x3_v3p1(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p]), %[x3]")                 /* x3   = p       */
                __ASM_EMIT("vmulps          (%[v0]), %[x3], %[x0]")         /* x0   = v0 * p  */
                __ASM_EMIT("vmulps          (%[v1]), %[x3], %[x1]")         /* x1   = v1 * p  */
                __ASM_EMIT("vmulps          (%[v2]), %[x3], %[x2]")         /* x2   = v2 * p  */
                __ASM_EMIT("vhaddps         %[x1], %[x0], %[x0]")
                __ASM_EMIT("vhaddps         %[x3], %[x2], %[x2]")
                __ASM_EMIT("vhaddps         %[x2], %[x0], %[x0]")           /* x0    = k0 k1 k2 ? */
                __ASM_EMIT("vcmpps          $1, %[MTOL], %[x0], %[x1]")     /* x1   = k0 < -TOL  k1 < -TOL ? ? */
                __ASM_EMIT("vcmpps          $2, %[PTOL], %[x0], %[x2]")     /* x2   = k0 <= +TOL k1 <= +TOL ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x1], %[x1]")         /* x1   = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x2], %[x2]")         /* x2   = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
                __ASM_EMIT("vpaddd          %[x1], %[x2], %[x0]")
                __ASM_EMIT("vextractps      $0x00, %[x0], %[v0]")
                __ASM_EMIT("vextractps      $0x01, %[x0], %[v1]")
                __ASM_EMIT("vextractps      $0x02, %[x0], %[v2]")
                __ASM_EMIT("lea             (%[v1], %[v2], 4), %[v1]")
                __ASM_EMIT("lea             (%[v0], %[v1], 4), %[v0]")

                : [v0] "+r" (v0), [v1] "+r" (v1), [v2] "+r" (v2),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [p] "r" (p),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc", "memory"
            );

            return size_t(v0);
        }

        size_t colocation_x3_vvp1(const vector3d_t *vv, const point3d_t *p)
        {
            float x0, x1, x2, x3;
            size_t pt;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p]), %[x3]")                 /* x3   = p       */
                __ASM_EMIT("vmulps          0x00(%[vv]), %[x3], %[x0]")     /* x0   = v0 * p  */
                __ASM_EMIT("vmulps          0x10(%[vv]), %[x3], %[x1]")     /* x1   = v1 * p  */
                __ASM_EMIT("vmulps          0x20(%[vv]), %[x3], %[x2]")     /* x2   = v2 * p  */
                __ASM_EMIT("vhaddps         %[x1], %[x0], %[x0]")
                __ASM_EMIT("vhaddps         %[x3], %[x2], %[x2]")
                __ASM_EMIT("vhaddps         %[x2], %[x0], %[x0]")           /* x0    = k0 k1 k2 ? */
                __ASM_EMIT("vcmpps          $1, %[MTOL], %[x0], %[x1]")     /* x1   = k0 < -TOL  k1 < -TOL ? ? */
                __ASM_EMIT("vcmpps          $2, %[PTOL], %[x0], %[x2]")     /* x2   = k0 <= +TOL k1 <= +TOL ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x1], %[x1]")         /* x1   = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
                __ASM_EMIT("vpand           %[IONE], %[x2], %[x2]")         /* x2   = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
                __ASM_EMIT("vpaddd          %[x1], %[x2], %[x0]")
                __ASM_EMIT("vextractps      $0x00, %[x0], %[pt]")
                __ASM_EMIT("vextractps      $0x01, %[x0], %[vv]")
                __ASM_EMIT("vextractps      $0x02, %[x0], %[p]")
                __ASM_EMIT("lea             (%[vv], %[p], 4), %[vv]")
                __ASM_EMIT("lea             (%[pt], %[vv], 4), %[pt]")

                : [p] "+r" (p), [vv] "+r" (vv), [pt] "=&r" (pt),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc", "memory"
            );

            return pt;
        }

        void split_triangle_raw(
            raw_triangle_t *out,
            size_t *n_out,
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv)
        {
            float    x0, x1, x2, x3, x4, x5, x6, x7;
            float    k[4] __lsp_aligned16;
            size_t   t0;

            in     += *n_in;
            out    += *n_out;

            // Estimate co-location of triangle and plane
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[pl]), %[x3]")                /* x3   = pl    */
                __ASM_EMIT("vmulps          0x00(%[pv]), %[x3], %[x0]")     /* x0   = p0 * pl = sx0 sy0 sz0 sw0 */
                __ASM_EMIT("vmulps          0x10(%[pv]), %[x3], %[x1]")     /* x1   = p1 * pl = sx1 sy1 sz1 sw1 */
                __ASM_EMIT("vmulps          0x20(%[pv]), %[x3], %[x2]")     /* x2   = p2 * pl = sx2 sy2 sz2 sw2 */
                __ASM_EMIT("vhaddps         %[x1], %[x0], %[x0]")           /* x0   = sx0+sy0 sz0+sw0 sx1+sy1 sz1+sw1 */
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")           /* x2   = sx2+sy2 sz2+sw2 sx2+sy2 sz2+sw2 */
                __ASM_EMIT("vhaddps         %[x2], %[x0], %[x0]")           /* x0   = sx0+sy0+sz0+sw0 sx1+sy1+sz1+sw1 sx2+sy2+sz2+sw2 sx2+sy2+sz2+sw2 = k0 k1 k2 k2 */
                __ASM_EMIT("vmovaps         %[x0], (%[k])")                 /* *k   = x0 */
                __ASM_EMIT("vcmpps          $1, %[MTOL], %[x0], %[x1]")     /* x1   = k0 < -TOL   k1 < -TOL   k2 < -TOL   k2 < -TOL */
                __ASM_EMIT("vcmpps          $2, %[PTOL], %[x0], %[x2]")     /* x2   = k0 <= +TOL  k1 <= +TOL  k2 <= +TOL  k2 <= +TOL */
                __ASM_EMIT("vpand           %[ICULL3], %[x1], %[x1]")       /* x1   = 0x01*[k0 < -TOL]   0x04*[k1 < -TOL]   0x10*[k2 < -TOL] 0 */
                __ASM_EMIT("vpand           %[ICULL3], %[x2], %[x2]")       /* x2   = 0x01*[k0 <= +TOL]  0x04*[k1 <= +TOL]  0x10*[k2 <= +TOL] 0 */
                __ASM_EMIT("vpaddd          %[x2], %[x1], %[x0]")           /* x0   = x0 x1 x2 0 */
                __ASM_EMIT("vphaddd         %[x0], %[x0], %[x0]")           /* x0   = x0+x1 x2+0 x0+x1 x2+0 */
                __ASM_EMIT("vphaddd         %[x0], %[x0], %[x0]")           /* x0   = x0+x1+x2+0 x0+x1+x2+0 x0+x1+x2+0 */
                __ASM_EMIT32("vmovd         %[x0], %[t0]")
                __ASM_EMIT64("vmovd         %[x0], %k[t0]")

                : [t0] "=&r" (t0),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4)
                : [pl] "r" (pl), [pv] "r" (pv),
                  [k] "r" (k),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [ICULL3] "m" (ICULL3)
                : "cc", "memory"
            );

            #define STR_COPY_TO(tgt) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("vmovups     0x00(%[src]), %%ymm0") \
                    __ASM_EMIT("vmovups     0x20(%[src]), %%xmm1") \
                    __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst])") \
                    __ASM_EMIT("vmovups     %%xmm1, 0x20(%[dst])") \
                    : \
                    : [dst] "r" (tgt), \
                      [src] "r" (pv) \
                    : "cc", "memory", \
                      "%xmm0", "%xmm1" \
                );

            #define STR_SPLIT_1P(off0, off1, koff, store) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("vmovups     0x" off0 "(%[st]), %[x0]")      /* x0   = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("vmovups     0x" off1 "(%[st]), %[x1]")      /* x1   = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("vmovups     (%[pl]), %[x2]")                /* x2   = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("vsubps      %[x0], %[x1], %[x1]")           /* x1   = d = p1 - p0 = dx dy dz 0 */ \
                    __ASM_EMIT("vbroadcastss    0x" koff "(%[k]), %[x3]")   /* x3   = k k k k */ \
                    __ASM_EMIT("vmulps      %[x1], %[x2], %[x2]")           /* x2   = dx*nx dy*ny dz*nz 0 */ \
                    __ASM_EMIT("vmulps      %[x3], %[x1], %[x1]")           /* x1   = k*dx k*dy k*dz 0 */ \
                    __ASM_EMIT("vhaddps     %[x2], %[x2], %[x2]")           /* x2   = dx*nx+dy*ny dz*nz dx*nx+dy*ny dz*nz */ \
                    __ASM_EMIT("vhaddps     %[x2], %[x2], %[x2]")           /* x2   = dx*nx+dy*ny+dz*nz = b b b b */ \
                    __ASM_EMIT("vdivps      %[x2], %[x1], %[x1]")           /* x1   = k*dx/b k*dy/b k*dz/b 0 */ \
                    __ASM_EMIT("vsubps      %[x1], %[x0], %[x0]")           /* x0   = lx0-k*dx/b ly0-k*dy/b lz0-k*dz/b 1 */ \
                    \
                    store \
                    \
                    : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4) \
                    : [st] "r" (pv), [pl] "r" (pl), [out] "r" (out), [in] "r" (in), \
                      [k] "r" (k) \
                    : "cc", "memory" \
                );

            #define STR_SPLIT_2P(off0, off1, off2, koff, store) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("vmovups     0x" off0 "(%[st]), %[x0]")      /* x0   = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("vmovups     0x" off1 "(%[st]), %[x2]")      /* x2   = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("vmovups     0x" off2 "(%[st]), %[x3]")      /* x3   = p2 = lx2 ly2 lz2 1 */ \
                    __ASM_EMIT("vmovups     (%[pl]), %[x4]")                /* x4   = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("vsubps      %[x0], %[x3], %[x3]")           /* x3   = d2 = p2 - p0 = dx2 dy2 dz2 0 */ \
                    __ASM_EMIT("vsubps      %[x0], %[x2], %[x2]")           /* x2   = d1 = p1 - p0 = dx1 dy1 dz1 0 */ \
                    __ASM_EMIT("vmulps      %[x3], %[x4], %[x5]")           /* x5   = dx2*nx dy2*ny dz2*nz 0 */ \
                    __ASM_EMIT("vmulps      %[x2], %[x4], %[x4]")           /* x4   = dx1*nx dy1*ny dz1*nz 0 */ \
                    __ASM_EMIT("vbroadcastss    0x" koff "(%[k]), %[x6]")   /* x6   = k k k k */ \
                    __ASM_EMIT("vhaddps     %[x5], %[x5], %[x5]")           /* x5   = dx2*nx+dy2*ny dz2*nz dx2*nx+dy2*ny dz2*nz */ \
                    __ASM_EMIT("vhaddps     %[x4], %[x4], %[x4]")           /* x4   = dx1*nx+dy1*ny dz1*nz dx1*nx+dy1*ny dz1*nz */ \
                    __ASM_EMIT("vhaddps     %[x5], %[x5], %[x5]")           /* x5   = dx2*nx+dy2*ny+dz2*nz = b2 b2 b2 b2 */ \
                    __ASM_EMIT("vhaddps     %[x4], %[x4], %[x4]")           /* x4   = dx1*nx+dy1*ny+dz1*nz = b1 b1 b1 b1 */ \
                    __ASM_EMIT("vdivps      %[x5], %[x3], %[x3]")           /* x3   = dx2/b2 dy2/b2 dz2/b2 0 */ \
                    __ASM_EMIT("vdivps      %[x4], %[x2], %[x2]")           /* x2   = dx1/b1 dy1/b1 dz1/b1 0 */ \
                    __ASM_EMIT("vmulps      %[x6], %[x3], %[x3]")           /* x3   = k*dx2/b2 k*dy2/b2 k*dz2/b2 0 */ \
                    __ASM_EMIT("vmulps      %[x6], %[x2], %[x2]")           /* x2   = k*dx1/b1 k*dy1/b1 k*dz1/b1 0 */ \
                    __ASM_EMIT("vsubps      %[x3], %[x0], %[x1]")           /* x1   = lx0-k*dx2/b2 ly0-k*dy2/b2 lz0-k*dz2/b2 1 */ \
                    __ASM_EMIT("vsubps      %[x2], %[x0], %[x0]")           /* x0   = lx0-k*dx1/b1 ly0-k*dy1/b1 lz0-k*dz1/b1 1 */ \
                    \
                    store \
                    \
                    : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), \
                      [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7) \
                    : [st] "r" (pv), [pl] "r" (pl), [out] "r" (out), [in] "r" (in), \
                      [k] "r" (k) \
                    : "cc", "memory" \
                );

            switch (t0)
            {
                // 0 intersections, triangle is above
                case 0x00:  // 0 0 0
                case 0x01:  // 0 0 1
                case 0x04:  // 0 1 0
                case 0x05:  // 0 1 1
                case 0x10:  // 1 0 0
                case 0x11:  // 1 0 1
                case 0x14:  // 1 1 0
                case 0x15:  // 1 1 1
                    STR_COPY_TO(out);
                    ++(*n_out);
                    break;

                // 0 intersections, triangle is below
                case 0x16:  // 1 1 2
                case 0x19:  // 1 2 1
                case 0x1a:  // 1 2 2
                case 0x25:  // 2 1 1
                case 0x26:  // 2 1 2
                case 0x29:  // 2 2 1
                case 0x2a:  // 2 2 2
                    STR_COPY_TO(in);
                    ++(*n_in);
                    break;

                // 1 intersection, 1 triangle above, 1 triangle below, counter-clockwise
                case 0x06:  // 0 1 2
                    STR_SPLIT_1P("00", "20", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x1], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x3], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;
                case 0x21:  // 2 0 1
                    STR_SPLIT_1P("00", "10", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;
                case 0x18:  // 1 2 0
                    STR_SPLIT_1P("10", "20", "04",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x3], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;

                // 1 intersection, 1 triangle above, 1 triangle below, clockwise
                case 0x24:  // 2 1 0
                    STR_SPLIT_1P("00", "20", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x2], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;
                case 0x12:  // 1 0 2
                    STR_SPLIT_1P("00", "10", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x1], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x3], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;
                case 0x09:  // 0 2 1
                    STR_SPLIT_1P("10", "20", "04",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;

                // 2 intersections, 1 triangle below, 2 triangles above
                case 0x02:  // 0 0 2
                    STR_SPLIT_2P("00", "10", "20", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[out])")
                        __ASM_EMIT("vmovups     %[x4], 0x30(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x40(%[out])")
                        __ASM_EMIT("vmovups     %[x3], 0x50(%[out])")
                    );
                    (*n_out) += 2; ++(*n_in);
                    break;
                case 0x08:  // 0 2 0
                    STR_SPLIT_2P("10", "00", "20", "04",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x4], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[out])")
                        __ASM_EMIT("vmovups     %[x2], 0x30(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x40(%[out])")
                        __ASM_EMIT("vmovups     %[x4], 0x50(%[out])")
                    );
                    (*n_out) += 2; ++(*n_in);
                    break;
                case 0x20:  // 2 0 0
                    STR_SPLIT_2P("20", "00", "10", "08",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x4], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[out])")
                        __ASM_EMIT("vmovups     %[x3], 0x30(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x40(%[out])")
                        __ASM_EMIT("vmovups     %[x2], 0x50(%[out])")
                    );
                    (*n_out) += 2; ++(*n_in);
                    break;

                // 2 intersections, 1 triangle above, 2 triangles below
                case 0x28:  // 2 2 0
                    STR_SPLIT_2P("00", "10", "20", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x4], 0x30(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x40(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x50(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[out])")
                    );
                    ++(*n_out); (*n_in) += 2;
                    break;

                case 0x22:  // 2 0 2
                    STR_SPLIT_2P("10", "00", "20", "04",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x4], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x30(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x40(%[in])")
                        __ASM_EMIT("vmovups     %[x4], 0x50(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); (*n_in) += 2;
                    break;

                case 0x0a:  // 0 2 2
                    STR_SPLIT_2P("20", "00", "10", "08",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x30(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x40(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x50(%[in])")
                        __ASM_EMIT("vmovups     %[x4], 0x00(%[out])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[out])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[out])")
                    );
                    ++(*n_out); (*n_in) += 2;
                    break;

                default:
                    break;
            }

            #undef STR_COPY_TO
            #undef STR_SPLIT_1P
            #undef STR_SPLIT_2P
        }

        void cull_triangle_raw(
                raw_triangle_t *in,
                size_t *n_in,
                const vector3d_t *pl,
                const raw_triangle_t *pv
            )
        {
            float    x0, x1, x2, x3, x4, x5, x6, x7;
            float    k[4] __lsp_aligned16;
            size_t   t0;

            in     += *n_in;

            // Estimate co-location of triangle and plane
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[pl]), %[x3]")                /* x3   = pl    */
                __ASM_EMIT("vmulps          0x00(%[pv]), %[x3], %[x0]")     /* x0   = p0 * pl = sx0 sy0 sz0 sw0 */
                __ASM_EMIT("vmulps          0x10(%[pv]), %[x3], %[x1]")     /* x1   = p1 * pl = sx1 sy1 sz1 sw1 */
                __ASM_EMIT("vmulps          0x20(%[pv]), %[x3], %[x2]")     /* x2   = p2 * pl = sx2 sy2 sz2 sw2 */
                __ASM_EMIT("vhaddps         %[x1], %[x0], %[x0]")           /* x0   = sx0+sy0 sz0+sw0 sx1+sy1 sz1+sw1 */
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")           /* x2   = sx2+sy2 sz2+sw2 sx2+sy2 sz2+sw2 */
                __ASM_EMIT("vhaddps         %[x2], %[x0], %[x0]")           /* x0   = sx0+sy0+sz0+sw0 sx1+sy1+sz1+sw1 sx2+sy2+sz2+sw2 sx2+sy2+sz2+sw2 = k0 k1 k2 k2 */
                __ASM_EMIT("vmovaps         %[x0], (%[k])")                 /* *k   = x0 */
                __ASM_EMIT("vcmpps          $1, %[MTOL], %[x0], %[x1]")     /* x1   = k0 < -TOL   k1 < -TOL   k2 < -TOL   k2 < -TOL */
                __ASM_EMIT("vcmpps          $2, %[PTOL], %[x0], %[x2]")     /* x2   = k0 <= +TOL  k1 <= +TOL  k2 <= +TOL  k2 <= +TOL */
                __ASM_EMIT("vpand           %[ICULL3], %[x1], %[x1]")       /* x1   = 0x01*[k0 < -TOL]   0x04*[k1 < -TOL]   0x10*[k2 < -TOL] 0 */
                __ASM_EMIT("vpand           %[ICULL3], %[x2], %[x2]")       /* x2   = 0x01*[k0 <= +TOL]  0x04*[k1 <= +TOL]  0x10*[k2 <= +TOL] 0 */
                __ASM_EMIT("vpaddd          %[x2], %[x1], %[x0]")           /* x0   = x0 x1 x2 0 */
                __ASM_EMIT("vphaddd         %[x0], %[x0], %[x0]")           /* x0   = x0+x1 x2+0 x0+x1 x2+0 */
                __ASM_EMIT("vphaddd         %[x0], %[x0], %[x0]")           /* x0   = x0+x1+x2+0 x0+x1+x2+0 x0+x1+x2+0 */
                __ASM_EMIT32("vmovd         %[x0], %[t0]")
                __ASM_EMIT64("vmovd         %[x0], %k[t0]")

                : [t0] "=&r" (t0),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4)
                : [pl] "r" (pl), [pv] "r" (pv),
                  [k] "r" (k),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [ICULL3] "m" (ICULL3)
                : "cc", "memory"
            );

            #define STR_COPY_TO(tgt) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("vmovups     0x00(%[src]), %%ymm0") \
                    __ASM_EMIT("vmovups     0x20(%[src]), %%xmm1") \
                    __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst])") \
                    __ASM_EMIT("vmovups     %%xmm1, 0x20(%[dst])") \
                    : \
                    : [dst] "r" (tgt), \
                      [src] "r" (pv) \
                    : "cc", "memory", \
                      "%xmm0", "%xmm1" \
                );

            #define STR_SPLIT_1P(off0, off1, koff, store) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("vmovups     0x" off0 "(%[st]), %[x0]")      /* x0   = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("vmovups     0x" off1 "(%[st]), %[x1]")      /* x1   = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("vmovups     (%[pl]), %[x2]")                /* x2   = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("vsubps      %[x0], %[x1], %[x1]")           /* x1   = d = p1 - p0 = dx dy dz 0 */ \
                    __ASM_EMIT("vbroadcastss    0x" koff "(%[k]), %[x3]")   /* x3   = k k k k */ \
                    __ASM_EMIT("vmulps      %[x1], %[x2], %[x2]")           /* x2   = dx*nx dy*ny dz*nz 0 */ \
                    __ASM_EMIT("vmulps      %[x3], %[x1], %[x1]")           /* x1   = k*dx k*dy k*dz 0 */ \
                    __ASM_EMIT("vhaddps     %[x2], %[x2], %[x2]")           /* x2   = dx*nx+dy*ny dz*nz dx*nx+dy*ny dz*nz */ \
                    __ASM_EMIT("vhaddps     %[x2], %[x2], %[x2]")           /* x2   = dx*nx+dy*ny+dz*nz = b b b b */ \
                    __ASM_EMIT("vdivps      %[x2], %[x1], %[x1]")           /* x1   = k*dx/b k*dy/b k*dz/b 0 */ \
                    __ASM_EMIT("vsubps      %[x1], %[x0], %[x0]")           /* x0   = lx0-k*dx/b ly0-k*dy/b lz0-k*dz/b 1 */ \
                    \
                    store \
                    \
                    : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4) \
                    : [st] "r" (pv), [pl] "r" (pl), [in] "r" (in), \
                      [k] "r" (k) \
                    : "cc", "memory" \
                );

            #define STR_SPLIT_2P(off0, off1, off2, koff, store) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("vmovups     0x" off0 "(%[st]), %[x0]")      /* x0   = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("vmovups     0x" off1 "(%[st]), %[x2]")      /* x2   = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("vmovups     0x" off2 "(%[st]), %[x3]")      /* x3   = p2 = lx2 ly2 lz2 1 */ \
                    __ASM_EMIT("vmovups     (%[pl]), %[x4]")                /* x4   = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("vsubps      %[x0], %[x3], %[x3]")           /* x3   = d2 = p2 - p0 = dx2 dy2 dz2 0 */ \
                    __ASM_EMIT("vsubps      %[x0], %[x2], %[x2]")           /* x2   = d1 = p1 - p0 = dx1 dy1 dz1 0 */ \
                    __ASM_EMIT("vmulps      %[x3], %[x4], %[x5]")           /* x5   = dx2*nx dy2*ny dz2*nz 0 */ \
                    __ASM_EMIT("vmulps      %[x2], %[x4], %[x4]")           /* x4   = dx1*nx dy1*ny dz1*nz 0 */ \
                    __ASM_EMIT("vbroadcastss    0x" koff "(%[k]), %[x6]")   /* x6   = k k k k */ \
                    __ASM_EMIT("vhaddps     %[x5], %[x5], %[x5]")           /* x5   = dx2*nx+dy2*ny dz2*nz dx2*nx+dy2*ny dz2*nz */ \
                    __ASM_EMIT("vhaddps     %[x4], %[x4], %[x4]")           /* x4   = dx1*nx+dy1*ny dz1*nz dx1*nx+dy1*ny dz1*nz */ \
                    __ASM_EMIT("vhaddps     %[x5], %[x5], %[x5]")           /* x5   = dx2*nx+dy2*ny+dz2*nz = b2 b2 b2 b2 */ \
                    __ASM_EMIT("vhaddps     %[x4], %[x4], %[x4]")           /* x4   = dx1*nx+dy1*ny+dz1*nz = b1 b1 b1 b1 */ \
                    __ASM_EMIT("vdivps      %[x5], %[x3], %[x3]")           /* x3   = dx2/b2 dy2/b2 dz2/b2 0 */ \
                    __ASM_EMIT("vdivps      %[x4], %[x2], %[x2]")           /* x2   = dx1/b1 dy1/b1 dz1/b1 0 */ \
                    __ASM_EMIT("vmulps      %[x6], %[x3], %[x3]")           /* x3   = k*dx2/b2 k*dy2/b2 k*dz2/b2 0 */ \
                    __ASM_EMIT("vmulps      %[x6], %[x2], %[x2]")           /* x2   = k*dx1/b1 k*dy1/b1 k*dz1/b1 0 */ \
                    __ASM_EMIT("vsubps      %[x3], %[x0], %[x1]")           /* x1   = lx0-k*dx2/b2 ly0-k*dy2/b2 lz0-k*dz2/b2 1 */ \
                    __ASM_EMIT("vsubps      %[x2], %[x0], %[x0]")           /* x0   = lx0-k*dx1/b1 ly0-k*dy1/b1 lz0-k*dz1/b1 1 */ \
                    \
                    store \
                    \
                    : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), \
                      [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7) \
                    : [st] "r" (pv), [pl] "r" (pl), [in] "r" (in), \
                      [k] "r" (k) \
                    : "cc", "memory" \
                );

            switch (t0)
            {
                // 0 intersections, triangle is above
                case 0x00:  // 0 0 0
                case 0x01:  // 0 0 1
                case 0x04:  // 0 1 0
                case 0x05:  // 0 1 1
                case 0x10:  // 1 0 0
                case 0x11:  // 1 0 1
                case 0x14:  // 1 1 0
                case 0x15:  // 1 1 1
                    break;

                // 0 intersections, triangle is below
                case 0x16:  // 1 1 2
                case 0x19:  // 1 2 1
                case 0x1a:  // 1 2 2
                case 0x25:  // 2 1 1
                case 0x26:  // 2 1 2
                case 0x29:  // 2 2 1
                case 0x2a:  // 2 2 2
                    STR_COPY_TO(in);
                    ++(*n_in);
                    break;

                // 1 intersection, 1 triangle above, 1 triangle below, counter-clockwise
                case 0x06:  // 0 1 2
                    STR_SPLIT_1P("00", "20", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     %[x1], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x21:  // 2 0 1
                    STR_SPLIT_1P("00", "10", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x18:  // 1 2 0
                    STR_SPLIT_1P("10", "20", "04",
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;

                // 1 intersection, 1 triangle above, 1 triangle below, clockwise
                case 0x24:  // 2 1 0
                    STR_SPLIT_1P("00", "20", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x12:  // 1 0 2
                    STR_SPLIT_1P("00", "10", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x1], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x09:  // 0 2 1
                    STR_SPLIT_1P("10", "20", "04",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x1]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;

                // 2 intersections, 1 triangle below, 2 triangles above
                case 0x02:  // 0 0 2
                    STR_SPLIT_2P("00", "10", "20", "00",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x08:  // 0 2 0
                    STR_SPLIT_2P("10", "00", "20", "04",
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x20:  // 2 0 0
                    STR_SPLIT_2P("20", "00", "10", "08",
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x4], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;

                // 2 intersections, 1 triangle above, 2 triangles below
                case 0x28:  // 2 2 0
                    STR_SPLIT_2P("00", "10", "20", "00",
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x3], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x4], 0x30(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x40(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x50(%[in])")
                    );
                    (*n_in) += 2;
                    break;

                case 0x22:  // 2 0 2
                    STR_SPLIT_2P("10", "00", "20", "04",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x20(%[st]), %[x4]")
                        __ASM_EMIT("vmovups     %[x4], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x30(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x40(%[in])")
                        __ASM_EMIT("vmovups     %[x4], 0x50(%[in])")
                    );
                    (*n_in) += 2;
                    break;

                case 0x0a:  // 0 2 2
                    STR_SPLIT_2P("20", "00", "10", "08",
                        __ASM_EMIT("vmovups     0x00(%[st]), %[x2]")
                        __ASM_EMIT("vmovups     0x10(%[st]), %[x3]")
                        __ASM_EMIT("vmovups     %[x2], 0x00(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x10(%[in])")
                        __ASM_EMIT("vmovups     %[x0], 0x20(%[in])")
                        __ASM_EMIT("vmovups     %[x3], 0x30(%[in])")
                        __ASM_EMIT("vmovups     %[x1], 0x40(%[in])")
                        __ASM_EMIT("vmovups     %[x2], 0x50(%[in])")
                    );
                    (*n_in) += 2;
                    break;

                default:
                    break;
            }

            #undef STR_COPY_TO
            #undef STR_SPLIT_1P
            #undef STR_SPLIT_2P
        }

        size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3)
        {
            size_t res;
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                // Calc vectors
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")            // x0   = x0 y0 z0 w0
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")            // x1   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         (%[p3]), %[x2]")            // x2   = x2 y2 z2 w2
                __ASM_EMIT("vsubps          %[x0], %[x1], %[x3]")       // x3   = x1-x0 y1-y0 z1-z0 w1-w0 = dx0 dy0 dz0 0
                __ASM_EMIT("vsubps          %[x1], %[x2], %[x1]")       // x1   = x2-x1 y2-y1 z2-z1 w2-w1 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")       // x0   = x0-x2 y0-y2 z0-z2 w0-w2 = dx2 dy2 dz2 0

                __ASM_EMIT("vmulps          %[x3], %[x3], %[x2]")       // x2   = ax2 ay2 az2 0
                __ASM_EMIT("vmulps          %[x1], %[x1], %[x1]")       // x1   = bx2 by2 bz2 0
                __ASM_EMIT("vmulps          %[x0], %[x0], %[x0]")       // x0   = cx2 cy2 cz2 0
                MAT3_TRANSPOSE("x2", "x1", "x0", "x3")
                __ASM_EMIT("vaddps          %[x2], %[x0], %[x3]")       // x3   = ax2+az2 + bx2+bz2 cx2+cz2 ?
                __ASM_EMIT("vaddps          %[x1], %[x0], %[x3]")       // x3   = A2 B2 C2
                __ASM_EMIT("xor             %[res], %[res]")            // res  = 0
                __ASM_EMIT("vunpcklps       %[x3], %[x3], %[x2]")       // x2   = A2 A2 B2 B2
                __ASM_EMIT("vmovhlps        %[x3], %[x0], %[x0]")       // x0   = C2 ? ? ?
                __ASM_EMIT("vmovhlps        %[x2], %[x1], %[x1]")       // x1   = B2 B2 ? ?

                // Solve result
                __ASM_EMIT("vucomiss        %[x1], %[x2]")              // A2 <?> B2
                __ASM_EMIT("jae             1f")
                    // A2 < B2
                    __ASM_EMIT("vucomiss        %[x1], %[x0]")          // C2 <?> B2
                    __ASM_EMIT("seta            %%al")                  // res = (B2 >= C2) ? 0 : 1
                    __ASM_EMIT("add             $1, %[res]")            // res = (B2 >= C2) ? 1 : 2
                    __ASM_EMIT("jmp             2f")
                __ASM_EMIT("1:")
                    // A2 >= B2
                    __ASM_EMIT("vucomiss        %[x0], %[x2]")          // A2 <?> C2
                    __ASM_EMIT("jae             2f")                    //
                    __ASM_EMIT("mov             $2, %[res]")            // res = (A2 >= C2) ? 0 : 2
                __ASM_EMIT("2:")
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [res] "=a" (res)
                : [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3)
            );

            return res;
        }

        size_t longest_edge3d_pv(const point3d_t *p)
        {
            size_t res;
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                // Calc vectors
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x0]")        // x0   = x0 y0 z0 w0
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x1]")        // x1   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x2]")        // x2   = x2 y2 z2 w2
                __ASM_EMIT("vsubps          %[x0], %[x1], %[x3]")       // x3   = x1-x0 y1-y0 z1-z0 w1-w0 = dx0 dy0 dz0 0
                __ASM_EMIT("vsubps          %[x1], %[x2], %[x1]")       // x1   = x2-x1 y2-y1 z2-z1 w2-w1 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")       // x0   = x0-x2 y0-y2 z0-z2 w0-w2 = dx2 dy2 dz2 0

                __ASM_EMIT("vmulps          %[x3], %[x3], %[x2]")       // x2   = ax2 ay2 az2 0
                __ASM_EMIT("vmulps          %[x1], %[x1], %[x1]")       // x1   = bx2 by2 bz2 0
                __ASM_EMIT("vmulps          %[x0], %[x0], %[x0]")       // x0   = cx2 cy2 cz2 0
                MAT3_TRANSPOSE("x2", "x1", "x0", "x3")
                __ASM_EMIT("vaddps          %[x2], %[x0], %[x3]")       // x3   = ax2+az2 + bx2+bz2 cx2+cz2 ?
                __ASM_EMIT("vaddps          %[x1], %[x0], %[x3]")       // x3   = A2 B2 C2
                __ASM_EMIT("xor             %[res], %[res]")            // res  = 0
                __ASM_EMIT("vunpcklps       %[x3], %[x3], %[x2]")       // x2   = A2 A2 B2 B2
                __ASM_EMIT("vmovhlps        %[x3], %[x0], %[x0]")       // x0   = C2 ? ? ?
                __ASM_EMIT("vmovhlps        %[x2], %[x1], %[x1]")       // x1   = B2 B2 ? ?

                // Solve result
                __ASM_EMIT("vucomiss        %[x1], %[x2]")              // A2 <?> B2
                __ASM_EMIT("jae             1f")
                    // A2 < B2
                    __ASM_EMIT("vucomiss        %[x1], %[x0]")          // C2 <?> B2
                    __ASM_EMIT("seta            %%al")                  // res = (B2 >= C2) ? 0 : 1
                    __ASM_EMIT("add             $1, %[res]")            // res = (B2 >= C2) ? 1 : 2
                    __ASM_EMIT("jmp             2f")
                __ASM_EMIT("1:")
                    // A2 >= B2
                    __ASM_EMIT("vucomiss        %[x0], %[x2]")          // A2 <?> C2
                    __ASM_EMIT("jae             2f")                    //
                    __ASM_EMIT("mov             $2, %[res]")            // res = (A2 >= C2) ? 0 : 2
                __ASM_EMIT("2:")
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [res] "=a" (res)
                : [pv] "r" (p)
            );

            return res;
        }

        void calc_normal3d_v2(vector3d_t *n, const vector3d_t *v1, const vector3d_t *v2)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[v1]), %[x0]")            // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vmovups         (%[v2]), %[x1]")            // x1   = dx2 dy2 dz2 dw2
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")           // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = W = sqrtf(W2)
                __ASM_EMIT("vcmpps          $4, %[x3], %[x0], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W NZ/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x0]")       // x0   = (W != 0) ? NX/W NY/W NZ/W NZ/W : 0
                __ASM_EMIT("vmovups         %[x0], (%[n])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [n] "r" (n), [v1] "r" (v1), [v2] "r" (v2),
                  [X_3DMASK] "m" (X_MASK0111)
                : "memory"
            );
        }

        void calc_normal3d_vv(vector3d_t *n, const vector3d_t *vv)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[vv]), %[x0]")        // x0   = dx1 dy1 dz1 0
                __ASM_EMIT("vmovups         0x10(%[vv]), %[x1]")        // x1   = dx2 dy2 dz2 0
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")           // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = W = sqrtf(W2)
                __ASM_EMIT("vcmpps          $4, %[x3], %[x0], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W NZ/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x0]")       // x0   = (W != 0) ? NX/W NY/W NZ/W NZ/W : 0
                __ASM_EMIT("vmovups         %[x0], (%[n])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [n] "r" (n), [vv] "r" (vv)
                : "memory"
            );
        }

        void calc_normal3d_p3(vector3d_t *n, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")            // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vmovups         (%[p3]), %[x2]")            // x1   = dx2 dy2 dz2 dw2
                __ASM_EMIT("vsubps          (%[p1]), %[x1], %[x0]")     // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vsubps          %[x1], %[x2], %[x1]")       // x1   = dx2 dy2 dz2 dw2
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")           // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = W = sqrtf(W2)
                __ASM_EMIT("vcmpps          $4, %[x3], %[x0], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W NZ/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x0]")       // x0   = (W != 0) ? NX/W NY/W NZ/W NZ/W : 0
                __ASM_EMIT("vmovups         %[x0], (%[n])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [n] "r" (n), [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3),
                  [X_3DMASK] "m" (X_MASK0111)
                : "memory"
            );
        }

        void calc_normal3d_pv(vector3d_t *n, const point3d_t *pv)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x1]")        // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x2]")        // x1   = dx2 dy2 dz2 dw2
                __ASM_EMIT("vsubps          0x00(%[pv]), %[x1], %[x0]") // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vsubps          %[x1], %[x2], %[x1]")       // x1   = dx2 dy2 dz2 dw2
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")           // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = W = sqrtf(W2)
                __ASM_EMIT("vcmpps          $4, %[x3], %[x0], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W NZ/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x0]")       // x0   = (W != 0) ? NX/W NY/W NZ/W NZ/W : 0
                __ASM_EMIT("vmovups         %[x0], (%[n])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [n] "r" (n), [pv] "r" (pv),
                  [X_3DMASK] "m" (X_MASK0111)
                : "memory"
            );
        }

        void calc_normal3d_v2_fma3(vector3d_t *n, const vector3d_t *v1, const vector3d_t *v2)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[v1]), %[x0]")            // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vmovups         (%[v2]), %[x1]")            // x1   = dx2 dy2 dz2 dw2
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")      // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = W = sqrtf(W2)
                __ASM_EMIT("vcmpps          $4, %[x3], %[x0], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W NZ/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x0]")       // x0   = (W != 0) ? NX/W NY/W NZ/W NZ/W : 0
                __ASM_EMIT("vmovups         %[x0], (%[n])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [n] "r" (n), [v1] "r" (v1), [v2] "r" (v2),
                  [X_3DMASK] "m" (X_MASK0111)
                : "memory"
            );
        }

        void calc_normal3d_vv_fma3(vector3d_t *n, const vector3d_t *vv)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[vv]), %[x0]")        // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vmovups         0x10(%[vv]), %[x1]")        // x1   = dx2 dy2 dz2 dw2
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")      // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = W = sqrtf(W2)
                __ASM_EMIT("vcmpps          $4, %[x3], %[x0], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W NZ/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x0]")       // x0   = (W != 0) ? NX/W NY/W NZ/W NZ/W : 0
                __ASM_EMIT("vmovups         %[x0], (%[n])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [n] "r" (n), [vv] "r" (vv),
                  [X_3DMASK] "m" (X_MASK0111)
                : "memory"
            );
        }

        void calc_normal3d_p3_fma3(vector3d_t *n, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")            // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vmovups         (%[p3]), %[x2]")            // x1   = dx2 dy2 dz2 dw2
                __ASM_EMIT("vsubps          (%[p1]), %[x1], %[x0]")     // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vsubps          %[x1], %[x2], %[x1]")       // x1   = dx2 dy2 dz2 dw2
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")      // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = W = sqrtf(W2)
                __ASM_EMIT("vcmpps          $4, %[x3], %[x0], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W NZ/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x0]")       // x0   = (W != 0) ? NX/W NY/W NZ/W NZ/W : 0
                __ASM_EMIT("vmovups         %[x0], (%[n])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [n] "r" (n), [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3),
                  [X_3DMASK] "m" (X_MASK0111)
                : "memory"
            );
        }

        void calc_normal3d_pv_fma3(vector3d_t *n, const point3d_t *pv)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x1]")        // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x2]")        // x1   = dx2 dy2 dz2 dw2
                __ASM_EMIT("vsubps          0x00(%[pv]), %[x1], %[x0]") // x0   = dx1 dy1 dz1 dw1
                __ASM_EMIT("vsubps          %[x1], %[x2], %[x1]")       // x1   = dx2 dy2 dz2 dw2
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")      // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = W = sqrtf(W2)
                __ASM_EMIT("vcmpps          $4, %[x3], %[x0], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W NZ/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x0]")       // x0   = (W != 0) ? NX/W NY/W NZ/W NZ/W : 0
                __ASM_EMIT("vmovups         %[x0], (%[n])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [n] "r" (n), [pv] "r" (pv),
                  [X_3DMASK] "m" (X_MASK0111)
                : "memory"
            );
        }

        float calc_plane_p3(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p0]), %[x2]")            // x2   = x0 y0 z0 w0
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")            // x0   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")            // x1   = x2 y2 z2 w2
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")       // x0   = p1 - p0 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x2], %[x1], %[x1]")       // x1   = p2 - p0 = dx2 dy2 dz2 0
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")           // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W
                __ASM_EMIT("vcmpps          $4, %[x0], %[x3], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W 0/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x1]")       // x1   = nx ny nz 0 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz
                __ASM_EMIT("vmovups         (%[p0]), %[x3]")            // x3   = x0 y0 z0 w0
                __ASM_EMIT("vxorps          %[X_ISIGN], %[x1], %[x2]")  // x2   = -nx -ny -nz 0
                VECTOR_DOT3("[x2]", "[x3]", "[x3]", "[x4]")             // x2   = -(nx*x0 + ny*y0 + nz*z0) = dw
                __ASM_EMIT("vinsertps       $0x30, %[x2], %[x1], %[x1]")// x1   = nx ny nz dw
                __ASM_EMIT("vmovups         %[x1], (%[v])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v), [p0] "r" (p0), [p1] "r" (p1), [p2] "r" (p2),
                  [X_ISIGN] "m" (X_SMASK0111)
                : "memory"
            );

            return x0;
        }

        float calc_plane_pv(vector3d_t *v, const point3d_t *pv)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x2]")        // x2   = x0 y0 z0 w0
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x0]")        // x0   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x1]")        // x1   = x2 y2 z2 w2
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")       // x0   = p1 - p0 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x2], %[x1], %[x1]")       // x1   = p2 - p0 = dx2 dy2 dz2 0
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")           // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W
                __ASM_EMIT("vcmpps          $4, %[x0], %[x3], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W 0/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x1]")       // x1   = nx ny nz 0 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x3]")        // x3   = x0 y0 z0 w0
                __ASM_EMIT("vxorps          %[X_ISIGN], %[x1], %[x2]")  // x2   = -nx -ny -nz 0
                VECTOR_DOT3("[x2]", "[x3]", "[x3]", "[x4]")             // x2   = -(nx*x0 + ny*y0 + nz*z0) = dw
                __ASM_EMIT("vinsertps       $0x30, %[x2], %[x1], %[x1]")// x1   = nx ny nz dw
                __ASM_EMIT("vmovups         %[x1], (%[v])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v), [pv] "r" (pv),
                  [X_ISIGN] "m" (X_SMASK0111)
                : "memory"
            );

            return x0;
        }

        float calc_plane_v1p2(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")            // x0   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         (%[v0]), %[x1]")            // x1   = v = dx2 dy2 dz2 0
                __ASM_EMIT("vsubps          (%[p0]), %[x0], %[x0]")     // x0   = p1 - p0 = dx1 dy1 dz1 0
                VECTOR_CROSS3("[x0]", "[x1]", "[x2]", "[x3]")           // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W
                __ASM_EMIT("vcmpps          $4, %[x0], %[x3], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W 0/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x1]")       // x1   = nx ny nz 0 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz
                __ASM_EMIT("vmovups         (%[p0]), %[x3]")            // x3   = x0 y0 z0 w0
                __ASM_EMIT("vxorps          %[X_ISIGN], %[x1], %[x2]")  // x2   = -nx -ny -nz 0
                VECTOR_DOT3("[x2]", "[x3]", "[x3]", "[x4]")             // x2   = -(nx*x0 + ny*y0 + nz*z0) = dw
                __ASM_EMIT("vinsertps       $0x30, %[x2], %[x1], %[x1]")// x1   = nx ny nz dw
                __ASM_EMIT("vmovups         %[x1], (%[v])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v), [v0] "r" (v0), [p0] "r" (p0), [p1] "r" (p1),
                  [X_ISIGN] "m" (X_SMASK0111)
                : "memory"
            );

            return x0;
        }

        float calc_plane_p3_fma3(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p0]), %[x2]")            // x2   = x0 y0 z0 w0
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")            // x0   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")            // x1   = x2 y2 z2 w2
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")       // x0   = p1 - p0 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x2], %[x1], %[x1]")       // x1   = p2 - p0 = dx2 dy2 dz2 0
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")      // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W
                __ASM_EMIT("vcmpps          $4, %[x0], %[x3], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W 0/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x1]")       // x1   = nx ny nz 0 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz
                __ASM_EMIT("vmovups         (%[p0]), %[x3]")            // x3   = x0 y0 z0 w0
                __ASM_EMIT("vxorps          %[X_ISIGN], %[x1], %[x2]")  // x2   = -nx -ny -nz 0
                VECTOR_DOT3("[x2]", "[x3]", "[x3]", "[x4]")             // x2   = -(nx*x0 + ny*y0 + nz*z0) = dw
                __ASM_EMIT("vinsertps       $0x30, %[x2], %[x1], %[x1]")// x1   = nx ny nz dw
                __ASM_EMIT("vmovups         %[x1], (%[v])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v), [p0] "r" (p0), [p1] "r" (p1), [p2] "r" (p2),
                  [X_ISIGN] "m" (X_SMASK0111)
                : "memory"
            );

            return x0;
        }

        float calc_plane_pv_fma3(vector3d_t *v, const point3d_t *pv)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x2]")        // x2   = x0 y0 z0 w0
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x0]")        // x0   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x1]")        // x1   = x2 y2 z2 w2
                __ASM_EMIT("vsubps          %[x2], %[x0], %[x0]")       // x0   = p1 - p0 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x2], %[x1], %[x1]")       // x1   = p2 - p0 = dx2 dy2 dz2 0
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")      // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W
                __ASM_EMIT("vcmpps          $4, %[x0], %[x3], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W 0/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x1]")       // x1   = nx ny nz 0 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x3]")        // x3   = x0 y0 z0 w0
                __ASM_EMIT("vxorps          %[X_ISIGN], %[x1], %[x2]")  // x2   = -nx -ny -nz 0
                VECTOR_DOT3("[x2]", "[x3]", "[x3]", "[x4]")             // x2   = -(nx*x0 + ny*y0 + nz*z0) = dw
                __ASM_EMIT("vinsertps       $0x30, %[x2], %[x1], %[x1]")// x1   = nx ny nz dw
                __ASM_EMIT("vmovups         %[x1], (%[v])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v), [pv] "r" (pv),
                  [X_ISIGN] "m" (X_SMASK0111)
                : "memory"
            );

            return x0;
        }

        float calc_plane_v1p2_fma3(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")            // x0   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         (%[v0]), %[x1]")            // x1   = v = dx2 dy2 dz2 0
                __ASM_EMIT("vsubps          (%[p0]), %[x0], %[x0]")     // x0   = p1 - p0 = dx1 dy1 dz1 0
                VECTOR_CROSS3_FMA3("[x0]", "[x1]", "[x2]", "[x3]")      // x0   = NZ NX NY 0
                __ASM_EMIT("vshufps         $0xc9, %[x0], %[x0], %[x1]")// x1   = NX NY NZ 0
                VECTOR_DOT3("[x0]", "[x0]", "[x2]", "[x3]")             // x0   = NX*NX + NY*NY + NZ*NZ = W2
                __ASM_EMIT("vshufps         $0x00, %[x0], %[x0], %[x0]")// x0   = W2 W2 W2 W2
                __ASM_EMIT("vxorps          %[x3], %[x3], %[x3]")       // x3   = 0
                __ASM_EMIT("vsqrtps         %[x0], %[x0]")              // x0   = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W
                __ASM_EMIT("vcmpps          $4, %[x0], %[x3], %[x3]")   // x3   = W != 0
                __ASM_EMIT("vdivps          %[x0], %[x1], %[x1]")       // x1   = NX/W NY/W NZ/W 0/W
                __ASM_EMIT("vandps          %[x3], %[x1], %[x1]")       // x1   = nx ny nz 0 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz
                __ASM_EMIT("vmovups         (%[p0]), %[x3]")            // x3   = x0 y0 z0 w0
                __ASM_EMIT("vxorps          %[X_ISIGN], %[x1], %[x2]")  // x2   = -nx -ny -nz 0
                VECTOR_DOT3("[x2]", "[x3]", "[x3]", "[x4]")             // x2   = -(nx*x0 + ny*y0 + nz*z0) = dw
                __ASM_EMIT("vinsertps       $0x30, %[x2], %[x1], %[x1]")// x1   = nx ny nz dw
                __ASM_EMIT("vmovups         %[x1], (%[v])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v), [v0] "r" (v0), [p0] "r" (p0), [p1] "r" (p1),
                  [X_ISIGN] "m" (X_SMASK0111)
                : "memory"
            );

            return x0;
        }

        void calc_split_point_p2v1(point3d_t *sp, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[l0]), %[x1]")            // x1   = l0 = lx0 ly0 lz0 1
                __ASM_EMIT("vmovups         (%[l1]), %[x0]")            // x0   = l1 = lx1 ly1 lz1 1
                __ASM_EMIT("vmovups         (%[pl]), %[x2]")            // x2   = pl = nx ny nz nw
                __ASM_EMIT("vsubps          %[x1], %[x0], %[x0]")       // x0   = d = l1 - l0 = dx dy dz 0
                __ASM_EMIT("vmulps          %[x2], %[x1], %[x3]")       // x3   = nx*lx0 ny*ly0 nz*lz0 nw */
                __ASM_EMIT("vmulps          %[x0], %[x2], %[x2]")       // x2   = nx*dx ny*dy nz*dz 0 */
                __ASM_EMIT("vhaddps         %[x3], %[x3], %[x3]")       // x3   = nx*lx0+ny*ly0 nz*lz0+nw nx*lx0+ny*ly0 nz*lz0+nw
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")       // x2   = nx*dx+ny*dy nz*dz nx*dx+ny*dy nz*dz
                __ASM_EMIT("vhaddps         %[x3], %[x3], %[x3]")       // x3   = nx*lx0+ny*ly0+nz*lz0+nw = T T T T
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")       // x2   = nx*dx+ny*dy nz*dz B B B B
                __ASM_EMIT("vdivps          %[x2], %[x3], %[x3]")       // x3   = T/B T/B T/B T/B = W
                __ASM_EMIT("vmulps          %[x3], %[x0], %[x0]")       // x0   = dx*W dy*W dz*W 0
                __ASM_EMIT("vsubps          %[x0], %[x1], %[x1]")       // x1   = lx0-dx*W ly0-dy*W lz0-dz*W 1
                __ASM_EMIT("vmovups         %[x1], (%[sp])")
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [sp] "r" (sp), [l0] "r" (l0), [l1] "r" (l1), [pl] "r" (pl)
                : "memory"
            );
        }

        void calc_split_point_pvv1(point3d_t *sp, const point3d_t *lv, const vector3d_t *pl)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[lv]), %[x1]")        // x1   = l0 = lx0 ly0 lz0 1
                __ASM_EMIT("vmovups         0x10(%[lv]), %[x0]")        // x0   = l1 = lx1 ly1 lz1 1
                __ASM_EMIT("vmovups         (%[pl]), %[x2]")            // x2   = pl = nx ny nz nw
                __ASM_EMIT("vsubps          %[x1], %[x0], %[x0]")       // x0   = d = l1 - l0 = dx dy dz 0
                __ASM_EMIT("vmulps          %[x2], %[x1], %[x3]")       // x3   = nx*lx0 ny*ly0 nz*lz0 nw */
                __ASM_EMIT("vmulps          %[x0], %[x2], %[x2]")       // x2   = nx*dx ny*dy nz*dz 0 */
                __ASM_EMIT("vhaddps         %[x3], %[x3], %[x3]")       // x3   = nx*lx0+ny*ly0 nz*lz0+nw nx*lx0+ny*ly0 nz*lz0+nw
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")       // x2   = nx*dx+ny*dy nz*dz nx*dx+ny*dy nz*dz
                __ASM_EMIT("vhaddps         %[x3], %[x3], %[x3]")       // x3   = nx*lx0+ny*ly0+nz*lz0+nw = T T T T
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")       // x2   = nx*dx+ny*dy nz*dz B B B B
                __ASM_EMIT("vdivps          %[x2], %[x3], %[x3]")       // x3   = T/B T/B T/B T/B = W
                __ASM_EMIT("vmulps          %[x3], %[x0], %[x0]")       // x0   = dx*W dy*W dz*W 0
                __ASM_EMIT("vsubps          %[x0], %[x1], %[x1]")       // x1   = lx0-dx*W ly0-dy*W lz0-dz*W 1
                __ASM_EMIT("vmovups         %[x1], (%[sp])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [sp] "r" (sp), [lv] "r" (lv), [pl] "r" (pl)
                : "cc", "memory"
            );
        }

        void calc_split_point_p2v1_fma3(point3d_t *sp, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[l0]), %[x1]")            // x1   = l0 = lx0 ly0 lz0 1
                __ASM_EMIT("vmovups         (%[l1]), %[x0]")            // x0   = l1 = lx1 ly1 lz1 1
                __ASM_EMIT("vmovups         (%[pl]), %[x2]")            // x2   = pl = nx ny nz nw
                __ASM_EMIT("vsubps          %[x1], %[x0], %[x0]")       // x0   = d = l1 - l0 = dx dy dz 0
                __ASM_EMIT("vmulps          %[x2], %[x1], %[x3]")       // x3   = nx*lx0 ny*ly0 nz*lz0 nw */
                __ASM_EMIT("vmulps          %[x0], %[x2], %[x2]")       // x2   = nx*dx ny*dy nz*dz 0 */
                __ASM_EMIT("vhaddps         %[x3], %[x3], %[x3]")       // x3   = nx*lx0+ny*ly0 nz*lz0+nw nx*lx0+ny*ly0 nz*lz0+nw
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")       // x2   = nx*dx+ny*dy nz*dz nx*dx+ny*dy nz*dz
                __ASM_EMIT("vhaddps         %[x3], %[x3], %[x3]")       // x3   = nx*lx0+ny*ly0+nz*lz0+nw = T T T T
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")       // x2   = nx*dx+ny*dy nz*dz B B B B
                __ASM_EMIT("vdivps          %[x2], %[x3], %[x3]")       // x3   = T/B T/B T/B T/B = W
                __ASM_EMIT("vfnmadd231ps    %[x3], %[x0], %[x1]")       // x1   = lx0-dx*W ly0-dy*W lz0-dz*W 1
                __ASM_EMIT("vmovups         %[x1], (%[sp])")
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [sp] "r" (sp), [l0] "r" (l0), [l1] "r" (l1), [pl] "r" (pl)
                : "memory"
            );
        }

        void calc_split_point_pvv1_fma3(point3d_t *sp, const point3d_t *lv, const vector3d_t *pl)
        {
            float x0, x1, x2, x3;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[lv]), %[x1]")        // x1   = l0 = lx0 ly0 lz0 1
                __ASM_EMIT("vmovups         0x10(%[lv]), %[x0]")        // x0   = l1 = lx1 ly1 lz1 1
                __ASM_EMIT("vmovups         (%[pl]), %[x2]")            // x2   = pl = nx ny nz nw
                __ASM_EMIT("vsubps          %[x1], %[x0], %[x0]")       // x0   = d = l1 - l0 = dx dy dz 0
                __ASM_EMIT("vmulps          %[x2], %[x1], %[x3]")       // x3   = nx*lx0 ny*ly0 nz*lz0 nw */
                __ASM_EMIT("vmulps          %[x0], %[x2], %[x2]")       // x2   = nx*dx ny*dy nz*dz 0 */
                __ASM_EMIT("vhaddps         %[x3], %[x3], %[x3]")       // x3   = nx*lx0+ny*ly0 nz*lz0+nw nx*lx0+ny*ly0 nz*lz0+nw
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")       // x2   = nx*dx+ny*dy nz*dz nx*dx+ny*dy nz*dz
                __ASM_EMIT("vhaddps         %[x3], %[x3], %[x3]")       // x3   = nx*lx0+ny*ly0+nz*lz0+nw = T T T T
                __ASM_EMIT("vhaddps         %[x2], %[x2], %[x2]")       // x2   = nx*dx+ny*dy nz*dz B B B B
                __ASM_EMIT("vdivps          %[x2], %[x3], %[x3]")       // x3   = T/B T/B T/B T/B = W
                __ASM_EMIT("vfnmadd231ps    %[x3], %[x0], %[x1]")       // x1   = lx0-dx*W ly0-dy*W lz0-dz*W 1
                __ASM_EMIT("vmovups         %[x1], (%[sp])")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [sp] "r" (sp), [lv] "r" (lv), [pl] "r" (pl)
                : "cc", "memory"
            );
        }

        float check_triplet3d_p3n(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p2]), %[x2]")            // x2   = p2 = px2 py2 pz2 1
                __ASM_EMIT("vmovups         (%[p3]), %[x3]")            // x3   = p3 = px3 py3 pz3 1
                __ASM_EMIT("vsubps          (%[p1]), %[x2], %[x1]")     // x1   = p2 - p1
                __ASM_EMIT("vmovups         (%[n]), %[x0]")             // x0   = nx ny nz 0
                __ASM_EMIT("vsubps          %[x2], %[x3], %[x2]")       // x2   = p3 - p2

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3), [n] "r" (n)
            );

            return x0;
        }

        float check_triplet3d_pvn(const point3d_t *pv, const vector3d_t *n)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x2]")        // x2   = p2 = px2 py2 pz2 1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x3]")        // x3   = p3 = px3 py3 pz3 1
                __ASM_EMIT("vsubps          0x00(%[pv]), %[x2], %[x1]") // x1   = p2 - p1
                __ASM_EMIT("vmovups         (%[n]), %[x0]")             // x0   = nx ny nz nw
                __ASM_EMIT("vsubps          %[x2], %[x3], %[x2]")       // x2   = p3 - p2

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [pv] "r" (pv), [n] "r" (n)
            );

            return x0;
        }

        float check_triplet3d_v2n(const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *n)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[v1]), %[x1]")            // x1   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         (%[v2]), %[x2]")            // x2   = x2 y2 z2 w2
                __ASM_EMIT("vmovups         (%[n]), %[x0]")             // x0   = nx ny nz nw

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v1] "r" (v1), [v2] "r" (v2), [n] "r" (n)
            );

            return x0;
        }

        float check_triplet3d_vvn(const vector3d_t *v, const vector3d_t *n)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[v]), %[x1]")         // x1   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         0x10(%[v]), %[x2]")         // x2   = x2 y2 z2 w2
                __ASM_EMIT("vmovups         (%[n]), %[x0]")             // x0   = nx ny nz nw

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v), [n] "r" (n)
            );

            return x0;
        }

        float check_triplet3d_vv(const vector3d_t *v)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups      0x00(%[v]), %[x1]")            // x1   = x1 y1 z1 w1
                __ASM_EMIT("vmovups      0x10(%[v]), %[x2]")            // x2   = x2 y2 z2 w2
                __ASM_EMIT("vmovups      0x20(%[v]), %[x0]")            // x0   = nx ny nz nw

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v)
            );

            return x0;
        }

        float check_triplet3d_p3n_fma3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[p2]), %[x2]")            // x2   = p2 = px2 py2 pz2 1
                __ASM_EMIT("vmovups         (%[p3]), %[x3]")            // x3   = p3 = px3 py3 pz3 1
                __ASM_EMIT("vsubps          (%[p1]), %[x2], %[x1]")     // x1   = p2 - p1
                __ASM_EMIT("vmovups         (%[n]), %[x0]")             // x0   = nx ny nz 0
                __ASM_EMIT("vsubps          %[x2], %[x3], %[x2]")       // x2   = p3 - p2

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3), [n] "r" (n)
            );

            return x0;
        }

        float check_triplet3d_pvn_fma3(const point3d_t *pv, const vector3d_t *n)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x2]")        // x2   = p2 = px2 py2 pz2 1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x3]")        // x3   = p3 = px3 py3 pz3 1
                __ASM_EMIT("vsubps          0x00(%[pv]), %[x2], %[x1]") // x1   = p2 - p1
                __ASM_EMIT("vmovups         (%[n]), %[x0]")             // x0   = nx ny nz nw
                __ASM_EMIT("vsubps          %[x2], %[x3], %[x2]")       // x2   = p3 - p2

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [pv] "r" (pv), [n] "r" (n)
            );

            return x0;
        }

        float check_triplet3d_v2n_fma3(const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *n)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         (%[v1]), %[x1]")            // x1   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         (%[v2]), %[x2]")            // x2   = x2 y2 z2 w2
                __ASM_EMIT("vmovups         (%[n]), %[x0]")             // x0   = nx ny nz nw

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v1] "r" (v1), [v2] "r" (v2), [n] "r" (n)
            );

            return x0;
        }

        float check_triplet3d_vvn_fma3(const vector3d_t *v, const vector3d_t *n)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups         0x00(%[v]), %[x1]")         // x1   = x1 y1 z1 w1
                __ASM_EMIT("vmovups         0x10(%[v]), %[x2]")         // x2   = x2 y2 z2 w2
                __ASM_EMIT("vmovups         (%[n]), %[x0]")             // x0   = nx ny nz nw

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v), [n] "r" (n)
            );

            return x0;
        }

        float check_triplet3d_vv_fma3(const vector3d_t *v)
        {
            float x0, x1, x2, x3, x4;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovups      0x00(%[v]), %[x1]")            // x1   = x1 y1 z1 w1
                __ASM_EMIT("vmovups      0x10(%[v]), %[x2]")            // x2   = x2 y2 z2 w2
                __ASM_EMIT("vmovups      0x20(%[v]), %[x0]")            // x0   = nx ny nz nw

                MIXED_PRODUCT3("x0", "x1", "x2", "x3", "x4")

                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
                : [v] "r" (v)
            );

            return x0;
        }

        float check_point3d_on_triangle_pvp(const point3d_t *pv, const point3d_t *p)
        {
            float x0, x1, x2, x3, x4, x5, x6, x7;

            ARCH_X86_ASM
            (
                /* Load vectors */
                __ASM_EMIT("vmovups         (%[p]), %[x3]")             // x3   = px py pz 1
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x0]")        // x0   = x0 y0 z0 1
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x1]")        // x1   = x1 y1 z1 1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x2]")        // x2   = x2 y2 z2 1
                __ASM_EMIT("vsubps          %[x3], %[x0], %[x0]")       // x0   = v0 = dx0 dy0 dz0 0
                __ASM_EMIT("vsubps          %[x3], %[x1], %[x1]")       // x1   = v1 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x3], %[x2], %[x2]")       // x2   = v2 = dx2 dy2 dz2 0
                /* 3x vector multiplications */
                VECTOR_CROSS3_X3("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
                /* 3x scalar multiplications */
                /* x0 = m0 = v0 cross v1 */
                /* x1 = m1 = v1 cross v2 */
                /* x2 = m2 = v2 cross v0 */
                VECTOR_DOT3_X3("[x0]", "[x1]", "[x2]", "[x3]")
                /* Compare with zeros */
                /* x0 = r0 = m0 dot m1 */
                /* x1 = r1 = m1 dot m2 */
                /* x2 = r2 = m2 dot m0 */
                __ASM_EMIT("vxorps          %[x4], %[x4], %[x4]")       // x4   = 0
                __ASM_EMIT("vucomiss        %[x4], %[x0]")              // r0 <=> 0
                __ASM_EMIT("jb              110f")                      // r0 < 0
                __ASM_EMIT("vmulss          %[x1], %[x0], %[x0]")       // x0 = r0 * r1
                __ASM_EMIT("vucomiss        %[x4], %[x1]")              // r1 <=> 0
                __ASM_EMIT("jb              109f")                      // r1 < 0
                __ASM_EMIT("vmulss          %[x2], %[x0], %[x0]")       // x0 = r0 * r1 * r2
                __ASM_EMIT("vucomiss        %[x4], %[x2]")              // r2 <=> 0
                __ASM_EMIT("jae             110f")                      // r2 >= 0
                /* Fail cases */
                __ASM_EMIT("vmovaps         %[x2], %[x0]")              // x0 = r2
                __ASM_EMIT("jmp             110f")
                __ASM_EMIT("109:")
                __ASM_EMIT("vmovaps         %[x1], %[x0]")              // x0 = r1
                /* End */
                __ASM_EMIT("110:")
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
                : [pv] "r" (pv), [p] "r" (p)
            );

            return x0;
        }

        float check_point3d_on_triangle_p3p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p)
        {
            float x0, x1, x2, x3, x4, x5, x6, x7;

            ARCH_X86_ASM
            (
                /* Load vectors */
                __ASM_EMIT("vmovups         (%[p]), %[x3]")             // x3   = px py pz 1
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")            // x0   = x0 y0 z0 1
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")            // x1   = x1 y1 z1 1
                __ASM_EMIT("vmovups         (%[p3]), %[x2]")            // x2   = x2 y2 z2 1
                __ASM_EMIT("vsubps          %[x3], %[x0], %[x0]")       // x0   = v0 = dx0 dy0 dz0 0
                __ASM_EMIT("vsubps          %[x3], %[x1], %[x1]")       // x1   = v1 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x3], %[x2], %[x2]")       // x2   = v2 = dx2 dy2 dz2 0
                /* 3x vector multiplications */
                VECTOR_CROSS3_X3("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
                /* 3x scalar multiplications */
                /* x0 = m0 = v0 cross v1 */
                /* x1 = m1 = v1 cross v2 */
                /* x2 = m2 = v2 cross v0 */
                VECTOR_DOT3_X3("[x0]", "[x1]", "[x2]", "[x3]")
                /* Compare with zeros */
                /* x0 = r0 = m0 dot m1 */
                /* x1 = r1 = m1 dot m2 */
                /* x2 = r2 = m2 dot m0 */
                __ASM_EMIT("vxorps          %[x4], %[x4], %[x4]")       // x4   = 0
                __ASM_EMIT("vucomiss        %[x4], %[x0]")              // r0 <=> 0
                __ASM_EMIT("jb              110f")                      // r0 < 0
                __ASM_EMIT("vmulss          %[x1], %[x0], %[x0]")       // x0 = r0 * r1
                __ASM_EMIT("vucomiss        %[x4], %[x1]")              // r1 <=> 0
                __ASM_EMIT("jb              109f")                      // r1 < 0
                __ASM_EMIT("vmulss          %[x2], %[x0], %[x0]")       // x0 = r0 * r1 * r2
                __ASM_EMIT("vucomiss        %[x4], %[x2]")              // r2 <=> 0
                __ASM_EMIT("jae             110f")                      // r2 >= 0
                /* Fail cases */
                __ASM_EMIT("vmovaps         %[x2], %[x0]")              // x0 = r2
                __ASM_EMIT("jmp             110f")
                __ASM_EMIT("109:")
                __ASM_EMIT("vmovaps         %[x1], %[x0]")              // x0 = r1
                /* End */
                __ASM_EMIT("110:")
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
                : [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3), [p] "r" (p)
            );

            return x0;
        }

        float check_point3d_on_triangle_pvp_fma3(const point3d_t *pv, const point3d_t *p)
        {
            float x0, x1, x2, x3, x4, x5, x6, x7;

            ARCH_X86_ASM
            (
                /* Load vectors */
                __ASM_EMIT("vmovups         (%[p]), %[x3]")             // x3   = px py pz 1
                __ASM_EMIT("vmovups         0x00(%[pv]), %[x0]")        // x0   = x0 y0 z0 1
                __ASM_EMIT("vmovups         0x10(%[pv]), %[x1]")        // x1   = x1 y1 z1 1
                __ASM_EMIT("vmovups         0x20(%[pv]), %[x2]")        // x2   = x2 y2 z2 1
                __ASM_EMIT("vsubps          %[x3], %[x0], %[x0]")       // x0   = v0 = dx0 dy0 dz0 0
                __ASM_EMIT("vsubps          %[x3], %[x1], %[x1]")       // x1   = v1 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x3], %[x2], %[x2]")       // x2   = v2 = dx2 dy2 dz2 0
                /* 3x vector multiplications */
                VECTOR_CROSS3_X3_FMA3("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
                /* 3x scalar multiplications */
                /* x0 = m0 = v0 cross v1 */
                /* x1 = m1 = v1 cross v2 */
                /* x2 = m2 = v2 cross v0 */
                VECTOR_DOT3_X3("[x0]", "[x1]", "[x2]", "[x3]")
                /* Compare with zeros */
                /* x0 = r0 = m0 dot m1 */
                /* x1 = r1 = m1 dot m2 */
                /* x2 = r2 = m2 dot m0 */
                __ASM_EMIT("vxorps          %[x4], %[x4], %[x4]")       // x4   = 0
                __ASM_EMIT("vucomiss        %[x4], %[x0]")              // r0 <=> 0
                __ASM_EMIT("jb              110f")                      // r0 < 0
                __ASM_EMIT("vmulss          %[x1], %[x0], %[x0]")       // x0 = r0 * r1
                __ASM_EMIT("vucomiss        %[x4], %[x1]")              // r1 <=> 0
                __ASM_EMIT("jb              109f")                      // r1 < 0
                __ASM_EMIT("vmulss          %[x2], %[x0], %[x0]")       // x0 = r0 * r1 * r2
                __ASM_EMIT("vucomiss        %[x4], %[x2]")              // r2 <=> 0
                __ASM_EMIT("jae             110f")                      // r2 >= 0
                /* Fail cases */
                __ASM_EMIT("vmovaps         %[x2], %[x0]")              // x0 = r2
                __ASM_EMIT("jmp             110f")
                __ASM_EMIT("109:")
                __ASM_EMIT("vmovaps         %[x1], %[x0]")              // x0 = r1
                /* End */
                __ASM_EMIT("110:")
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
                : [pv] "r" (pv), [p] "r" (p)
            );

            return x0;
        }

        float check_point3d_on_triangle_p3p_fma3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p)
        {
            float x0, x1, x2, x3, x4, x5, x6, x7;

            ARCH_X86_ASM
            (
                /* Load vectors */
                __ASM_EMIT("vmovups         (%[p]), %[x3]")             // x3   = px py pz 1
                __ASM_EMIT("vmovups         (%[p1]), %[x0]")            // x0   = x0 y0 z0 1
                __ASM_EMIT("vmovups         (%[p2]), %[x1]")            // x1   = x1 y1 z1 1
                __ASM_EMIT("vmovups         (%[p3]), %[x2]")            // x2   = x2 y2 z2 1
                __ASM_EMIT("vsubps          %[x3], %[x0], %[x0]")       // x0   = v0 = dx0 dy0 dz0 0
                __ASM_EMIT("vsubps          %[x3], %[x1], %[x1]")       // x1   = v1 = dx1 dy1 dz1 0
                __ASM_EMIT("vsubps          %[x3], %[x2], %[x2]")       // x2   = v2 = dx2 dy2 dz2 0
                /* 3x vector multiplications */
                VECTOR_CROSS3_X3_FMA3("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
                /* 3x scalar multiplications */
                /* x0 = m0 = v0 cross v1 */
                /* x1 = m1 = v1 cross v2 */
                /* x2 = m2 = v2 cross v0 */
                VECTOR_DOT3_X3("[x0]", "[x1]", "[x2]", "[x3]")
                /* Compare with zeros */
                /* x0 = r0 = m0 dot m1 */
                /* x1 = r1 = m1 dot m2 */
                /* x2 = r2 = m2 dot m0 */
                __ASM_EMIT("vxorps          %[x4], %[x4], %[x4]")       // x4   = 0
                __ASM_EMIT("vucomiss        %[x4], %[x0]")              // r0 <=> 0
                __ASM_EMIT("jb              110f")                      // r0 < 0
                __ASM_EMIT("vmulss          %[x1], %[x0], %[x0]")       // x0 = r0 * r1
                __ASM_EMIT("vucomiss        %[x4], %[x1]")              // r1 <=> 0
                __ASM_EMIT("jb              109f")                      // r1 < 0
                __ASM_EMIT("vmulss          %[x2], %[x0], %[x0]")       // x0 = r0 * r1 * r2
                __ASM_EMIT("vucomiss        %[x4], %[x2]")              // r2 <=> 0
                __ASM_EMIT("jae             110f")                      // r2 >= 0
                /* Fail cases */
                __ASM_EMIT("vmovaps         %[x2], %[x0]")              // x0 = r2
                __ASM_EMIT("jmp             110f")
                __ASM_EMIT("109:")
                __ASM_EMIT("vmovaps         %[x1], %[x0]")              // x0 = r1
                /* End */
                __ASM_EMIT("110:")
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
                : [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3), [p] "r" (p)
            );

            return x0;
        }

    } /* namespace avx */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX_3DMATH_H_ */
