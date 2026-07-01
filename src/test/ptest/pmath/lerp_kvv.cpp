/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 июл. 2026 г.
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
        void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count);
        }

        namespace avx
        {
            void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count);
//            void lerp_kvv_fma3(float *dst, float a, const float *b, const float *k, size_t count);
        }

//        namespace avx512
//        {
//            void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count);
//        }
    )

//    IF_ARCH_ARM(
//        namespace neon_d32
//        {
//            void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count);
//        }
//    )
//
//    IF_ARCH_AARCH64(
//        namespace asimd
//        {
//            void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count);
//        }
//    )

    typedef void (* lerp_kvv_t)(float *dst, float a, const float *b, const float *k, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.pmath", lerp_kvv, 5, 1000)

    void call(const char *label, float *dst, float a, const float *b, const float *k, size_t count, lerp_kvv_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, a, b, k, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *b        = &dst[buf_size];
        float *k        = &dst[buf_size];

        for (size_t i=0; i < buf_size*3; ++i)
            dst[i]          = randf(-1.0f, 1.0f);

        #define CALL(func) \
            call(#func, dst, 1.0f, b, k, count, func);


        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::lerp_kvv);
            IF_ARCH_X86(CALL(sse::lerp_kvv));
            IF_ARCH_X86(CALL(avx::lerp_kvv));
//            IF_ARCH_X86(CALL(avx::lerp_kvv_fma3));
//            IF_ARCH_X86(CALL(avx512::lerp_kvv));
//            IF_ARCH_ARM(CALL(neon_d32::lerp_kvv));
//            IF_ARCH_AARCH64(CALL(asimd::lerp_kvv));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END





