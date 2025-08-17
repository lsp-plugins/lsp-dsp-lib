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

namespace lsp
{
    namespace sse2
    {
        using namespace dsp;

        IF_ARCH_X86(
            static const float X_3D_TOLERANCE[] __lsp_aligned16 = { LSP_DSP_VEC4(DSP_3D_TOLERANCE) };
            static const float X_3D_MTOLERANCE[] __lsp_aligned16 = { LSP_DSP_VEC4(-DSP_3D_TOLERANCE) };

            static const uint32_t IONE[] __lsp_aligned16        = { LSP_DSP_VEC4(1) };
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

    } /* namespace sse2 */
} /* namespace lsp */

#undef MAT4_TRANSPOSE

#endif /* PRIVATE_DSP_ARCH_X86_SSE2_3DMATH_H_ */
