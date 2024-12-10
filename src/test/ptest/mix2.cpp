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
        void mix2(float *dst, const float *src, float k1, float k2, size_t count);
        void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void mix2(float *dst, const float *src, float k1, float k2, size_t count);
            void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
            void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        }

        namespace avx
        {
            void mix2(float *dst, const float *src, float k1, float k2, size_t count);
            void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
            void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        }

        namespace avx512
        {
            void mix2(float *dst, const float *src, float k1, float k2, size_t count);
            void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
            void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void mix2(float *dst, const float *src, float k1, float k2, size_t count);
            void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
            void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void mix2(float *dst, const float *src, float k1, float k2, size_t count);
            void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
            void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        }
    )

    typedef void (* mix2_t)(float *dst, const float *src, float k1, float k2, size_t count);
    typedef void (* mix_dst2_t)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
}

PTEST_BEGIN("dsp", mix2, 5, 1000)

void call(const char *label, float **src, mix2_t func, size_t count)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(src[0], src[1], 1.0f, 1.1f, count);
        func(src[2], src[3], 1.2f, 1.3f, count);
        func(src[4], src[5], 1.4f, 1.5f, count);
        func(src[6], src[7], 1.6f, 1.7f, count);
        func(src[8], src[9], 1.8f, 1.9f, count);
        func(src[10], src[11], 2.1f, 2.2f, count);
    );
}

void call(const char *label, float **src, mix_dst2_t func, size_t count)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(src[0], src[0], src[1], 1.0f, 1.1f, count);
        func(src[2], src[2], src[3], 1.2f, 1.3f, count);
        func(src[4], src[4], src[5], 1.4f, 1.5f, count);
        func(src[6], src[6], src[7], 1.6f, 1.7f, count);
        func(src[8], src[8], src[9], 1.8f, 1.9f, count);
        func(src[10], src[10], src[11], 2.1f, 2.2f, count);
    );
}

PTEST_MAIN
{
    size_t buf_size = 1 << MAX_RANK;
    uint8_t *data   = NULL;
    float *src      = alloc_aligned<float>(data, buf_size*12*2, 64);
    float *buf[12];

    for (size_t i=0; i < 12; ++i)
    {
        buf[i]          = src;
        src            += buf_size;
    }
    randomize(src, buf_size*12, -10.0f, 10.0f);

    #define CALL(func) \
        dsp::copy(buf[0], src, buf_size*12); \
        call(#func, buf, func, count)

    for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
    {
        size_t count = 1 << i;

        CALL(generic::mix2);
        IF_ARCH_X86(CALL(sse::mix2));
        IF_ARCH_X86(CALL(avx::mix2));
        IF_ARCH_X86(CALL(avx512::mix2));
        IF_ARCH_ARM(CALL(neon_d32::mix2));
        IF_ARCH_AARCH64(CALL(asimd::mix2));
        PTEST_SEPARATOR;

        CALL(generic::mix_copy2);
        IF_ARCH_X86(CALL(sse::mix_copy2));
        IF_ARCH_X86(CALL(avx::mix_copy2));
        IF_ARCH_X86(CALL(avx512::mix_copy2));
        IF_ARCH_ARM(CALL(neon_d32::mix_copy2));
        IF_ARCH_AARCH64(CALL(asimd::mix_copy2));
        PTEST_SEPARATOR;

        CALL(generic::mix_add2);
        IF_ARCH_X86(CALL(sse::mix_add2));
        IF_ARCH_X86(CALL(avx::mix_add2));
        IF_ARCH_X86(CALL(avx512::mix_add2));
        IF_ARCH_ARM(CALL(neon_d32::mix_add2));
        IF_ARCH_AARCH64(CALL(asimd::mix_add2));
        PTEST_SEPARATOR2;
    }

    free_aligned(data);
}

PTEST_END

