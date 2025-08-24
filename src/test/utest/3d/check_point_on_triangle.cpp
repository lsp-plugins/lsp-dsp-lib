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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <private/utest/dsp/3d/helpers.h>

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


UTEST_BEGIN("dsp.3d", check_point_on_triangle)

    void test_point(
        size_t k, size_t i, size_t j,
        const dsp::point3d_t *t,
        check_point3d_on_triangle_p3p_t check_p3p,
        check_point3d_on_triangle_pvp_t check_pvp,
        const dsp::point3d_t *p,
        float sign)
    {
        // Check p3p
        float result = check_p3p(&t[0], &t[1], &t[2], p);
        if (((sign < 0.0f) && (result >= 0.0f)) ||
            ((sign > 0.0f) && (result <= 0.0f)) ||
            ((sign == 0.0f) && (result != 0.0f)))
        {
            for (size_t i=0; i<3; ++i)
                printf("t[%d] = {%f, %f, %f, %f}\n", int(i), t[i].x, t[i].y, t[i].z, t[i].w);
            printf("p    = {%f, %f, %f, %f}\n", p->x, p->y, p->z, p->w);
            UTEST_ASSERT_MSG(false,
                "k=%d, i=%d, j=%d: failed check_point3d_on_triangle_p3p check, result=%f, expected=%f",
                int(k), int(i), int(j),
                result, sign);
        }

        // Check pvp
        result = check_pvp(t, p);
        if (((sign < 0.0f) && (result >= 0.0f)) ||
            ((sign > 0.0f) && (result <= 0.0f)) ||
            ((sign == 0.0f) && (result != 0.0f)))
        {
            for (size_t i=0; i<3; ++i)
                printf("t[%d] = {%f, %f, %f, %f}\n", int(i), t[i].x, t[i].y, t[i].z, t[i].w);
            printf("p    = {%f, %f, %f, %f}\n", p->x, p->y, p->z, p->w);
            UTEST_ASSERT_MSG(false,
                "k=%d, i=%d, j=%d: failed check_point3d_on_triangle_pvp check, result=%f, expected=%f",
                int(k), int(i), int(j),
                result, sign);
        }
    }

    #define TEST_POINT(x, y, z, s) \
        if (j == 0) \
            dsp::init_point_xyz(&p, x, y, z); \
        else if (j == 1) \
            dsp::init_point_xyz(&p, y, z, x); \
        else \
            dsp::init_point_xyz(&p, z, x, y); \
        test_point(k, i, j, t, check_p3p, check_pvp, &p, s);

    void call(
        const char *label,
        check_point3d_on_triangle_p3p_t check_p3p,
        check_point3d_on_triangle_pvp_t check_pvp)
    {
        if ((!UTEST_SUPPORTED(check_p3p)) || (!UTEST_SUPPORTED(check_pvp)))
            return;

        printf("Testing %s...\n", label);

        // Init points
        dsp::point3d_t t[3], p;
        dsp::init_point_xyz(&t[0], 1.0f, 1.0f, 1.0f);
        dsp::init_point_xyz(&t[1], 3.0f, 1.0f, 1.0f);
        dsp::init_point_xyz(&t[2], 2.0f, 3.0f, 1.0f);

        static const float delta = 1e-4f;

        for (size_t k = 0; k < 2; ++k)
        {
            for (size_t i=0; i<3; ++i)
            {
                for (size_t j=0; j<3; ++j)
                {
                    // Test points inside of triangle
                    TEST_POINT(2.0f, 2.0f, 1.0f, 1.0f);
                    TEST_POINT(2.0f, 2.75f, 1.0f, 1.0f);
                    TEST_POINT(2.0f, 1.25f, 1.0f, 1.0f);
                    TEST_POINT(1.25f, 1.25f, 1.0f, 1.0f);
                    TEST_POINT(2.75f, 1.25f, 1.0f, 1.0f);
                    // Test points inside of triangle with small offsets
                    TEST_POINT(2.0f, 2.0f, 1.0f + delta, 1.0f);
                    TEST_POINT(2.0f, 2.75f, 1.0f + delta, 1.0f);
                    TEST_POINT(2.0f, 1.25f, 1.0f + delta, 1.0f);
                    TEST_POINT(1.25f, 1.25f, 1.0f + delta, 1.0f);
                    TEST_POINT(2.75f, 1.25f, 1.0f + delta, 1.0f);
                    TEST_POINT(2.0f, 2.0f, 1.0f - delta, 1.0f);
                    TEST_POINT(2.0f, 2.75f, 1.0f - delta, 1.0f);
                    TEST_POINT(2.0f, 1.25f, 1.0f - delta, 1.0f);
                    TEST_POINT(1.25f, 1.25f, 1.0f - delta, 1.0f);
                    TEST_POINT(2.75f, 1.25f, 1.0f - delta, 1.0f);
                    // Test points nearby vertices
                    TEST_POINT(1.0f + delta, 1.0f + delta, 1.0f, 1.0f);
                    TEST_POINT(3.0f - delta, 1.0f + delta, 1.0f, 1.0f);
                    TEST_POINT(2.0f, 3.0f - delta, 1.0f, 1.0f);
                    // Test points nearby edges
                    TEST_POINT(1.5f + delta, 2.0f, 1.0f, 1.0f);
                    TEST_POINT(2.5f - delta, 2.0f, 1.0f, 1.0f);
                    TEST_POINT(2.0f, 1.0f + delta, 1.0f, 1.0f);
                    // Test points lying on vertices
                    TEST_POINT(1.0f, 1.0f, 1.0f, 0.0f);
                    TEST_POINT(3.0f, 1.0f, 1.0f, 0.0f);
                    TEST_POINT(2.0f, 3.0f, 1.0f, 0.0f);
                    // Test points lying on edges of triangle
                    TEST_POINT(1.5f, 2.0f, 1.0f, 0.0f);
                    TEST_POINT(2.5f, 2.0f, 1.0f, 0.0f);
                    TEST_POINT(2.0f, 1.0f, 1.0f, 0.0f);
                    // Test points lying on edges continuations of triangle
                    TEST_POINT(0.5f, 0.0f, 1.0f, -1.0f);
                    TEST_POINT(0.5f, 1.0f, 1.0f, -1.0f);
                    TEST_POINT(4.0f, -1.0f, 1.0f, -1.0f);
                    TEST_POINT(4.0f, 1.0f, 1.0f, -1.0f);
                    TEST_POINT(1.0f, 5.0f, 1.0f, -1.0f);
                    TEST_POINT(2.5f, 4.0f, 1.0f, -1.0f);

                    // Rotate point coordinates
                    for (size_t j=0; j<3; ++j)
                    {
                        const float c = t[j].x;
                        t[j].x = t[j].y;
                        t[j].y = t[j].z;
                        t[j].z = c;
                    }
                }

                // Rotate triangle edges
                p = t[0];
                t[0] = t[1];
                t[1] = t[2];
                t[2] = p;
            }

            // Flip order of vertices
            p = t[0];
            t[0] = t[1];
            t[1] = p;
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call(
            "generic::check_point3d_on_triangle",
            generic::check_point3d_on_triangle_p3p,
            generic::check_point3d_on_triangle_pvp));

        IF_ARCH_X86(call(
            "sse::check_point3d_on_triangle",
            sse::check_point3d_on_triangle_p3p,
            sse::check_point3d_on_triangle_pvp));

        IF_ARCH_X86(call(
            "avx::check_point3d_on_triangle",
            avx::check_point3d_on_triangle_p3p,
            avx::check_point3d_on_triangle_pvp));

        IF_ARCH_X86(call(
            "avx_fma3::check_point3d_on_triangle",
            avx::check_point3d_on_triangle_p3p_fma3,
            avx::check_point3d_on_triangle_pvp_fma3));
    }
UTEST_END;



