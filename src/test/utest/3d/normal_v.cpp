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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <private/utest/dsp/3d/helpers.h>

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

UTEST_BEGIN("dsp.3d", normal_v)

    void call(const char *label, calc_normal3d_v2_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        dsp::vector3d_t sv[2], n[2];

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize vectors
            if (i != 0)
            {
                dsp::init_vector_dxyz(&sv[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
                dsp::init_vector_dxyz(&sv[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            }
            else
            {
                dsp::init_vector_dxyz(&sv[0], 0.0f, 0.0f, 0.0f);
                dsp::init_vector_dxyz(&sv[1], 0.0f, 0.0f, 0.0f);
            }

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
            if (i != 0)
            {
                dsp::init_vector_dxyz(&sv[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
                dsp::init_vector_dxyz(&sv[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            }
            else
            {
                dsp::init_vector_dxyz(&sv[0], 0.0f, 0.0f, 0.0f);
                dsp::init_vector_dxyz(&sv[1], 0.0f, 0.0f, 0.0f);
            }

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


    UTEST_MAIN
    {
    #define CALL(func) \
        call(#func, func)

        IF_ARCH_X86(CALL(sse::calc_normal3d_v2));
        IF_ARCH_X86(CALL(sse::calc_normal3d_vv));

        IF_ARCH_X86(CALL(avx::calc_normal3d_v2));
        IF_ARCH_X86(CALL(avx::calc_normal3d_vv));

        IF_ARCH_X86(CALL(avx::calc_normal3d_v2_fma3));
        IF_ARCH_X86(CALL(avx::calc_normal3d_vv_fma3));
    }

UTEST_END





