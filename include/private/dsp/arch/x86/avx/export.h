/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 авг. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_EXPORT_H_
#define PRIVATE_DSP_ARCH_X86_AVX_EXPORT_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

#define EXPORT2(function, export) \
{ \
    dsp::function                       = avx::export; \
    dsp::LSP_DSP_LIB_MANGLE(function)   = avx::export; \
    TEST_EXPORT(avx::export); \
}
#define EXPORT1(function)                       EXPORT2(function, function)

#define EXPORT2_X64(function, export)           IF_ARCH_X86_64(EXPORT2(function, export));
#define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(avx::function))

#define CEXPORT2(cond, function, export)    \
IF_ARCH_X86( \
        TEST_EXPORT(avx::export); \
        if (cond) \
            dsp::function = avx::export; \
    );

#define CEXPORT1(cond, export)    \
IF_ARCH_X86( \
        TEST_EXPORT(avx::export); \
        if (cond) \
            dsp::export = avx::export; \
    );

#define CEXPORT2_X64(cond, function, export)    \
    IF_ARCH_X86_64( \
            TEST_EXPORT(avx::export); \
            if (cond) \
                dsp::function = avx::export; \
        );

#define CEXPORT1_X64(cond, export)    \
    IF_ARCH_X86_64( \
            TEST_EXPORT(avx::export); \
            if (cond) \
                dsp::export = avx::export; \
        );



#endif /* PRIVATE_DSP_ARCH_X86_AVX_EXPORT_H_ */
