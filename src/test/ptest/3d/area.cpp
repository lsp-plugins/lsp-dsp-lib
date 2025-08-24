/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 авг. 2025 г.
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

#define POINTS_COUNT    0x10000

namespace lsp
{
    namespace generic
    {
        float calc_area_p3(const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
        float calc_area_pv(const dsp::point3d_t *pv);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float calc_area_p3(const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            float calc_area_pv(const dsp::point3d_t *pv);
        }

        namespace avx
        {
            float calc_area_p3(const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            float calc_area_p3_fma3(const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            float calc_area_pv(const dsp::point3d_t *pv);
            float calc_area_pv_fma3(const dsp::point3d_t *pv);
        }
    )

    typedef float (* calc_area_p3_t)(const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
    typedef float (* calc_area_pv_t)(const dsp::point3d_t *pv);
}


//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", area, 5, 1000)

    void call(const char *label, const dsp::point3d_t *pv, calc_area_p3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);

        PTEST_LOOP(label,
            const dsp::point3d_t *p = pv;
            for (size_t i=0; i<(POINTS_COUNT - 2); ++i, ++p)
            {
                func(&p[0], &p[1], &p[2]);
            }
        );
    }

    void call(const char *label, const dsp::point3d_t *pv, calc_area_pv_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);

        PTEST_LOOP(label,
            for (size_t i=0; i<(POINTS_COUNT - 2); ++i)
                func(&pv[i]);
        );
    }

    PTEST_MAIN
    {
        const size_t buf_size   = POINTS_COUNT;
        uint8_t *data           = NULL;
        dsp::point3d_t *src     = alloc_aligned<dsp::point3d_t>(data, buf_size, 64);
        lsp_finally {
            free_aligned(data);
        };

        // Initialize data
        for (size_t i=0; i < POINTS_COUNT; ++i)
            dsp::init_point_xyz(&src[i], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

        #define CALL(func) \
            call(#func, src, func);

        CALL(generic::calc_area_p3);
        IF_ARCH_X86(CALL(sse::calc_area_p3));
        IF_ARCH_X86(CALL(avx::calc_area_p3));
        IF_ARCH_X86(CALL(avx::calc_area_p3_fma3));
        PTEST_SEPARATOR;

        CALL(generic::calc_area_pv);
        IF_ARCH_X86(CALL(sse::calc_area_pv));
        IF_ARCH_X86(CALL(avx::calc_area_pv));
        IF_ARCH_X86(CALL(avx::calc_area_pv_fma3));
        PTEST_SEPARATOR;
    }
PTEST_END

