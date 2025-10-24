/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 14 мая 2020 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * ${project_name} is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LSP_PLUG_IN_DSP_VERSION_H_
#define LSP_PLUG_IN_DSP_VERSION_H_

// Define version of headers
#define LSP_DSP_LIB_MAJOR           1
#define LSP_DSP_LIB_MINOR           0
#define LSP_DSP_LIB_MICRO           32

#if defined(__WINDOWS__) || defined(__WIN32__) || defined(__WIN64__) || defined(_WIN64) || defined(_WIN32) || defined(__WINNT) || defined(__WINNT__)
    #define LSP_DSP_LIB_EXPORT_MODIFIER     __declspec(dllexport)
    #define LSP_DSP_LIB_IMPORT_MODIFIER     __declspec(dllimport)
#else
    #define LSP_DSP_LIB_EXPORT_MODIFIER     __attribute__((visibility("default")))
    #define LSP_DSP_LIB_IMPORT_MODIFIER
#endif /* __WINDOWS__ */

#if defined(LSP_DSP_LIB_PUBLISHER)
    #define LSP_DSP_LIB_PUBLIC          LSP_DSP_LIB_EXPORT_MODIFIER
#elif defined(LSP_DSP_LIB_BUILTIN) || defined(LSP_IDE_DEBUG)
    #define LSP_DSP_LIB_PUBLIC
#else
    #define LSP_DSP_LIB_PUBLIC          LSP_DSP_LIB_IMPORT_MODIFIER
#endif

#define LSP_DSP_LIB_MANGLE(name)    lsp_dsp_ ## name

// Determine what interface to use (C or C++)
#if defined(LSP_DSP_LIB_USE_C_IFACE)
    #ifdef LSP_DSP_LIB_USE_CXX_IFACE
        #undef LSP_DSP_LIB_USE_CXX_IFACE
    #endif /* LSP_DSP_LIB_USE_CXX_IFACE */
#elif defined(__cplusplus) && !defined(LSP_DSP_LIB_USE_CXX_IFACE)
    #define LSP_DSP_LIB_USE_CXX_IFACE
#endif /* LSP_DSP_LIB_USE_C_IFACE */

#ifdef LSP_DSP_LIB_USE_CXX_IFACE
    #define LSP_DSP_LIB_TYPE(name)      name
    #define LSP_DSP_LIB_BEGIN_NAMESPACE namespace lsp { namespace dsp {
    #define LSP_DSP_LIB_END_NAMESPACE   } }
#else
    #define LSP_DSP_LIB_TYPE(name)      LSP_DSP_LIB_MANGLE(name)
    #define LSP_DSP_LIB_BEGIN_NAMESPACE
    #define LSP_DSP_LIB_END_NAMESPACE
#endif

#endif /* LSP_PLUG_IN_DSP_VERSION_H_ */
