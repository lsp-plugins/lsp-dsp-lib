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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

#define BIQUAD_X1_FLOATS    (sizeof(dsp::biquad_x1_t) / sizeof(float))
#define BIQUAD_X2_FLOATS    (sizeof(dsp::biquad_x2_t) / sizeof(float))
#define BIQUAD_X4_FLOATS    (sizeof(dsp::biquad_x4_t) / sizeof(float))
#define BIQUAD_X8_FLOATS    (sizeof(dsp::biquad_x8_t) / sizeof(float))
#define CASCADE_FLOATS      (sizeof(dsp::f_cascade_t) / sizeof(float))

namespace lsp
{
    namespace generic
    {
        void bilinear_transform_x1(dsp::biquad_x1_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x2(dsp::biquad_x2_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x4(dsp::biquad_x4_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
        void bilinear_transform_x8(dsp::biquad_x8_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void bilinear_transform_x1(dsp::biquad_x1_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x2(dsp::biquad_x2_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x4(dsp::biquad_x4_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x8(dsp::biquad_x8_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
        }

        IF_ARCH_X86_64(
            namespace sse3
            {
                void x64_bilinear_transform_x8(dsp::biquad_x8_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            }
        )

        namespace avx
        {
            void bilinear_transform_x1(dsp::biquad_x1_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x2(dsp::biquad_x2_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x4(dsp::biquad_x4_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void x64_bilinear_transform_x8(dsp::biquad_x8_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void bilinear_transform_x1(dsp::biquad_x1_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x2(dsp::biquad_x2_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x4(dsp::biquad_x4_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x8(dsp::biquad_x8_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void bilinear_transform_x1(dsp::biquad_x1_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x2(dsp::biquad_x2_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x4(dsp::biquad_x4_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
            void bilinear_transform_x8(dsp::biquad_x8_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
        }
    )

    typedef void (* bilinear_transform_x1_t)(dsp::biquad_x1_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
    typedef void (* bilinear_transform_x2_t)(dsp::biquad_x2_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
    typedef void (* bilinear_transform_x4_t)(dsp::biquad_x4_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
    typedef void (* bilinear_transform_x8_t)(dsp::biquad_x8_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count);
}

UTEST_BEGIN("dsp.filters", bt)

    void call(const char *text, bilinear_transform_x1_t f1, bilinear_transform_x1_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 0x1ff)
        {
            size_t filters = count;
            size_t cascades = filters;
            printf("Testing %s bilinear transformation, filters=%d, cascades=%d\n", text, int(filters), int(cascades));

            FloatBuffer src(CASCADE_FLOATS * cascades, 64, true);
            FloatBuffer dst1(BIQUAD_X1_FLOATS * filters, 64, true);
            FloatBuffer dst2(dst1);

            dsp::f_cascade_t *bc = src.data<dsp::f_cascade_t>();
            for (size_t i=0; i<cascades; ++i)
            {
                float kt = i * 0.1;
                float kb = i * 0.05;
                bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
                bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
            }

            f1(dst1.data<dsp::biquad_x1_t>(), bc, 1.5f, filters);
            f2(dst2.data<dsp::biquad_x1_t>(), bc, 1.5f, filters);

            UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
            UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
            UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
            if (!dst1.equals_relative(dst2, 1e-4f))
            {
                src.dump("src");
                src.dump_hex("srch");
                dst1.dump("dst1");
                dst2.dump("dst2");
                UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
            }
        }
    }

    void call(const char *text, bilinear_transform_x2_t f1, bilinear_transform_x2_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 0x1ff)
        {
            size_t filters = count + 1;
            size_t cascades = filters * 2;
            printf("Testing %s bilinear transformation, filters=%d, cascades=%d\n", text, int(filters), int(cascades));

            FloatBuffer src(CASCADE_FLOATS * cascades, 64, true);
            FloatBuffer dst1(BIQUAD_X2_FLOATS * filters, 64, true);
            FloatBuffer dst2(dst1);

            dsp::f_cascade_t *bc = src.data<dsp::f_cascade_t>();
            for (size_t i=0; i<cascades; ++i)
            {
                float kt = i * 0.1;
                float kb = i * 0.05;
                bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
                bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
            }

            f1(dst1.data<dsp::biquad_x2_t>(), bc, 1.5f, filters);
            f2(dst2.data<dsp::biquad_x2_t>(), bc, 1.5f, filters);

            UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
            UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
            UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
            if (!dst1.equals_relative(dst2, 1e-4f))
            {
                src.dump("src");
                src.dump_hex("srch");
                dst1.dump("dst1");
                dst2.dump("dst2");
                UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
            }
        }
    }

    void call(const char *text, bilinear_transform_x4_t f1, bilinear_transform_x4_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 0x1ff)
        {
            size_t filters = count + 3;
            size_t cascades = filters * 4;
            printf("Testing %s bilinear transformation, filters=%d, cascades=%d\n", text, int(filters), int(cascades));

            FloatBuffer src(CASCADE_FLOATS * cascades, 64, true);
            FloatBuffer dst1(BIQUAD_X4_FLOATS * filters, 64, true);
            FloatBuffer dst2(dst1);

            dsp::f_cascade_t *bc = src.data<dsp::f_cascade_t>();
            for (size_t i=0; i<cascades; ++i)
            {
                float kt = i * 0.1;
                float kb = i * 0.05;
                bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
                bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
            }

            f1(dst1.data<dsp::biquad_x4_t>(), bc, 1.5f, filters);
            f2(dst2.data<dsp::biquad_x4_t>(), bc, 1.5f, filters);

            UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
            UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
            UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
            if (!dst1.equals_relative(dst2, 1e-4f))
            {
                src.dump("src");
                src.dump_hex("srch");
                dst1.dump("dst1");
                dst2.dump("dst2");
                UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
            }
        }
    }

    void call(const char *text, bilinear_transform_x8_t f1, bilinear_transform_x8_t f2)
    {
        if (!UTEST_SUPPORTED(f1))
            return;
        if (!UTEST_SUPPORTED(f2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 16, 17, 0x1ff)
        {
            size_t filters = count + 7;
            size_t cascades = filters * 8;
            printf("Testing %s bilinear transformation, filters=%d, cascades=%d\n", text, int(filters), int(cascades));

            FloatBuffer src(CASCADE_FLOATS * cascades, 64, true);
            FloatBuffer dst1(BIQUAD_X8_FLOATS * filters, 64, true);
            FloatBuffer dst2(dst1);

            dsp::f_cascade_t *bc = src.data<dsp::f_cascade_t>();
            for (size_t i=0; i<cascades; ++i)
            {
                float kt = i * 0.1;
                float kb = i * 0.05;
                bc[i].t[0] = 1 + kt; bc[i].t[1] = 2 + kt;  bc[i].t[2] = 1 - kt; bc[i].t[3] = 0;
                bc[i].b[0] = 1 + kb; bc[i].b[1] = -2 + kb; bc[i].b[2] = 1 - kb; bc[i].b[3] = 0;
            }

            f1(dst1.data<dsp::biquad_x8_t>(), bc, 1.5f, filters);
            f2(dst2.data<dsp::biquad_x8_t>(), bc, 1.5f, filters);

            UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
            UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
            UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
            if (!dst1.equals_relative(dst2, 1e-4f))
            {
                src.dump("src");
                src.dump_hex("srch");
                dst1.dump("dst1");
                dst2.dump("dst2");
                UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func) \
            call(#func, generic, func)

        IF_ARCH_X86(CALL(generic::bilinear_transform_x1, sse::bilinear_transform_x1));
        IF_ARCH_X86(CALL(generic::bilinear_transform_x1, avx::bilinear_transform_x1));
        IF_ARCH_ARM(CALL(generic::bilinear_transform_x1, neon_d32::bilinear_transform_x1));
        IF_ARCH_AARCH64(CALL(generic::bilinear_transform_x1, asimd::bilinear_transform_x1));

        IF_ARCH_X86(CALL(generic::bilinear_transform_x2, sse::bilinear_transform_x2));
        IF_ARCH_X86(CALL(generic::bilinear_transform_x2, avx::bilinear_transform_x2));
        IF_ARCH_ARM(CALL(generic::bilinear_transform_x2, neon_d32::bilinear_transform_x2));
        IF_ARCH_AARCH64(CALL(generic::bilinear_transform_x2, asimd::bilinear_transform_x2));

        IF_ARCH_X86(CALL(generic::bilinear_transform_x4, sse::bilinear_transform_x4));
        IF_ARCH_X86(CALL(generic::bilinear_transform_x4, avx::bilinear_transform_x4));
        IF_ARCH_ARM(CALL(generic::bilinear_transform_x4, neon_d32::bilinear_transform_x4));
        IF_ARCH_AARCH64(CALL(generic::bilinear_transform_x4, asimd::bilinear_transform_x4));

        IF_ARCH_X86(CALL(generic::bilinear_transform_x8, sse::bilinear_transform_x8));
        IF_ARCH_X86_64(CALL(generic::bilinear_transform_x8, sse3::x64_bilinear_transform_x8));
        IF_ARCH_X86(CALL(generic::bilinear_transform_x8, avx::x64_bilinear_transform_x8));
        IF_ARCH_ARM(CALL(generic::bilinear_transform_x8, neon_d32::bilinear_transform_x8));
        IF_ARCH_AARCH64(CALL(generic::bilinear_transform_x8, asimd::bilinear_transform_x8));
    }

UTEST_END;
