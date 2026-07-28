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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define FTEST_BUF_SIZE 0x200

namespace lsp
{
    namespace generic
    {
        void biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
        void biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
        void biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
        void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
        void biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }


        namespace avx
        {
            void biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void biquad_process_x1_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);

            void biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void biquad_process_x2_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);

            void biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void biquad_process_x4_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);

            void biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);

            void biquad_process_x16_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }

        namespace avx512
        {
            void biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }
    )

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void x64_biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }

        namespace avx
        {
            void x64_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void x64_biquad_process_x16_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void biquad_process_x4(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
            void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
            void biquad_process_x16(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);
        }
    )

    typedef void (* biquad_process_x1_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
    typedef void (* biquad_process_x2_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
    typedef void (* biquad_process_x4_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x4_t *f);
    typedef void (* biquad_process_x8_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
    typedef void (* biquad_process_x16_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x16_t *f);

    static dsp::biquad_x1_t bq_normal =
    {
        .b0 = 1.0f,
        .b1 = 2.0f,
        .b2 = 1.0f,
        .a1 = -2.0f,
        .a2 = -1.0f,
        .p0 = 0.0f,
        .p1 = 0.0f,
        .p2 = 0.0f,
    };

    constexpr size_t d_buffer_size = 2 * 16;
}

//-----------------------------------------------------------------------------
// Performance test for static biquad processing
PTEST_BEGIN("dsp.filters", static, 5, 1000)

    void process_16x1(const char *text, float *out, const float *in, biquad_process_x1_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;

        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        dsp::biquad_x1_t x1 __lsp_aligned64;
        float d[d_buffer_size] __lsp_aligned64;

        // Filers x1
        x1 = bq_normal;
        dsp::fill_zero(d, d_buffer_size);

        PTEST_LOOP(text,
            process(out, in,  &d[0],  FTEST_BUF_SIZE, &x1);
            process(out, out, &d[2],  FTEST_BUF_SIZE, &x1);
            process(out, out, &d[4],  FTEST_BUF_SIZE, &x1);
            process(out, out, &d[6],  FTEST_BUF_SIZE, &x1);
            process(out, out, &d[8],  FTEST_BUF_SIZE, &x1);
            process(out, out, &d[10], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[12], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[14], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[16], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[18], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[20], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[22], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[24], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[26], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[28], FTEST_BUF_SIZE, &x1);
            process(out, out, &d[30], FTEST_BUF_SIZE, &x1);
        );
    }

    void process_8x2(const char *text, float *out, const float *in, biquad_process_x2_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        dsp::biquad_x2_t x2 __lsp_aligned64;
        float d[d_buffer_size] __lsp_aligned64;

        // Filters x2
        for (size_t i=0; i<2; ++i)
        {
            x2.b0[i]        = bq_normal.b0;
            x2.b1[i]        = bq_normal.b1;
            x2.b2[i]        = bq_normal.b2;
            x2.a1[i]        = bq_normal.a1;
            x2.a2[i]        = bq_normal.a2;
        }
        dsp::fill_zero(d, d_buffer_size);

        PTEST_LOOP(text,
            process(out, in,  &d[0],  FTEST_BUF_SIZE, &x2);
            process(out, out, &d[4],  FTEST_BUF_SIZE, &x2);
            process(out, out, &d[8],  FTEST_BUF_SIZE, &x2);
            process(out, out, &d[12], FTEST_BUF_SIZE, &x2);
            process(out, out, &d[16], FTEST_BUF_SIZE, &x2);
            process(out, out, &d[20], FTEST_BUF_SIZE, &x2);
            process(out, out, &d[24], FTEST_BUF_SIZE, &x2);
            process(out, out, &d[28], FTEST_BUF_SIZE, &x2);
        );
    }

    void process_4x4(const char *text, float *out, const float *in, biquad_process_x4_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        dsp::biquad_x4_t x4 __lsp_aligned64;
        float d[d_buffer_size] __lsp_aligned64;

        // Filters x8
        for (size_t i=0; i<4; ++i)
        {
            x4.b0[i]        = bq_normal.b0;
            x4.b1[i]        = bq_normal.b1;
            x4.b2[i]        = bq_normal.b2;
            x4.a1[i]        = bq_normal.a1;
            x4.a2[i]        = bq_normal.a2;
        }
        dsp::fill_zero(d, d_buffer_size);

        PTEST_LOOP(text,
            process(out, in,  &d[0],  FTEST_BUF_SIZE, &x4);
            process(out, out, &d[8],  FTEST_BUF_SIZE, &x4);
            process(out, out, &d[16], FTEST_BUF_SIZE, &x4);
            process(out, out, &d[24], FTEST_BUF_SIZE, &x4);
        );
    }

    void process_2x8(const char *text, float *out, const float *in, biquad_process_x8_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        dsp::biquad_x8_t x8 __lsp_aligned64;
        float d[d_buffer_size] __lsp_aligned64;

        // Filters x8
        for (size_t i=0; i<8; ++i)
        {
            x8.b0[i]        = bq_normal.b0;
            x8.b1[i]        = bq_normal.b1;
            x8.b2[i]        = bq_normal.b2;
            x8.a1[i]        = bq_normal.a1;
            x8.a2[i]        = bq_normal.a2;
        }
        dsp::fill_zero(d, d_buffer_size);

        PTEST_LOOP(text,
            process(out, in,  &d[0],  FTEST_BUF_SIZE, &x8);
            process(out, out, &d[16], FTEST_BUF_SIZE, &x8);
        );
    }

    void process_1x16(const char *text, float *out, const float *in, biquad_process_x16_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(FTEST_BUF_SIZE));

        dsp::biquad_x16_t x16 __lsp_aligned64;
        float d[d_buffer_size] __lsp_aligned64;

        // Filters x16
        for (size_t i=0; i<16; ++i)
        {
            x16.b0[i]       = bq_normal.b0;
            x16.b1[i]       = bq_normal.b1;
            x16.b2[i]       = bq_normal.b2;
            x16.a1[i]       = bq_normal.a1;
            x16.a2[i]       = bq_normal.a2;
        }
        dsp::fill_zero(d, d_buffer_size);

        PTEST_LOOP(text,
            process(out, in,  &d[0],  FTEST_BUF_SIZE, &x16);
        );
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
            process_16x1("generic::biquad_process_x1 x16", out, in, generic::biquad_process_x1);
            IF_ARCH_X86(process_16x1("sse::biquad_process_x1 x16", out, in, sse::biquad_process_x1));
            IF_ARCH_X86(process_16x1("avx::biquad_process_x1 x16", out, in, avx::biquad_process_x1));
            IF_ARCH_X86(process_16x1("avx::biquad_process_x1_fma3 x16", out, in, avx::biquad_process_x1_fma3));
            IF_ARCH_ARM(process_16x1("neon_d32::biquad_process_x1 x16", out, in, neon_d32::biquad_process_x1));
            IF_ARCH_AARCH64(process_16x1("asimd::biquad_process_x1 x16", out, in, asimd::biquad_process_x1));
            if (i == 0)
                PTEST_SEPARATOR;

            process_8x2("generic::biquad_process_x2 x8", out, in, generic::biquad_process_x2);
            IF_ARCH_X86(process_8x2("sse::biquad_process_x2 x8", out, in, sse::biquad_process_x2));
            IF_ARCH_X86(process_8x2("avx::biquad_process_x2 x8", out, in, avx::biquad_process_x2));
            IF_ARCH_X86(process_8x2("avx::biquad_process_x2_fma3 x8", out, in, avx::biquad_process_x2_fma3));
            IF_ARCH_ARM(process_8x2("neon_d32::biquad_process_x2 x8", out, in, neon_d32::biquad_process_x2));
            IF_ARCH_AARCH64(process_8x2("asimd::biquad_process_x2 x8", out, in, asimd::biquad_process_x2));
            if (i == 0)
                PTEST_SEPARATOR;

            process_4x4("generic::biquad_process_x4 x4", out, in, generic::biquad_process_x4);
            IF_ARCH_X86(process_4x4("sse::biquad_process_x4 x4", out, in, sse::biquad_process_x4));
            IF_ARCH_X86(process_4x4("avx::biquad_process_x4 x4", out, in, avx::biquad_process_x4));
            IF_ARCH_X86(process_4x4("avx::biquad_process_x4_fma3 x4", out, in, avx::biquad_process_x4_fma3));
            IF_ARCH_ARM(process_4x4("neon_d32::biquad_process_x4 x4", out, in, neon_d32::biquad_process_x4));
            IF_ARCH_AARCH64(process_4x4("asimd::biquad_process_x4 x4", out, in, asimd::biquad_process_x4));
            if (i == 0)
                PTEST_SEPARATOR;

            process_2x8("generic::biquad_process_x8 x2", out, in, generic::biquad_process_x8);
            IF_ARCH_X86(process_2x8("sse::biquad_process_x8 x2", out, in, sse::biquad_process_x8));
            IF_ARCH_X86_64(process_2x8("sse3::x64_biquad_process_x8 x2", out, in, sse3::x64_biquad_process_x8));
            IF_ARCH_X86_64(process_2x8("avx::x64_biquad_process_x8 x2", out, in, avx::x64_biquad_process_x8));
            IF_ARCH_X86(process_2x8("avx::biquad_process_x8_fma3 x2", out, in, avx::biquad_process_x8_fma3));
            IF_ARCH_ARM(process_2x8("neon_d32::biquad_process_x8 x2", out, in, neon_d32::biquad_process_x8));
            IF_ARCH_AARCH64(process_2x8("asimd::biquad_process_x8 x2", out, in, asimd::biquad_process_x8));
            if (i == 0)
                PTEST_SEPARATOR;

            process_1x16("generic::biquad_process_x16 x1", out, in, generic::biquad_process_x16);
            IF_ARCH_X86(process_1x16("sse::biquad_process_x16 x1", out, in, sse::biquad_process_x16));
            IF_ARCH_X86_64(process_1x16("sse3::x64_biquad_process_x16 x1", out, in, sse3::x64_biquad_process_x16));
            IF_ARCH_X86(process_1x16("avx::biquad_process_x16_fma3 x1", out, in, avx::biquad_process_x16_fma3));
            IF_ARCH_X86(process_1x16("avx::x64_biquad_process_x16_fma3 x1", out, in, avx::x64_biquad_process_x16_fma3));
            IF_ARCH_X86(process_1x16("avx512::biquad_process_x16 x1", out, in, avx512::biquad_process_x16));
            IF_ARCH_ARM(process_1x16("neon_d32::biquad_process_x16 x1", out, in, neon_d32::biquad_process_x16));
            IF_ARCH_AARCH64(process_1x16("asimd::biquad_process_x16 x1", out, in, asimd::biquad_process_x16));
            PTEST_SEPARATOR2;
        }

        delete [] out;
        delete [] in;
    }

PTEST_END

