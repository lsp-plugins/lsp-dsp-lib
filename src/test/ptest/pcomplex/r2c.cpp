/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 окт. 2023 г.
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

#define MIN_RANK 6
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void pcomplex_r2c(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void pcomplex_r2c(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void pcomplex_r2c(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void pcomplex_r2c(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void pcomplex_r2c(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void pcomplex_r2c(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* complex_cvt_t) (float *dst, const float *src, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pcomplex", r2c, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, complex_cvt_t op)
    {
        if (!PTEST_SUPPORTED(op))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            op(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size*6, 64);
        float *in       = &out[buf_size*2];
        float *backup   = &in[buf_size*1];

        randomize_sign(out, buf_size*3);
        dsp::copy(backup, out, buf_size*3);

        #define CALL(func) \
            dsp::copy(out, backup, buf_size*3); \
            call(#func, out, in, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::pcomplex_r2c);
            IF_ARCH_X86(CALL(sse::pcomplex_r2c));
            IF_ARCH_X86(CALL(avx::pcomplex_r2c));
            IF_ARCH_X86(CALL(avx512::pcomplex_r2c));
            IF_ARCH_ARM(CALL(neon_d32::pcomplex_r2c));
            IF_ARCH_AARCH64(CALL(asimd::pcomplex_r2c));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



