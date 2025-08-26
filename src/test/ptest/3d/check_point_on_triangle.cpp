/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 25 авг. 2025 г.
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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define TRIANGLES_COUNT    0x4000

namespace lsp
{
    namespace generic
    {
        float check_point3d_on_triangle_p3p(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::point3d_t *p);
        float check_point3d_on_triangle_pvp(const dsp::point3d_t *pv, const dsp::point3d_t *p);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float check_point3d_on_triangle_p3p(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::point3d_t *p);
            float check_point3d_on_triangle_pvp(const dsp::point3d_t *pv, const dsp::point3d_t *p);
        }

        namespace avx
        {
            float check_point3d_on_triangle_p3p(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::point3d_t *p);
            float check_point3d_on_triangle_pvp(const dsp::point3d_t *pv, const dsp::point3d_t *p);

            float check_point3d_on_triangle_p3p_fma3(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::point3d_t *p);
            float check_point3d_on_triangle_pvp_fma3(const dsp::point3d_t *pv, const dsp::point3d_t *p);
        }
    )

    typedef float (* check_point3d_on_triangle_p3p_t)(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::point3d_t *p);
    typedef float (* check_point3d_on_triangle_pvp_t)(const dsp::point3d_t *pv, const dsp::point3d_t *p);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", check_point_on_triangle, 5, 1000)

    void call(const char *label, const dsp::point3d_t *pv, check_point3d_on_triangle_p3p_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);

        dsp::point3d_t t;
        dsp::init_point_xyz(&t, 0.0f, 0.0f, 0.0f);

        PTEST_LOOP(label,
            const dsp::point3d_t *p = pv;
            for (size_t i=0; i<TRIANGLES_COUNT; ++i, p += 3)
                func(&p[0], &p[1], &p[2], &t);
        );
    }

    void call(const char *label, const dsp::point3d_t *pv, check_point3d_on_triangle_pvp_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);

        dsp::point3d_t t;
        dsp::init_point_xyz(&t, 0.0f, 0.0f, 0.0f);

        PTEST_LOOP(label,
            const dsp::point3d_t *p = pv;
            for (size_t i=0; i<TRIANGLES_COUNT; ++i, p += 3)
                func(p, &t);
        );
    }

    PTEST_MAIN
    {
        const size_t buf_size   = TRIANGLES_COUNT * 3;
        uint8_t *data           = NULL;
        dsp::point3d_t *src     = alloc_aligned<dsp::point3d_t>(data, buf_size, 64);
        lsp_finally {
            free_aligned(data);
        };

        // Initialize data
        dsp::point3d_t *p       = src;
        for (size_t i=0; i < TRIANGLES_COUNT; ++i, p += 3)
        {
            size_t m = i % 3;
            const float x1 = randf(-10.0f, 10.0f);
            const float x2 = randf(-10.0f, 10.0f);
            const float x3 = randf(-10.0f, 10.0f);
            const float y1 = randf(-10.0f, 10.0f);
            const float y2 = randf(-10.0f, 10.0f);
            const float y3 = randf(-10.0f, 10.0f);

            if (m == 0)
            {
                dsp::init_point_xyz(&p[0], x1, y1, 0.0f);
                dsp::init_point_xyz(&p[1], x2, y2, 0.0f);
                dsp::init_point_xyz(&p[2], x3, y3, 0.0f);
            }
            else if (m == 1)
            {
                dsp::init_point_xyz(&p[0], y1, 0.0f, x1);
                dsp::init_point_xyz(&p[1], y2, 0.0f, x2);
                dsp::init_point_xyz(&p[2], y3, 0.0f, x3);
            }
            else
            {
                dsp::init_point_xyz(&p[0], 0.0f, x1, y1);
                dsp::init_point_xyz(&p[1], 0.0f, x2, y2);
                dsp::init_point_xyz(&p[2], 0.0f, x3, y3);
            }
        }

        #define CALL(func) \
            call(#func, src, func);

        CALL(generic::check_point3d_on_triangle_p3p);
        IF_ARCH_X86(CALL(sse::check_point3d_on_triangle_p3p));
        IF_ARCH_X86(CALL(avx::check_point3d_on_triangle_p3p));
        IF_ARCH_X86(CALL(avx::check_point3d_on_triangle_p3p_fma3));
        PTEST_SEPARATOR;

        CALL(generic::check_point3d_on_triangle_pvp);
        IF_ARCH_X86(CALL(sse::check_point3d_on_triangle_pvp));
        IF_ARCH_X86(CALL(avx::check_point3d_on_triangle_pvp));
        IF_ARCH_X86(CALL(avx::check_point3d_on_triangle_pvp_fma3));
        PTEST_SEPARATOR;
    }
PTEST_END




