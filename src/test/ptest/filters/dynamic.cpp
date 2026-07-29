/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
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
        void dyn_biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void dyn_biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }

        namespace avx
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x1_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);

            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x2_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);

            void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void dyn_biquad_process_x4_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);

            void dyn_biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void dyn_biquad_process_x16_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }

        namespace avx512
        {
            void dyn_biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }
    )

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void x64_dyn_biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }

        namespace avx
        {
            void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void x64_dyn_biquad_process_x16_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void dyn_biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void dyn_biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }
    )

    typedef void (* dyn_biquad_process_x1_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
    typedef void (* dyn_biquad_process_x2_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
    typedef void (* dyn_biquad_process_x4_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
    typedef void (* dyn_biquad_process_x8_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
    typedef void (* dyn_biquad_process_x16_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);

    static dsp::biquad_x1_t bq_normal = {
        1.0f, 2.0f, 1.0f,
        -2.0f, -1.0f,
        0.0f, 0.0f, 0.0f
    };

    constexpr size_t d_buffer_size = 2 * 16;
}

//-----------------------------------------------------------------------------
// Performance test for dynamic biquad processing
PTEST_BEGIN("dsp.filters", dynamic, 5, 1000)

    void process_16x1(const char *text, float *out, const float *in, dyn_biquad_process_x1_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;

        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        float d[d_buffer_size] __lsp_aligned64;
        dsp::fill_zero(d, d_buffer_size);

        void *ptr = NULL;
        dsp::biquad_x1_t *f = alloc_aligned<dsp::biquad_x1_t>(ptr, FTEST_BUF_SIZE, 64);
        for (size_t i=0; i<FTEST_BUF_SIZE; ++i)
            f[i]        = bq_normal;

        PTEST_LOOP(text,
            process(out, in, &d[0], FTEST_BUF_SIZE, f);
            process(out, out, &d[2], FTEST_BUF_SIZE, f);
            process(out, out, &d[4], FTEST_BUF_SIZE, f);
            process(out, out, &d[6], FTEST_BUF_SIZE, f);
            process(out, out, &d[8], FTEST_BUF_SIZE, f);
            process(out, out, &d[10], FTEST_BUF_SIZE, f);
            process(out, out, &d[12], FTEST_BUF_SIZE, f);
            process(out, out, &d[14], FTEST_BUF_SIZE, f);
            process(out, out, &d[16], FTEST_BUF_SIZE, f);
            process(out, out, &d[18], FTEST_BUF_SIZE, f);
            process(out, out, &d[20], FTEST_BUF_SIZE, f);
            process(out, out, &d[22], FTEST_BUF_SIZE, f);
            process(out, out, &d[24], FTEST_BUF_SIZE, f);
            process(out, out, &d[26], FTEST_BUF_SIZE, f);
            process(out, out, &d[28], FTEST_BUF_SIZE, f);
            process(out, out, &d[30], FTEST_BUF_SIZE, f);
        );

        free_aligned(ptr);
    }

    void process_8x2(const char *text, float *out, const float *in, dyn_biquad_process_x2_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        float d[d_buffer_size] __lsp_aligned64;
        dsp::fill_zero(d, d_buffer_size);

        void *ptr = NULL;
        dsp::biquad_x2_t *f = alloc_aligned<dsp::biquad_x2_t>(ptr, FTEST_BUF_SIZE+1, 64);
        for (size_t i=0; i<(FTEST_BUF_SIZE+1); ++i)
        {
            for (size_t j=0; j<2; ++j)
            {
                f[i].b0[j]  = bq_normal.b0;
                f[i].b1[j]  = bq_normal.b1;
                f[i].b2[j]  = bq_normal.b2;
                f[i].a1[j]  = bq_normal.a1;
                f[i].a2[j]  = bq_normal.a2;
            }
        }

        PTEST_LOOP(text,
            process(out, in, &d[0], FTEST_BUF_SIZE, f);
            process(out, out, &d[4], FTEST_BUF_SIZE, f);
            process(out, out, &d[8], FTEST_BUF_SIZE, f);
            process(out, out, &d[12], FTEST_BUF_SIZE, f);
            process(out, out, &d[16], FTEST_BUF_SIZE, f);
            process(out, out, &d[20], FTEST_BUF_SIZE, f);
            process(out, out, &d[24], FTEST_BUF_SIZE, f);
            process(out, out, &d[28], FTEST_BUF_SIZE, f);
        );

        free_aligned(ptr);
    }

    void process_4x4(const char *text, float *out, const float *in, dyn_biquad_process_x4_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        float d[d_buffer_size] __lsp_aligned64;
        dsp::fill_zero(d, d_buffer_size);

        void *ptr = NULL;
        dsp::biquad_x4_t *f = alloc_aligned<dsp::biquad_x4_t>(ptr, FTEST_BUF_SIZE+3, 64);
        for (size_t i=0; i<(FTEST_BUF_SIZE+3); ++i)
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
            process(out, in, &d[0], FTEST_BUF_SIZE, f);
            process(out, out, &d[8], FTEST_BUF_SIZE, f);
            process(out, out, &d[16], FTEST_BUF_SIZE, f);
            process(out, out, &d[24], FTEST_BUF_SIZE, f);
        );

        free_aligned(ptr);
    }

    void process_2x8(const char *text, float *out, const float *in, dyn_biquad_process_x8_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        float d[d_buffer_size] __lsp_aligned64;
        dsp::fill_zero(d, d_buffer_size);

        void *ptr = NULL;
        dsp::biquad_x8_t *f = alloc_aligned<dsp::biquad_x8_t>(ptr, FTEST_BUF_SIZE+7, 64);
        for (size_t i=0; i<(FTEST_BUF_SIZE+7); ++i)
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
            process(out, in, d, FTEST_BUF_SIZE, f);
            process(out, out, &d[16], FTEST_BUF_SIZE, f);
        );

        free_aligned(ptr);
    }

    void process_1x16(const char *text, float *out, const float *in, dyn_biquad_process_x16_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        float d[d_buffer_size] __lsp_aligned64;
        dsp::fill_zero(d, d_buffer_size);

        void *ptr = NULL;
        dsp::biquad_x16_t *f = alloc_aligned<dsp::biquad_x16_t>(ptr, FTEST_BUF_SIZE+15, 64);
        for (size_t i=0; i<(FTEST_BUF_SIZE+15); ++i)
        {
            for (size_t j=0; j<16; ++j)
            {
                f[i].b0[j]  = bq_normal.b0;
                f[i].b1[j]  = bq_normal.b1;
                f[i].b2[j]  = bq_normal.b2;
                f[i].a1[j]  = bq_normal.a1;
                f[i].a2[j]  = bq_normal.a2;
            }
        }

        PTEST_LOOP(text,
            process(out, in, d, FTEST_BUF_SIZE, f);
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

        for (size_t i=0; i<2; ++i)
        {
            process_16x1("generic::dyn_biquad_process_x1 x16", out, in, generic::dyn_biquad_process_x1);
            IF_ARCH_X86(process_16x1("sse::dyn_biquad_process_x1 x16", out, in, sse::dyn_biquad_process_x1));
            IF_ARCH_X86(process_16x1("avx::dyn_biquad_process_x1 x16", out, in, avx::dyn_biquad_process_x1));
            IF_ARCH_X86(process_16x1("avx::dyn_biquad_process_x1_fma3 x16", out, in, avx::dyn_biquad_process_x1_fma3));
            IF_ARCH_ARM(process_16x1("neon_d32::dyn_biquad_process_x1 x16", out, in, neon_d32::dyn_biquad_process_x1));
            IF_ARCH_AARCH64(process_16x1("asimd::dyn_biquad_process_x1 x16", out, in, asimd::dyn_biquad_process_x1));
            if (i == 0)
                PTEST_SEPARATOR;

            process_8x2("generic::dyn_biquad_process_x2 x8", out, in, generic::dyn_biquad_process_x2);
            IF_ARCH_X86(process_8x2("sse::dyn_biquad_process_x2 x8", out, in, sse::dyn_biquad_process_x2));
            IF_ARCH_X86(process_8x2("avx::dyn_biquad_process_x2 x8", out, in, avx::dyn_biquad_process_x2));
            IF_ARCH_X86(process_8x2("avx::dyn_biquad_process_x2_fma3 x8", out, in, avx::dyn_biquad_process_x2_fma3));
            IF_ARCH_ARM(process_8x2("neon_d32::dyn_biquad_process_x2 x8", out, in, neon_d32::dyn_biquad_process_x2));
            IF_ARCH_AARCH64(process_8x2("asimd::dyn_biquad_process_x2 x8", out, in, asimd::dyn_biquad_process_x2));
            if (i == 0)
                PTEST_SEPARATOR;

            process_4x4("generic::dyn_biquad_process_x4 x4", out, in, generic::dyn_biquad_process_x4);
            IF_ARCH_X86(process_4x4("sse::dyn_biquad_process_x4 x4", out, in, sse::dyn_biquad_process_x4));
            IF_ARCH_X86(process_4x4("avx::dyn_biquad_process_x4 x4", out, in, avx::dyn_biquad_process_x4));
            IF_ARCH_X86(process_4x4("avx::dyn_biquad_process_x4_fma3 x4", out, in, avx::dyn_biquad_process_x4_fma3));
            IF_ARCH_ARM(process_4x4("neon_d32::dyn_biquad_process_x4 x4", out, in, neon_d32::dyn_biquad_process_x4));
            IF_ARCH_AARCH64(process_4x4("asimd::dyn_biquad_process_x4 x4", out, in, asimd::dyn_biquad_process_x4));
            if (i == 0)
                PTEST_SEPARATOR;

            process_2x8("generic::dyn_biquad_process_x8 x2", out, in, generic::dyn_biquad_process_x8);
            IF_ARCH_X86(process_2x8("sse::dyn_biquad_process_x8 x2", out, in, sse::dyn_biquad_process_x8));
            IF_ARCH_X86_64(process_2x8("sse3::x64_dyn_biquad_process_x8 x2", out, in, sse3::x64_dyn_biquad_process_x8));
            IF_ARCH_X86_64(process_2x8("avx::x64_dyn_biquad_process_x8 x2", out, in, avx::x64_dyn_biquad_process_x8));
            IF_ARCH_X86(process_2x8("avx::dyn_biquad_process_x8_fma3 x2", out, in, avx::dyn_biquad_process_x8_fma3));
            IF_ARCH_ARM(process_2x8("neon_d32::dyn_biquad_process_x8 x2", out, in, neon_d32::dyn_biquad_process_x8));
            IF_ARCH_AARCH64(process_2x8("asimd::dyn_biquad_process_x8 x2", out, in, asimd::dyn_biquad_process_x8));
            if (i == 0)
                PTEST_SEPARATOR;

            process_1x16("generic::dyn_biquad_process_x16 x1", out, in, generic::dyn_biquad_process_x16);
            IF_ARCH_X86(process_1x16("sse::dyn_biquad_process_x16 x1", out, in, sse::dyn_biquad_process_x16));
            IF_ARCH_X86_64(process_1x16("sse3::x64_dyn_biquad_process_x16 x1", out, in, sse3::x64_dyn_biquad_process_x16));
            IF_ARCH_X86(process_1x16("avx::dyn_biquad_process_x16_fma3 x1", out, in, avx::dyn_biquad_process_x16_fma3));
            IF_ARCH_X86_64(process_1x16("avx::x64_dyn_biquad_process_x16_fma3 x1", out, in, avx::x64_dyn_biquad_process_x16_fma3));
            IF_ARCH_X86(process_1x16("avx512::dyn_biquad_process_x16 x1", out, in, avx512::dyn_biquad_process_x16));
            IF_ARCH_ARM(process_1x16("neon_d32::dyn_biquad_process_x16 x1", out, in, neon_d32::dyn_biquad_process_x16));
            IF_ARCH_AARCH64(process_1x16("asimd::dyn_biquad_process_x16 x1", out, in, asimd::dyn_biquad_process_x16));
            PTEST_SEPARATOR2;
        }

        delete [] out;
        delete [] in;
    }

PTEST_END


