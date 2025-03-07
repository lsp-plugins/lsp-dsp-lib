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
        void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        }

        namespace sse2
        {
            void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        }

        namespace avx
        {
            void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);

            void    fmadd_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmod_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrmod_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        }

        namespace avx2
        {
            void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);

            void    fmadd_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmod_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrmod_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        }

        namespace avx512
        {
            void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
            void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        }
    )

    typedef void (* fmop_k4_t)(float *dst, const float *src1, const float *src2, float k, size_t count);
}

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", fmop_k4, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, fmop_k4_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src1, src2, 0.5f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 6, 64);
        float *src1     = &dst[buf_size];
        float *src2     = &src1[buf_size];
        float *backup   = &src2[buf_size];

        randomize_sign(dst, buf_size*3);
        dsp::copy(backup, dst, buf_size*3);

        #define CALL(func) \
            dsp::copy(dst, backup, buf_size*3); \
            call(#func, dst, src1, src2, count, func);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::fmadd_k4);
            IF_ARCH_X86(CALL(sse::fmadd_k4));
            IF_ARCH_X86(CALL(avx::fmadd_k4));
            IF_ARCH_X86(CALL(avx::fmadd_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmadd_k4));
            IF_ARCH_X86(CALL(avx2::fmadd_k4_fma3));
            IF_ARCH_X86(CALL(avx512::fmadd_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmadd_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmadd_k4));
            PTEST_SEPARATOR;

            CALL(generic::fmsub_k4);
            IF_ARCH_X86(CALL(sse::fmsub_k4));
            IF_ARCH_X86(CALL(avx::fmsub_k4));
            IF_ARCH_X86(CALL(avx::fmsub_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmsub_k4));
            IF_ARCH_X86(CALL(avx2::fmsub_k4_fma3));
            IF_ARCH_X86(CALL(avx512::fmsub_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmsub_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmsub_k4));
            PTEST_SEPARATOR;

            CALL(generic::fmrsub_k4);
            IF_ARCH_X86(CALL(sse::fmrsub_k4));
            IF_ARCH_X86(CALL(avx::fmrsub_k4));
            IF_ARCH_X86(CALL(avx::fmrsub_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmrsub_k4));
            IF_ARCH_X86(CALL(avx2::fmrsub_k4_fma3));
            IF_ARCH_X86(CALL(avx512::fmrsub_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmrsub_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmrsub_k4));
            PTEST_SEPARATOR;

            CALL(generic::fmmul_k4);
            IF_ARCH_X86(CALL(sse::fmmul_k4));
            IF_ARCH_X86(CALL(avx::fmmul_k4));
            IF_ARCH_X86(CALL(avx2::fmmul_k4));
            IF_ARCH_X86(CALL(avx512::fmmul_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmmul_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmmul_k4));
            PTEST_SEPARATOR;

            CALL(generic::fmdiv_k4);
            IF_ARCH_X86(CALL(sse::fmdiv_k4));
            IF_ARCH_X86(CALL(avx::fmdiv_k4));
            IF_ARCH_X86(CALL(avx2::fmdiv_k4));
            IF_ARCH_X86(CALL(avx512::fmdiv_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmdiv_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmdiv_k4));
            PTEST_SEPARATOR;

            CALL(generic::fmrdiv_k4);
            IF_ARCH_X86(CALL(sse::fmrdiv_k4));
            IF_ARCH_X86(CALL(avx::fmrdiv_k4));
            IF_ARCH_X86(CALL(avx2::fmrdiv_k4));
            IF_ARCH_X86(CALL(avx512::fmrdiv_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmrdiv_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmrdiv_k4));
            PTEST_SEPARATOR;

            CALL(generic::fmmod_k4);
            IF_ARCH_X86(CALL(sse2::fmmod_k4));
            IF_ARCH_X86(CALL(avx::fmmod_k4));
            IF_ARCH_X86(CALL(avx::fmmod_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmmod_k4));
            IF_ARCH_X86(CALL(avx2::fmmod_k4_fma3));
            IF_ARCH_X86(CALL(avx512::fmmod_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmmod_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmmod_k4));
            PTEST_SEPARATOR;

            CALL(generic::fmrmod_k4);
            IF_ARCH_X86(CALL(sse2::fmrmod_k4));
            IF_ARCH_X86(CALL(avx::fmrmod_k4));
            IF_ARCH_X86(CALL(avx::fmrmod_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmrmod_k4));
            IF_ARCH_X86(CALL(avx2::fmrmod_k4_fma3));
            IF_ARCH_X86(CALL(avx512::fmrmod_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmrmod_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmrmod_k4));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


