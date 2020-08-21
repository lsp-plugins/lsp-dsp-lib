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
        float check_triplet3d_p3n(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::vector3d_t *n);
        float check_triplet3d_pvn(const dsp::point3d_t *pv, const dsp::vector3d_t *n);
        float check_triplet3d_v2n(const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::vector3d_t *n);
        float check_triplet3d_vvn(const dsp::vector3d_t *v, const dsp::vector3d_t *n);
        float check_triplet3d_vv(const dsp::vector3d_t *v);
        float check_triplet3d_t(const dsp::triangle3d_t *t);
        float check_triplet3d_tn(const dsp::triangle3d_t *t, const dsp::vector3d_t *n);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float check_triplet3d_p3n(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::vector3d_t *n);
            float check_triplet3d_pvn(const dsp::point3d_t *pv, const dsp::vector3d_t *n);
            float check_triplet3d_v2n(const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::vector3d_t *n);
            float check_triplet3d_vvn(const dsp::vector3d_t *v, const dsp::vector3d_t *n);
            float check_triplet3d_vv(const dsp::vector3d_t *v);
            float check_triplet3d_t(const dsp::triangle3d_t *t);
            float check_triplet3d_tn(const dsp::triangle3d_t *t, const dsp::vector3d_t *n);
        }
    )

    typedef float (* check_triplet3d_p3n_t)(const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3, const dsp::vector3d_t *n);
    typedef float (* check_triplet3d_pvn_t)(const dsp::point3d_t *pv, const dsp::vector3d_t *n);
    typedef float (* check_triplet3d_v2n_t)(const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, const dsp::vector3d_t *n);
    typedef float (* check_triplet3d_vvn_t)(const dsp::vector3d_t *v, const dsp::vector3d_t *n);
    typedef float (* check_triplet3d_vv_t)(const dsp::vector3d_t *v);
    typedef float (* check_triplet3d_t_t)(const dsp::triangle3d_t *t);
    typedef float (* check_triplet3d_tn_t)(const dsp::triangle3d_t *t, const dsp::vector3d_t *n);
}

UTEST_BEGIN("dsp.3d", triplet)

    void call(
            const char *label,
            check_triplet3d_p3n_t check_triplet3d_p3n,
            check_triplet3d_pvn_t check_triplet3d_pvn,
            check_triplet3d_v2n_t check_triplet3d_v2n,
            check_triplet3d_vvn_t check_triplet3d_vvn,
            check_triplet3d_vv_t check_triplet3d_vv,
            check_triplet3d_t_t check_triplet3d_t,
            check_triplet3d_tn_t check_triplet3d_tn
        )
    {
        if ((!UTEST_SUPPORTED(check_triplet3d_p3n)) ||
            (!UTEST_SUPPORTED(check_triplet3d_pvn)) ||
            (!UTEST_SUPPORTED(check_triplet3d_v2n)) ||
            (!UTEST_SUPPORTED(check_triplet3d_vvn)) ||
            (!UTEST_SUPPORTED(check_triplet3d_vv)) ||
            (!UTEST_SUPPORTED(check_triplet3d_t)) ||
            (!UTEST_SUPPORTED(check_triplet3d_tn))
        )
            return;

        printf("Launching %s implementation\n", label);

        dsp::triangle3d_t t[3];
        dsp::calc_triangle3d_xyz(&t[0], 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, -3.0f, 3.0f, 1.0f);
        dsp::calc_triangle3d_xyz(&t[1], 1.0f, 2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 3.0f, -3.0f);
        dsp::calc_triangle3d_xyz(&t[2], 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, 2.0f, 3.0f, 1.0f, -3.0f);

        dsp::vector3d_t n1, n2;
        dsp::init_vector_dxyz(&n1, 1.0f, 1.0f, 1.0f);
        dsp::init_vector_dxyz(&n2, -1.0f, -1.0f, -1.0f);

        for (size_t i=0; i<3; ++i)
        {
            printf("Checking triangle %d\n", int(i));

            UTEST_ASSERT(check_triplet3d_t(&t[i]) >= 0.0f);
            UTEST_ASSERT(check_triplet3d_tn(&t[i], &n1) >= 0.0f);
            UTEST_ASSERT(check_triplet3d_tn(&t[i], &n2) <= 0.0f);

            UTEST_ASSERT(check_triplet3d_p3n(&t[i].p[0], &t[i].p[1], &t[i].p[2], &n1) >= 0.0f);
            UTEST_ASSERT(check_triplet3d_p3n(&t[i].p[0], &t[i].p[1], &t[i].p[2], &n2) <= 0.0f);
            UTEST_ASSERT(check_triplet3d_pvn(&t[i].p[0], &n1) >= 0.0f);
            UTEST_ASSERT(check_triplet3d_pvn(&t[i].p[0], &n2) <= 0.0f);

            dsp::vector3d_t v[3];
            dsp::init_vector_p2(&v[0], &t->p[0], &t->p[1]);
            dsp::init_vector_p2(&v[1], &t->p[1], &t->p[2]);
            dsp::init_vector_p2(&v[2], &t->p[2], &t->p[0]);

            UTEST_ASSERT(check_triplet3d_v2n(&v[0], &v[1], &n1) >= 0.0f);
            UTEST_ASSERT(check_triplet3d_v2n(&v[1], &v[2], &n1) >= 0.0f);
            UTEST_ASSERT(check_triplet3d_v2n(&v[2], &v[0], &n1) >= 0.0f);

            UTEST_ASSERT(check_triplet3d_v2n(&v[0], &v[1], &n2) <= 0.0f);
            UTEST_ASSERT(check_triplet3d_v2n(&v[1], &v[2], &n2) <= 0.0f);
            UTEST_ASSERT(check_triplet3d_v2n(&v[2], &v[0], &n2) <= 0.0f);

            dsp::init_vector(&v[2], &n1);
            UTEST_ASSERT(check_triplet3d_vv(v) >= 0.0f);

            dsp::init_vector(&v[2], &n2);
            UTEST_ASSERT(check_triplet3d_vv(v) <= 0.0f);
        }
    }

    UTEST_MAIN
    {
        call("generic_ck_triplet",
                generic::check_triplet3d_p3n,
                generic::check_triplet3d_pvn,
                generic::check_triplet3d_v2n,
                generic::check_triplet3d_vvn,
                generic::check_triplet3d_vv,
                generic::check_triplet3d_t,
                generic::check_triplet3d_tn
                );

        IF_ARCH_X86(call("sse_ck_triplet",
                sse::check_triplet3d_p3n,
                sse::check_triplet3d_pvn,
                sse::check_triplet3d_v2n,
                sse::check_triplet3d_vvn,
                sse::check_triplet3d_vv,
                sse::check_triplet3d_t,
                sse::check_triplet3d_tn
                ));
    }

UTEST_END
