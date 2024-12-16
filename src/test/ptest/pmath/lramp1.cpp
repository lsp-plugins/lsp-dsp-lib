/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 февр. 2023 г.
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
        void lramp_set1(float *dst, float v1, float v2, size_t count);
        void lramp1(float *dst, float v1, float v2, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }

        namespace avx
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }

        namespace avx512
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }
    )
}

typedef void (* lramp1_t)(float *dst, float v1, float v2, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", lramp1, 5, 1000)

    void call(const char *label, float *dst, size_t count, lramp1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, 0.25f, 0.75f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 2, 64);
        float *backup   = &dst[buf_size];

        for (size_t i=0; i < buf_size; ++i)
            backup[i]       = randf(0.001f, 1.0f);

        #define CALL(func) \
            dsp::copy(dst, backup, buf_size); \
            call(#func, dst, count, func);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::lramp_set1);
            IF_ARCH_X86(CALL(sse::lramp_set1));
            IF_ARCH_X86(CALL(avx::lramp_set1));
            IF_ARCH_X86(CALL(avx512::lramp_set1));
            IF_ARCH_ARM(CALL(neon_d32::lramp_set1));
            IF_ARCH_AARCH64(CALL(asimd::lramp_set1));
            PTEST_SEPARATOR;

            CALL(generic::lramp1);
            IF_ARCH_X86(CALL(sse::lramp1));
            IF_ARCH_X86(CALL(avx::lramp1));
            IF_ARCH_X86(CALL(avx512::lramp1));
            IF_ARCH_ARM(CALL(neon_d32::lramp1));
            IF_ARCH_AARCH64(CALL(asimd::lramp1));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


