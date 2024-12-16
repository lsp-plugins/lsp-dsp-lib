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
        void    add_k2(float *dst, float k, size_t count);
        void    sub_k2(float *dst, float k, size_t count);
        void    rsub_k2(float *dst, float k, size_t count);
        void    mul_k2(float *dst, float k, size_t count);
        void    div_k2(float *dst, float k, size_t count);
        void    rdiv_k2(float *dst, float k, size_t count);
        void    mod_k2(float *dst, float k, size_t count);
        void    rmod_k2(float *dst, float k, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    add_k2(float *dst, float k, size_t count);
            void    sub_k2(float *dst, float k, size_t count);
            void    rsub_k2(float *dst, float k, size_t count);
            void    mul_k2(float *dst, float k, size_t count);
            void    div_k2(float *dst, float k, size_t count);
            void    rdiv_k2(float *dst, float k, size_t count);
        }

        namespace sse2
        {
            void    mod_k2(float *dst, float k, size_t count);
            void    rmod_k2(float *dst, float k, size_t count);
        }

        namespace avx
        {
            void    add_k2(float *dst, float k, size_t count);
            void    sub_k2(float *dst, float k, size_t count);
            void    rsub_k2(float *dst, float k, size_t count);
            void    mul_k2(float *dst, float k, size_t count);
            void    div_k2(float *dst, float k, size_t count);
            void    rdiv_k2(float *dst, float k, size_t count);
            void    mod_k2(float *dst, float k, size_t count);
            void    rmod_k2(float *dst, float k, size_t count);
            void    mod_k2_fma3(float *dst, float k, size_t count);
            void    rmod_k2_fma3(float *dst, float k, size_t count);
        }

        namespace avx2
        {
            void    add_k2(float *dst, float k, size_t count);
            void    sub_k2(float *dst, float k, size_t count);
            void    rsub_k2(float *dst, float k, size_t count);
            void    mul_k2(float *dst, float k, size_t count);
            void    div_k2(float *dst, float k, size_t count);
            void    rdiv_k2(float *dst, float k, size_t count);
            void    mod_k2(float *dst, float k, size_t count);
            void    rmod_k2(float *dst, float k, size_t count);
            void    mod_k2_fma3(float *dst, float k, size_t count);
            void    rmod_k2_fma3(float *dst, float k, size_t count);
        }

        namespace avx512
        {
            void    add_k2(float *dst, float k, size_t count);
            void    sub_k2(float *dst, float k, size_t count);
            void    rsub_k2(float *dst, float k, size_t count);
            void    mul_k2(float *dst, float k, size_t count);
            void    div_k2(float *dst, float k, size_t count);
            void    rdiv_k2(float *dst, float k, size_t count);
            void    mod_k2(float *dst, float k, size_t count);
            void    rmod_k2(float *dst, float k, size_t count);
        }

    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    add_k2(float *dst, float k, size_t count);
            void    sub_k2(float *dst, float k, size_t count);
            void    rsub_k2(float *dst, float k, size_t count);
            void    mul_k2(float *dst, float k, size_t count);
            void    div_k2(float *dst, float k, size_t count);
            void    rdiv_k2(float *dst, float k, size_t count);
            void    mod_k2(float *dst, float k, size_t count);
            void    rmod_k2(float *dst, float k, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    add_k2(float *dst, float k, size_t count);
            void    sub_k2(float *dst, float k, size_t count);
            void    rsub_k2(float *dst, float k, size_t count);
            void    mul_k2(float *dst, float k, size_t count);
            void    div_k2(float *dst, float k, size_t count);
            void    rdiv_k2(float *dst, float k, size_t count);
            void    mod_k2(float *dst, float k, size_t count);
            void    rmod_k2(float *dst, float k, size_t count);
        }
    )

    typedef void (* op_k2_t)(float *dst, float k, size_t count);
}

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", op_k2, 5, 1000)

    void call(const char *label, float *dst, size_t count, op_k2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, 1.001f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 2, 64);
        float *backup   = &dst[buf_size];

        for (size_t i=0; i < buf_size; ++i)
            dst[i]          = randf(-1.0f, 1.0f);
        dsp::copy(backup, dst, buf_size);

        #define CALL(func) \
            dsp::copy(dst, backup, buf_size); \
            call(#func, dst, count, func);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::add_k2);
            IF_ARCH_X86(CALL(sse::add_k2));
            IF_ARCH_X86(CALL(avx::add_k2));
            IF_ARCH_X86(CALL(avx2::add_k2));
            IF_ARCH_X86(CALL(avx512::add_k2));
            IF_ARCH_ARM(CALL(neon_d32::add_k2));
            IF_ARCH_AARCH64(CALL(asimd::add_k2));
            PTEST_SEPARATOR;

            CALL(generic::sub_k2);
            IF_ARCH_X86(CALL(sse::sub_k2));
            IF_ARCH_X86(CALL(avx::sub_k2));
            IF_ARCH_X86(CALL(avx2::sub_k2));
            IF_ARCH_X86(CALL(avx512::sub_k2));
            IF_ARCH_ARM(CALL(neon_d32::sub_k2));
            IF_ARCH_AARCH64(CALL(asimd::sub_k2));
            PTEST_SEPARATOR;

            CALL(generic::rsub_k2);
            IF_ARCH_X86(CALL(sse::rsub_k2));
            IF_ARCH_X86(CALL(avx::rsub_k2));
            IF_ARCH_X86(CALL(avx2::rsub_k2));
            IF_ARCH_X86(CALL(avx512::rsub_k2));
            IF_ARCH_ARM(CALL(neon_d32::rsub_k2));
            IF_ARCH_AARCH64(CALL(asimd::rsub_k2));
            PTEST_SEPARATOR;

            CALL(generic::mul_k2);
            IF_ARCH_X86(CALL(sse::mul_k2));
            IF_ARCH_X86(CALL(avx::mul_k2));
            IF_ARCH_X86(CALL(avx2::mul_k2));
            IF_ARCH_X86(CALL(avx512::mul_k2));
            IF_ARCH_ARM(CALL(neon_d32::mul_k2));
            IF_ARCH_AARCH64(CALL(asimd::mul_k2));
            PTEST_SEPARATOR;

            CALL(generic::div_k2);
            IF_ARCH_X86(CALL(sse::div_k2));
            IF_ARCH_X86(CALL(avx::div_k2));
            IF_ARCH_X86(CALL(avx2::div_k2));
            IF_ARCH_X86(CALL(avx512::div_k2));
            IF_ARCH_ARM(CALL(neon_d32::div_k2));
            IF_ARCH_AARCH64(CALL(asimd::div_k2));
            PTEST_SEPARATOR;

            CALL(generic::rdiv_k2);
            IF_ARCH_X86(CALL(sse::rdiv_k2));
            IF_ARCH_X86(CALL(avx::rdiv_k2));
            IF_ARCH_X86(CALL(avx2::rdiv_k2));
            IF_ARCH_X86(CALL(avx512::rdiv_k2));
            IF_ARCH_ARM(CALL(neon_d32::rdiv_k2));
            IF_ARCH_AARCH64(CALL(asimd::rdiv_k2));
            PTEST_SEPARATOR;

            CALL(generic::mod_k2);
            IF_ARCH_X86(CALL(sse2::mod_k2));
            IF_ARCH_X86(CALL(avx::mod_k2));
            IF_ARCH_X86(CALL(avx::mod_k2_fma3));
            IF_ARCH_X86(CALL(avx2::mod_k2));
            IF_ARCH_X86(CALL(avx2::mod_k2_fma3))
            IF_ARCH_X86(CALL(avx512::mod_k2));
            IF_ARCH_ARM(CALL(neon_d32::mod_k2));
            IF_ARCH_AARCH64(CALL(asimd::mod_k2));
            PTEST_SEPARATOR;

            CALL(generic::rmod_k2);
            IF_ARCH_X86(CALL(sse2::rmod_k2));
            IF_ARCH_X86(CALL(avx::rmod_k2));
            IF_ARCH_X86(CALL(avx::rmod_k2_fma3));
            IF_ARCH_X86(CALL(avx2::rmod_k2));
            IF_ARCH_X86(CALL(avx2::rmod_k2_fma3));
            IF_ARCH_X86(CALL(avx512::rmod_k2));
            IF_ARCH_ARM(CALL(neon_d32::rmod_k2));
            IF_ARCH_AARCH64(CALL(asimd::rmod_k2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END
