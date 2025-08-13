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
#include <private/utest/dsp/3d/helpers.h>

namespace lsp
{
    namespace generic
    {
        void init_vector_dxyz(dsp::vector3d_t *v, float dx, float dy, float dz);
        void init_vector(dsp::vector3d_t *p, const dsp::vector3d_t *s);
        void normalize_vector(dsp::vector3d_t *v);
        void normalize_vector2(dsp::vector3d_t *v, const dsp::vector3d_t *sv);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void init_vector_dxyz(dsp::vector3d_t *v, float dx, float dy, float dz);
            void init_vector(dsp::vector3d_t *p, const dsp::vector3d_t *s);
            void normalize_vector(dsp::vector3d_t *v);
            void normalize_vector2(dsp::vector3d_t *v, const dsp::vector3d_t *sv);
        }

        namespace avx
        {
            void init_vector_dxyz(dsp::vector3d_t *v, float dx, float dy, float dz);
            void init_vector(dsp::vector3d_t *p, const dsp::vector3d_t *s);
            void normalize_vector(dsp::vector3d_t *v);
            void normalize_vector2(dsp::vector3d_t *v, const dsp::vector3d_t *sv);
        }
    )

    typedef void (* init_vector_dxyz_t)(dsp::vector3d_t *v, float dx, float dy, float dz);
    typedef void (* init_vector_t)(dsp::vector3d_t *p, const dsp::vector3d_t *s);
    typedef void (* normalize_vector_t)(dsp::vector3d_t *v);
    typedef void (* normalize_vector2_t)(dsp::vector3d_t *v, const dsp::vector3d_t *sv);
}

UTEST_BEGIN("dsp.3d", vector)

    static void fill_vector(dsp::vector3d_t *v)
    {
        v->dx   = 0.1f;
        v->dy   = 0.2f;
        v->dz   = 0.3f;
        v->dw   = 0.4f;
    }

    void call(
            const char *label,
            init_vector_dxyz_t init_dxyz,
            init_vector_t init,
            normalize_vector_t norm,
            normalize_vector2_t norm2
        )
    {
        if ((!UTEST_SUPPORTED(init_dxyz)) || (!UTEST_SUPPORTED(init)) || (!UTEST_SUPPORTED(norm)) || (!UTEST_SUPPORTED(norm2)))
            return;

        printf("Testing %s\n", label);

        dsp::vector3d_t  v1, v2, v3, v4, v5, v6;

        fill_vector(&v1);
        fill_vector(&v2);
        fill_vector(&v3);
        fill_vector(&v4);
        fill_vector(&v5);
        fill_vector(&v6);

        generic::init_vector_dxyz(&v1, 2.0f, 3.0f, 4.0f);
        init_dxyz(&v2, 2.0f, 3.0f, 4.0f);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v2), "Failed init_vector_dxyz");

        generic::init_vector(&v3, &v1);
        init(&v4, &v1);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v3), "Failed generic init_vector");
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v4), "Failed optimized init_vector");

        generic::normalize_vector2(&v5, &v1);
        norm2(&v6, &v1);
        UTEST_ASSERT_MSG(vector3d_sck(&v5, &v6), "Failed normalize vector 2");

        generic::normalize_vector(&v3);
        norm(&v4);
        UTEST_ASSERT_MSG(vector3d_sck(&v3, &v4), "Failed normalize vector");
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse_vector", sse::init_vector_dxyz, sse::init_vector, sse::normalize_vector, sse::normalize_vector2));
        IF_ARCH_X86(call("avx_vector", avx::init_vector_dxyz, avx::init_vector, avx::normalize_vector, avx::normalize_vector2));
    }
UTEST_END;



