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

#ifndef LSP_PLUG_IN_DSP_COMMON_CONTEXT_H_
#define LSP_PLUG_IN_DSP_COMMON_CONTEXT_H_

#include <lsp-plug.in/dsp/common/types.h>

LSP_DSP_LIB_BEGIN_NAMESPACE

#pragma pack(push, 1)
/**
 * DSP context to store and restore machine state
 */
typedef struct LSP_DSP_LIB_TYPE(context_t)
{
    uint32_t        top;
    uint32_t        data[15];
} LSP_DSP_LIB_TYPE(context_t);
#pragma pack(pop)

typedef struct LSP_DSP_LIB_TYPE(info_t)
{
    const char     *arch;       /* Architecture information */
    const char     *cpu;        /* CPU information */
    const char     *model;      /* CPU model information */
    const char     *features;   /* CPU features */
} LSP_DSP_LIB_TYPE(info_t);

// Start and finish types
typedef void (* LSP_DSP_LIB_TYPE(start_t))(LSP_DSP_LIB_TYPE(context_t) *ctx);
typedef void (* LSP_DSP_LIB_TYPE(finish_t))(LSP_DSP_LIB_TYPE(context_t) *ctx);

LSP_DSP_LIB_END_NAMESPACE

#ifdef LSP_DSP_LIB_USE_CXX_IFACE
namespace lsp
{
    namespace dsp
    {
        /** Initialize DSP
         *
         */
        LSP_DSP_LIB_PUBLIC
        void init();
    }
}
#else
    /** Initialize DSP
     *
     */
    LSP_DSP_LIB_PUBLIC
    void LSP_DSP_LIB_MANGLE(init());
#endif /* LSP_DSP_LIB_USE_CXX_IFACE */


/** Start DSP processing, save machine context
 *
 * @param ctx structure to save context
 */
LSP_DSP_LIB_SYMBOL(void, start, LSP_DSP_LIB_TYPE(context_t) *ctx);

/** Finish DSP processing, restore machine context
 *
 * @param ctx structure to restore context
 */
LSP_DSP_LIB_SYMBOL(void, finish, LSP_DSP_LIB_TYPE(context_t) *ctx);

/**
 * Get DSP information, returns pointer to dsp::info_t structure
 * that can be freed by free()
 * @return pointer to dsp::info_t structure
 */
LSP_DSP_LIB_SYMBOL(LSP_DSP_LIB_TYPE(info_t) *, info, );

#endif /* LSP_PLUG_IN_DSP_COMMON_CONTEXT_H_ */
