/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 17 авг. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_3DMATH_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_3DMATH_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE2_IMPL */

/* 1x matrix transpose
 * Input:
 *   x0 = row 0 (a1 a2 a3 a4)
 *   x1 = row 1 (b1 b2 b3 b4)
 *   x2 = row 2 (c1 c2 c3 c4)
 *   x3 = row 3 (d1 d2 d3 d4)
 *   x4 = temporary
 *
 * Output:
 *   x0 = row 0 (a1 b1 c1 d1)
 *   x1 = row 1 (a2 b2 c2 d2)
 *   x2 = row 2 (a3 b3 c3 d3)
 *   x3 = row 3 (a4 b4 c4 d4)
 */
#define MAT4_TRANSPOSE(x0, x1, x2, x3, x4)    \
    __ASM_EMIT("movaps      %" x2 ", %" x4)      /* xmm4 = c1 c2 c3 c4 */   \
    __ASM_EMIT("punpckldq   %" x3 ", %" x2)      /* xmm2 = c1 d1 c2 d2 */   \
    __ASM_EMIT("punpckhdq   %" x3 ", %" x4)      /* xmm4 = c3 d3 c4 d4 */   \
    __ASM_EMIT("movaps      %" x0 ", %" x3)      /* xmm3 = a1 a2 a3 a4 */   \
    __ASM_EMIT("punpckldq   %" x1 ", %" x0)      /* xmm0 = a1 b1 a2 b2 */   \
    __ASM_EMIT("punpckhdq   %" x1 ", %" x3)      /* xmm3 = a3 b3 a4 b4 */   \
    __ASM_EMIT("movaps      %" x0 ", %" x1)      /* xmm1 = a1 b2 a2 b2 */   \
    __ASM_EMIT("punpcklqdq  %" x2 ", %" x0)      /* xmm0 = a1 b1 c1 d1 */   \
    __ASM_EMIT("punpckhqdq  %" x2 ", %" x1)      /* xmm1 = a2 b2 c2 d2 */   \
    __ASM_EMIT("movaps      %" x3 ", %" x2)      /* xmm2 = a3 b3 a4 b4 */   \
    __ASM_EMIT("punpcklqdq  %" x4 ", %" x2)      /* xmm2 = a3 b3 c3 d3 */   \
    __ASM_EMIT("punpckhqdq  %" x4 ", %" x3)      /* xmm3 = a4 b4 c4 d4 */

#define MATRIX_LOAD(ptr, x0, x1, x2, x3) \
    __ASM_EMIT("movups      0x00(%[" ptr "]), %" x0 ) \
    __ASM_EMIT("movups      0x10(%[" ptr "]), %" x1 ) \
    __ASM_EMIT("movups      0x20(%[" ptr "]), %" x2 ) \
    __ASM_EMIT("movups      0x30(%[" ptr "]), %" x3 )

#define MATRIX_STORE(ptr, x0, x1, x2, x3) \
    __ASM_EMIT("movups      %" x0 ", 0x00(%[" ptr "])") \
    __ASM_EMIT("movups      %" x1 ", 0x10(%[" ptr "])") \
    __ASM_EMIT("movups      %" x2 ", 0x20(%[" ptr "])") \
    __ASM_EMIT("movups      %" x3 ", 0x30(%[" ptr "])")

namespace lsp
{
    namespace sse2
    {
        using namespace dsp;

        IF_ARCH_X86(
            static const float X_3D_TOLERANCE[] __lsp_aligned16 = { LSP_DSP_VEC4(DSP_3D_TOLERANCE) };
            static const float X_3D_MTOLERANCE[] __lsp_aligned16 = { LSP_DSP_VEC4(-DSP_3D_TOLERANCE) };

            static const uint32_t IONE[] __lsp_aligned16        = { LSP_DSP_VEC4(1) };

            static const uint32_t ICULL3[] __lsp_aligned16      = { 0x01, 0x04, 0x10, 0 };
        )

