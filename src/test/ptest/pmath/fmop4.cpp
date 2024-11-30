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
        void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }

        namespace sse2
        {
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }

        namespace avx
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);

            void    fmadd4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
        }

        namespace avx512
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }
    )

    typedef void (* fmop4_t)(float *dst, const float *a, const float *b, const float *c, size_t count);
}

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", fmop4, 5, 1000)

    void call(const char *label, float *dst, const float *a, const float *b, const float *c, size_t count, fmop4_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, a, b, c, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *a        = &dst[buf_size];
        float *b        = &a[buf_size];
        float *c        = &b[buf_size];

        randomize_sign(dst, buf_size*4);

        #define CALL(method) \
            call(#method, dst, a, b, c, count, method);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::fmadd4);
            IF_ARCH_X86(CALL(sse::fmadd4));
            IF_ARCH_X86(CALL(avx::fmadd4));
            IF_ARCH_X86(CALL(avx::fmadd4_fma3));
            IF_ARCH_X86(CALL(avx512::fmadd4));
            IF_ARCH_ARM(CALL(neon_d32::fmadd4));
            IF_ARCH_AARCH64(CALL(asimd::fmadd4));
            PTEST_SEPARATOR;

            CALL(generic::fmsub4);
            IF_ARCH_X86(CALL(sse::fmsub4));
            IF_ARCH_X86(CALL(avx::fmsub4));
            IF_ARCH_X86(CALL(avx::fmsub4_fma3));
            IF_ARCH_X86(CALL(avx512::fmsub4));
            IF_ARCH_ARM(CALL(neon_d32::fmsub4));
            IF_ARCH_AARCH64(CALL(asimd::fmsub4));
            PTEST_SEPARATOR;

            CALL(generic::fmrsub4);
            IF_ARCH_X86(CALL(sse::fmrsub4));
            IF_ARCH_X86(CALL(avx::fmrsub4));
            IF_ARCH_X86(CALL(avx::fmrsub4_fma3));
            IF_ARCH_X86(CALL(avx512::fmrsub4));
            IF_ARCH_ARM(CALL(neon_d32::fmrsub4));
            IF_ARCH_AARCH64(CALL(asimd::fmrsub4));
            PTEST_SEPARATOR;

            CALL(generic::fmmul4);
            IF_ARCH_X86(CALL(sse::fmmul4));
            IF_ARCH_X86(CALL(avx::fmmul4));
            IF_ARCH_X86(CALL(avx512::fmmul4));
            IF_ARCH_ARM(CALL(neon_d32::fmmul4));
            IF_ARCH_AARCH64(CALL(asimd::fmmul4));
            PTEST_SEPARATOR;

            CALL(generic::fmdiv4);
            IF_ARCH_X86(CALL(sse::fmdiv4));
            IF_ARCH_X86(CALL(avx::fmdiv4));
            IF_ARCH_X86(CALL(avx512::fmdiv4));
            IF_ARCH_ARM(CALL(neon_d32::fmdiv4));
            IF_ARCH_AARCH64(CALL(asimd::fmdiv4));
            PTEST_SEPARATOR;

            CALL(generic::fmrdiv4);
            IF_ARCH_X86(CALL(sse::fmrdiv4));
            IF_ARCH_X86(CALL(avx::fmrdiv4));
            IF_ARCH_X86(CALL(avx512::fmrdiv4));
            IF_ARCH_ARM(CALL(neon_d32::fmrdiv4));
            IF_ARCH_AARCH64(CALL(asimd::fmrdiv4));
            PTEST_SEPARATOR;

            CALL(generic::fmmod4);
            IF_ARCH_X86(CALL(sse2::fmmod4));
            IF_ARCH_X86(CALL(avx::fmmod4));
            IF_ARCH_X86(CALL(avx::fmmod4_fma3));
            IF_ARCH_X86(CALL(avx512::fmmod4));
            IF_ARCH_ARM(CALL(neon_d32::fmmod4));
            IF_ARCH_AARCH64(CALL(asimd::fmmod4));
            PTEST_SEPARATOR;

            CALL(generic::fmrmod4);
            IF_ARCH_X86(CALL(sse2::fmrmod4));
            IF_ARCH_X86(CALL(avx::fmrmod4));
            IF_ARCH_X86(CALL(avx::fmrmod4_fma3));
            IF_ARCH_X86(CALL(avx512::fmrmod4));
            IF_ARCH_ARM(CALL(neon_d32::fmrmod4));
            IF_ARCH_AARCH64(CALL(asimd::fmrmod4));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END




