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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <private/utest/dsp/3d/helpers.h>

#define TOLERANCE       1e-4f

namespace lsp
{
    namespace generic
    {
        float calc_plane_p3(dsp::vector3d_t *v, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
        float calc_plane_pv(dsp::vector3d_t *v, const dsp::point3d_t *pv);
        float calc_plane_v1p2(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float calc_plane_p3(dsp::vector3d_t *v, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            float calc_plane_pv(dsp::vector3d_t *v, const dsp::point3d_t *pv);
            float calc_plane_v1p2(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
        }

        namespace avx
        {
            float calc_plane_p3(dsp::vector3d_t *v, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            float calc_plane_pv(dsp::vector3d_t *v, const dsp::point3d_t *pv);
            float calc_plane_v1p2(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);

            float calc_plane_p3_fma3(dsp::vector3d_t *v, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
            float calc_plane_pv_fma3(dsp::vector3d_t *v, const dsp::point3d_t *pv);
            float calc_plane_v1p2_fma3(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
        }
    )

    typedef float (* calc_plane_p3_t)(dsp::vector3d_t *v, const dsp::point3d_t *p0, const dsp::point3d_t *p1, const dsp::point3d_t *p2);
    typedef float (* calc_plane_pv_t)(dsp::vector3d_t *v, const dsp::point3d_t *pv);
    typedef float (* calc_plane_v1p2_t)(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
}

UTEST_BEGIN("dsp.3d", plane)

    void call(const char *label, calc_plane_p3_t f)
    {
        if (!UTEST_SUPPORTED(f))
            return;

        printf("Testing %s...\n", label);

        dsp::point3d_t pv[3];
        dsp::vector3d_t v1, v2;

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize points
            for (size_t j=0; j<3; ++j)
                dsp::init_point_xyz(&pv[j], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

//            pv[0] = { 6.411041e+00, 5.610571e+00, 3.340876e+00, 1.000000e+00 };
//            pv[1] = { 2.980657e+00, 5.403591e+00, -8.846043e+00, 1.000000e+00 };
//            pv[2] = { 8.912350e+00, 4.731712e+00, 2.927614e+00, 1.000000e+00 };

            // Compute the value
            float w1  = generic::calc_plane_p3(&v1, &pv[0], &pv[1], &pv[2]);
            float w2  = f(&v2, &pv[0], &pv[1], &pv[2]);

            if ((!float_equals_adaptive(w1, w2, DSP_3D_TOLERANCE)) ||
                (!vector3d_ack(&v1, &v2, TOLERANCE)))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                dump_vector("v[0]", &v1);
                dump_vector("v[1]", &v2);
                printf("w[0] = %e, w[1] = %e\n", w1, w2);
                UTEST_FAIL_MSG("result of functions differ");
            }
        }
    }

    void call(const char *label, calc_plane_pv_t f)
    {
        if (!UTEST_SUPPORTED(f))
            return;

        printf("Testing %s...\n", label);

        dsp::point3d_t pv[3];
        dsp::vector3d_t v1, v2;

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize points
            for (size_t j=0; j<3; ++j)
                dsp::init_point_xyz(&pv[j], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

    //            pv[0] = { 6.411041e+00, 5.610571e+00, 3.340876e+00, 1.000000e+00 };
    //            pv[1] = { 2.980657e+00, 5.403591e+00, -8.846043e+00, 1.000000e+00 };
    //            pv[2] = { 8.912350e+00, 4.731712e+00, 2.927614e+00, 1.000000e+00 };

            // Compute the value
            float w1  = generic::calc_plane_pv(&v1, pv);
            float w2  = f(&v2, pv);

            if ((!float_equals_adaptive(w1, w2, DSP_3D_TOLERANCE)) ||
                (!vector3d_ack(&v1, &v2, TOLERANCE)))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                dump_vector("v[0]", &v1);
                dump_vector("v[1]", &v2);
                printf("w[0] = %e, w[1] = %e\n", w1, w2);
                UTEST_FAIL_MSG("result of functions differ");
            }
        }
    }

    void call(const char *label, calc_plane_v1p2_t f)
    {
        if (!UTEST_SUPPORTED(f))
            return;

        printf("Testing %s...\n", label);

        dsp::point3d_t pv[2];
        dsp::vector3d_t sv;
        dsp::vector3d_t v1, v2;

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize points
            dsp::init_vector_dxyz(&sv, randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&pv[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&pv[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

            // Compute the value
            float w1  = generic::calc_plane_v1p2(&v1, &sv, &pv[0], &pv[1]);
            float w2  = f(&v2, &sv, &pv[0], &pv[1]);

            if ((!float_equals_adaptive(w1, w2, DSP_3D_TOLERANCE)) ||
                (!vector3d_ack(&v1, &v2, TOLERANCE)))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                dump_vector("v[0]", &v1);
                dump_vector("v[1]", &v2);
                printf("w[0] = %e, w[1] = %e\n", w1, w2);
                UTEST_FAIL_MSG("result of functions differ");
            }
        }
    }

    UTEST_MAIN
    {
    #define CALL(func) \
        call(#func, func)

        IF_ARCH_X86(CALL(sse::calc_plane_p3));
        IF_ARCH_X86(CALL(sse::calc_plane_pv));
        IF_ARCH_X86(CALL(sse::calc_plane_v1p2));

        IF_ARCH_X86(CALL(avx::calc_plane_p3));
        IF_ARCH_X86(CALL(avx::calc_plane_pv));
        IF_ARCH_X86(CALL(avx::calc_plane_v1p2));

        IF_ARCH_X86(CALL(avx::calc_plane_p3_fma3));
        IF_ARCH_X86(CALL(avx::calc_plane_pv_fma3));
        IF_ARCH_X86(CALL(avx::calc_plane_v1p2_fma3));
    }
UTEST_END;


