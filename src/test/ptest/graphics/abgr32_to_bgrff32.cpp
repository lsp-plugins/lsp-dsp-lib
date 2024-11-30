/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 окт. 2020 г.
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
#define MAX_RANK 14

namespace lsp
{
    namespace generic
    {
        void abgr32_to_bgrff32(void *dst, const void *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void abgr32_to_bgrff32(void *dst, const void *src, size_t count);
        }

        namespace avx2
        {
            void abgr32_to_bgrff32(void *dst, const void *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void abgr32_to_bgrff32(void *dst, const void *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void abgr32_to_bgrff32(void *dst, const void *src, size_t count);
        }
    )

    typedef void (* abgr32_to_bgrff32_t)(void *dst, const void *src, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", abgr32_to_bgrff32, 5, 5000)

    void call(const char *label, void *dst, const void *src, size_t count, abgr32_to_bgrff32_t func)
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

        uint8_t *dst        = alloc_aligned<uint8_t>(data, buf_size * sizeof(uint32_t) * 2, 64);
        uint8_t *src        = reinterpret_cast<uint8_t *>(&dst[buf_size * sizeof(uint32_t)]);

        for (size_t i=0; i<buf_size*4; ++i)
             src[i]             = 0xff * rand();

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            #define CALL(func) \
                call(#func, dst, src, count, func)

            CALL(generic::abgr32_to_bgrff32);
            IF_ARCH_X86(CALL(sse2::abgr32_to_bgrff32));
            IF_ARCH_X86(CALL(avx2::abgr32_to_bgrff32));
            IF_ARCH_ARM(CALL(neon_d32::abgr32_to_bgrff32));
            IF_ARCH_AARCH64(CALL(asimd::abgr32_to_bgrff32));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END




