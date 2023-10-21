/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 окт. 2023 г.
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
#define MAX_RANK 10

namespace lsp
{
    namespace generic
    {
        void compressor_env(float *dst, const float *src, dsp::compressor_env_t *env, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void compressor_env(float *dst, const float *src, dsp::compressor_env_t *env, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void compressor_env(float *dst, const float *src, dsp::compressor_env_t *env, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void compressor_env(float *dst, const float *src, dsp::compressor_env_t *env, size_t count);
        }
    )

    static void compressor_env_naive(float *dst, const float *src, dsp::compressor_env_t *env, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float s         = src[i];
            if (env->env > env->rel_thresh)
                env->env        += (s > env->env) ? env->attack * (s - env->env) : env->release * (s - env->env);
            else
                env->env        += env->attack * (s - env->env);
            dst[i]          = env->env;
        }
    }
}

typedef void (* compressor_env_func_t)(float *dst, const float *src, lsp::dsp::compressor_env_t *env, size_t count);

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.dynamics", compressor_env, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, compressor_env_func_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s points...\n", buf);
        dsp::compressor_env_t env;

        PTEST_LOOP(buf,
            env.env         = 0.0f;
            env.rel_thresh  = 0.25f;
            env.attack      = 0.55f;
            env.release     = 0.45f;

            func(dst, src, &env, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *ptr          = alloc_aligned<float>(data, buf_size * 2, 64);

        TEST_EXPORT(compressor_env_naive);

        float *src          = ptr;
        float *dst          = &src[buf_size];

        randomize_0to1(src, buf_size);

        #define CALL(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(compressor_env_naive);
            CALL(generic::compressor_env);
            IF_ARCH_X86(CALL(sse2::compressor_env));
//            IF_ARCH_ARM(CALL(neon_d32::compressor_env));
//            IF_ARCH_AARCH64(CALL(asimd::compressor_env));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



