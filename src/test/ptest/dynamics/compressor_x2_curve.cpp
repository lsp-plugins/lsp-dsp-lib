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

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }

        namespace avx2
        {
            void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
            void compressor_x2_curve_fma3(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }

        namespace avx512
        {
            void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace avx2
        {
            void x64_compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
            void x64_compressor_x2_curve_fma3(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }
    )
}

typedef void (* compressor_x2_func_t)(float *dst, const float *src, const lsp::dsp::compressor_x2_t *c, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.dynamics", compressor_x2_curve, 5, 1000)

    void call(const char *label, float *dst, const float *src, const dsp::compressor_x2_t *comp, size_t count, compressor_x2_func_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s points...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, comp, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        float *ptr          = alloc_aligned<float>(data, buf_size * 2, 64);

        dsp::compressor_x2_t comp;
        comp.k[0] = {
            0.125891402,
            0.501197219,
            1.0f,
            { -0.271428347, -1.12498128, -1.16566944 },
            { -0.75, -1.03615928 }};
        comp.k[1] = {
            0.0f,
            0.0f,
            1.0f,
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f }};

        float *src          = ptr;
        float *dst          = &src[buf_size];
        float k             = 72.0f / (1 << MIN_RANK);

        for (size_t i=0; i<buf_size; ++i)
        {
            float db        = -72.0f + (i % (1 << MIN_RANK)) * k;
            src[i]          = expf(db * M_LN10 * 0.05f);
        }

        #define CALL(func) \
            call(#func, dst, src, &comp, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::compressor_x2_curve);
            IF_ARCH_X86(CALL(sse2::compressor_x2_curve));
            IF_ARCH_X86(CALL(avx2::compressor_x2_curve));
            IF_ARCH_X86_64(CALL(avx2::x64_compressor_x2_curve));
            IF_ARCH_X86(CALL(avx2::compressor_x2_curve_fma3));
            IF_ARCH_X86_64(CALL(avx2::x64_compressor_x2_curve_fma3));
            IF_ARCH_X86(CALL(avx512::compressor_x2_curve));
            IF_ARCH_ARM(CALL(neon_d32::compressor_x2_curve));
            IF_ARCH_AARCH64(CALL(asimd::compressor_x2_curve));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



