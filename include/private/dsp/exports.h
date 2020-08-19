/*
 * exports.h
 *
 *  Created on: 19 авг. 2020 г.
 *      Author: sadko
 */

#ifndef PRIVATE_DSP_EXPORTS_H_
#define PRIVATE_DSP_EXPORTS_H_

#include <lsp-plug.in/dsp/version.h>

#define LSP_DSP_LIB_SYMBOL(ret, name, ...) \
    namespace lsp { \
        namespace dsp { \
            LSP_DSP_LIB_CPPIMPORT ret (* name)(__VA_ARGS__); \
            LSP_DSP_LIB_CIMPORT   ret (* LSP_DSP_LIB_MANGLE(name))(__VA_ARGS__); \
        } \
    }

#endif /* PRIVATE_DSP_EXPORTS_H_ */
