/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 авг. 2025 г.
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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define CALLS_COUNT     0x10000

namespace lsp
{
    namespace generic
    {
        float calc_plane_v1p2(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float calc_plane_v1p2(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
        }

        namespace avx
        {
            float calc_plane_v1p2(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
            float calc_plane_v1p2_fma3(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
        }
    )

    typedef float (* calc_plane_v1p2_t)(dsp::vector3d_t *v, const dsp::vector3d_t *v0, const dsp::point3d_t *p0, const dsp::point3d_t *p1);
}


//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", plane_vp, 5, 1000)

    typedef struct vplane_t
    {
        dsp::vector3d_t v;
        dsp::point3d_t p1;
        dsp::point3d_t p2;
    } vplane_t;

    void call(const char *label, const vplane_t *pv, calc_plane_v1p2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);

        dsp::vector3d_t dst;

        PTEST_LOOP(label,
            const vplane_t *p = pv;
            for (size_t i=0; i<CALLS_COUNT; ++i, ++p)
            {
                func(&dst, &p->v, &p->p1, &p->p2);
            }
        );
    }

    PTEST_MAIN
    {
        const size_t buf_size   = CALLS_COUNT;
        uint8_t *data           = NULL;
        vplane_t *src           = alloc_aligned<vplane_t>(data, buf_size, 64);
        lsp_finally {
            free_aligned(data);
        };

        // Initialize data
        for (size_t i=0; i < CALLS_COUNT; ++i)
        {
            vplane_t *p = &src[i];
            dsp::init_vector_dxyz(&p->v, randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&p->p1, randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&p->p2, randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
        }

        #define CALL(func) \
            call(#func, src, func);

        CALL(generic::calc_plane_v1p2);
        IF_ARCH_X86(CALL(sse::calc_plane_v1p2));
        IF_ARCH_X86(CALL(avx::calc_plane_v1p2));
        IF_ARCH_X86(CALL(avx::calc_plane_v1p2_fma3));
        PTEST_SEPARATOR;
    }
PTEST_END







