/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 мар. 2026 г.
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
        void lr_pmin2(float *dst, const float *src, size_t count);
        void lr_pmax2(float *dst, const float *src, size_t count);
        void lr_psmin2(float *dst, const float *src, size_t count);
        void lr_psmax2(float *dst, const float *src, size_t count);
        void lr_pamin2(float *dst, const float *src, size_t count);
        void lr_pamax2(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }
    )
}

typedef void (* lr_poper2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", lr_minmax2, 2, 2000)

    void call(const char *label, float *dst, const float *src, size_t count, lr_poper2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        randomize_sign(dst, buf_size*2);
        dsp::copy(backup, dst, buf_size*2);

        #define CALL(method) \
            dsp::copy(dst, backup, buf_size*2); \
            call(#method, dst, src, count, method);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::lr_pmin2);
            IF_ARCH_X86(CALL(sse::lr_pmin2));
            IF_ARCH_X86(CALL(avx::lr_pmin2));
            IF_ARCH_X86(CALL(avx512::lr_pmin2));
            IF_ARCH_ARM(CALL(neon_d32::lr_pmin2));
            IF_ARCH_AARCH64(CALL(asimd::lr_pmin2));
            PTEST_SEPARATOR;

            CALL(generic::lr_pmax2);
            IF_ARCH_X86(CALL(sse::lr_pmax2));
            IF_ARCH_X86(CALL(avx::lr_pmax2));
            IF_ARCH_X86(CALL(avx512::lr_pmax2));
            IF_ARCH_ARM(CALL(neon_d32::lr_pmax2));
            IF_ARCH_AARCH64(CALL(asimd::lr_pmax2));
            PTEST_SEPARATOR;

            CALL(generic::lr_psmin2);
            IF_ARCH_X86(CALL(sse::lr_psmin2));
            IF_ARCH_X86(CALL(avx::lr_psmin2));
            IF_ARCH_X86(CALL(avx512::lr_psmin2));
            IF_ARCH_ARM(CALL(neon_d32::lr_psmin2));
            IF_ARCH_AARCH64(CALL(asimd::lr_psmin2));
            PTEST_SEPARATOR;

            CALL(generic::lr_psmax2);
            IF_ARCH_X86(CALL(sse::lr_psmax2));
            IF_ARCH_X86(CALL(avx::lr_psmax2));
            IF_ARCH_X86(CALL(avx512::lr_psmax2));
            IF_ARCH_ARM(CALL(neon_d32::lr_psmax2));
            IF_ARCH_AARCH64(CALL(asimd::lr_psmax2));
            PTEST_SEPARATOR;

            CALL(generic::lr_pamin2);
            IF_ARCH_X86(CALL(sse::lr_pamin2));
            IF_ARCH_X86(CALL(avx::lr_pamin2));
            IF_ARCH_X86(CALL(avx512::lr_pamin2));
            IF_ARCH_ARM(CALL(neon_d32::lr_pamin2));
            IF_ARCH_AARCH64(CALL(asimd::lr_pamin2));
            PTEST_SEPARATOR;

            CALL(generic::lr_pamax2);
            IF_ARCH_X86(CALL(sse::lr_pamax2));
            IF_ARCH_X86(CALL(avx::lr_pamax2));
            IF_ARCH_X86(CALL(avx512::lr_pamax2));
            IF_ARCH_ARM(CALL(neon_d32::lr_pamax2));
            IF_ARCH_AARCH64(CALL(asimd::lr_pamax2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END



