/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 23 окт. 2023 г.
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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 6
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
        }

        namespace avx
        {
            float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
        }
    )

    static float sidechain_rms_naive(float *dst, float *head, const float *tail, float rms, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float s         = dst[i];
            head[i]         = s;
            float t         = tail[i];
            rms            += s*s - t*t;
            dst[i]          = (rms > 0.0f) ? sqrtf(rms * k) : 0.0f;
        }

        return rms;
    }
}

typedef float (* sidechain_rms_func_t)(float *dst, float *head, const float *tail, float rms, float k, size_t count);

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.dynamics", sidechain_rms, 5, 1000)

    void call(const char *label, float *dst, const float *src, float *head, const float *tail, size_t count, sidechain_rms_func_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s points...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, head, tail, 1000.0f, 0.001f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *ptr          = alloc_aligned<float>(data, buf_size * 8, 64);

        TEST_EXPORT(sidechain_rms_naive);

        float *src          = ptr;
        float *dst          = &src[buf_size];
        float *head         = &dst[buf_size];
        float *tail         = &head[buf_size];
        float *backup       = &tail[buf_size];

        randomize_sign(src, buf_size*4);
        dsp::copy(backup, src, buf_size*4);

        #define CALL(func) \
            dsp::copy(src, backup, buf_size*4); \
            call(#func, dst, src, head, tail, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(sidechain_rms_naive);
            CALL(generic::sidechain_rms);
            IF_ARCH_X86(CALL(sse::sidechain_rms));
            IF_ARCH_X86(CALL(avx::sidechain_rms));
//            IF_ARCH_ARM(CALL(neon_d32::sidechain_rms));
//            IF_ARCH_AARCH64(CALL(asimd::sidechain_rms));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



