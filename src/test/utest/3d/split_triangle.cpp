/*
 * split_triangle.cpp
 *
 *  Created on: 4 апр. 2019 г.
 *      Author: sadko
 */

#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/test-fw/utest.h>

namespace lsp
{
    namespace generic
    {
        void split_triangle_raw(dsp::raw_triangle_t *out, size_t *n_out, dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void split_triangle_raw(dsp::raw_triangle_t *out, size_t *n_out, dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
        }

        namespace sse3
        {
            void split_triangle_raw(dsp::raw_triangle_t *out, size_t *n_out, dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
        }
    )

    typedef void (* split_triangle_raw_t)(dsp::raw_triangle_t *out, size_t *n_out, dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
}

UTEST_BEGIN("dsp.3d", split_triangle)

    bool do_test(split_triangle_raw_t fn,
            const dsp::vector3d_t &pl, const dsp::point3d_t &p0, const dsp::point3d_t &p1, const dsp::point3d_t &p2,
            size_t ein, size_t eout)
    {
    dsp::raw_triangle_t rt, in[2], out[2];
        size_t nin, nout;
        rt.v[0] = p0;
        rt.v[1] = p1;
        rt.v[2] = p2;

        nin  = 0;
        nout = 0;

        fn(out, &nout, in, &nin, &pl, &rt);
        UTEST_ASSERT(nout <= 2);
        UTEST_ASSERT(nin <= 2);

        return (nout == eout) && (nin == ein);
    }

    void test_func(const char *label, split_triangle_raw_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        // Create culling plane
        dsp::vector3d_t pl;
        dsp::point3d_t p[3];

        // Culling plane
        dsp::init_vector_dxyz(&pl, 0.0f, 1.0f, 0.0f);

        // Test 1 intersection
        dsp::init_point_xyz(&p[0], -1.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], 1.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[2], -1.0f, 1.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 1, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 1, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 1, 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 1, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 1, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 1, 1));

        // Test 2 intersections
        dsp::init_point_xyz(&p[0], 0.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, -1.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 2, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 2, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 2, 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 2, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 2, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 2, 1));

        dsp::init_point_xyz(&p[0], 0.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, 1.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 1, 2));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 1, 2));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 1, 2));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 1, 2));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 1, 2));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 1, 2));

        // Test 1 touch
        dsp::init_point_xyz(&p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, -1.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 1, 0));

        dsp::init_point_xyz(&p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, 1.0f, 0.0f);
        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 0, 1));

        // Test 2 touches
        dsp::init_point_xyz(&p[0], 0.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, 0.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 0, 1));

        dsp::init_point_xyz(&p[0], 0.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], 1.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[2], -1.0f, 0.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 1, 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 1, 0));

        // Test 3 touches
        dsp::init_point_xyz(&p[0], 0.0f, 0.0f, 1.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 0.0f, -1.0f);
        dsp::init_point_xyz(&p[2], 1.0f, 0.0f, -1.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 0, 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 0, 1));
    }

    UTEST_MAIN
    {
        test_func("generic::split_triangle_raw", generic::split_triangle_raw);
        IF_ARCH_X86(test_func("sse::split_triangle_raw", sse::split_triangle_raw));
        IF_ARCH_X86(test_func("sse3::split_triangle_raw", sse3::split_triangle_raw));
    }
UTEST_END;


