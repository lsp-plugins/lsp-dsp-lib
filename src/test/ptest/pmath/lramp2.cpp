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
        void lramp2(float *dst, const float *src, float v1, float v2, size_t count);
        void lramp_add2(float *dst, const float *src, float v1, float v2, size_t count);
        void lramp_sub2(float *dst, const float *src, float v1, float v2, size_t count);
        void lramp_rsub2(float *dst, const float *src, float v1, float v2, size_t count);
        void lramp_mul2(float *dst, const float *src, float v1, float v2, size_t count);
        void lramp_div2(float *dst, const float *src, float v1, float v2, size_t count);
        void lramp_rdiv2(float *dst, const float *src, float v1, float v2, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lramp2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_add2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_sub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rsub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_mul2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_div2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rdiv2(float *dst, const float *src, float v1, float v2, size_t count);
        }

        namespace avx
        {
            void lramp2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_add2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_sub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rsub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_mul2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_div2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rdiv2(float *dst, const float *src, float v1, float v2, size_t count);
        }

        namespace avx512
        {
            void lramp2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_add2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_sub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rsub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_mul2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_div2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rdiv2(float *dst, const float *src, float v1, float v2, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void lramp2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_add2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_sub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rsub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_mul2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_div2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rdiv2(float *dst, const float *src, float v1, float v2, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void lramp2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_add2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_sub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rsub2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_mul2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_div2(float *dst, const float *src, float v1, float v2, size_t count);
            void lramp_rdiv2(float *dst, const float *src, float v1, float v2, size_t count);
        }
    )
}

typedef void (* lramp2_t)(float *dst, const float *src, float v1, float v2, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", lramp2, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, lramp2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, 0.25f, 0.75f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        for (size_t i=0; i < buf_size*2; ++i)
            backup[i]       = randf(0.001f, 1.0f);

        #define CALL(func) \
            dsp::copy(dst, backup, buf_size*2); \
            call(#func, dst, src, count, func);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::lramp2);
            IF_ARCH_X86(CALL(sse::lramp2));
            IF_ARCH_X86(CALL(avx::lramp2));
            IF_ARCH_X86(CALL(avx512::lramp2));
            IF_ARCH_ARM(CALL(neon_d32::lramp2));
            IF_ARCH_AARCH64(CALL(asimd::lramp2));
            PTEST_SEPARATOR;

            CALL(generic::lramp_add2);
            IF_ARCH_X86(CALL(sse::lramp_add2));
            IF_ARCH_X86(CALL(avx::lramp_add2));
            IF_ARCH_X86(CALL(avx512::lramp_add2));
            IF_ARCH_ARM(CALL(neon_d32::lramp_add2));
            IF_ARCH_AARCH64(CALL(asimd::lramp_add2));
            PTEST_SEPARATOR;

            CALL(generic::lramp_sub2);
            IF_ARCH_X86(CALL(sse::lramp_sub2));
            IF_ARCH_X86(CALL(avx::lramp_sub2));
            IF_ARCH_X86(CALL(avx512::lramp_sub2));
            IF_ARCH_ARM(CALL(neon_d32::lramp_sub2));
            IF_ARCH_AARCH64(CALL(asimd::lramp_sub2));
            PTEST_SEPARATOR;

            CALL(generic::lramp_rsub2);
            IF_ARCH_X86(CALL(sse::lramp_rsub2));
            IF_ARCH_X86(CALL(avx::lramp_rsub2));
            IF_ARCH_X86(CALL(avx512::lramp_rsub2));
            IF_ARCH_ARM(CALL(neon_d32::lramp_rsub2));
            IF_ARCH_AARCH64(CALL(asimd::lramp_rsub2));
            PTEST_SEPARATOR;

            CALL(generic::lramp_mul2);
            IF_ARCH_X86(CALL(sse::lramp_mul2));
            IF_ARCH_X86(CALL(avx::lramp_mul2));
            IF_ARCH_X86(CALL(avx512::lramp_mul2));
            IF_ARCH_ARM(CALL(neon_d32::lramp_mul2));
            IF_ARCH_AARCH64(CALL(asimd::lramp_mul2));
            PTEST_SEPARATOR;

            CALL(generic::lramp_div2);
            IF_ARCH_X86(CALL(sse::lramp_div2));
            IF_ARCH_X86(CALL(avx::lramp_div2));
            IF_ARCH_X86(CALL(avx512::lramp_div2));
            IF_ARCH_ARM(CALL(neon_d32::lramp_div2));
            IF_ARCH_AARCH64(CALL(asimd::lramp_div2));
            PTEST_SEPARATOR;

            CALL(generic::lramp_rdiv2);
            IF_ARCH_X86(CALL(sse::lramp_rdiv2));
            IF_ARCH_X86(CALL(avx::lramp_rdiv2));
            IF_ARCH_X86(CALL(avx512::lramp_rdiv2));
            IF_ARCH_ARM(CALL(neon_d32::lramp_rdiv2));
            IF_ARCH_AARCH64(CALL(asimd::lramp_rdiv2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


