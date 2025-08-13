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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <private/utest/dsp/3d/helpers.h>

namespace lsp
{
    namespace generic
    {
        void init_point_xyz(dsp::point3d_t *p, float x, float y, float z);
        void init_point(dsp::point3d_t *p, const dsp::point3d_t *s);
        void normalize_point(dsp::point3d_t *p);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void init_point_xyz(dsp::point3d_t *p, float x, float y, float z);
            void init_point(dsp::point3d_t *p, const dsp::point3d_t *s);
            void normalize_point(dsp::point3d_t *p);
        }

        namespace avx
        {
            void init_point_xyz(dsp::point3d_t *p, float x, float y, float z);
            void init_point(dsp::point3d_t *p, const dsp::point3d_t *s);
            void normalize_point(dsp::point3d_t *p);
        }
    )

    typedef void (* init_point_xyz_t)(dsp::point3d_t *p, float x, float y, float z);
    typedef void (* init_point_t)(dsp::point3d_t *p, const dsp::point3d_t *s);
    typedef void (* normalize_point_t)(dsp::point3d_t *p);
}

UTEST_BEGIN("dsp.3d", point)

    static void fill_point(dsp::point3d_t *p)
    {
        p->x    = 0.1f;
        p->y    = 0.2f;
        p->z    = 0.3f;
        p->w    = 0.4f;
    }

    void call(
            const char *label,
            init_point_xyz_t init_xyz,
            init_point_t init,
            normalize_point_t norm
        )
    {
        if ((!UTEST_SUPPORTED(init_xyz)) || (!UTEST_SUPPORTED(init)) || (!UTEST_SUPPORTED(norm)))
            return;

        printf("Testing %s\n", label);

        dsp::point3d_t   p1, p2, p3, p4;
        fill_point(&p1);
        fill_point(&p2);
        fill_point(&p3);
        fill_point(&p4);

        generic::init_point_xyz(&p1, 2.0f, 3.0f, 4.0f);
        init_xyz(&p2, 2.0f, 3.0f, 4.0f);
        UTEST_ASSERT_MSG(point3d_sck(&p1, &p2), "Failed init_point_xyz");

        generic::init_point(&p3, &p1);
        init(&p4, &p1);
        UTEST_ASSERT_MSG(point3d_sck(&p1, &p3), "Failed generic init_point");
        UTEST_ASSERT_MSG(point3d_sck(&p1, &p4), "Failed optimized init_point");

        generic::normalize_point(&p3);
        norm(&p4);

        UTEST_ASSERT_MSG(point3d_sck(&p3, &p4), "Failed normalize point");
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse::point", sse::init_point_xyz, sse::init_point, sse::normalize_point));
        IF_ARCH_X86(call("avx::point", avx::init_point_xyz, avx::init_point, avx::normalize_point));
    }
UTEST_END;



