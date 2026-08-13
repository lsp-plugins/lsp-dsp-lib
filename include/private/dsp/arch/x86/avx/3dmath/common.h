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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_3DMATH_COMMON_H_
#define PRIVATE_DSP_ARCH_X86_AVX_3DMATH_COMMON_H_

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
    __ASM_EMIT("vmovshdup       %" x0 ", %" x3)                     /* x3   = dy1*dy2 dy1*dy2 ? ? */ \
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
    } /* namespace avx */
} /* naemspace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX_3DMATH_COMMON_H_ */
