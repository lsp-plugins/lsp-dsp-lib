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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <private/utest/dsp/3d/helpers.h>

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


UTEST_BEGIN("dsp.3d", area)

    void call(const char *label, calc_area_p3_t ca_p3, calc_area_pv_t ca_pv)
    {
        dsp::point3d_t pv[3];

        if ((!UTEST_SUPPORTED(ca_p3)) || (!UTEST_SUPPORTED(ca_pv)))
            return;

        printf("Testing %s...\n", label);

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize points
            for (size_t j=0; j<3; ++j)
                dsp::init_point_xyz(&pv[j], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

            // Compute the value
            float np3 = generic::calc_area_p3(&pv[0], &pv[1], &pv[2]);
            float npv = generic::calc_area_pv(pv);

            if (!float_equals_adaptive(np3, npv, DSP_3D_TOLERANCE))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                UTEST_FAIL_MSG("result of generic::calc_area_p3 (%e) differs from result of native::calc_area_pv(%e)", np3, npv);
            }

            float cp3 = ca_p3(&pv[0], &pv[1], &pv[2]);
            if (!float_equals_adaptive(np3, cp3, DSP_3D_TOLERANCE))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                UTEST_FAIL_MSG("result of generic::calc_area_p3 (%e) differs from result of %s_p3(%e)", np3, label, cp3);
            }

            float cpv = ca_pv(pv);
            if (!float_equals_adaptive(npv, cpv, DSP_3D_TOLERANCE))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                UTEST_FAIL_MSG("result of generic::calc_area_pv (%e) differs from result of %s_pv(%e)", npv, label, cpv);
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse::calc_area", sse::calc_area_p3, sse::calc_area_pv));
        IF_ARCH_X86(call("avx::calc_area", avx::calc_area_p3, avx::calc_area_pv));
        IF_ARCH_X86(call("avx_fma3::calc_area", avx::calc_area_p3_fma3, avx::calc_area_pv_fma3));
    }
UTEST_END;