        size_t colocation_x2_v1p2(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1)
        {
            float x0, x1, x2, x3;
            float res[4] __lsp_aligned16;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm3 = pl    */
                __ASM_EMIT("movups      (%[p0]), %[x0]")        /* xmm0 = p0    */
                __ASM_EMIT("movups      (%[p1]), %[x1]")        /* xmm1 = p1    */

                __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = x0*px y0*py z0*pz w0*pw */
                __ASM_EMIT("mulps       %[x2], %[x1]")          /* xmm1 = x1*px y1*py z1*pz w1*pw */
                __ASM_EMIT("movaps      %[x0], %[x3]")          /* xmm3 = x0*px y0*py z0*pz w0*pw */
                __ASM_EMIT("movlhps     %[x1], %[x0]")          /* xmm0 = x0*px y0*py x1*px y1*py */
                __ASM_EMIT("movhlps     %[x3], %[x1]")          /* xmm1 = z0*pz w0*pw z1*pz w1*pw */
                __ASM_EMIT("addps       %[x1], %[x0]")          /* xmm0 = x0*px+z0*pz y0*py+w0*pw x1*px+z1*pz y1*py+w1*pw */
                __ASM_EMIT("shufps      $0xd8, %[x0], %[x0]")   /* xmm0 = x0*px+z0*pz x1*px+z1*pz y0*py+w0*pw y1*py+w1*pw */
                __ASM_EMIT("movhlps     %[x0], %[x1]")          /* xmm1 = y0*py+w0*pw y1*py+w1*pw ? ? */
                __ASM_EMIT("addps       %[x1], %[x0]")          /* xmm0 = x0*px+y0*py+z0*pz+w0*pw x1*px+y1*py+z1*pz+w1*pw ? ? = k0 k1 ? ? */
                __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 ? ? */

                __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL ? ? */
                __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL ? ? */
                __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
                __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
                __ASM_EMIT("paddd       %[x1], %[x0]")
                __ASM_EMIT("movdqa      %[x0], (%[res])")
                __ASM_EMIT32("movl      0x00(%[res]), %[p0]")
                __ASM_EMIT32("movl      0x04(%[res]), %[p1]")
                __ASM_EMIT64("movl      0x00(%[res]), %k[p0]")
                __ASM_EMIT64("movl      0x04(%[res]), %k[p1]")
                __ASM_EMIT("lea         (%[p0], %[p1], 4), %[p0]")
                : [p0] "+r" (p0), [p1] "+r" (p1),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [pl] "r" (pl),
                  [res] "r" (res),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc"
            );

            return size_t(p0);
        }

        size_t colocation_x2_v1pv(const vector3d_t *pl, const point3d_t *pv)
        {
            float x0, x1, x2, x3;
            float res[4] __lsp_aligned16;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm3 = pl    */
                __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
                __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */

                __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = x0*px y0*py z0*pz w0*pw */
                __ASM_EMIT("mulps       %[x2], %[x1]")          /* xmm1 = x1*px y1*py z1*pz w1*pw */
                __ASM_EMIT("movaps      %[x0], %[x3]")          /* xmm3 = x0*px y0*py z0*pz w0*pw */
                __ASM_EMIT("movlhps     %[x1], %[x0]")          /* xmm0 = x0*px y0*py x1*px y1*py */
                __ASM_EMIT("movhlps     %[x3], %[x1]")          /* xmm1 = z0*pz w0*pw z1*pz w1*pw */
                __ASM_EMIT("addps       %[x1], %[x0]")          /* xmm0 = x0*px+z0*pz y0*py+w0*pw x1*px+z1*pz y1*py+w1*pw */
                __ASM_EMIT("shufps      $0xd8, %[x0], %[x0]")   /* xmm0 = x0*px+z0*pz x1*px+z1*pz y0*py+w0*pw y1*py+w1*pw */
                __ASM_EMIT("movhlps     %[x0], %[x1]")          /* xmm1 = y0*py+w0*pw y1*py+w1*pw ? ? */
                __ASM_EMIT("addps       %[x1], %[x0]")          /* xmm0 = x0*px+y0*py+z0*pz+w0*pw x1*px+y1*py+z1*pz+w1*pw ? ? = k0 k1 ? ? */
                __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 ? ? */

                __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL ? ? */
                __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL ? ? */
                __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
                __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
                __ASM_EMIT("paddd       %[x1], %[x0]")
                __ASM_EMIT("movdqa      %[x0], (%[res])")
                __ASM_EMIT32("movl      0x00(%[res]), %[pl]")
                __ASM_EMIT32("movl      0x04(%[res]), %[pv]")
                __ASM_EMIT64("movl      0x00(%[res]), %k[pl]")
                __ASM_EMIT64("movl      0x04(%[res]), %k[pv]")
                __ASM_EMIT("lea         (%[pl], %[pv], 4), %[pl]")
                : [pl] "+r" (pl), [pv] "+r" (pv),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
                : [res] "r" (res),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc"
            );

