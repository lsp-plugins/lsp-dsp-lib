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
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    add3(float *dst, const float *src1, const float *src2, size_t count);
            void    sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    div3(float *dst, const float *src1, const float *src2, size_t count);
        }

        namespace sse2
        {
            void    mod3(float *dst, const float *src1, const float *src2, size_t count);
        }

        namespace avx
        {
            void    add3(float *dst, const float *src1, const float *src2, size_t count);
            void    sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    div3(float *dst, const float *src1, const float *src2, size_t count);
            void    mod3(float *dst, const float *src1, const float *src2, size_t count);
            void    mod3_fma3(float *dst, const float *src1, const float *src2, size_t count);
        }

        namespace avx512
        {
            void    add3(float *dst, const float *src1, const float *src2, size_t count);
            void    sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    div3(float *dst, const float *src1, const float *src2, size_t count);
            void    mod3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    add3(float *dst, const float *src1, const float *src2, size_t count);
            void    sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    div3(float *dst, const float *src1, const float *src2, size_t count);
            void    mod3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    add3(float *dst, const float *src1, const float *src2, size_t count);
            void    sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    div3(float *dst, const float *src1, const float *src2, size_t count);
            void    mod3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )

    typedef void (* func3)(float *dst, const float *src1, const float *src2, size_t count);
}

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", op3, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, func3 func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src1, src2, count);
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
            call(#func, dst, src1, src2, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::add3);
            IF_ARCH_X86(CALL(sse::add3));
            IF_ARCH_X86(CALL(avx::add3));
            IF_ARCH_X86(CALL(avx512::add3));
            IF_ARCH_ARM(CALL(neon_d32::add3));
            IF_ARCH_AARCH64(CALL(asimd::add3));
            PTEST_SEPARATOR;

            CALL(generic::sub3);
            IF_ARCH_X86(CALL(sse::sub3));
            IF_ARCH_X86(CALL(avx::sub3));
            IF_ARCH_X86(CALL(avx512::sub3));
            IF_ARCH_ARM(CALL(neon_d32::sub3));
            IF_ARCH_AARCH64(CALL(asimd::sub3));
            PTEST_SEPARATOR;

            CALL(generic::mul3);
            IF_ARCH_X86(CALL(sse::mul3));
            IF_ARCH_X86(CALL(avx::mul3));
            IF_ARCH_X86(CALL(avx512::mul3));
            IF_ARCH_ARM(CALL(neon_d32::mul3));
            IF_ARCH_AARCH64(CALL(asimd::mul3));
            PTEST_SEPARATOR;

            CALL(generic::div3);
            IF_ARCH_X86(CALL(sse::div3));
            IF_ARCH_X86(CALL(avx::div3));
            IF_ARCH_X86(CALL(avx512::div3));
            IF_ARCH_ARM(CALL(neon_d32::div3));
            IF_ARCH_AARCH64(CALL(asimd::div3));
            PTEST_SEPARATOR;

            CALL(generic::mod3);
            IF_ARCH_X86(CALL(sse2::mod3));
            IF_ARCH_X86(CALL(avx::mod3));
            IF_ARCH_X86(CALL(avx::mod3_fma3));
            IF_ARCH_X86(CALL(avx512::mod3));
            IF_ARCH_ARM(CALL(neon_d32::mod3));
            IF_ARCH_AARCH64(CALL(asimd::mod3));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


