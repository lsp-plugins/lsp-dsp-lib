/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 нояб. 2024 г.
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

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count);
        }

        namespace avx
        {
            void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count);
        }

        namespace avx512
        {
            void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count);
        }
    )

    typedef void (* depan_t) (float *dst, const float *l, const float *r, float dfl, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.pan", depan_lin, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, depan_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src1, src2, 0.5f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *src1     = &dst[buf_size];
        float *src2     = &src1[buf_size];

        randomize_sign(src1, buf_size);
        randomize_sign(src2, buf_size);

        #define CALL(func) \
            call(#func, dst, src1, src2, count, func);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::depan_lin);
            IF_ARCH_X86(CALL(sse::depan_lin));
            IF_ARCH_X86(CALL(avx::depan_lin));
            IF_ARCH_X86(CALL(avx512::depan_lin));
            IF_ARCH_ARM(CALL(neon_d32::depan_lin));
            IF_ARCH_AARCH64(CALL(asimd::depan_lin));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END




