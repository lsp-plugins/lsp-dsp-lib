/*
 * types.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_VERSION_H_
#define LSP_PLUG_IN_DSP_VERSION_H_

#include <lsp-plug.in/common/types.h>

// Define version of headers
#define LSP_DSP_LIB_MAJOR           1
#define LSP_DSP_LIB_MINOR           0
#define LSP_DSP_LIB_MICRO           0

// Macro definitions
#define LSP_DSP_VEC2(v)     v, v
#define LSP_DSP_VEC4(v)     v, v, v, v
#define LSP_DSP_VEC8(v)     v, v, v, v, v, v, v, v
#define LSP_DSP_VEC16(v)    v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v
#define LSP_DSP_VEC32(v)    v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v


#endif /* LSP_PLUG_IN_DSP_VERSION_H_ */
