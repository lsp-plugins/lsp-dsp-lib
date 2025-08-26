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

UTEST_BEGIN("dsp.3d", colocation_x2)

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

    void test(const char *name, colocation_x2_v1p2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        dsp::point3d_t p[3], xp[2];
        dsp::vector3d_t pl; // Plane
        size_t r1, r2;

        printf("Testing %s...\n", name);

        for (size_t n=0; n<32; ++n)
        {
            // Generate plane
            gen_plane(&pl, p);

            // For each point location
            for (ssize_t mask=0; mask<=0xfff; ++mask)
            {
                xp[0].x = add(p[0].x, mask >> 0);
                xp[0].y = add(p[0].y, mask >> 2);
                xp[0].z = add(p[0].z, mask >> 4);
                xp[0].w = 1.0f;

                xp[1].x = add(p[1].x, mask >> 6);
                xp[1].y = add(p[1].y, mask >> 8);
                xp[1].z = add(p[1].z, mask >> 10);
                xp[1].w = 1.0f;

                r1      = generic::colocation_x2_v1p2(&pl, &xp[0], &xp[1]);
                r2      = func(&pl, &xp[0], &xp[1]);

                if (r1 != r2)
                {
                    eprintf("pl    = {%f, %f, %f, %f}\n", pl.dx, pl.dy, pl.dz, pl.dw);
                    eprintf("xp[0] = {%f, %f, %f, %f}\n", xp[0].x, xp[0].y, xp[0].z, xp[0].w);
                    eprintf("xp[1] = {%f, %f, %f, %f}\n", xp[1].x, xp[1].y, xp[1].z, xp[1].w);
                    eprintf("r1    = 0x%x", int(r1));
                    eprintf("r2    = 0x%x", int(r2));
                    UTEST_FAIL_MSG("Failed execution (see input data)");
                }
            }
        }
    }

    void test(const char *name, colocation_x2_v1pv_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        dsp::point3d_t p[3], xp[2];
        dsp::vector3d_t pl; // Plane
        size_t r1, r2;

        printf("Testing %s...\n", name);

        for (size_t n=0; n<32; ++n)
        {
            // Generate plane
            gen_plane(&pl, p);

            // For each point location
            for (ssize_t mask=0; mask<=0xfff; ++mask)
            {
                xp[0].x = add(p[0].x, mask >> 0);
                xp[0].y = add(p[0].y, mask >> 2);
                xp[0].z = add(p[0].z, mask >> 4);
                xp[0].w = 1.0f;

                xp[1].x = add(p[1].x, mask >> 6);
                xp[1].y = add(p[1].y, mask >> 8);
                xp[1].z = add(p[1].z, mask >> 10);
                xp[1].w = 1.0f;

                r1      = generic::colocation_x2_v1pv(&pl, xp);
                r2      = func(&pl, xp);

                if (r1 != r2)
                {
                    eprintf("pl    = {%f, %f, %f, %f}\n", pl.dx, pl.dy, pl.dz, pl.dw);
                    eprintf("xp[0] = {%f, %f, %f, %f}\n", xp[0].x, xp[0].y, xp[0].z, xp[0].w);
                    eprintf("xp[1] = {%f, %f, %f, %f}\n", xp[1].x, xp[1].y, xp[1].z, xp[1].w);
                    eprintf("r1    = 0x%x", int(r1));
                    eprintf("r2    = 0x%x", int(r2));
                    UTEST_FAIL_MSG("Failed execution (see input data)");
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func) \
            test(#func, func)

        IF_ARCH_X86(CALL(sse::colocation_x2_v1p2));
        IF_ARCH_X86(CALL(sse2::colocation_x2_v1p2));
        IF_ARCH_X86(CALL(sse3::colocation_x2_v1p2));
        IF_ARCH_X86(CALL(avx::colocation_x2_v1p2));

        IF_ARCH_X86(CALL(sse::colocation_x2_v1pv));
        IF_ARCH_X86(CALL(sse2::colocation_x2_v1pv));
        IF_ARCH_X86(CALL(sse3::colocation_x2_v1pv));
        IF_ARCH_X86(CALL(avx::colocation_x2_v1pv));
    }
UTEST_END;
