/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 нояб. 2022 г.
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

#ifndef INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_PMATH_H_
#define INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_PMATH_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

#include <private/dsp/arch/x86/avx/pmath/abs_vv.h>
#include <private/dsp/arch/x86/avx/pmath/fmop_kx.h>
#include <private/dsp/arch/x86/avx/pmath/fmop_vv.h>
#include <private/dsp/arch/x86/avx/pmath/lramp.h>
#include <private/dsp/arch/x86/avx/pmath/minmax.h>
#include <private/dsp/arch/x86/avx/pmath/normalize.h>
#include <private/dsp/arch/x86/avx/pmath/op_kx.h>
#include <private/dsp/arch/x86/avx/pmath/op_vv.h>
#include <private/dsp/arch/x86/avx/pmath/sqr.h>
#include <private/dsp/arch/x86/avx/pmath/ssqrt.h>

#endif /* INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_PMATH_H_ */
