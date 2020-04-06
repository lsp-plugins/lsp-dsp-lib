/*
 * raw_triangle.cpp
 *
 *  Created on: 5 апр. 2019 г.
 *      Author: sadko
 */

#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/ptest.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/stdlib/math.h>

#define N_PLANES    128
#define N_TRIANGLES 1024

namespace lsp
{
    namespace generic
    {
        void cull_triangle_raw(dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
        void split_triangle_raw(dsp::raw_triangle_t *out, size_t *n_out, dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void cull_triangle_raw(dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
            void split_triangle_raw(dsp::raw_triangle_t *out, size_t *n_out, dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
        }

        namespace sse3
        {
            void cull_triangle_raw(dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
            void split_triangle_raw(dsp::raw_triangle_t *out, size_t *n_out, dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
        }
    )

    typedef void (* cull_triangle_raw_t)(dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
    typedef void (* split_triangle_raw_t)(dsp::raw_triangle_t *out, size_t *n_out, dsp::raw_triangle_t *in, size_t *n_in, const dsp::vector3d_t *pl, const dsp::raw_triangle_t *pv);
}


//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", raw_triangle, 5, 1000)

    void call(const char *label, const dsp::vector3d_t *vp, const dsp::raw_triangle_t *vt, cull_triangle_raw_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);
        dsp::raw_triangle_t in[N_TRIANGLES*2];

        PTEST_LOOP(label,
            const dsp::vector3d_t *pl = vp;
            for (size_t i=0; i<N_PLANES; ++i, ++pl)
            {
                const dsp::raw_triangle_t *t = vt;
                size_t nin = 0;
                for (size_t j=0; j<N_TRIANGLES; ++j, ++t)
                    func(in, &nin, pl, vt);
            }
        );
    }

    void call(const char *label, const dsp::vector3d_t *vp, const dsp::raw_triangle_t *vt, split_triangle_raw_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);
        dsp::raw_triangle_t in[N_TRIANGLES*2], out[N_TRIANGLES*2];

        PTEST_LOOP(label,
            const dsp::vector3d_t *pl = vp;
            for (size_t i=0; i<N_PLANES; ++i, ++pl)
            {
                const dsp::raw_triangle_t *t = vt;
                size_t nin = 0, nout=0;
                for (size_t j=0; j<N_TRIANGLES; ++j, ++t)
                    func(out, &nout, in, &nin, pl, vt);
            }
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = N_PLANES * sizeof(dsp::vector3d_t) + N_TRIANGLES * sizeof(dsp::raw_triangle_t);
        uint8_t *data       = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, buf_size, 64);

        dsp::vector3d_t *planes          = reinterpret_cast<dsp::vector3d_t *>(ptr);
        ptr                             += N_PLANES * sizeof(dsp::vector3d_t);
        dsp::raw_triangle_t *triangles   = reinterpret_cast<dsp::raw_triangle_t *>(ptr);

        // Initialize planes
        for (size_t i=0; i < N_PLANES;)
        {
            dsp::vector3d_t *pl  = &planes[i];

            dsp::init_vector_dxyz(pl, randf(-1.0f, 1.0f), randf(-1.0f, 1.0f), randf(-1.0f, 1.0f));
            float w = sqrtf(pl->dx * pl->dx + pl->dy * pl->dy + pl->dz * pl->dz);
            if (w <= DSP_3D_TOLERANCE)
                continue;

            pl->dx     /= w;
            pl->dy     /= w;
            pl->dz     /= w;
            pl->dw      = randf(-1.0f, 1.0f);

            ++i;
        }

        // Initialize triangles
        for (size_t i=0; i < N_TRIANGLES; ++i)
        {
            dsp::init_point_xyz(&triangles[i].v[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&triangles[i].v[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&triangles[i].v[2], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
        }

        call("generic::split_triangle_raw", planes, triangles, generic::split_triangle_raw);
        IF_ARCH_X86(call("sse::split_triangle_raw", planes, triangles, sse::split_triangle_raw));
        IF_ARCH_X86(call("sse3::split_triangle_raw", planes, triangles, sse3::split_triangle_raw));
        PTEST_SEPARATOR;

        call("generic::cull_triangle_raw", planes, triangles, generic::cull_triangle_raw);
        IF_ARCH_X86(call("sse::cull_triangle_raw", planes, triangles, sse::cull_triangle_raw));
        IF_ARCH_X86(call("sse3::cull_triangle_raw", planes, triangles, sse3::cull_triangle_raw));
        PTEST_SEPARATOR;

        free_aligned(data);
    }
PTEST_END



