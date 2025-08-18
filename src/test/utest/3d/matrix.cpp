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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <private/utest/dsp/3d/helpers.h>

namespace lsp
{
    namespace generic
    {
        void init_matrix3d(dsp::matrix3d_t *dst, const dsp::matrix3d_t *src);
        void init_matrix3d_zero(dsp::matrix3d_t *m);
        void init_matrix3d_one(dsp::matrix3d_t *m);
        void init_matrix3d_identity(dsp::matrix3d_t *m);
        void transpose_matrix3d1(dsp::matrix3d_t *r);
        void transpose_matrix3d2(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);

        void init_matrix3d_translate(dsp::matrix3d_t *m, float dx, float dy, float dz);
        void init_matrix3d_scale(dsp::matrix3d_t *m, float sx, float sy, float sz);
        void init_matrix3d_rotate_x(dsp::matrix3d_t *m, float angle);
        void init_matrix3d_rotate_y(dsp::matrix3d_t *m, float angle);
        void init_matrix3d_rotate_z(dsp::matrix3d_t *m, float angle);
        void init_matrix3d_rotate_xyz(dsp::matrix3d_t *m, float x, float y, float z, float angle);

        void apply_matrix3d_mv2(dsp::vector3d_t *r, const dsp::vector3d_t *v, const dsp::matrix3d_t *m);
        void apply_matrix3d_mv1(dsp::vector3d_t *r, const dsp::matrix3d_t *m);
        void apply_matrix3d_mp2(dsp::point3d_t *r, const dsp::point3d_t *p, const dsp::matrix3d_t *m);
        void apply_matrix3d_mp1(dsp::point3d_t *r, const dsp::matrix3d_t *m);
        void apply_matrix3d_mm2(dsp::matrix3d_t *r, const dsp::matrix3d_t *s, const dsp::matrix3d_t *m);
        void apply_matrix3d_mm1(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void init_matrix3d(dsp::matrix3d_t *dst, const dsp::matrix3d_t *src);
            void init_matrix3d_zero(dsp::matrix3d_t *m);
            void init_matrix3d_one(dsp::matrix3d_t *m);
            void init_matrix3d_identity(dsp::matrix3d_t *m);
            void transpose_matrix3d1(dsp::matrix3d_t *r);
            void transpose_matrix3d2(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);

            void init_matrix3d_translate(dsp::matrix3d_t *m, float dx, float dy, float dz);
            void init_matrix3d_scale(dsp::matrix3d_t *m, float sx, float sy, float sz);
            void init_matrix3d_rotate_x(dsp::matrix3d_t *m, float angle);
            void init_matrix3d_rotate_y(dsp::matrix3d_t *m, float angle);
            void init_matrix3d_rotate_z(dsp::matrix3d_t *m, float angle);
            void init_matrix3d_rotate_xyz(dsp::matrix3d_t *m, float x, float y, float z, float angle);

            void apply_matrix3d_mv2(dsp::vector3d_t *r, const dsp::vector3d_t *v, const dsp::matrix3d_t *m);
            void apply_matrix3d_mv1(dsp::vector3d_t *r, const dsp::matrix3d_t *m);
            void apply_matrix3d_mp2(dsp::point3d_t *r, const dsp::point3d_t *p, const dsp::matrix3d_t *m);
            void apply_matrix3d_mp1(dsp::point3d_t *r, const dsp::matrix3d_t *m);
            void apply_matrix3d_mm2(dsp::matrix3d_t *r, const dsp::matrix3d_t *s, const dsp::matrix3d_t *m);
            void apply_matrix3d_mm1(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
        }

        namespace sse2
        {
            void transpose_matrix3d1(dsp::matrix3d_t *r);
            void transpose_matrix3d2(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
        }

        namespace avx
        {
            void init_matrix3d(dsp::matrix3d_t *dst, const dsp::matrix3d_t *src);
            void init_matrix3d_zero(dsp::matrix3d_t *m);
            void init_matrix3d_one(dsp::matrix3d_t *m);
            void init_matrix3d_identity(dsp::matrix3d_t *m);
            void transpose_matrix3d1(dsp::matrix3d_t *r);
            void transpose_matrix3d2(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);

            void init_matrix3d_translate(dsp::matrix3d_t *m, float dx, float dy, float dz);
            void init_matrix3d_scale(dsp::matrix3d_t *m, float sx, float sy, float sz);
            void init_matrix3d_rotate_x(dsp::matrix3d_t *m, float angle);
            void init_matrix3d_rotate_y(dsp::matrix3d_t *m, float angle);
            void init_matrix3d_rotate_z(dsp::matrix3d_t *m, float angle);
//            void init_matrix3d_rotate_xyz(dsp::matrix3d_t *m, float x, float y, float z, float angle);

            void apply_matrix3d_mp2(dsp::point3d_t *r, const dsp::point3d_t *p, const dsp::matrix3d_t *m);
            void apply_matrix3d_mp2_fma3(dsp::point3d_t *r, const dsp::point3d_t *p, const dsp::matrix3d_t *m);
            void apply_matrix3d_mp1(dsp::point3d_t *r, const dsp::matrix3d_t *m);
            void apply_matrix3d_mp1_fma3(dsp::point3d_t *r, const dsp::matrix3d_t *m);

            void apply_matrix3d_mm2(dsp::matrix3d_t *r, const dsp::matrix3d_t *s, const dsp::matrix3d_t *m);
            void apply_matrix3d_mm2_fma3(dsp::matrix3d_t *r, const dsp::matrix3d_t *s, const dsp::matrix3d_t *m);
            void apply_matrix3d_mm1(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
            void apply_matrix3d_mm1_fma3(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);

            void apply_matrix3d_mv2(dsp::vector3d_t *r, const dsp::vector3d_t *v, const dsp::matrix3d_t *m);
            void apply_matrix3d_mv2_fma3(dsp::vector3d_t *r, const dsp::vector3d_t *v, const dsp::matrix3d_t *m);
            void apply_matrix3d_mv1(dsp::vector3d_t *r, const dsp::matrix3d_t *m);
            void apply_matrix3d_mv1_fma3(dsp::vector3d_t *r, const dsp::matrix3d_t *m);
        }
    )

    typedef void (* init_matrix3d_t)(dsp::matrix3d_t *dst, const dsp::matrix3d_t *src);
    typedef void (* init_matrix3d_zero_t)(dsp::matrix3d_t *m);
    typedef void (* init_matrix3d_one_t)(dsp::matrix3d_t *m);
    typedef void (* init_matrix3d_identity_t)(dsp::matrix3d_t *m);
    typedef void (* init_matrix3d_translate_t)(dsp::matrix3d_t *m, float dx, float dy, float dz);
    typedef void (* init_matrix3d_scale_t)(dsp::matrix3d_t *m, float sx, float sy, float sz);
    typedef void (* init_matrix3d_rotate_x_t)(dsp::matrix3d_t *m, float angle);
    typedef void (* init_matrix3d_rotate_y_t)(dsp::matrix3d_t *m, float angle);
    typedef void (* init_matrix3d_rotate_z_t)(dsp::matrix3d_t *m, float angle);
    typedef void (* init_matrix3d_rotate_xyz_t)(dsp::matrix3d_t *m, float x, float y, float z, float angle);
    typedef void (* apply_matrix3d_mv2_t)(dsp::vector3d_t *r, const dsp::vector3d_t *v, const dsp::matrix3d_t *m);
    typedef void (* apply_matrix3d_mv1_t)(dsp::vector3d_t *r, const dsp::matrix3d_t *m);
    typedef void (* apply_matrix3d_mp2_t)(dsp::point3d_t *r, const dsp::point3d_t *p, const dsp::matrix3d_t *m);
    typedef void (* apply_matrix3d_mp1_t)(dsp::point3d_t *r, const dsp::matrix3d_t *m);
    typedef void (* apply_matrix3d_mm2_t)(dsp::matrix3d_t *r, const dsp::matrix3d_t *s, const dsp::matrix3d_t *m);
    typedef void (* apply_matrix3d_mm1_t)(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
    typedef void (* transpose_matrix3d1_t)(dsp::matrix3d_t *r);
    typedef void (* transpose_matrix3d2_t)(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
}

UTEST_BEGIN("dsp.3d", matrix)

    static void fill_matrix(dsp::matrix3d_t & m)
    {
        for (size_t i=0; i<16; ++i)
            m.m[i]  = (i + 1) * 0.01f;
    }

    void init_data(
            const char *label,
            init_matrix3d_t init_matrix3d,
            init_matrix3d_zero_t init_matrix3d_zero,
            init_matrix3d_one_t init_matrix3d_one,
            init_matrix3d_identity_t init_matrix3d_identity,
            transpose_matrix3d1_t transpose_matrix3d1,
            transpose_matrix3d2_t transpose_matrix3d2
        )
    {
        if ((!UTEST_SUPPORTED(init_matrix3d)) ||
            (!UTEST_SUPPORTED(init_matrix3d_zero)) ||
            (!UTEST_SUPPORTED(init_matrix3d_one)) ||
            (!UTEST_SUPPORTED(init_matrix3d_identity)) ||
            (!UTEST_SUPPORTED(transpose_matrix3d1)) ||
            (!UTEST_SUPPORTED(transpose_matrix3d2))
           )
            return;

        printf("Testing %s\n", label);

        dsp::matrix3d_t m1, m2, m3;
        fill_matrix(m1);
        fill_matrix(m2);
        fill_matrix(m3);

        generic::init_matrix3d_zero(&m1);
        init_matrix3d_zero(&m2);
        UTEST_ASSERT_MSG(matrix3d_ck(&m1, &m2), "init_matrix3d_zero failed");

        generic::init_matrix3d_one(&m1);
        init_matrix3d_one(&m2);
        UTEST_ASSERT_MSG(matrix3d_ck(&m1, &m2), "init_matrix3d_one failed");

        generic::init_matrix3d_identity(&m1);
        init_matrix3d_identity(&m2);
        UTEST_ASSERT_MSG(matrix3d_ck(&m1, &m2), "init_matrix3d_identity failed");

        for (size_t i=0; i<16; ++i)
            m3.m[i] = i + 1;

        generic::init_matrix3d(&m1, &m3);
        init_matrix3d(&m2, &m3);

        UTEST_ASSERT_MSG(matrix3d_ck(&m1, &m2), "init_matrix3d failed");
        UTEST_ASSERT_MSG(matrix3d_ck(&m2, &m2), "init_matrix3d failed");

        dsp::matrix3d_t m1t, m2t;
        generic::transpose_matrix3d1(&m1);
        transpose_matrix3d1(&m2);
        UTEST_ASSERT_MSG(matrix3d_ck(&m1, &m2), "transpose_matrix3d1 failed");

        generic::transpose_matrix3d2(&m1t, &m3);
        transpose_matrix3d2(&m2t, &m3);
        UTEST_ASSERT_MSG(matrix3d_ck(&m1t, &m2t), "transpose_matrix3d2 failed");

        UTEST_ASSERT_MSG(matrix3d_ck(&m1, &m1t), "transpose_matrix3d2 not matches transpose_matrix3d1");
        UTEST_ASSERT_MSG(matrix3d_ck(&m2, &m2t), "transpose_matrix3d2 not matches transpose_matrix3d1");
    }

    void transform(
            const char *label,
            init_matrix3d_identity_t init_matrix3d_identity,
            init_matrix3d_translate_t init_matrix3d_translate,
            init_matrix3d_scale_t init_matrix3d_scale,
            init_matrix3d_rotate_x_t init_matrix3d_rotate_x,
            init_matrix3d_rotate_y_t init_matrix3d_rotate_y,
            init_matrix3d_rotate_z_t init_matrix3d_rotate_z,
            init_matrix3d_rotate_xyz_t init_matrix3d_rotate_xyz,
            apply_matrix3d_mm1_t apply_matrix3d_mm1,
            apply_matrix3d_mm2_t apply_matrix3d_mm2
    )
    {
        if ((!UTEST_SUPPORTED(init_matrix3d_identity)) ||
            (!UTEST_SUPPORTED(init_matrix3d_translate)) ||
            (!UTEST_SUPPORTED(init_matrix3d_scale)) ||
            (!UTEST_SUPPORTED(init_matrix3d_rotate_x)) ||
            (!UTEST_SUPPORTED(init_matrix3d_rotate_y)) ||
            (!UTEST_SUPPORTED(init_matrix3d_rotate_z)) ||
            (!UTEST_SUPPORTED(init_matrix3d_rotate_xyz)) ||
            (!UTEST_SUPPORTED(apply_matrix3d_mm1)) ||
            (!UTEST_SUPPORTED(apply_matrix3d_mm2))
           )
            return;

        printf("Testing %s\n", label);

        dsp::matrix3d_t  i1, t1, s1, rx1, ry1, rz1, rxyz1;
        dsp::matrix3d_t  i2, t2, s2, rx2, ry2, rz2, rxyz2;

        fill_matrix(i1);
        fill_matrix(t1);
        fill_matrix(s1);
        fill_matrix(rx1);
        fill_matrix(ry1);
        fill_matrix(rz1);
        fill_matrix(rxyz1);

        fill_matrix(i2);
        fill_matrix(t2);
        fill_matrix(s2);
        fill_matrix(rx2);
        fill_matrix(ry2);
        fill_matrix(rz2);
        fill_matrix(rxyz2);

        generic::init_matrix3d_identity(&i1);
        init_matrix3d_identity(&i2);
        UTEST_ASSERT_MSG(matrix3d_ck(&i1, &i2), "init_matrix3d_identity failed");

        generic::init_matrix3d_translate(&t1, 1.0f, 2.0f, 3.0f);
        init_matrix3d_translate(&t2, 1.0f, 2.0f, 3.0f);
        UTEST_ASSERT_MSG(matrix3d_ck(&t1, &t2), "init_dsp::matrix3d_translate failed");

        generic::init_matrix3d_scale(&s1, 1.0f, 2.0f, 3.0f);
        init_matrix3d_scale(&s2, 1.0f, 2.0f, 3.0f);
        UTEST_ASSERT_MSG(matrix3d_ck(&s1, &s2), "init_matrix3d_scale failed");

        generic::init_matrix3d_rotate_x(&rx1, 2.0f);
        init_matrix3d_rotate_x(&rx2, 2.0f);
        UTEST_ASSERT_MSG(matrix3d_ck(&rx1, &rx2), "init_matrix3d_rotate_x failed");

        generic::init_matrix3d_rotate_y(&ry1, 2.0f);
        init_matrix3d_rotate_y(&ry2, 2.0f);
        UTEST_ASSERT_MSG(matrix3d_ck(&ry1, &ry2), "init_matrix3d_rotate_y failed");

        generic::init_matrix3d_rotate_z(&rz1, 2.0f);
        init_matrix3d_rotate_z(&rz2, 2.0f);
        UTEST_ASSERT_MSG(matrix3d_ck(&rz1, &rz2), "init_matrix3d_rotate_z failed");

        generic::init_matrix3d_rotate_xyz(&rxyz1, 1.0f, 2.0f, 3.0f, 2.0f);
        init_matrix3d_rotate_xyz(&rxyz2, 1.0f, 2.0f, 3.0f, 2.0f);
        UTEST_ASSERT_MSG(matrix3d_ck(&rxyz1, &rxyz2), "init_matrix3d_rotate_xyz failed");

        generic::apply_matrix3d_mm1(&i1, &t1);
        apply_matrix3d_mm1(&i2, &t2);
        UTEST_ASSERT_MSG(matrix3d_ck(&i1, &i2), "apply_matrix3d_mm1 failed");

        generic::apply_matrix3d_mm2(&t1, &i1, &s1);
        apply_matrix3d_mm2(&t2, &i2, &s2);
        UTEST_ASSERT_MSG(matrix3d_ck(&t1, &t2), "apply_matrix3d_mm2 failed");

        generic::apply_matrix3d_mm2(&i1, &t1, &rx1);
        apply_matrix3d_mm2(&i2, &t2, &rx2);
        UTEST_ASSERT_MSG(matrix3d_ck(&i1, &i2), "apply_matrix3d_mm2 failed");

        generic::apply_matrix3d_mm1(&i1, &ry1);
        apply_matrix3d_mm1(&i2, &ry2);
        UTEST_ASSERT_MSG(matrix3d_ck(&i1, &i2), "apply_matrix3d_mm1 failed");

        generic::apply_matrix3d_mm1(&i1, &rz1);
        apply_matrix3d_mm1(&i2, &rz2);
        UTEST_ASSERT_MSG(matrix3d_ck(&i1, &i2), "apply_matrix3d_mm1 failed");

        generic::apply_matrix3d_mm2(&t1, &i1, &rxyz1);
        apply_matrix3d_mm2(&t2, &i2, &rxyz2);
        UTEST_ASSERT_MSG(matrix3d_ck(&t1, &t2), "apply_matrix3d_mm2 failed");
    }

    void modify_point(
            const char *label,
            apply_matrix3d_mp1_t apply_matrix3d_mp1,
            apply_matrix3d_mp2_t apply_matrix3d_mp2
            )
    {
        if ((!UTEST_SUPPORTED(apply_matrix3d_mp1)) ||
            (!UTEST_SUPPORTED(apply_matrix3d_mp2)))
            return;

        dsp::matrix3d_t m;
        dsp::point3d_t p1, p2, p3, p4, pc;

        printf("Testing %s\n", label);

        // Apply identity matrix
        dsp::init_point_xyz(&pc, 1.0f, 2.0f, 3.0f);
        dsp::init_point_xyz(&p1, 1.0f, 2.0f, 3.0f);
        dsp::init_point(&p2, &p1);
        dsp::init_matrix3d_identity(&m);

        generic::apply_matrix3d_mp1(&p1, &m);
        apply_matrix3d_mp1(&p2, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on identity matrix");
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on identity matrix");

        generic::apply_matrix3d_mp2(&p3, &pc, &m);
        apply_matrix3d_mp2(&p4, &pc, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on identity matrix");
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on identity matrix");

        // Apply rotation Z matrix
        dsp::init_point_xyz(&pc, M_SQRT1_2, M_SQRT1_2, 0.0f);
        dsp::init_point_xyz(&p1, 0.0f, 1.0f, 0.0f);
        dsp::init_point(&p2, &p1);
        dsp::init_matrix3d_rotate_z(&m, M_PI * 0.25f);

        generic::apply_matrix3d_mp2(&p3, &p1, &m);
        apply_matrix3d_mp2(&p4, &p1, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on rotation z");
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on rotation z");

        generic::apply_matrix3d_mp1(&p1, &m);
        apply_matrix3d_mp1(&p2, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on rotation z");
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on rotation z");

        // Apply rotation X matrix
        dsp::init_point_xyz(&pc, 0.0f, M_SQRT1_2, M_SQRT1_2);
        dsp::init_point_xyz(&p1, 0.0f, 0.0f, 1.0f);
        dsp::init_point(&p2, &p1);
        generic::init_matrix3d_rotate_x(&m, M_PI * 0.25f);

        generic::apply_matrix3d_mp2(&p3, &p1, &m);
        apply_matrix3d_mp2(&p4, &p1, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on rotation x");
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on rotation x");

        generic::apply_matrix3d_mp1(&p1, &m);
        apply_matrix3d_mp1(&p2, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on rotation x");
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on rotation x");

        // Apply rotation Y matrix
        dsp::init_point_xyz(&pc, M_SQRT1_2, 0.0f, M_SQRT1_2);
        dsp::init_point_xyz(&p1, 1.0f, 0.0f, 0.0f);
        dsp::init_point(&p2, &p1);
        apply_matrix3d_mp1(&p2, &m);
        dsp::init_matrix3d_rotate_y(&m, M_PI * 0.25f);

        generic::apply_matrix3d_mp2(&p3, &p1, &m);
        apply_matrix3d_mp2(&p4, &p1, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on rotation y");
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on rotation y");

        generic::apply_matrix3d_mp1(&p1, &m);
        apply_matrix3d_mp1(&p2, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on rotation y");
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on rotation y");

        // Apply scale matrix
        dsp::init_point_xyz(&pc, 2.0f, 4.0f, 8.0f);
        dsp::init_point_xyz(&p1, 1.0f, 1.0f, 1.0f);
        dsp::init_point(&p2, &p1);
        generic::init_matrix3d_scale(&m, 2.0f, 4.0f, 8.0f);

        generic::apply_matrix3d_mp2(&p3, &p1, &m);
        apply_matrix3d_mp2(&p4, &p1, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on scale");
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on scale");

        generic::apply_matrix3d_mp1(&p1, &m);
        apply_matrix3d_mp1(&p2, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on scale");
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on scale");

        // Apply translate matrix
        dsp::init_point_xyz(&pc, 3.0f, 5.0f, 9.0f);
        dsp::init_point_xyz(&p1, 1.0f, 1.0f, 1.0f);
        dsp::init_point(&p2, &p1);
        generic::init_matrix3d_translate(&m, 2.0f, 4.0f, 8.0f);

        generic::apply_matrix3d_mp2(&p3, &p1, &m);
        apply_matrix3d_mp2(&p4, &p1, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on translate");
        UTEST_ASSERT_MSG(point3d_ck(&p3, &pc), "Failed apply_matrix3d_mp2 on translate");

        generic::apply_matrix3d_mp1(&p1, &m);
        apply_matrix3d_mp1(&p2, &m);
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on translate");
        UTEST_ASSERT_MSG(point3d_ck(&p1, &pc), "Failed apply_matrix3d_mp1 on translate");
    }

    void modify_vector(
            const char *label,
            apply_matrix3d_mv1_t apply_matrix3d_mv1,
            apply_matrix3d_mv2_t apply_matrix3d_mv2
            )
    {
        if ((!UTEST_SUPPORTED(apply_matrix3d_mv1)) ||
            (!UTEST_SUPPORTED(apply_matrix3d_mv2)))
            return;

        printf("Testing %s\n", label);

        dsp::matrix3d_t m;
        dsp::vector3d_t v1, v2, v3, v4, vc;

        // Apply identity matrix
        dsp::init_vector_dxyz(&vc, 1.0f, 2.0f, 3.0f);
        dsp::init_vector_dxyz(&v1, 1.0f, 2.0f, 3.0f);
        dsp::init_vector(&v2, &v1);
        dsp::init_matrix3d_identity(&m);

        generic::apply_matrix3d_mv1(&v1, &m);
        apply_matrix3d_mv1(&v2, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on identity matrix");
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on identity matrix");

        generic::apply_matrix3d_mv2(&v3, &vc, &m);
        apply_matrix3d_mv2(&v4, &vc, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on identity matrix");
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on identity matrix");

        // Apply rotation Z matrix
        dsp::init_vector_dxyz(&vc, M_SQRT1_2, M_SQRT1_2, 0.0f);
        dsp::init_vector_dxyz(&v1, 0.0f, 1.0f, 0.0f);
        dsp::init_vector(&v2, &v1);
        dsp::init_matrix3d_rotate_z(&m, M_PI * 0.25f);

        generic::apply_matrix3d_mv2(&v3, &v1, &m);
        apply_matrix3d_mv2(&v4, &v1, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on rotation z");
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on rotation z");

        generic::apply_matrix3d_mv1(&v1, &m);
        apply_matrix3d_mv1(&v2, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on rotation z");
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on rotation z");

        // Apply rotation X matrix
        dsp::init_vector_dxyz(&vc, 0.0f, M_SQRT1_2, M_SQRT1_2);
        dsp::init_vector_dxyz(&v1, 0.0f, 0.0f, 1.0f);
        dsp::init_vector(&v2, &v1);
        generic::init_matrix3d_rotate_x(&m, M_PI * 0.25f);

        generic::apply_matrix3d_mv2(&v3, &v1, &m);
        apply_matrix3d_mv2(&v4, &v1, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on rotation x");
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on rotation x");

        generic::apply_matrix3d_mv1(&v1, &m);
        apply_matrix3d_mv1(&v2, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on rotation x");
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on rotation x");

        // Apply rotation Y matrix
        dsp::init_vector_dxyz(&vc, M_SQRT1_2, 0.0f, M_SQRT1_2);
        dsp::init_vector_dxyz(&v1, 1.0f, 0.0f, 0.0f);
        dsp::init_vector(&v2, &v1);
        apply_matrix3d_mv1(&v2, &m);
        dsp::init_matrix3d_rotate_y(&m, M_PI * 0.25f);

        generic::apply_matrix3d_mv2(&v3, &v1, &m);
        apply_matrix3d_mv2(&v4, &v1, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on rotation y");
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on rotation y");

        generic::apply_matrix3d_mv1(&v1, &m);
        apply_matrix3d_mv1(&v2, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on rotation y");
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on rotation y");

        // Apply scale matrix
        dsp::init_vector_dxyz(&vc, 2.0f, 4.0f, 8.0f);
        dsp::init_vector_dxyz(&v1, 1.0f, 1.0f, 1.0f);
        dsp::init_vector(&v2, &v1);
        generic::init_matrix3d_scale(&m, 2.0f, 4.0f, 8.0f);

        generic::apply_matrix3d_mv2(&v3, &v1, &m);
        apply_matrix3d_mv2(&v4, &v1, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on scale");
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &vc), "Failed apply_matrix3d_mv2 on scale");

        generic::apply_matrix3d_mv1(&v1, &m);
        apply_matrix3d_mv1(&v2, &m);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on scale");
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &vc), "Failed apply_matrix3d_mv1 on scale");
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(
            init_data(
                "sse init_matrix",
                sse::init_matrix3d,
                sse::init_matrix3d_zero,
                sse::init_matrix3d_one,
                sse::init_matrix3d_identity,
                sse::transpose_matrix3d1,
                sse::transpose_matrix3d2
            ));

        IF_ARCH_X86(
            init_data(
                "sse2 init_matrix",
                sse::init_matrix3d,
                sse::init_matrix3d_zero,
                sse::init_matrix3d_one,
                sse::init_matrix3d_identity,
                sse2::transpose_matrix3d1,
                sse2::transpose_matrix3d2
            ));

        IF_ARCH_X86(
            init_data(
                "avx init_matrix",
                avx::init_matrix3d,
                avx::init_matrix3d_zero,
                avx::init_matrix3d_one,
                avx::init_matrix3d_identity,
                avx::transpose_matrix3d1,
                avx::transpose_matrix3d2
            ));

        IF_ARCH_X86(
            transform(
                "sse init_matrix_transform",
                sse::init_matrix3d_identity,
                sse::init_matrix3d_translate,
                sse::init_matrix3d_scale,
                sse::init_matrix3d_rotate_x,
                sse::init_matrix3d_rotate_y,
                sse::init_matrix3d_rotate_z,
                sse::init_matrix3d_rotate_xyz,
                sse::apply_matrix3d_mm1,
                sse::apply_matrix3d_mm2
            ));

        IF_ARCH_X86(
            transform(
                "avx init_matrix_transform",
                avx::init_matrix3d_identity,
                avx::init_matrix3d_translate,
                avx::init_matrix3d_scale,
                avx::init_matrix3d_rotate_x,
                avx::init_matrix3d_rotate_y,
                avx::init_matrix3d_rotate_z,
                generic::init_matrix3d_rotate_xyz, // avoid currently due to FMA implementation
                avx::apply_matrix3d_mm1,
                avx::apply_matrix3d_mm2
            ));

        IF_ARCH_X86(
            transform(
                "avx_fma3 init_matrix_transform",
                avx::init_matrix3d_identity,
                avx::init_matrix3d_translate,
                avx::init_matrix3d_scale,
                avx::init_matrix3d_rotate_x,
                avx::init_matrix3d_rotate_y,
                avx::init_matrix3d_rotate_z,
                generic::init_matrix3d_rotate_xyz, // avoid currently due to FMA implementation
                avx::apply_matrix3d_mm1_fma3,
                avx::apply_matrix3d_mm2_fma3
            ));

        IF_ARCH_X86(
            modify_point(
                "sse modify_point",
                sse::apply_matrix3d_mp1,
                sse::apply_matrix3d_mp2
            ));

        IF_ARCH_X86(
            modify_point(
                "avx modify_point",
                avx::apply_matrix3d_mp1,
                avx::apply_matrix3d_mp2
            ));

        IF_ARCH_X86(
            modify_point(
                "avx_fma3 modify_point",
                avx::apply_matrix3d_mp1_fma3,
                avx::apply_matrix3d_mp2_fma3
            ));

        IF_ARCH_X86(
            modify_vector(
                "sse modify_vector",
                sse::apply_matrix3d_mv1,
                sse::apply_matrix3d_mv2
            ));

        IF_ARCH_X86(
            modify_vector(
                "avx modify_vector",
                avx::apply_matrix3d_mv1,
                avx::apply_matrix3d_mv2
            ));

        IF_ARCH_X86(
            modify_vector(
                "avx_fma3 modify_vector",
                avx::apply_matrix3d_mv1_fma3,
                avx::apply_matrix3d_mv2_fma3
            ));
    }
UTEST_END;
