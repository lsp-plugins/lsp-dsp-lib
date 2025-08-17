/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define N_PLANES        256
#define N_POINTS        256

namespace lsp
{
    namespace generic
    {
        size_t colocation_x3_v1p3(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
        size_t colocation_x3_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
        size_t colocation_x3_v3p1(const dsp::vector3d_t *v0, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::point3d_t *p);
        size_t colocation_x3_vvp1(const dsp::vector3d_t *vv, const dsp::point3d_t *p);
    }

    IF_ARCH_X86(
        namespace sse
        {
            size_t colocation_x3_v1p3(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            size_t colocation_x3_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
            size_t colocation_x3_v3p1(const dsp::vector3d_t *v0, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::point3d_t *p);
            size_t colocation_x3_vvp1(const dsp::vector3d_t *vv, const dsp::point3d_t *p);
        }

        namespace sse2
        {
            size_t colocation_x3_v1p3(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            size_t colocation_x3_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
            size_t colocation_x3_v3p1(const dsp::vector3d_t *v0, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::point3d_t *p);
            size_t colocation_x3_vvp1(const dsp::vector3d_t *vv, const dsp::point3d_t *p);
        }

        namespace sse3
        {
            size_t colocation_x3_v1p3(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            size_t colocation_x3_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
            size_t colocation_x3_v3p1(const dsp::vector3d_t *v0, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::point3d_t *p);
            size_t colocation_x3_vvp1(const dsp::vector3d_t *vv, const dsp::point3d_t *p);
        }

        namespace avx
        {
            size_t colocation_x3_v1p3(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            size_t colocation_x3_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
            size_t colocation_x3_v3p1(const dsp::vector3d_t *v0, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::point3d_t *p);
            size_t colocation_x3_vvp1(const dsp::vector3d_t *vv, const dsp::point3d_t *p);
        }
    )

    typedef size_t (* colocation_x3_v1p3_t)(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
    typedef size_t (* colocation_x3_v1pv_t)(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
    typedef size_t (* colocation_x3_v3p1_t)(const dsp::vector3d_t *v0, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::point3d_t *p);
    typedef size_t (* colocation_x3_vvp1_t)(const dsp::vector3d_t *vv, const dsp::point3d_t *p);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", colocation_x3, 5, 1000)

    void call_pv(const char *label, const dsp::vector3d_t *pl, const dsp::point3d_t *pv, colocation_x3_v1p3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s", label);
        printf("Testing %s ...\n", buf);

        PTEST_LOOP(buf,
            const dsp::vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES*3; ++i, ++xpl)
            {
                const dsp::point3d_t *xp = pv;
                for (size_t j=0; j<N_POINTS; ++j, xp += 3)
                    func(xpl, &xp[0], &xp[1], &xp[2]);
            }
        );
    }

    void call_pv(const char *label, const dsp::vector3d_t *pl, const dsp::point3d_t *pv, colocation_x3_v1pv_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s", label);
        printf("Testing %s ...\n", buf);

        PTEST_LOOP(buf,
            const dsp::vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES*3; ++i, ++xpl)
            {
                const dsp::point3d_t *xp = pv;
                for (size_t j=0; j<N_POINTS; ++j, xp += 3)
                    func(xpl, xp);
            }
        );
    }

    void call_vv(const char *label, const dsp::vector3d_t *pl, const dsp::point3d_t *pv, colocation_x3_v3p1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s", label);
        printf("Testing %s ...\n", buf);

        PTEST_LOOP(buf,
            const dsp::vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES; ++i, xpl += 3)
            {
                const dsp::point3d_t *xp = pv;
                for (size_t j=0; j<N_POINTS*3; ++j, ++xp)
                    func(&xpl[0], &xpl[1], &xpl[2], xp);
            }
        );
    }

    void call_vv(const char *label, const dsp::vector3d_t *pl, const dsp::point3d_t *pv, colocation_x3_vvp1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s", label);
        printf("Testing %s ...\n", buf);

        PTEST_LOOP(buf,
            const dsp::vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES; ++i, xpl += 3)
            {
                const dsp::point3d_t *xp = pv;
                for (size_t j=0; j<N_POINTS*3; ++j, ++xp)
                    func(xpl, xp);
            }
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = N_PLANES * 3 * sizeof(dsp::vector3d_t) + N_POINTS * 3 * sizeof(dsp::point3d_t);
        uint8_t *data       = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, buf_size, 64);

        dsp::vector3d_t *planes  = reinterpret_cast<dsp::vector3d_t *>(ptr);
        ptr                     += N_PLANES * 3 * sizeof(dsp::vector3d_t);
        dsp::point3d_t *points   = reinterpret_cast<dsp::point3d_t *>(ptr);

        // Initialize planes
        for (size_t i=0; i < N_PLANES*3;)
        {
            dsp::vector3d_t *pl  = &planes[i];

            dsp::init_vector_dxyz(pl, randf(-1.0f, 1.0f), randf(-1.0f, 1.0f), randf(-1.0f, 1.0f));
            float w = sqrtf(pl->dx * pl->dx + pl->dy * pl->dy + pl->dz * pl->dz);
            if (w <= DSP_3D_TOLERANCE)
                continue;

            pl->dx     /= w;
            pl->dy     /= w;
            pl->dz     /= w;
            pl->dw      = randf(-1.0f, 1.0f);

            ++i;
        }

        // Initialize points
        for (size_t i=0; i < N_POINTS*3; ++i)
            dsp::init_point_xyz(&points[i], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

    #define CALL_PV(func) \
        call_pv(#func, planes, points, func)
    #define CALL_VV(func) \
        call_vv(#func, planes, points, func)

        CALL_PV(generic::colocation_x3_v1p3);
        IF_ARCH_X86(CALL_PV(sse::colocation_x3_v1p3));
        IF_ARCH_X86(CALL_PV(sse2::colocation_x3_v1p3));
        IF_ARCH_X86(CALL_PV(sse3::colocation_x3_v1p3));
        IF_ARCH_X86(CALL_PV(avx::colocation_x3_v1p3));
        PTEST_SEPARATOR;

        CALL_PV(generic::colocation_x3_v1pv);
        IF_ARCH_X86(CALL_PV(sse::colocation_x3_v1pv));
        IF_ARCH_X86(CALL_PV(sse2::colocation_x3_v1pv));
        IF_ARCH_X86(CALL_PV(sse3::colocation_x3_v1pv));
        IF_ARCH_X86(CALL_PV(avx::colocation_x3_v1pv));
        PTEST_SEPARATOR;

        CALL_VV(generic::colocation_x3_v3p1);
        IF_ARCH_X86(CALL_VV(sse::colocation_x3_v3p1));
        IF_ARCH_X86(CALL_VV(sse2::colocation_x3_v3p1));
        IF_ARCH_X86(CALL_VV(sse3::colocation_x3_v3p1));
        IF_ARCH_X86(CALL_VV(avx::colocation_x3_v3p1));
        PTEST_SEPARATOR;

        CALL_VV(generic::colocation_x3_vvp1);
        IF_ARCH_X86(CALL_VV(sse::colocation_x3_vvp1));
        IF_ARCH_X86(CALL_VV(sse2::colocation_x3_vvp1));
        IF_ARCH_X86(CALL_VV(sse3::colocation_x3_vvp1));
        IF_ARCH_X86(CALL_VV(avx::colocation_x3_vvp1));
        PTEST_SEPARATOR;

        free_aligned(data);
    }
PTEST_END



