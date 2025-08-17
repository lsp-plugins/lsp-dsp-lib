/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 17 нояб. 2020 г.
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

UTEST_BEGIN("dsp.3d", colocation_x3)

    UTEST_TIMELIMIT(20)

    float add(float v, ssize_t mask)
    {
        static const float inc[4] = { 0.0f, 1.0f, -1.0f, 0.0f };
        return v + inc[mask & 0x3];
    }

    static void gen_plane(dsp::vector3d_t *pl, dsp::point3d_t *p)
    {
        do
        {
            p[0].x  = randf(-10.0f, 10.0f);
            p[0].y  = randf(-10.0f, 10.0f);
            p[0].z  = randf(-10.0f, 10.0f);
            p[0].w  = 1.0f;

            p[1].x  = randf(-10.0f, 10.0f);
            p[1].y  = randf(-10.0f, 10.0f);
            p[1].z  = randf(-10.0f, 10.0f);
            p[1].w  = 1.0f;

            p[2].x  = randf(-10.0f, 10.0f);
            p[2].y  = randf(-10.0f, 10.0f);
            p[2].z  = randf(-10.0f, 10.0f);
            p[2].w  = 1.0f;

        } while ((dsp::calc_distance_p2(&p[0], &p[1]) < 0.1f) ||
                (dsp::calc_distance_p2(&p[1], &p[2]) < 0.1f) ||
                (dsp::calc_distance_p2(&p[2], &p[0]) < 0.1f));

        dsp::calc_plane_pv(pl, p);
    }

    void test1(const char *name, colocation_x3_v1p3_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        dsp::point3d_t p[3], xp[3];
        dsp::vector3d_t pl; // Plane
        size_t r1, r2;

        printf("Testing %s...\n", name);

        for (size_t n=0; n<8; ++n)
        {
            // Generate plane
            gen_plane(&pl, p);

            // For each point location
            for (ssize_t mask=0; mask<=0x3ffff; ++mask)
            {
                xp[0].x = add(p[0].x, mask >> 0);
                xp[0].y = add(p[0].y, mask >> 2);
                xp[0].z = add(p[0].z, mask >> 4);
                xp[0].w = 1.0f;

                xp[1].x = add(p[1].x, mask >> 6);
                xp[1].y = add(p[1].y, mask >> 8);
                xp[1].z = add(p[1].z, mask >> 10);
                xp[1].w = 1.0f;

                xp[2].x = add(p[2].x, mask >> 12);
                xp[2].y = add(p[2].y, mask >> 14);
                xp[2].z = add(p[2].z, mask >> 16);
                xp[2].w = 1.0f;

                r1      = generic::colocation_x3_v1p3(&pl, &xp[0], &xp[1], &xp[2]);
                r2      = func(&pl, &xp[0], &xp[1], &xp[2]);

                if (r1 != r2)
                {
                    eprintf("pl    = {%f, %f, %f, %f}\n", pl.dx, pl.dy, pl.dz, pl.dw);
                    eprintf("xp[0] = {%f, %f, %f, %f}\n", xp[0].x, xp[0].y, xp[0].z, xp[0].w);
                    eprintf("xp[1] = {%f, %f, %f, %f}\n", xp[1].x, xp[1].y, xp[1].z, xp[1].w);
                    eprintf("xp[2] = {%f, %f, %f, %f}\n", xp[2].x, xp[2].y, xp[2].z, xp[2].w);
                    eprintf("r1    = 0x%x", int(r1));
                    eprintf("r2    = 0x%x", int(r2));
                    UTEST_FAIL_MSG("Failed execution (see input data)");
                }
            }
        }
    }

    void test2(const char *name, colocation_x3_v1pv_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        dsp::point3d_t p[3], xp[3];
        dsp::vector3d_t pl; // Plane
        size_t r1, r2;

        printf("Testing %s...\n", name);

        for (size_t n=0; n<8; ++n)
        {
            // Generate plane
            gen_plane(&pl, p);

            // For each point location
            for (ssize_t mask=0; mask<=0x3ffff; ++mask)
            {
                xp[0].x = add(p[0].x, mask >> 0);
                xp[0].y = add(p[0].y, mask >> 2);
                xp[0].z = add(p[0].z, mask >> 4);
                xp[0].w = 1.0f;

                xp[1].x = add(p[1].x, mask >> 6);
                xp[1].y = add(p[1].y, mask >> 8);
                xp[1].z = add(p[1].z, mask >> 10);
                xp[1].w = 1.0f;

                xp[2].x = add(p[2].x, mask >> 12);
                xp[2].y = add(p[2].y, mask >> 14);
                xp[2].z = add(p[2].z, mask >> 16);
                xp[2].w = 1.0f;

                r1      = generic::colocation_x3_v1pv(&pl, xp);
                r2      = func(&pl, xp);

                if (r1 != r2)
                {
                    eprintf("pl    = {%f, %f, %f, %f}\n", pl.dx, pl.dy, pl.dz, pl.dw);
                    eprintf("xp[0] = {%f, %f, %f, %f}\n", xp[0].x, xp[0].y, xp[0].z, xp[0].w);
                    eprintf("xp[1] = {%f, %f, %f, %f}\n", xp[1].x, xp[1].y, xp[1].z, xp[1].w);
                    eprintf("xp[2] = {%f, %f, %f, %f}\n", xp[2].x, xp[2].y, xp[2].z, xp[2].w);
                    eprintf("r1    = 0x%x", int(r1));
                    eprintf("r2    = 0x%x", int(r2));
                    UTEST_FAIL_MSG("Failed execution (see input data)");
                }
            }
        }
    }

    void test3(const char *name, colocation_x3_v3p1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        dsp::point3d_t p[3], xp;
        dsp::vector3d_t pl[3]; // Plane
        size_t r1, r2;

        printf("Testing %s...\n", name);

        for (size_t n=0; n<8; ++n)
        {
            // Generate planes
            gen_plane(&pl[0], p);
            gen_plane(&pl[1], p);
            gen_plane(&pl[2], p);

            // For each point location
            for (ssize_t mask=0; mask<=0x3f; ++mask)
            {
                for (size_t i=0; i<3; ++i)
                {
                    xp.x = add(p[0].x, mask >> 0);
                    xp.y = add(p[0].y, mask >> 2);
                    xp.z = add(p[0].z, mask >> 4);
                    xp.w = 1.0f;

                    r1      = generic::colocation_x3_v3p1(&pl[0], &pl[1], &pl[2], &xp);
                    r2      = func(&pl[0], &pl[1], &pl[2], &xp);

                    if (r1 != r2)
                    {
                        eprintf("pl[0] = {%f, %f, %f, %f}\n", pl[0].dx, pl[0].dy, pl[0].dz, pl[0].dw);
                        eprintf("pl[1] = {%f, %f, %f, %f}\n", pl[1].dx, pl[1].dy, pl[1].dz, pl[1].dw);
                        eprintf("pl[2] = {%f, %f, %f, %f}\n", pl[2].dx, pl[2].dy, pl[2].dz, pl[2].dw);
                        eprintf("xp    = {%f, %f, %f, %f}\n", xp.x, xp.y, xp.z, xp.w);
                        eprintf("r1    = 0x%x", int(r1));
                        eprintf("r2    = 0x%x", int(r2));
                        UTEST_FAIL_MSG("Failed execution (see input data)");
                    }
                }
            }
        }
    }

    void test4(const char *name, colocation_x3_vvp1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        dsp::point3d_t p[3], xp;
        dsp::vector3d_t pl[3]; // Plane
        size_t r1, r2;

        printf("Testing %s...\n", name);

        for (size_t n=0; n<8; ++n)
        {
            // Generate planes
            gen_plane(&pl[0], p);
            gen_plane(&pl[1], p);
            gen_plane(&pl[2], p);

            // For each point location
            for (ssize_t mask=0; mask<=0x3f; ++mask)
            {
                for (size_t i=0; i<3; ++i)
                {
                    xp.x = add(p[0].x, mask >> 0);
                    xp.y = add(p[0].y, mask >> 2);
                    xp.z = add(p[0].z, mask >> 4);
                    xp.w = 1.0f;

                    r1      = generic::colocation_x3_vvp1(pl, &xp);
                    r2      = func(pl, &xp);

                    if (r1 != r2)
                    {
                        eprintf("pl[0] = {%f, %f, %f, %f}\n", pl[0].dx, pl[0].dy, pl[0].dz, pl[0].dw);
                        eprintf("pl[1] = {%f, %f, %f, %f}\n", pl[1].dx, pl[1].dy, pl[1].dz, pl[1].dw);
                        eprintf("pl[2] = {%f, %f, %f, %f}\n", pl[2].dx, pl[2].dy, pl[2].dz, pl[2].dw);
                        eprintf("xp    = {%f, %f, %f, %f}\n", xp.x, xp.y, xp.z, xp.w);
                        eprintf("r1    = 0x%x", int(r1));
                        eprintf("r2    = 0x%x", int(r2));
                        UTEST_FAIL_MSG("Failed execution (see input data)");
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(id, func) \
            test ## id(#func, func)

        IF_ARCH_X86(CALL(1, sse::colocation_x3_v1p3));
        IF_ARCH_X86(CALL(1, sse2::colocation_x3_v1p3));
        IF_ARCH_X86(CALL(1, sse3::colocation_x3_v1p3));
        IF_ARCH_X86(CALL(1, avx::colocation_x3_v1p3));

        IF_ARCH_X86(CALL(2, sse::colocation_x3_v1pv));
        IF_ARCH_X86(CALL(2, sse2::colocation_x3_v1pv));
        IF_ARCH_X86(CALL(2, sse3::colocation_x3_v1pv));
        IF_ARCH_X86(CALL(2, avx::colocation_x3_v1pv));

        IF_ARCH_X86(CALL(3, sse::colocation_x3_v3p1));
        IF_ARCH_X86(CALL(3, sse2::colocation_x3_v3p1));
        IF_ARCH_X86(CALL(3, sse3::colocation_x3_v3p1));
        IF_ARCH_X86(CALL(3, avx::colocation_x3_v3p1));

        IF_ARCH_X86(CALL(4, sse::colocation_x3_vvp1));
        IF_ARCH_X86(CALL(4, sse2::colocation_x3_vvp1));
        IF_ARCH_X86(CALL(4, sse3::colocation_x3_vvp1));
        IF_ARCH_X86(CALL(4, avx::colocation_x3_vvp1));
    }
UTEST_END;
