/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 19 окт. 2023 г.
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

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
        }

        namespace avx2
        {
            void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
            void gate_x1_gain_fma3(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
        }

        namespace avx512
        {
            void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace avx2
        {
            void x64_gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
            void x64_gate_x1_gain_fma3(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count);
        }
    )
}

typedef void (* gate_x1_func_t)(float *dst, const float *src, const lsp::dsp::gate_knee_t *c, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.dynamics", gate_x1_gain, 5, 1000)

    void call(const char *label, float *dst, const float *src, const dsp::gate_knee_t *gate, size_t count, gate_x1_func_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s points...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, gate, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *ptr          = alloc_aligned<float>(data, buf_size * 2, 64);

        dsp::gate_knee_t gate;
        gate = {
            0.00794381928f,
            0.0631000027f,
            0.0631000027f,
            1.0f,
            {-0.620928824f, -7.07709408f, -24.8873253f, -27.8333282f}
        };

        float *src          = ptr;
        float *dst          = &src[buf_size];
        float k             = 72.0f / (1 << MIN_RANK);

        for (size_t i=0; i<buf_size; ++i)
        {
            float db        = -72.0f + (i % (1 << MIN_RANK)) * k;
            src[i]          = expf(db * M_LN10 * 0.05f);
        }

        #define CALL(func) \
            call(#func, dst, src, &gate, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::gate_x1_gain);
            IF_ARCH_X86(CALL(sse2::gate_x1_gain));
            IF_ARCH_X86(CALL(avx2::gate_x1_gain));
            IF_ARCH_X86_64(CALL(avx2::x64_gate_x1_gain));
            IF_ARCH_X86(CALL(avx2::gate_x1_gain_fma3));
            IF_ARCH_X86_64(CALL(avx2::x64_gate_x1_gain_fma3));
            IF_ARCH_X86(CALL(avx512::gate_x1_gain));
            IF_ARCH_ARM(CALL(neon_d32::gate_x1_gain));
            IF_ARCH_AARCH64(CALL(asimd::gate_x1_gain));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



