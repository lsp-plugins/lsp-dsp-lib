/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/dsp/dsp.h>

namespace lsp
{
    namespace generic
    {
        float check_point3d_on_triangle_p3p(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::point3d_t *p);
        float check_point3d_on_triangle_pvp(const dsp::point3d_t *pv, const dsp::point3d_t *p);
        float check_point3d_on_triangle_tp(const dsp::triangle3d_t *t, const dsp::point3d_t *p);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float check_point3d_on_triangle_p3p(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::point3d_t *p);
            float check_point3d_on_triangle_pvp(const dsp::point3d_t *pv, const dsp::point3d_t *p);
            float check_point3d_on_triangle_tp(const dsp::triangle3d_t *t, const dsp::point3d_t *p);
        }
    )

    typedef float (* check_point3d_on_triangle_p3p_t)(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::point3d_t *p);
    typedef float (* check_point3d_on_triangle_pvp_t)(const dsp::point3d_t *pv, const dsp::point3d_t *p);
    typedef float (* check_point3d_on_triangle_tp_t)(const dsp::triangle3d_t *t, const dsp::point3d_t *p);
}

UTEST_BEGIN("dsp.3d", triangle)

    void call(const char *label,
        check_point3d_on_triangle_p3p_t check_point3d_on_triangle_p3p,
        check_point3d_on_triangle_pvp_t check_point3d_on_triangle_pvp,
        check_point3d_on_triangle_tp_t check_point3d_on_triangle_tp
    )
    {
        if ((!UTEST_SUPPORTED(check_point3d_on_triangle_p3p)) ||
            (!UTEST_SUPPORTED(check_point3d_on_triangle_pvp)) ||
            (!UTEST_SUPPORTED(check_point3d_on_triangle_tp)))
            return;

        printf("Testing %s...\n", label);

        dsp::triangle3d_t t;
        dsp::point3d_t cp[12];
        dsp::point3d_t ip[10];
        float ck;

        // Special check
        dsp::init_triangle3d_xyz(&t, 2.0f, -1.0f, 0.0f, 0.0f, 2.0f, 0.0f, -2.0f, 4.0f, 0.0f);
        dsp::init_point_xyz(&cp[0], -0.5f, 0.5f, 0.0f);
        ck = generic::check_point3d_on_triangle_tp(&t, &cp[0]);
        printf("ck=%f\n", ck);

        dsp::init_triangle3d_xyz(&t, -8.0f, -2.0f, 0.0f, -2.0f, -4.0f, 0.0f, 0.0f, -2.0f, 0.0f);
        dsp::init_point_xyz(&cp[0], 6.0f, 4.0f, 0.0f);
        ck = generic::check_point3d_on_triangle_tp(&t, &cp[0]);
        printf("ck=%f\n", ck);

        // Main check
        dsp::init_triangle3d_xyz(&t, -2.0f, -1.0f, 0.0f, 2.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        dsp::init_point_xyz(&cp[0], -0.5f, 0.5f, 0.0f);
        dsp::init_point_xyz(&cp[1], 0.5f, 0.5f, 0.0f);
        dsp::init_point_xyz(&cp[2], 1.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[3], -1.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[4], 0.75f, -1.0f, 0.0f);
        dsp::init_point_xyz(&cp[5], -0.75f, -1.0f, 0.0f);
        dsp::init_point_xyz(&cp[6], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&cp[7], -0.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[8], 0.5f, -0.5f, 0.0f);
        dsp::init_point(&cp[9], &t.p[0]);
        dsp::init_point(&cp[10], &t.p[1]);
        dsp::init_point(&cp[11], &t.p[2]);

        for (size_t i=0; i<12; ++i)
        {
            ck = check_point3d_on_triangle_tp(&t, &cp[i]);
            UTEST_ASSERT_MSG(ck >= 0.0f, "check_point3d_on_triangle_tp(%d) failed", int(i));

            ck = check_point3d_on_triangle_pvp(t.p, &cp[i]);
            UTEST_ASSERT_MSG(ck >= 0.0f, "check_point3d_on_triangle_pvp(%d) failed", int(i));

            ck = check_point3d_on_triangle_p3p(&t.p[0], &t.p[1], &t.p[2], &cp[i]);
            UTEST_ASSERT_MSG(ck >= 0.0f, "check_point3d_on_triangle_p3p(%d) failed", int(i));
        }

        dsp::init_point_xyz(&ip[0], 0.0f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[1], 0.0f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[2], -1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&ip[3], 1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&ip[4], 2.5f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[5], -2.5f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[6], -0.5f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[7], 0.5f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[8], 2.5f, -1.0f, 0.0f);
        dsp::init_point_xyz(&ip[9], -2.5f, -1.0f, 0.0f);

        for (size_t i=0; i<10; ++i)
        {
            ck = check_point3d_on_triangle_tp(&t, &ip[i]);
            UTEST_ASSERT_MSG(ck < 0.0f, "check_point3d_on_triangle_tp(%d) failed", int(i));

            ck = check_point3d_on_triangle_pvp(t.p, &ip[i]);
            UTEST_ASSERT_MSG(ck < 0.0f, "check_point3d_on_triangle_pvp(%d) failed", int(i));

            ck = check_point3d_on_triangle_p3p(&t.p[0], &t.p[1], &t.p[2], &ip[i]);
            UTEST_ASSERT_MSG(ck < 0.0f, "check_point3d_on_triangle_p3p(%d) failed", int(i));
        }
    }

    UTEST_MAIN
    {
        call("generic::ck_triangle",
                generic::check_point3d_on_triangle_p3p,
                generic::check_point3d_on_triangle_pvp,
                generic::check_point3d_on_triangle_tp
                );

        IF_ARCH_X86(
            call("sse::ck_triangle",
                    sse::check_point3d_on_triangle_p3p,
                    sse::check_point3d_on_triangle_pvp,
                    sse::check_point3d_on_triangle_tp
                    );
        )
    }

UTEST_END

