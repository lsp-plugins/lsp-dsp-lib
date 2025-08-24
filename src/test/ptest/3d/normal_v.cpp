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

#define VECTORS_COUNT       0x10000

namespace lsp
{
    namespace generic
    {
        void calc_normal3d_v2(dsp::vector3d_t *n, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2);
        void calc_normal3d_vv(dsp::vector3d_t *n, const dsp::vector3d_t *vv);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void calc_normal3d_v2(dsp::vector3d_t *n, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2);
            void calc_normal3d_vv(dsp::vector3d_t *n, const dsp::vector3d_t *vv);
        }

        namespace avx
        {
            void calc_normal3d_v2(dsp::vector3d_t *n, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2);
            void calc_normal3d_vv(dsp::vector3d_t *n, const dsp::vector3d_t *vv);

            void calc_normal3d_v2_fma3(dsp::vector3d_t *n, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2);
            void calc_normal3d_vv_fma3(dsp::vector3d_t *n, const dsp::vector3d_t *vv);
        }
    )

    typedef void (* calc_normal3d_v2_t)(dsp::vector3d_t *n, const dsp::vector3d_t *v1, const dsp::vector3d_t *v2);
    typedef void (* calc_normal3d_vv_t)(dsp::vector3d_t *n, const dsp::vector3d_t *vv);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", normal_v, 5, 1000)

    void call(const char *label, const dsp::vector3d_t *vv, calc_normal3d_v2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);

        dsp::vector3d_t dst;
        PTEST_LOOP(label,
            const dsp::vector3d_t *v = vv;
            for (size_t i=0; i<(VECTORS_COUNT - 1); ++i, ++v)
                func(&dst, &v[0], &v[1]);
        );
    }

    void call(const char *label, const dsp::vector3d_t *pv, calc_normal3d_vv_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s...\n", label);

        dsp::vector3d_t dst;
        PTEST_LOOP(label,
            for (size_t i=0; i<(VECTORS_COUNT - 1); ++i)
                func(&dst, &pv[i]);
        );
    }

    PTEST_MAIN
    {
        const size_t buf_size   = VECTORS_COUNT;
        uint8_t *data           = NULL;
        dsp::vector3d_t *src    = alloc_aligned<dsp::vector3d_t>(data, buf_size, 64);
        lsp_finally {
            free_aligned(data);
        };

        // Initialize data
        for (size_t i=0; i < VECTORS_COUNT; ++i)
            dsp::init_vector_dxyz(&src[i], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

        #define CALL(func) \
            call(#func, src, func);

        CALL(generic::calc_normal3d_v2);
        IF_ARCH_X86(CALL(sse::calc_normal3d_v2));
        IF_ARCH_X86(CALL(avx::calc_normal3d_v2));
        IF_ARCH_X86(CALL(avx::calc_normal3d_v2_fma3));
        PTEST_SEPARATOR;

        CALL(generic::calc_normal3d_vv);
        IF_ARCH_X86(CALL(sse::calc_normal3d_vv));
        IF_ARCH_X86(CALL(avx::calc_normal3d_vv));
        IF_ARCH_X86(CALL(avx::calc_normal3d_vv_fma3));
        PTEST_SEPARATOR;
    }
PTEST_END







