/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 21 авг. 2025 г.
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
        size_t longest_edge3d_p3(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
        size_t longest_edge3d_pv(const dsp::point3d_t *p);
    }

    IF_ARCH_X86(
        namespace sse
        {
            size_t longest_edge3d_p3(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
            size_t longest_edge3d_pv(const dsp::point3d_t *p);
        }

        namespace avx
        {
            size_t longest_edge3d_p3(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
            size_t longest_edge3d_pv(const dsp::point3d_t *p);
        }
    )

    typedef size_t (* longest_edge3d_p3_t)(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
    typedef size_t (* longest_edge3d_pv_t)(const dsp::point3d_t *p);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", edge, 5, 1000)

    void call(const char *label, const dsp::point3d_t *pv, longest_edge3d_p3_t func)
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

    void call(const char *label, const dsp::point3d_t *pv, longest_edge3d_pv_t func)
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

        CALL(generic::longest_edge3d_p3);
        IF_ARCH_X86(CALL(sse::longest_edge3d_p3));
        IF_ARCH_X86(CALL(avx::longest_edge3d_p3));
        PTEST_SEPARATOR;

        CALL(generic::longest_edge3d_pv);
        IF_ARCH_X86(CALL(sse::longest_edge3d_pv));
        IF_ARCH_X86(CALL(avx::longest_edge3d_pv));
        PTEST_SEPARATOR;
    }
PTEST_END


