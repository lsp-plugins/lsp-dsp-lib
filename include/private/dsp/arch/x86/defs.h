/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 авг. 2022 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_DEFS_H_
#define PRIVATE_DSP_ARCH_X86_DEFS_H_

#ifndef PRIVATE_DSP_ARCH_X86_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_IMPL */


#include <lsp-plug.in/common/types.h>

#ifdef ARCH_32BIT
    #ifdef COMPILER_CLANG
        #define X86_GREG        "m"
        #define X86_PGREG       "+m"
    #else
        #define X86_GREG        "g"
        #define X86_PGREG       "+g"
    #endif /* COMPILER_CLANG */
#else
    #define X86_GREG        "r"
    #define X86_PGREG       "+r"
#endif /* ARCH_32BIT */


#endif /* PRIVATE_DSP_ARCH_X86_DEFS_H_ */
