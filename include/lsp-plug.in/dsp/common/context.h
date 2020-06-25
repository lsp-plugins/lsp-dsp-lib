/*
 * context.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_CONTEXT_H_
#define LSP_PLUG_IN_DSP_COMMON_CONTEXT_H_

#include <lsp-plug.in/dsp/common/types.h>

namespace lsp
{
    namespace dsp
    {
        #pragma pack(push, 1)
        /**
         * DSP context to store and restore machine state
         */
        typedef struct context_t
        {
            uint32_t        top;
            uint32_t        data[15];
        } context_t;
        #pragma pack(pop)

        typedef struct info_t
        {
            const char     *arch;       /* Architecture information */
            const char     *cpu;        /* CPU information */
            const char     *model;      /* CPU model information */
            const char     *features;   /* CPU features */
        } info_t;

        // Start and finish types
        typedef void (* start_t)(context_t *ctx);
        typedef void (* finish_t)(context_t *ctx);

        /** Initialize DSP
         *
         */
        void init();

        /** Start DSP processing, save machine context
         *
         * @param ctx structure to save context
         */
        LSP_DSP_LIB_IMPORT
        void (* start)(context_t *ctx);

        /** Finish DSP processing, restore machine context
         *
         * @param ctx structure to restore context
         */
        LSP_DSP_LIB_IMPORT
        void (* finish)(context_t *ctx);

        /**
         * Get DSP information, returns pointer to dsp::info_t structure
         * that can be freed by free()
         * @return pointer to dsp::info_t structure
         */
        LSP_DSP_LIB_IMPORT
        info_t * (*info)();
    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_CONTEXT_H_ */
