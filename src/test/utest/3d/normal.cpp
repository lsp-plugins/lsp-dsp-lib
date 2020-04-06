/*
 * normal.cpp
 *
 *  Created on: 29 мар. 2019 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <private/utest/dsp/3d/helpers.h>

namespace lsp
{
    namespace generic
    {
        void calc_normal3d_v2(dsp::vector3d_t *n, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2);
        void calc_normal3d_vv(dsp::vector3d_t *n, const dsp::vector3d_t *vv);
        void calc_normal3d_p3(dsp::vector3d_t *n, const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
        void calc_normal3d_pv(dsp::vector3d_t *n, const dsp::point3d_t *pv);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void calc_normal3d_v2(dsp::vector3d_t *n, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2);
            void calc_normal3d_vv(dsp::vector3d_t *n, const dsp::vector3d_t *vv);
            void calc_normal3d_p3(dsp::vector3d_t *n, const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
            void calc_normal3d_pv(dsp::vector3d_t *n, const dsp::point3d_t *pv);
        }
    )

    typedef void (* calc_normal3d_v2_t)(dsp::vector3d_t *n, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2);
    typedef void (* calc_normal3d_vv_t)(dsp::vector3d_t *n, const dsp::vector3d_t *vv);
    typedef void (* calc_normal3d_p3_t)(dsp::vector3d_t *n, const dsp::point3d_t *p1, const dsp::point3d_t *p2, const dsp::point3d_t *p3);
    typedef void (* calc_normal3d_pv_t)(dsp::vector3d_t *n, const dsp::point3d_t *pv);
}

UTEST_BEGIN("dsp.3d", normal)

    void call(const char *label, calc_normal3d_v2_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        dsp::vector3d_t sv[2], n[2];

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize vectors
            dsp::init_vector_dxyz(&sv[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_vector_dxyz(&sv[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

            // Compute the value
            generic::calc_normal3d_v2(&n[0], &sv[0], &sv[1]);
            fn(&n[1], &sv[0], &sv[1]);

            if (!vector3d_ack(&n[0], &n[1]))
            {
                dump_vector("sv[0]", &sv[0]);
                dump_vector("sv[1]", &sv[1]);
                dump_vector("n[0]", &n[0]);
                dump_vector("n[1]", &n[1]);
                UTEST_FAIL_MSG("result of %s differs", label);
            }
        }
    }

    void call(const char *label, calc_normal3d_vv_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        dsp::vector3d_t sv[2], n[2];

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize vectors
            dsp::init_vector_dxyz(&sv[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_vector_dxyz(&sv[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

            // Compute the value
            generic::calc_normal3d_vv(&n[0], sv);
            fn(&n[1], sv);

            if (!vector3d_ack(&n[0], &n[1]))
            {
                dump_vector("sv[0]", &sv[0]);
                dump_vector("sv[1]", &sv[1]);
                dump_vector("n[0]", &n[0]);
                dump_vector("n[1]", &n[1]);
                UTEST_FAIL_MSG("result of %s differs", label);
            }
        }
    }

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
            dsp::init_point_xyz(&p[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&p[2], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

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
            dsp::init_point_xyz(&p[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&p[2], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

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
        IF_ARCH_X86(call("sse::calc_normal3d_v2", sse::calc_normal3d_v2));
        IF_ARCH_X86(call("sse::calc_normal3d_vv", sse::calc_normal3d_vv));
        IF_ARCH_X86(call("sse::calc_normal3d_p3", sse::calc_normal3d_p3));
        IF_ARCH_X86(call("sse::calc_normal3d_pv", sse::calc_normal3d_pv));
    }

UTEST_END


