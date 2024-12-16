/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 23 нояб. 2024 г.
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
        void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count);
        }

        namespace avx
        {
            void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count);
            void pcomplex_corr_fma3(float *dst_corr, const float *src1, const float *src2, size_t count);
        }

        namespace avx512
        {
            void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count);
        }
    )

    typedef void (* pcomplex_corr_t)(float *dst_corr, const float *src1, const float *src2, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pcomplex", corr, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, pcomplex_corr_t op)
    {
        if (!PTEST_SUPPORTED(op))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            op(dst, src1, src2, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size*5, 64);
        float *src1     = &dst[buf_size];
        float *src2     = &src1[buf_size*2];

        randomize_sign(src1, buf_size*2);
        randomize_sign(src2, buf_size*2);

        #define CALL(func) \
            call(#func, dst, src1, src2, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::pcomplex_corr);
            IF_ARCH_X86(CALL(sse::pcomplex_corr));
            IF_ARCH_X86_64(CALL(sse3::x64_pcomplex_corr));
            IF_ARCH_X86(CALL(avx::pcomplex_corr));
            IF_ARCH_X86(CALL(avx::pcomplex_corr_fma3));
            IF_ARCH_X86(CALL(avx512::pcomplex_corr));
            IF_ARCH_ARM(CALL(neon_d32::pcomplex_corr));
            IF_ARCH_AARCH64(CALL(asimd::pcomplex_corr));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



