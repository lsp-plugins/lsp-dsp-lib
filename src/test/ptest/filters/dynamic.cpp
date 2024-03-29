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
#include <lsp-plug.in/test-fw/ptest.h>

#define FTEST_BUF_SIZE 0x200

namespace lsp
{
    namespace generic
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
        void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
        void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
        }

        namespace sse3
        {
            void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
        }

        namespace avx
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x1_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);

            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x2_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);

            void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void dyn_biquad_process_x4_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);

            void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void dyn_biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
        }
    )

    typedef void (* dyn_biquad_process_x1_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
    typedef void (* dyn_biquad_process_x2_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
    typedef void (* dyn_biquad_process_x4_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
    typedef void (* dyn_biquad_process_x8_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);

    static dsp::biquad_x1_t bq_normal = {
        1.0, 2.0, 1.0,
        -2.0, -1.0,
        0.0, 0.0, 0.0
    };
}

//-----------------------------------------------------------------------------
// Performance test for dynamic biquad processing
PTEST_BEGIN("dsp.filters", dynamic, 10, 1000)

    void process_8x1(const char *text, float *out, const float *in, size_t count, dyn_biquad_process_x1_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;

        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(count));

        float d[16] __lsp_aligned64;
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        dsp::biquad_x1_t *f = alloc_aligned<dsp::biquad_x1_t>(ptr, count, 64);
        for (size_t i=0; i<count; ++i)
            f[i]        = bq_normal;

        PTEST_LOOP(text,
            process(out, in, &d[0], count, f);
            process(out, out, &d[2], count, f);
            process(out, out, &d[4], count, f);
            process(out, out, &d[6], count, f);
            process(out, out, &d[8], count, f);
            process(out, out, &d[10], count, f);
            process(out, out, &d[12], count, f);
            process(out, out, &d[14], count, f);
        );

        free_aligned(ptr);
    }

    void process_4x2(const char *text, float *out, const float *in, size_t count, dyn_biquad_process_x2_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(count));

        float d[16] __lsp_aligned64;
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        dsp::biquad_x2_t *f = alloc_aligned<dsp::biquad_x2_t>(ptr, count+1, 64);
        for (size_t i=0; i<(count+1); ++i)
        {
            for (size_t j=0; j<2; ++j)
            {
                f[i].b0[j]  = bq_normal.b0;
                f[i].b1[j]  = bq_normal.b1;
                f[i].b2[j]  = bq_normal.b2;
                f[i].a1[j]  = bq_normal.a1;
                f[i].a2[j]  = bq_normal.a2;
                f[i].p[j]   = 0.0f;
            }
        }

        PTEST_LOOP(text,
            process(out, in, &d[0], count, f);
            process(out, out, &d[4], count, f);
            process(out, out, &d[8], count, f);
            process(out, out, &d[12], count, f);
        );

        free_aligned(ptr);
    }

    void process_2x4(const char *text, float *out, const float *in, size_t count, dyn_biquad_process_x4_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(count));

        float d[16] __lsp_aligned64;
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        dsp::biquad_x4_t *f = alloc_aligned<dsp::biquad_x4_t>(ptr, count+3, 64);
        for (size_t i=0; i<(count+3); ++i)
        {
            for (size_t j=0; j<4; ++j)
            {
                f[i].b0[j]  = bq_normal.b0;
                f[i].b1[j]  = bq_normal.b1;
                f[i].b2[j]  = bq_normal.b2;
                f[i].a1[j]  = bq_normal.a1;
                f[i].a2[j]  = bq_normal.a2;
            }
        }

        PTEST_LOOP(text,
            process(out, in, &d[0], count, f);
            process(out, out, &d[8], count, f);
        );

        free_aligned(ptr);
    }

    void process_1x8(const char *text, float *out, const float *in, size_t count, dyn_biquad_process_x8_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(count));

        float d[16] __lsp_aligned64;
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        dsp::biquad_x8_t *f = alloc_aligned<dsp::biquad_x8_t>(ptr, count+7, 64);
        for (size_t i=0; i<(count+7); ++i)
        {
            for (size_t j=0; j<8; ++j)
            {
                f[i].b0[j]  = bq_normal.b0;
                f[i].b1[j]  = bq_normal.b1;
                f[i].b2[j]  = bq_normal.b2;
                f[i].a1[j]  = bq_normal.a1;
                f[i].a2[j]  = bq_normal.a2;
            }
        }

        PTEST_LOOP(text,
            process(out, in, d, count, f);
        );

        free_aligned(ptr);
    }

    PTEST_MAIN
    {
        float *out          = new float[FTEST_BUF_SIZE];
        float *in           = new float[FTEST_BUF_SIZE];

        for (size_t i=0; i<FTEST_BUF_SIZE; ++i)
        {
            in[i]               = (i % 1) ? 1.0f : -1.0f;
            out[i]              = 0.0f;
        }

        process_8x1("generic::dyn_biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, generic::dyn_biquad_process_x1);
        IF_ARCH_X86(process_8x1("sse::dyn_biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, sse::dyn_biquad_process_x1));
        IF_ARCH_X86(process_8x1("avx::dyn_biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, avx::dyn_biquad_process_x1));
        IF_ARCH_X86(process_8x1("avx::dyn_biquad_process_x1_fma3 x8", out, in, FTEST_BUF_SIZE, avx::dyn_biquad_process_x1_fma3));
        IF_ARCH_ARM(process_8x1("neon_d32::dyn_biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, neon_d32::dyn_biquad_process_x1));
        IF_ARCH_AARCH64(process_8x1("asimd::dyn_biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, asimd::dyn_biquad_process_x1));
        PTEST_SEPARATOR;

        process_4x2("generic::dyn_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, generic::dyn_biquad_process_x2);
        IF_ARCH_X86(process_4x2("sse::dyn_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, sse::dyn_biquad_process_x2));
        IF_ARCH_X86(process_4x2("avx::dyn_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, avx::dyn_biquad_process_x2));
        IF_ARCH_X86(process_4x2("avx::dyn_biquad_process_x2_fma3 x4", out, in, FTEST_BUF_SIZE, avx::dyn_biquad_process_x2_fma3));
        IF_ARCH_ARM(process_4x2("neon_d32::dyn_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, neon_d32::dyn_biquad_process_x2));
        IF_ARCH_AARCH64(process_4x2("asimd::dyn_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, asimd::dyn_biquad_process_x2));
        PTEST_SEPARATOR;

        process_2x4("generic::dyn_biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, generic::dyn_biquad_process_x4);
        IF_ARCH_X86(process_2x4("sse::dyn_biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, sse::dyn_biquad_process_x4));
        IF_ARCH_X86(process_2x4("avx::dyn_biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, avx::dyn_biquad_process_x4));
        IF_ARCH_X86(process_2x4("avx::dyn_biquad_process_x4_fma3 x2", out, in, FTEST_BUF_SIZE, avx::dyn_biquad_process_x4_fma3));
        IF_ARCH_ARM(process_2x4("neon_d32::dyn_biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, neon_d32::dyn_biquad_process_x4));
        IF_ARCH_AARCH64(process_2x4("asimd::dyn_biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, asimd::dyn_biquad_process_x4));
        PTEST_SEPARATOR;

        process_1x8("generic::dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, generic::dyn_biquad_process_x8);
        IF_ARCH_X86(process_1x8("sse::dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, sse::dyn_biquad_process_x8));
        IF_ARCH_X86(process_1x8("sse3::x64_dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, sse3::x64_dyn_biquad_process_x8));
        IF_ARCH_X86(process_1x8("avx::x64_dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, avx::x64_dyn_biquad_process_x8));
        IF_ARCH_X86(process_1x8("avx::dyn_biquad_process_x8_fma3 x1", out, in, FTEST_BUF_SIZE, avx::dyn_biquad_process_x8_fma3));
        IF_ARCH_ARM(process_1x8("neon_d32::dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, neon_d32::dyn_biquad_process_x8));
        IF_ARCH_AARCH64(process_1x8("asimd::dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, asimd::dyn_biquad_process_x8));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END


