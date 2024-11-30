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
        void rgba_to_hsla(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void rgba_to_hsla(float *dst, const float *src, size_t count);
        }

        namespace avx2
        {
            void rgba_to_hsla(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void rgba_to_hsla(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void rgba_to_hsla(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* rgba_to_hsla_t)(float *dst, const float *src, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", rgba_to_hsla, 5, 5000)

    void call(const char *label, float *dst, float *src, size_t count, rgba_to_hsla_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s pixels...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *src          = alloc_aligned<float>(data, (buf_size * 2) * 4, 64);
        float *dst          = &src[buf_size * 4];

        for (size_t i=0; i<buf_size*4; ++i)
            src[i]             = randf(0.0f, 1.0f);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            #define CALL(func) \
                call(#func, dst, src, count, func)

            CALL(generic::rgba_to_hsla);
            IF_ARCH_X86(CALL(sse2::rgba_to_hsla));
            IF_ARCH_X86(CALL(avx2::rgba_to_hsla));
            IF_ARCH_ARM(CALL(neon_d32::rgba_to_hsla));
            IF_ARCH_AARCH64(CALL(asimd::rgba_to_hsla));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END






