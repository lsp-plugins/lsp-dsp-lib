/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

#define TOLERANCE 1e-2

namespace lsp
{
    namespace generic
    {
        void axis_apply_lin1(float *x, const float *v, float zero, float norm_x, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void axis_apply_lin1(float *x, const float *v, float zero, float norm_x, size_t count);
        }

        namespace avx
        {
            void axis_apply_lin1(float *x, const float *v, float zero, float norm_x, size_t count);
            void axis_apply_lin1_fma3(float *x, const float *v, float zero, float norm_x, size_t count);
        }

        namespace avx512
        {
            void axis_apply_lin1(float *x, const float *v, float zero, float norm_x, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void axis_apply_lin1(float *x, const float *v, float zero, float norm_x, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void axis_apply_lin1(float *x, const float *v, float zero, float norm_x, size_t count);
        }
    )

    typedef void (* axis_apply_lin1_t)(float *x, const float *v, float zero, float norm_x, size_t count);
}

UTEST_BEGIN("dsp.graphics", axis_lin1)
    void call(const char *label, size_t align, axis_apply_lin1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        for (size_t mask=0; mask <= 0x03; ++mask)
        {
            UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x1ff)
            {
                FloatBuffer v(count, align, mask & 0x01);
                FloatBuffer x1(count, align, mask & 0x02);
                FloatBuffer x2(x1);

                for (size_t i=0; i<count; ++i)
                    v[i] += 0.001f;

                printf("Tesing %s count=%d mask=0x%x\n", label, int(count), int(mask));

                generic::axis_apply_lin1(x1, v, 0.1f, 0.5f, count);
                func(x2, v, 0.1f, 0.5f, count);

                UTEST_ASSERT_MSG(v.valid(), "v corrupted");
                UTEST_ASSERT_MSG(x1.valid(), "x1 corrupted");
                UTEST_ASSERT_MSG(x2.valid(), "x2 corrupted");

                // Compare buffers
                if (!x1.equals_relative(x2, TOLERANCE))
                {
                    v.dump("v ");
                    x1.dump("x1");
                    x2.dump("x2");
                    printf("x1=%.6f vs x2=%.6f\n", x1.get_diff(), x2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        IF_ARCH_X86(CALL(sse::axis_apply_lin1, 16));
        IF_ARCH_X86(CALL(avx::axis_apply_lin1, 32));
        IF_ARCH_X86(CALL(avx::axis_apply_lin1_fma3, 32));
        IF_ARCH_X86(CALL(avx512::axis_apply_lin1, 64));

        IF_ARCH_ARM(CALL(neon_d32::axis_apply_lin1, 16));
        IF_ARCH_AARCH64(CALL(asimd::axis_apply_lin1, 16));
    }

UTEST_END;
