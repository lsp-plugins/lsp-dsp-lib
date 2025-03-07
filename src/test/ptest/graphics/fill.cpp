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
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 6
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void fill_rgba(float *dst, float r, float g, float b, float a, size_t count);
        void fill_hsla(float *dst, float h, float s, float l, float a, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void fill_rgba(float *dst, float r, float g, float b, float a, size_t count);
            void fill_hsla(float *dst, float h, float s, float l, float a, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void fill_rgba(float *dst, float r, float g, float b, float a, size_t count);
            void fill_hsla(float *dst, float h, float s, float l, float a, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void fill_rgba(float *dst, float r, float g, float b, float a, size_t count);
            void fill_hsla(float *dst, float h, float s, float l, float a, size_t count);
        }
    )

    typedef void (* hsla_to_fill_t)(float *dst, float c1, float c2, float c3, float c4, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", fill, 5, 5000)

    void call(const char *label, float *dst, size_t count, hsla_to_fill_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s pixels...\n", buf);

        PTEST_LOOP(buf,
            func(dst, 1.0f, 2.0f, 3.0f, 4.0f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *dst          = alloc_aligned<float>(data, buf_size * 4, 64);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("generic::fill_rgba", dst, count, generic::fill_rgba);
            call("generic::fill_hsla", dst, count, generic::fill_hsla);

            IF_ARCH_X86(call("sse::fill_rgba", dst, count, sse::fill_rgba));
            IF_ARCH_X86(call("sse::fill_hsla", dst, count, sse::fill_hsla));

            IF_ARCH_ARM(call("neon_d32::fill_rgba", dst, count, neon_d32::fill_rgba));
            IF_ARCH_ARM(call("neon_d32::fill_hsla", dst, count, neon_d32::fill_hsla));

            IF_ARCH_AARCH64(call("asimd::fill_rgba", dst, count, asimd::fill_rgba));
            IF_ARCH_AARCH64(call("asimd::fill_hsla", dst, count, asimd::fill_hsla));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



