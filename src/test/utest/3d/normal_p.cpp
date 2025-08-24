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
#include <lsp-plug.in/test-fw/helpers.h>
#include <private/utest/dsp/3d/helpers.h>

namespace lsp
{
    namespace generic
    {
        void calc_normal3d_p3(dsp::vector3d_t *n, const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
        void calc_normal3d_pv(dsp::vector3d_t *n, const dsp::point3d_t *pv);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void calc_normal3d_p3(dsp::vector3d_t *n, const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
            void calc_normal3d_pv(dsp::vector3d_t *n, const dsp::point3d_t *pv);
        }

        namespace avx
        {
            void calc_normal3d_p3(dsp::vector3d_t *n, const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
            void calc_normal3d_pv(dsp::vector3d_t *n, const dsp::point3d_t *pv);

            void calc_normal3d_p3_fma3(dsp::vector3d_t *n, const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
            void calc_normal3d_pv_fma3(dsp::vector3d_t *n, const dsp::point3d_t *pv);
        }
    )

    typedef void (* calc_normal3d_p3_t)(dsp::vector3d_t *n, const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
    typedef void (* calc_normal3d_pv_t)(dsp::vector3d_t *n, const dsp::point3d_t *pv);
}

UTEST_BEGIN("dsp.3d", normal_p)

    void call(const char *label, calc_normal3d_p3_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        dsp::point3d_t p[3];
        dsp::vector3d_t n[2];

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize vectors
            dsp::init_point_xyz(&p[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            if (i != 0)
            {
                dsp::init_point_xyz(&p[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
                dsp::init_point_xyz(&p[2], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            }
            else
            {
                dsp::init_point(&p[1], &p[0]);
                dsp::init_point(&p[2], &p[0]);
            }

            // Compute the value
            generic::calc_normal3d_p3(&n[0], &p[0], &p[1], &p[2]);
            fn(&n[1], &p[0], &p[1], &p[2]);

            if (!vector3d_ack(&n[0], &n[1]))
            {
                dump_point("p[0]", &p[0]);
                dump_point("p[1]", &p[1]);
                dump_point("p[2]", &p[2]);
                dump_vector("n[0]", &n[0]);
                dump_vector("n[1]", &n[1]);
                UTEST_FAIL_MSG("result of %s differs", label);
            }
        }
    }

    void call(const char *label, calc_normal3d_pv_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        dsp::point3d_t p[3];
        dsp::vector3d_t n[2];

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize vectors
            dsp::init_point_xyz(&p[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            if (i != 0)
            {
                dsp::init_point_xyz(&p[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
                dsp::init_point_xyz(&p[2], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            }
            else
            {
                dsp::init_point(&p[1], &p[0]);
                dsp::init_point(&p[2], &p[0]);
            }

            // Compute the value
            generic::calc_normal3d_pv(&n[0], p);
            fn(&n[1], p);

            if (!vector3d_ack(&n[0], &n[1]))
            {
                dump_point("p[0]", &p[0]);
                dump_point("p[1]", &p[1]);
                dump_point("p[2]", &p[2]);
                dump_vector("n[0]", &n[0]);
                dump_vector("n[1]", &n[1]);
                UTEST_FAIL_MSG("result of %s differs", label);
            }
        }
    }

    UTEST_MAIN
    {
    #define CALL(func) \
        call(#func, func)

        IF_ARCH_X86(CALL(sse::calc_normal3d_p3));
        IF_ARCH_X86(CALL(sse::calc_normal3d_pv));

        IF_ARCH_X86(CALL(avx::calc_normal3d_p3));
        IF_ARCH_X86(CALL(avx::calc_normal3d_pv));

        IF_ARCH_X86(CALL(avx::calc_normal3d_p3_fma3));
        IF_ARCH_X86(CALL(avx::calc_normal3d_pv_fma3));
    }

UTEST_END