            return size_t(pl);
        }

        size_t colocation_x3_v1p3(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
        {
            float x0, x1, x2, x3, x4;
            float res[4] __lsp_aligned16;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
                __ASM_EMIT("movups      (%[p0]), %[x0]")        /* xmm0 = p0    */
                __ASM_EMIT("movups      (%[p1]), %[x1]")        /* xmm1 = p1    */
                __ASM_EMIT("movups      (%[p2]), %[x2]")        /* xmm2 = p2    */

                __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl */
                __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl */
                __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl */
                MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
                /* xmm0 = x0 x1 x2 ? */
                /* xmm1 = y0 y1 y2 ? */
                /* xmm2 = z0 z1 z2 ? */
                /* xmm3 = w0 w1 w2 ? */
                __ASM_EMIT("addps       %[x1], %[x0]")
                __ASM_EMIT("addps       %[x3], %[x2]")
                __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
                __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
                __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
                __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
                __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
                __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
                __ASM_EMIT("paddd       %[x1], %[x0]")
                __ASM_EMIT("movdqa      %[x0], (%[res])")
                __ASM_EMIT32("movl      0x00(%[res]), %[p0]")
                __ASM_EMIT32("movl      0x04(%[res]), %[p1]")
                __ASM_EMIT32("movl      0x08(%[res]), %[p2]")
                __ASM_EMIT64("movl      0x00(%[res]), %k[p0]")
                __ASM_EMIT64("movl      0x04(%[res]), %k[p1]")
                __ASM_EMIT64("movl      0x08(%[res]), %k[p2]")
                __ASM_EMIT("lea         (%[p1], %[p2], 4), %[p1]")
                __ASM_EMIT("lea         (%[p0], %[p1], 4), %[p0]")
                : [p0] "+r" (p0), [p1] "+r" (p1), [p2] "+r" (p2),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4)
                : [pl] "r" (pl),
                  [res] "r" (res),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc"
            );

            return size_t(p0);
        }

        size_t colocation_x3_v1pv(const vector3d_t *pl, const point3d_t *pv)
        {
            float x0, x1, x2, x3, x4;
            float res[4] __lsp_aligned16;
            size_t pt;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
                __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
                __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */
                __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = p2    */

                __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl */
                __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl */
                __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl */
                MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
                /* xmm0 = x0 x1 x2 ? */
                /* xmm1 = y0 y1 y2 ? */
                /* xmm2 = z0 z1 z2 ? */
                /* xmm3 = w0 w1 w2 ? */
                __ASM_EMIT("addps       %[x1], %[x0]")
                __ASM_EMIT("addps       %[x3], %[x2]")
                __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
                __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
                __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
                __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
                __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
                __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
                __ASM_EMIT("paddd       %[x1], %[x0]")
                __ASM_EMIT("movdqa      %[x0], (%[res])")
                __ASM_EMIT32("movl      0x00(%[res]), %[pl]")
                __ASM_EMIT32("movl      0x04(%[res]), %[pv]")
                __ASM_EMIT32("movl      0x08(%[res]), %[pt]")
                __ASM_EMIT64("movl      0x00(%[res]), %k[pl]")
                __ASM_EMIT64("movl      0x04(%[res]), %k[pv]")
                __ASM_EMIT64("movl      0x08(%[res]), %k[pt]")
                __ASM_EMIT("lea         (%[pv], %[pt], 4), %[pt]")
                __ASM_EMIT("lea         (%[pl], %[pt], 4), %[pt]")
                : [pl] "+r" (pl), [pv] "+r" (pv), [pt] "=&r" (pt),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4)
                : [res] "r" (res),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc"
            );

            return pt;
        }

        size_t colocation_x3_v3p1(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p)
        {
            float x0, x1, x2, x3, x4;
            float res[4] __lsp_aligned16;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = p     */
                __ASM_EMIT("movups      (%[v0]), %[x0]")        /* xmm0 = v0    */
                __ASM_EMIT("movups      (%[v1]), %[x1]")        /* xmm1 = v1    */
                __ASM_EMIT("movups      (%[v2]), %[x2]")        /* xmm2 = v2    */

                __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = v0 * p */
                __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = v1 * p */
                __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = v2 * p */
                MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
                /* xmm0 = x0 x1 x2 ? */
                /* xmm1 = y0 y1 y2 ? */
                /* xmm2 = z0 z1 z2 ? */
                /* xmm3 = w0 w1 w2 ? */
                __ASM_EMIT("addps       %[x1], %[x0]")
                __ASM_EMIT("addps       %[x3], %[x2]")
                __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
                __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
                __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
                __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
                __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
                __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
                __ASM_EMIT("paddd       %[x1], %[x0]")
                __ASM_EMIT("movdqa      %[x0], (%[res])")
                __ASM_EMIT32("movl      0x00(%[res]), %[v0]")
                __ASM_EMIT32("movl      0x04(%[res]), %[v1]")
                __ASM_EMIT32("movl      0x08(%[res]), %[v2]")
                __ASM_EMIT64("movl      0x00(%[res]), %k[v0]")
                __ASM_EMIT64("movl      0x04(%[res]), %k[v1]")
                __ASM_EMIT64("movl      0x08(%[res]), %k[v2]")
                __ASM_EMIT("lea         (%[v1], %[v2], 4), %[v1]")
                __ASM_EMIT("lea         (%[v0], %[v1], 4), %[v0]")

                : [v0] "+r" (v0), [v1] "+r" (v1), [v2] "+r" (v2),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4)
                : [p] "r" (p),
                  [res] "r" (res),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc"
            );

            return size_t(v0);
        }

        size_t colocation_x3_vvp1(const vector3d_t *vv, const point3d_t *p)
        {
            float x0, x1, x2, x3, x4;
            float res[4] __lsp_aligned16;
            size_t pt;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = p     */
                __ASM_EMIT("movups      0x00(%[vv]), %[x0]")    /* xmm0 = v0    */
                __ASM_EMIT("movups      0x10(%[vv]), %[x1]")    /* xmm1 = v1    */
                __ASM_EMIT("movups      0x20(%[vv]), %[x2]")    /* xmm2 = v2    */

                __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = v0 * p */
                __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = v1 * p */
                __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = v2 * p */
                MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
                /* xmm0 = x0 x1 x2 ? */
                /* xmm1 = y0 y1 y2 ? */
                /* xmm2 = z0 z1 z2 ? */
                /* xmm3 = w0 w1 w2 ? */
                __ASM_EMIT("addps       %[x1], %[x0]")
                __ASM_EMIT("addps       %[x3], %[x2]")
                __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
                __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
                __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
                __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
                __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
                __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
                __ASM_EMIT("paddd       %[x1], %[x0]")
                __ASM_EMIT("movdqa      %[x0], (%[res])")
                __ASM_EMIT32("movl      0x00(%[res]), %[vv]")
                __ASM_EMIT32("movl      0x04(%[res]), %[p]")
                __ASM_EMIT32("movl      0x08(%[res]), %[pt]")
                __ASM_EMIT64("movl      0x00(%[res]), %k[vv]")
                __ASM_EMIT64("movl      0x04(%[res]), %k[p]")
                __ASM_EMIT64("movl      0x08(%[res]), %k[pt]")
                __ASM_EMIT("lea         (%[p], %[pt], 4), %[pt]")
                __ASM_EMIT("lea         (%[vv], %[pt], 4), %[pt]")

                : [vv] "+r" (vv), [p] "+r" (p), [pt] "=&r" (pt),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4)
                : [res] "r" (res),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [IONE] "m" (IONE)
                : "cc"
            );

            return pt;
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
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4"
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
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4"
            );
        }

        void split_triangle_raw(
                raw_triangle_t *out,
                size_t *n_out,
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
            out    += *n_out;

            // Estimate co-location of triangle and point
            ARCH_X86_ASM
            (
                __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
                __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
                __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */
                __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = p2    */

                __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl */
                __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl */
                __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl */
                MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")

                __ASM_EMIT("addps       %[x1], %[x0]")
                __ASM_EMIT("addps       %[x3], %[x2]")
                __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
                __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
                __ASM_EMIT("movaps      %[x0], (%[k])")         /* *k   = xmm0 */
                __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
                __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
                __ASM_EMIT("pand        %[ICULL3], %[x0]")      /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
                __ASM_EMIT("pand        %[ICULL3], %[x1]")      /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
                __ASM_EMIT("paddd       %[x1], %[x0]")          /* xmm0 = x0 x1 x2 0 */
                __ASM_EMIT("movdqa      %[x0], %[x2]")          /* xmm2 = x0 x1 x2 0 */
                __ASM_EMIT("movdqa      %[x0], %[x1]")          /* xmm1 = x0 x1 x2 0 */
                __ASM_EMIT("psrldq      $8, %[x2]")             /* xmm2 = x2 0 0 0 */
                __ASM_EMIT("psrldq      $4, %[x1]")             /* xmm1 = x1 x2 0 0 */
                __ASM_EMIT("por         %[x2], %[x0]")
                __ASM_EMIT("por         %[x1], %[x0]")
                __ASM_EMIT32("movd      %[x0], %[t0]")
                __ASM_EMIT64("movd      %[x0], %k[t0]")
                : [t0] "=&r" (t0),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4)
                : [pl] "r" (pl), [pv] "r" (pv),
                  [k] "r" (k),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [ICULL3] "m" (ICULL3)
                : "cc"
            );

            #define STR_COPY_TO(tgt) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("movups      0x00(%[src]), %[x0]") \
                    __ASM_EMIT("movups      0x10(%[src]), %[x1]") \
                    __ASM_EMIT("movups      0x20(%[src]), %[x2]") \
                    __ASM_EMIT("movups      %[x0], 0x00(%[dst])") \
                    __ASM_EMIT("movups      %[x1], 0x10(%[dst])") \
                    __ASM_EMIT("movups      %[x2], 0x20(%[dst])") \
                    : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2) \
                    : [dst] "r" (tgt), \
                      [src] "r" (pv) \
                    : "cc", "memory" \
                );

            #define STR_SPLIT_1P(off0, off1, koff, store) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("movups      0x" off0 "(%[st]), %[x0]")      /* xmm0 = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("movups      0x" off1 "(%[st]), %[x1]")      /* xmm1 = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("movups      (%[pl]), %[x2]")                /* xmm2 = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("subps       %[x0], %[x1]")                  /* xmm1 = d = p1 - p0 = dx dy dz 0 */ \
                    __ASM_EMIT("movss       0x" koff "(%[k]), %[x3]")       /* xmm3 = k 0 0 0 */ \
                    __ASM_EMIT("mulps       %[x1], %[x2]")                  /* xmm2 = dx*nx dy*ny dz*nz 0 */ \
                    __ASM_EMIT("shufps      $0x00, %[x3], %[x3]")           /* xmm3 = k k k k */ \
                    __ASM_EMIT("movhlps     %[x2], %[x4]")                  /* xmm4 = dz*nz 0 ? ? */ \
                    __ASM_EMIT("addps       %[x4], %[x2]")                  /* xmm2 = dx*nx+dz*nz dy*ny ? ? */ \
                    __ASM_EMIT("unpcklps    %[x2], %[x2]")                  /* xmm2 = dx*nx+dz*nz dx*nx+dz*nz dy*ny dy*ny */ \
                    __ASM_EMIT("mulps       %[x3], %[x1]")                  /* xmm1 = k*dx k*dy k*dz 0 */ \
                    __ASM_EMIT("movhlps     %[x2], %[x4]")                  /* xmm4 = dy*ny dy*ny ? ? */ \
                    __ASM_EMIT("addps       %[x4], %[x2]")                  /* xmm2 = dx*nx+dy*ny+dw*nw dx*nx+dy*ny+dw*nw ? ? = b b ? ? */ \
                    __ASM_EMIT("unpcklps    %[x2], %[x2]")                  /* xmm2 = b b b b */ \
                    __ASM_EMIT("divps       %[x2], %[x1]")                  /* xmm1 = k*dx/b k*dy/b k*dz/b 0 */ \
                    __ASM_EMIT("subps       %[x1], %[x0]")                  /* xmm0 = lx0-k*dx/b ly0-k*dy/b lz0-k*dz/b 1 */ \
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
                    __ASM_EMIT("movups      0x" off0 "(%[st]), %[x0]")      /* xmm0 = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("movups      0x" off1 "(%[st]), %[x2]")      /* xmm2 = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("movups      0x" off2 "(%[st]), %[x3]")      /* xmm3 = p2 = lx2 ly2 lz2 1 */ \
                    __ASM_EMIT("movups      (%[pl]), %[x4]")                /* xmm4 = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("movaps      %[x0], %[x1]")                  /* xmm1 = pl = nx ny nz nw */ \
                    __ASM_EMIT("movaps      %[x4], %[x5]")                  /* xmm5 = pl = nx ny nz nw */ \
                    __ASM_EMIT("subps       %[x0], %[x2]")                  /* xmm2 = d1 = p1 - p0 = dx1 dy1 dz1 0 */ \
                    __ASM_EMIT("subps       %[x1], %[x3]")                  /* xmm3 = d2 = p2 - p0 = dx2 dy2 dz2 0 */ \
                    __ASM_EMIT("mulps       %[x2], %[x4]")                  /* xmm4 = dx1*nx dy1*ny dz1*nz 0 */ \
                    __ASM_EMIT("mulps       %[x3], %[x5]")                  /* xmm5 = dx2*nx dy2*ny dz2*nz 0 */ \
                    __ASM_EMIT("movhlps     %[x4], %[x6]")                  /* xmm6 = dz1*nz 0 ? ? */ \
                    __ASM_EMIT("movhlps     %[x5], %[x7]")                  /* xmm7 = dz2*nz 0 ? ? */ \
                    __ASM_EMIT("addps       %[x6], %[x4]")                  /* xmm4 = dx1*nx+dz1*nz dy1*ny ? ? */ \
                    __ASM_EMIT("addps       %[x7], %[x5]")                  /* xmm5 = dx1*nx+dz1*nz dy1*ny ? ? */ \
                    __ASM_EMIT("unpcklps    %[x4], %[x4]")                  /* xmm4 = dx1*nx+dz1*nz dx1*nx+dz1*nz dy1*ny dy1*ny */ \
                    __ASM_EMIT("unpcklps    %[x5], %[x5]")                  /* xmm5 = dx2*nx+dz2*nz dx2*nx+dz2*nz dy2*ny dy2*ny */ \
                    __ASM_EMIT("movhlps     %[x4], %[x6]")                  /* xmm6 = dy1*ny dy1*ny ? ? */ \
                    __ASM_EMIT("movhlps     %[x5], %[x7]")                  /* xmm7 = dy2*ny dy2*ny ? ? */ \
                    __ASM_EMIT("addps       %[x6], %[x4]")                  /* xmm4 = dx1*nx+dy1*ny+dz1*nz dx1*nx+dy1*ny+dz1*nz ? ? = b1 b1 ? ? */ \
                    __ASM_EMIT("addps       %[x7], %[x5]")                  /* xmm5 = dx2*nx+dy2*ny+dz2*nz dx2*nx+dy2*ny+dz2*nz ? ? = b2 b2 ? ? */ \
                    __ASM_EMIT("unpcklps    %[x4], %[x4]")                  /* xmm4 = b1 b1 b1 b1 */ \
                    __ASM_EMIT("unpcklps    %[x5], %[x5]")                  /* xmm5 = b2 b2 b2 b2 */ \
                    __ASM_EMIT("movss       0x" koff "(%[k]), %[x6]")       /* xmm6 = k */ \
                    __ASM_EMIT("divps       %[x4], %[x2]")                  /* xmm2 = dx1/b1 dy1/b1 dz1/b1 0 */ \
                    __ASM_EMIT("shufps      $0x00, %[x6], %[x6]")           /* xmm6 = k k k k */ \
                    __ASM_EMIT("divps       %[x5], %[x3]")                  /* xmm3 = dx2/b2 dy2/b2 dz2/b2 0 */ \
                    __ASM_EMIT("mulps       %[x6], %[x2]")                  /* xmm2 = k*dx1/b1 k*dy1/b1 k*dz1/b1 0 */ \
                    __ASM_EMIT("mulps       %[x6], %[x3]")                  /* xmm3 = k*dx2/b2 k*dy2/b2 k*dz2/b2 0 */ \
                    __ASM_EMIT("subps       %[x2], %[x0]")                  /* xmm0 = lx0-k*dx1/b1 ly0-k*dy1/b1 lz0-k*dz1/b1 1 */ \
                    __ASM_EMIT("subps       %[x3], %[x1]")                  /* xmm1 = lx1-k*dx2/b2 ly1-k*dy2/b2 lz1-k*dz2/b2 1 */ \
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
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x1], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x3], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;
                case 0x21:  // 2 0 1
                    STR_SPLIT_1P("00", "10", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;
                case 0x18:  // 1 2 0
                    STR_SPLIT_1P("10", "20", "04",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x3], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;

                // 1 intersection, 1 triangle above, 1 triangle below, clockwise
                case 0x24:  // 2 1 0
                    STR_SPLIT_1P("00", "20", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x2], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;
                case 0x12:  // 1 0 2
                    STR_SPLIT_1P("00", "10", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x1], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x3], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;
                case 0x09:  // 0 2 1
                    STR_SPLIT_1P("10", "20", "04",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); ++(*n_in);
                    break;

                // 2 intersections, 1 triangle below, 2 triangles above
                case 0x02:  // 0 0 2
                    STR_SPLIT_2P("00", "10", "20", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                        __ASM_EMIT("movups      %[x4], 0x30(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x40(%[out])")
                        __ASM_EMIT("movups      %[x3], 0x50(%[out])")
                    );
                    (*n_out) += 2; ++(*n_in);
                    break;
                case 0x08:  // 0 2 0
                    STR_SPLIT_2P("10", "00", "20", "04",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x4], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[out])")
                        __ASM_EMIT("movups      %[x2], 0x30(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x40(%[out])")
                        __ASM_EMIT("movups      %[x4], 0x50(%[out])")
                    );
                    (*n_out) += 2; ++(*n_in);
                    break;
                case 0x20:  // 2 0 0
                    STR_SPLIT_2P("20", "00", "10", "08",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x4], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                        __ASM_EMIT("movups      %[x3], 0x30(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x40(%[out])")
                        __ASM_EMIT("movups      %[x2], 0x50(%[out])")
                    );
                    (*n_out) += 2; ++(*n_in);
                    break;

                // 2 intersections, 1 triangle above, 2 triangles below
                case 0x28:  // 2 2 0
                    STR_SPLIT_2P("00", "10", "20", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x4], 0x30(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x40(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x50(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[out])")
                    );
                    ++(*n_out); (*n_in) += 2;
                    break;

                case 0x22:  // 2 0 2
                    STR_SPLIT_2P("10", "00", "20", "04",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x4], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x30(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x40(%[in])")
                        __ASM_EMIT("movups      %[x4], 0x50(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                    );
                    ++(*n_out); (*n_in) += 2;
                    break;

                case 0x0a:  // 0 2 2
                    STR_SPLIT_2P("20", "00", "10", "08",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x30(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x40(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x50(%[in])")
                        __ASM_EMIT("movups      %[x4], 0x00(%[out])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[out])")
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

            // Estimate co-location of triangle and point
            ARCH_X86_ASM
            (
                __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
                __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
                __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */
                __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = p2    */

                __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl */
                __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl */
                __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl */
                MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")

                __ASM_EMIT("addps       %[x1], %[x0]")
                __ASM_EMIT("addps       %[x3], %[x2]")
                __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
                __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
                __ASM_EMIT("movaps      %[x0], (%[k])")         /* *k   = xmm0 */
                __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
                __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
                __ASM_EMIT("pand        %[ICULL3], %[x0]")      /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
                __ASM_EMIT("pand        %[ICULL3], %[x1]")      /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
                __ASM_EMIT("paddd       %[x1], %[x0]")          /* xmm0 = x0 x1 x2 0 */
                __ASM_EMIT("movdqa      %[x0], %[x2]")          /* xmm2 = x0 x1 x2 0 */
                __ASM_EMIT("movdqa      %[x0], %[x1]")          /* xmm1 = x0 x1 x2 0 */
                __ASM_EMIT("psrldq      $8, %[x2]")             /* xmm2 = x2 0 0 0 */
                __ASM_EMIT("psrldq      $4, %[x1]")             /* xmm1 = x1 x2 0 0 */
                __ASM_EMIT("por         %[x2], %[x0]")
                __ASM_EMIT("por         %[x1], %[x0]")
                __ASM_EMIT32("movd      %[x0], %[t0]")
                __ASM_EMIT64("movd      %[x0], %k[t0]")
                : [t0] "=&r" (t0),
                  [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4)
                : [pl] "r" (pl), [pv] "r" (pv),
                  [k] "r" (k),
                  [PTOL] "m" (X_3D_TOLERANCE),
                  [MTOL] "m" (X_3D_MTOLERANCE),
                  [ICULL3] "m" (ICULL3)
                : "cc"
            );

            #define STR_COPY_TO(tgt) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("movups      0x00(%[src]), %[x0]") \
                    __ASM_EMIT("movups      0x10(%[src]), %[x1]") \
                    __ASM_EMIT("movups      0x20(%[src]), %[x2]") \
                    __ASM_EMIT("movups      %[x0], 0x00(%[dst])") \
                    __ASM_EMIT("movups      %[x1], 0x10(%[dst])") \
                    __ASM_EMIT("movups      %[x2], 0x20(%[dst])") \
                    : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2) \
                    : [dst] "r" (tgt), \
                      [src] "r" (pv) \
                    : "cc", "memory" \
                );

            #define STR_SPLIT_1P(off0, off1, koff, store) \
                ARCH_X86_ASM( \
                    __ASM_EMIT("movups      0x" off0 "(%[st]), %[x0]")      /* xmm0 = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("movups      0x" off1 "(%[st]), %[x1]")      /* xmm1 = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("movups      (%[pl]), %[x2]")                /* xmm2 = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("subps       %[x0], %[x1]")                  /* xmm1 = d = p1 - p0 = dx dy dz 0 */ \
                    __ASM_EMIT("movss       0x" koff "(%[k]), %[x3]")       /* xmm3 = k 0 0 0 */ \
                    __ASM_EMIT("mulps       %[x1], %[x2]")                  /* xmm2 = dx*nx dy*ny dz*nz 0 */ \
                    __ASM_EMIT("shufps      $0x00, %[x3], %[x3]")           /* xmm3 = k k k k */ \
                    __ASM_EMIT("movhlps     %[x2], %[x4]")                  /* xmm4 = dz*nz 0 ? ? */ \
                    __ASM_EMIT("addps       %[x4], %[x2]")                  /* xmm2 = dx*nx+dz*nz dy*ny ? ? */ \
                    __ASM_EMIT("unpcklps    %[x2], %[x2]")                  /* xmm2 = dx*nx+dz*nz dx*nx+dz*nz dy*ny dy*ny */ \
                    __ASM_EMIT("mulps       %[x3], %[x1]")                  /* xmm1 = k*dx k*dy k*dz 0 */ \
                    __ASM_EMIT("movhlps     %[x2], %[x4]")                  /* xmm4 = dy*ny dy*ny ? ? */ \
                    __ASM_EMIT("addps       %[x4], %[x2]")                  /* xmm2 = dx*nx+dy*ny+dw*nw dx*nx+dy*ny+dw*nw ? ? = b b ? ? */ \
                    __ASM_EMIT("unpcklps    %[x2], %[x2]")                  /* xmm2 = b b b b */ \
                    __ASM_EMIT("divps       %[x2], %[x1]")                  /* xmm1 = k*dx/b k*dy/b k*dz/b 0 */ \
                    __ASM_EMIT("subps       %[x1], %[x0]")                  /* xmm0 = lx0-k*dx/b ly0-k*dy/b lz0-k*dz/b 1 */ \
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
                    __ASM_EMIT("movups      0x" off0 "(%[st]), %[x0]")      /* xmm0 = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("movups      0x" off1 "(%[st]), %[x2]")      /* xmm2 = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("movups      0x" off2 "(%[st]), %[x3]")      /* xmm3 = p2 = lx2 ly2 lz2 1 */ \
                    __ASM_EMIT("movups      (%[pl]), %[x4]")                /* xmm4 = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("movaps      %[x0], %[x1]")                  /* xmm1 = pl = nx ny nz nw */ \
                    __ASM_EMIT("movaps      %[x4], %[x5]")                  /* xmm5 = pl = nx ny nz nw */ \
                    __ASM_EMIT("subps       %[x0], %[x2]")                  /* xmm2 = d1 = p1 - p0 = dx1 dy1 dz1 0 */ \
                    __ASM_EMIT("subps       %[x1], %[x3]")                  /* xmm3 = d2 = p2 - p0 = dx2 dy2 dz2 0 */ \
                    __ASM_EMIT("mulps       %[x2], %[x4]")                  /* xmm4 = dx1*nx dy1*ny dz1*nz 0 */ \
                    __ASM_EMIT("mulps       %[x3], %[x5]")                  /* xmm5 = dx2*nx dy2*ny dz2*nz 0 */ \
                    __ASM_EMIT("movhlps     %[x4], %[x6]")                  /* xmm6 = dz1*nz 0 ? ? */ \
                    __ASM_EMIT("movhlps     %[x5], %[x7]")                  /* xmm7 = dz2*nz 0 ? ? */ \
                    __ASM_EMIT("addps       %[x6], %[x4]")                  /* xmm4 = dx1*nx+dz1*nz dy1*ny ? ? */ \
                    __ASM_EMIT("addps       %[x7], %[x5]")                  /* xmm5 = dx1*nx+dz1*nz dy1*ny ? ? */ \
                    __ASM_EMIT("unpcklps    %[x4], %[x4]")                  /* xmm4 = dx1*nx+dz1*nz dx1*nx+dz1*nz dy1*ny dy1*ny */ \
                    __ASM_EMIT("unpcklps    %[x5], %[x5]")                  /* xmm5 = dx2*nx+dz2*nz dx2*nx+dz2*nz dy2*ny dy2*ny */ \
                    __ASM_EMIT("movhlps     %[x4], %[x6]")                  /* xmm6 = dy1*ny dy1*ny ? ? */ \
                    __ASM_EMIT("movhlps     %[x5], %[x7]")                  /* xmm7 = dy2*ny dy2*ny ? ? */ \
                    __ASM_EMIT("addps       %[x6], %[x4]")                  /* xmm4 = dx1*nx+dy1*ny+dz1*nz dx1*nx+dy1*ny+dz1*nz ? ? = b1 b1 ? ? */ \
                    __ASM_EMIT("addps       %[x7], %[x5]")                  /* xmm5 = dx2*nx+dy2*ny+dz2*nz dx2*nx+dy2*ny+dz2*nz ? ? = b2 b2 ? ? */ \
                    __ASM_EMIT("unpcklps    %[x4], %[x4]")                  /* xmm4 = b1 b1 b1 b1 */ \
                    __ASM_EMIT("unpcklps    %[x5], %[x5]")                  /* xmm5 = b2 b2 b2 b2 */ \
                    __ASM_EMIT("movss       0x" koff "(%[k]), %[x6]")       /* xmm6 = k */ \
                    __ASM_EMIT("divps       %[x4], %[x2]")                  /* xmm2 = dx1/b1 dy1/b1 dz1/b1 0 */ \
                    __ASM_EMIT("shufps      $0x00, %[x6], %[x6]")           /* xmm6 = k k k k */ \
                    __ASM_EMIT("divps       %[x5], %[x3]")                  /* xmm3 = dx2/b2 dy2/b2 dz2/b2 0 */ \
                    __ASM_EMIT("mulps       %[x6], %[x2]")                  /* xmm2 = k*dx1/b1 k*dy1/b1 k*dz1/b1 0 */ \
                    __ASM_EMIT("mulps       %[x6], %[x3]")                  /* xmm3 = k*dx2/b2 k*dy2/b2 k*dz2/b2 0 */ \
                    __ASM_EMIT("subps       %[x2], %[x0]")                  /* xmm0 = lx0-k*dx1/b1 ly0-k*dy1/b1 lz0-k*dz1/b1 1 */ \
                    __ASM_EMIT("subps       %[x3], %[x1]")                  /* xmm1 = lx1-k*dx2/b2 ly1-k*dy2/b2 lz1-k*dz2/b2 1 */ \
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
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      %[x1], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x21:  // 2 0 1
                    STR_SPLIT_1P("00", "10", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x18:  // 1 2 0
                    STR_SPLIT_1P("10", "20", "04",
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;

                // 1 intersection, 1 triangle above, 1 triangle below, clockwise
                case 0x24:  // 2 1 0
                    STR_SPLIT_1P("00", "20", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x12:  // 1 0 2
                    STR_SPLIT_1P("00", "10", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x1], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x09:  // 0 2 1
                    STR_SPLIT_1P("10", "20", "04",
                        __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                        __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;

                // 2 intersections, 1 triangle below, 2 triangles above
                case 0x02:  // 0 0 2
                    STR_SPLIT_2P("00", "10", "20", "00",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x08:  // 0 2 0
                    STR_SPLIT_2P("10", "00", "20", "04",
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;
                case 0x20:  // 2 0 0
                    STR_SPLIT_2P("20", "00", "10", "08",
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x4], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    );
                    ++(*n_in);
                    break;

                // 2 intersections, 1 triangle above, 2 triangles below
                case 0x28:  // 2 2 0
                    STR_SPLIT_2P("00", "10", "20", "00",
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x4], 0x30(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x40(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x50(%[in])")
                    );
                    (*n_in) += 2;
                    break;

                case 0x22:  // 2 0 2
                    STR_SPLIT_2P("10", "00", "20", "04",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                        __ASM_EMIT("movups      %[x4], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x30(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x40(%[in])")
                        __ASM_EMIT("movups      %[x4], 0x50(%[in])")
                    );
                    (*n_in) += 2;
                    break;

                case 0x0a:  // 0 2 2
                    STR_SPLIT_2P("20", "00", "10", "08",
                        __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                        __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                        __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                        __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                        __ASM_EMIT("movups      %[x3], 0x30(%[in])")
                        __ASM_EMIT("movups      %[x1], 0x40(%[in])")
                        __ASM_EMIT("movups      %[x2], 0x50(%[in])")
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

    } /* namespace sse2 */
} /* namespace lsp */

#undef MAT4_TRANSPOSE
#undef MAT4_LOAD
#undef MAT4_STORE

#endif /* PRIVATE_DSP_ARCH_X86_SSE2_3DMATH_H_ */
