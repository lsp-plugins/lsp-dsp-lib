/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 12 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_PMATH_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_PMATH_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

#include <private/dsp/arch/x86/avx2/pmath/cos.h>
#include <private/dsp/arch/x86/avx2/pmath/lanczos.h>
#include <private/dsp/arch/x86/avx2/pmath/op_kx.h>
#include <private/dsp/arch/x86/avx2/pmath/fmop_kx.h>
#include <private/dsp/arch/x86/avx2/pmath/exp.h>
#include <private/dsp/arch/x86/avx2/pmath/log.h>
#include <private/dsp/arch/x86/avx2/pmath/pow.h>
#include <private/dsp/arch/x86/avx2/pmath/sin.h>

#endif /* PRIVATE_DSP_ARCH_X86_AVX2_PMATH_H_ */
