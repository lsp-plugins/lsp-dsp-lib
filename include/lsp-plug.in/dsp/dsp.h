/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_DSP_DSP_H_
#define LSP_PLUG_IN_DSP_DSP_H_

#include <lsp-plug.in/dsp/version.h>

#ifndef LSP_DSP_LIB_SYMBOL
    #ifdef LSP_DSP_LIB_USE_CXX_IFACE
        #define LSP_DSP_LIB_SYMBOL(ret, name, ...) \
            namespace lsp { \
                namespace dsp { \
                    LSP_DSP_LIB_PUBLIC \
                    extern ret (* name)(__VA_ARGS__); \
                } \
            }
    #else
        #define LSP_DSP_LIB_SYMBOL(ret, name, ...) \
            LSP_DSP_LIB_PUBLIC \
            extern ret (* LSP_DSP_LIB_MANGLE(name))(__VA_ARGS__);
    #endif /* LSP_DSP_LIB_USE_CXX_IFACE */
#endif /* LSP_DSP_LIB_SYMBOL */

// Include all partial definitions
#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/3dmath.h>
#include <lsp-plug.in/dsp/common/bitmap.h>
#include <lsp-plug.in/dsp/common/coding.h>
#include <lsp-plug.in/dsp/common/complex.h>
#include <lsp-plug.in/dsp/common/context.h>
#include <lsp-plug.in/dsp/common/convolution.h>
#include <lsp-plug.in/dsp/common/correlation.h>
#include <lsp-plug.in/dsp/common/copy.h>
#include <lsp-plug.in/dsp/common/dynamics.h>
#include <lsp-plug.in/dsp/common/fastconv.h>
#include <lsp-plug.in/dsp/common/fft.h>
#include <lsp-plug.in/dsp/common/filters.h>
#include <lsp-plug.in/dsp/common/float.h>
#include <lsp-plug.in/dsp/common/graphics.h>
#include <lsp-plug.in/dsp/common/hmath.h>
#include <lsp-plug.in/dsp/common/mix.h>
#include <lsp-plug.in/dsp/common/pan.h>
#include <lsp-plug.in/dsp/common/msmatrix.h>
#include <lsp-plug.in/dsp/common/pcomplex.h>
#include <lsp-plug.in/dsp/common/pmath.h>
#include <lsp-plug.in/dsp/common/resampling.h>
#include <lsp-plug.in/dsp/common/search.h>
#include <lsp-plug.in/dsp/common/smath.h>
#include <lsp-plug.in/dsp/common/interpolation.h>

#endif /* LSP_PLUG_IN_DSP_DSP_H_ */
