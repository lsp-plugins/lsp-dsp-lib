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

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void pcomplex_rcp1(float *dst, size_t count);
        void pcomplex_rcp2(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void pcomplex_rcp1(float *dst, size_t count);
            void pcomplex_rcp2(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void pcomplex_rcp1(float *dst, size_t count);
            void pcomplex_rcp2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void pcomplex_rcp1(float *dst, size_t count);
            void pcomplex_rcp2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void pcomplex_rcp1(float *dst, size_t count);
            void pcomplex_rcp2(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* pcomplex_rcp1_t) (float *dst, size_t count);
    typedef void (* pcomplex_rcp2_t) (float *dst, const float *src, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for complex reciprocal
PTEST_BEGIN("dsp.pcomplex", rcp, 5, 1000)

    void call(const char *label, float *dst, size_t count, pcomplex_rcp1_t rcp)
    {
        if (!PTEST_SUPPORTED(rcp))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            rcp(dst, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, pcomplex_rcp2_t rcp)
    {
        if (!PTEST_SUPPORTED(rcp))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            rcp(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size * 8, 64);
        float *in       = &out[buf_size*2];
        float *backup   = &in[buf_size*2];

        for (size_t i=0; i < buf_size*4; ++i)
            out[i]          = randf(-1.0f, 1.0f);
        dsp::copy(backup, out, buf_size * 4);

        #define CALL1(func) \
            dsp::copy(out, backup, buf_size * 4); \
            call(#func, out, count, func)

        #define CALL2(func) \
            dsp::copy(out, backup, buf_size * 4); \
            call(#func, out, in, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL1(generic::pcomplex_rcp1);
            IF_ARCH_X86(CALL1(sse::pcomplex_rcp1));
            IF_ARCH_X86(CALL1(avx::pcomplex_rcp1));
            IF_ARCH_ARM(CALL1(neon_d32::pcomplex_rcp1));
            IF_ARCH_AARCH64(CALL1(asimd::pcomplex_rcp1));
            PTEST_SEPARATOR;

            CALL2(generic::pcomplex_rcp2);
            IF_ARCH_X86(CALL2(sse::pcomplex_rcp2));
            IF_ARCH_X86(CALL2(avx::pcomplex_rcp2));
            IF_ARCH_ARM(CALL2(neon_d32::pcomplex_rcp2));
            IF_ARCH_AARCH64(CALL2(asimd::pcomplex_rcp2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


