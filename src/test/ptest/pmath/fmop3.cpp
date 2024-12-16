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
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    fmadd3(float *dst, const float *a, const float *b, size_t count);
            void    fmsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmmul3(float *dst, const float *a, const float *b, size_t count);
            void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
        }

        namespace sse2
        {
            void    fmmod3(float *dst, const float *a, const float *b, size_t count);
            void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
        }

        namespace avx
        {
            void    fmadd3(float *dst, const float *a, const float *b, size_t count);
            void    fmsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmmul3(float *dst, const float *a, const float *b, size_t count);
            void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmmod3(float *dst, const float *a, const float *b, size_t count);
            void    fmrmod3(float *dst, const float *a, const float *b, size_t count);

            void    fmadd3_fma3(float *dst, const float *a, const float *b, size_t count);
            void    fmsub3_fma3(float *dst, const float *a, const float *b, size_t count);
            void    fmrsub3_fma3(float *dst, const float *a, const float *b, size_t count);
            void    fmmod3_fma3(float *dst, const float *a, const float *b, size_t count);
            void    fmrmod3_fma3(float *dst, const float *a, const float *b, size_t count);
        }

        namespace avx512
        {
            void    fmadd3(float *dst, const float *a, const float *b, size_t count);
            void    fmsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmmul3(float *dst, const float *a, const float *b, size_t count);
            void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmmod3(float *dst, const float *a, const float *b, size_t count);
            void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    fmadd3(float *dst, const float *a, const float *b, size_t count);
            void    fmsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmmul3(float *dst, const float *a, const float *b, size_t count);
            void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmmod3(float *dst, const float *a, const float *b, size_t count);
            void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    fmadd3(float *dst, const float *a, const float *b, size_t count);
            void    fmsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
            void    fmmul3(float *dst, const float *a, const float *b, size_t count);
            void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
            void    fmmod3(float *dst, const float *a, const float *b, size_t count);
            void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
        }
    )

    typedef void (* fmop3_t)(float *dst, const float *a, const float *b, size_t count);
}

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", fmop3, 5, 1000)

    void call(const char *label, float *dst, const float *a, const float *b, size_t count, fmop3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, a, b, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 6, 64);
        float *a        = &dst[buf_size];
        float *b        = &a[buf_size];
        float *backup   = &b[buf_size];

        randomize_sign(dst, buf_size*3);
        dsp::copy(backup, dst, buf_size*3);

        #define CALL(method) \
            dsp::copy(dst, backup, buf_size*3); \
            call(#method, dst, a, b, count, method);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::fmadd3);
            IF_ARCH_X86(CALL(sse::fmadd3));
            IF_ARCH_X86(CALL(avx::fmadd3));
            IF_ARCH_X86(CALL(avx::fmadd3_fma3));
            IF_ARCH_X86(CALL(avx512::fmadd3));
            IF_ARCH_ARM(CALL(neon_d32::fmadd3));
            IF_ARCH_AARCH64(CALL(asimd::fmadd3));
            PTEST_SEPARATOR;

            CALL(generic::fmsub3);
            IF_ARCH_X86(CALL(sse::fmsub3));
            IF_ARCH_X86(CALL(avx::fmsub3));
            IF_ARCH_X86(CALL(avx::fmsub3_fma3));
            IF_ARCH_X86(CALL(avx512::fmsub3));
            IF_ARCH_ARM(CALL(neon_d32::fmsub3));
            IF_ARCH_AARCH64(CALL(asimd::fmsub3));
            PTEST_SEPARATOR;

            CALL(generic::fmrsub3);
            IF_ARCH_X86(CALL(sse::fmrsub3));
            IF_ARCH_X86(CALL(avx::fmrsub3));
            IF_ARCH_X86(CALL(avx::fmrsub3_fma3));
            IF_ARCH_X86(CALL(avx512::fmrsub3));
            IF_ARCH_ARM(CALL(neon_d32::fmrsub3));
            IF_ARCH_AARCH64(CALL(asimd::fmrsub3));
            PTEST_SEPARATOR;

            CALL(generic::fmmul3);
            IF_ARCH_X86(CALL(sse::fmmul3));
            IF_ARCH_X86(CALL(avx::fmmul3));
            IF_ARCH_X86(CALL(avx512::fmmul3));
            IF_ARCH_ARM(CALL(neon_d32::fmmul3));
            IF_ARCH_AARCH64(CALL(asimd::fmmul3));
            PTEST_SEPARATOR;

            CALL(generic::fmdiv3);
            IF_ARCH_X86(CALL(sse::fmdiv3));
            IF_ARCH_X86(CALL(avx::fmdiv3));
            IF_ARCH_X86(CALL(avx512::fmdiv3));
            IF_ARCH_ARM(CALL(neon_d32::fmdiv3));
            IF_ARCH_AARCH64(CALL(asimd::fmdiv3));
            PTEST_SEPARATOR;

            CALL(generic::fmrdiv3);
            IF_ARCH_X86(CALL(sse::fmrdiv3));
            IF_ARCH_X86(CALL(avx::fmrdiv3));
            IF_ARCH_X86(CALL(avx512::fmrdiv3));
            IF_ARCH_ARM(CALL(neon_d32::fmrdiv3));
            IF_ARCH_AARCH64(CALL(asimd::fmrdiv3));
            PTEST_SEPARATOR;

            CALL(generic::fmmod3);
            IF_ARCH_X86(CALL(sse2::fmmod3));
            IF_ARCH_X86(CALL(avx::fmmod3));
            IF_ARCH_X86(CALL(avx::fmmod3_fma3));
            IF_ARCH_X86(CALL(avx512::fmmod3));
            IF_ARCH_ARM(CALL(neon_d32::fmmod3));
            IF_ARCH_AARCH64(CALL(asimd::fmmod3));
            PTEST_SEPARATOR;

            CALL(generic::fmrmod3);
            IF_ARCH_X86(CALL(sse2::fmrmod3));
            IF_ARCH_X86(CALL(avx::fmrmod3));
            IF_ARCH_X86(CALL(avx::fmrmod3_fma3));
            IF_ARCH_X86(CALL(avx512::fmrmod3));
            IF_ARCH_ARM(CALL(neon_d32::fmrmod3));
            IF_ARCH_AARCH64(CALL(asimd::fmrmod3));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END

