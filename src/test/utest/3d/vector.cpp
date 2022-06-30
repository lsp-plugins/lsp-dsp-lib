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

namespace lsp
{
    namespace generic
    {
        void init_vector_dxyz(dsp::vector3d_t *v, float dx, float dy, float dz);
        void init_vector(dsp::vector3d_t *p, const dsp::vector3d_t *s);
        void normalize_vector(dsp::vector3d_t *v);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void init_vector_dxyz(dsp::vector3d_t *v, float dx, float dy, float dz);
            void init_vector(dsp::vector3d_t *p, const dsp::vector3d_t *s);
            void normalize_vector(dsp::vector3d_t *v);
        }
    )

    typedef void (* init_vector_dxyz_t)(dsp::vector3d_t *v, float dx, float dy, float dz);
    typedef void (* init_vector_t)(dsp::vector3d_t *p, const dsp::vector3d_t *s);
    typedef void (* normalize_vector_t)(dsp::vector3d_t *v);
}

UTEST_BEGIN("dsp.3d", vector)

    void call(
            const char *label,
            init_vector_dxyz_t init_dxyz,
            init_vector_t init,
            normalize_vector_t norm
        )
    {
        if ((!UTEST_SUPPORTED(init_dxyz)) || (!UTEST_SUPPORTED(init)) || (!UTEST_SUPPORTED(norm)))
            return;

        printf("Testing %s\n", label);

        dsp::vector3d_t  v1, v2, v3;

        generic::init_vector_dxyz(&v1, 1.0f, 2.0f, 3.0f);
        init_dxyz(&v2, 1.0f, 2.0f, 3.0f);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v2), "Failed init_vector_dxyz");

        generic::init_vector(&v2, &v1);
        init(&v3, &v1);
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v2), "Failed generic init_vector");
        UTEST_ASSERT_MSG(vector3d_sck(&v1, &v3), "Failed optimized init_vector");

        generic::normalize_vector(&v2);
        norm(&v3);

        UTEST_ASSERT_MSG(vector3d_sck(&v2, &v3), "Failed normalize vector");
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse_vector", sse::init_vector_dxyz, sse::init_vector, sse::normalize_vector));
    }
UTEST_END;



