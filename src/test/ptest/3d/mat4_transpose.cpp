/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 18 авг. 2025 г.
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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define N_MATRICES      0x1000

namespace lsp
{
    namespace generic
    {
        void transpose_matrix3d1(dsp::matrix3d_t *r);
        void transpose_matrix3d2(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void transpose_matrix3d1(dsp::matrix3d_t *r);
            void transpose_matrix3d2(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
        }

        namespace sse2
        {
            void transpose_matrix3d1(dsp::matrix3d_t *r);
            void transpose_matrix3d2(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
        }

        namespace avx
        {
            void transpose_matrix3d1(dsp::matrix3d_t *r);
            void transpose_matrix3d2(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
        }
    )

    typedef void (* transpose_matrix3d1_t)(dsp::matrix3d_t *r);
    typedef void (* transpose_matrix3d2_t)(dsp::matrix3d_t *r, const dsp::matrix3d_t *m);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", mat4_transpose, 5, 1000)

    void call(const char *label, dsp::matrix3d_t *dst, const dsp::matrix3d_t *src, transpose_matrix3d2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s", label);
        printf("Testing %s ...\n", buf);

        PTEST_LOOP(buf,
            for (size_t i=0; i<N_MATRICES; ++i)
                func(&dst[i], &src[i]);
        );
    }

    void call(const char *label, dsp::matrix3d_t *dst, transpose_matrix3d1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s", label);
        printf("Testing %s ...\n", buf);

        PTEST_LOOP(buf,
            for (size_t i=0; i<N_MATRICES; ++i)
                func(&dst[i]);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = N_MATRICES * sizeof(dsp::matrix3d_t);
        uint8_t *data       = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, buf_size * 2, 64);

        dsp::matrix3d_t *dst    = reinterpret_cast<dsp::matrix3d_t *>(ptr);
        dsp::matrix3d_t *src    = reinterpret_cast<dsp::matrix3d_t *>(&dst[N_MATRICES]);

        // Initialize matrices
        for (size_t i=0; i < N_MATRICES;)
        {
            randomize_sign(src[i].m,  16);
            ++i;
        }

    #define CALL1(func) \
        memcpy(dst, src, buf_size); \
        call(#func, dst, func);

    #define CALL2(func) \
        memcpy(dst, src, buf_size); \
        call(#func, dst, src, func);

        CALL1(generic::transpose_matrix3d1);
        IF_ARCH_X86(CALL1(sse::transpose_matrix3d1));
        IF_ARCH_X86(CALL1(sse2::transpose_matrix3d1));
        IF_ARCH_X86(CALL1(avx::transpose_matrix3d1));
        PTEST_SEPARATOR;

        CALL2(generic::transpose_matrix3d2);
        IF_ARCH_X86(CALL2(sse::transpose_matrix3d2));
        IF_ARCH_X86(CALL2(sse2::transpose_matrix3d2));
        IF_ARCH_X86(CALL2(avx::transpose_matrix3d2));
        PTEST_SEPARATOR;

        free_aligned(data);
    }
PTEST_END





