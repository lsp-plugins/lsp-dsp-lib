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
#define N_LINES         256

namespace lsp
{
    namespace generic
    {
        size_t colocation_x2_v1p2(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
        size_t colocation_x2_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
    }

    IF_ARCH_X86(
        namespace sse
        {
            size_t colocation_x2_v1p2(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
            size_t colocation_x2_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
        }

        namespace sse2
        {
            size_t colocation_x2_v1p2(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
            size_t colocation_x2_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
        }

        namespace sse3
        {
            size_t colocation_x2_v1p2(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
            size_t colocation_x2_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
        }

        namespace avx
        {
            size_t colocation_x2_v1p2(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
            size_t colocation_x2_v1pv(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
        }
    )

    typedef size_t (* colocation_x2_v1p2_t)(const dsp::vector3d_t *pl, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
    typedef size_t (* colocation_x2_v1pv_t)(const dsp::vector3d_t *pl, const dsp::point3d_t *pv);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", colocation_x2, 5, 1000)

    void call(const char *label, const dsp::vector3d_t *pl, const dsp::point3d_t *pv, colocation_x2_v1p2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s", label);
        printf("Testing %s ...\n", buf);

        PTEST_LOOP(buf,
            const dsp::vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES; ++i, ++xpl)
            {
                const dsp::point3d_t *xp = pv;
                for (size_t j=0; j<N_LINES; ++j, xp += 2)
                    func(xpl, &xp[0], &xp[1]);
            }
        );
    }

    void call(const char *label, const dsp::vector3d_t *pl, const dsp::point3d_t *pv, colocation_x2_v1pv_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s", label);
        printf("Testing %s ...\n", buf);

        PTEST_LOOP(buf,
            const dsp::vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES; ++i, ++xpl)
            {
                const dsp::point3d_t *xp = pv;
                for (size_t j=0; j<N_LINES; ++j, xp += 2)
                    func(xpl, xp);
            }
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = N_PLANES * sizeof(dsp::vector3d_t) + N_LINES * 2 * sizeof(dsp::point3d_t);
        uint8_t *data       = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, buf_size, 64);

        dsp::vector3d_t *planes  = reinterpret_cast<dsp::vector3d_t *>(ptr);
        ptr                     += N_PLANES * sizeof(dsp::vector3d_t);
        dsp::point3d_t *points   = reinterpret_cast<dsp::point3d_t *>(ptr);

        // Initialize planes
        for (size_t i=0; i < N_PLANES;)
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
        for (size_t i=0; i < N_LINES*2; ++i)
            dsp::init_point_xyz(&points[i], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

    #define CALL(func) \
        call(#func, planes, points, func);

        CALL(generic::colocation_x2_v1p2);
        IF_ARCH_X86(CALL(sse::colocation_x2_v1p2));
        IF_ARCH_X86(CALL(sse2::colocation_x2_v1p2));
        IF_ARCH_X86(CALL(sse3::colocation_x2_v1p2));
        IF_ARCH_X86(CALL(avx::colocation_x2_v1p2));
        PTEST_SEPARATOR;

        CALL(generic::colocation_x2_v1pv);
        IF_ARCH_X86(CALL(sse::colocation_x2_v1pv));
        IF_ARCH_X86(CALL(sse2::colocation_x2_v1pv));
        IF_ARCH_X86(CALL(sse3::colocation_x2_v1pv));
        IF_ARCH_X86(CALL(avx::colocation_x2_v1pv));
        PTEST_SEPARATOR;

        free_aligned(data);
    }
PTEST_END


