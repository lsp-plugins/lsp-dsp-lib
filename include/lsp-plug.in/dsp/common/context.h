/*
 * context.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_CONTEXT_H_
#define LSP_PLUG_IN_DSP_COMMON_CONTEXT_H_

#include <lsp-plug.in/dsp/common/types.h>

#ifdef __cplusplus
namespace lsp
{
    namespace dsp
    {
#endif /* __cplusplus */

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

#ifdef __cplusplus
    }
}
#endif /* __cplusplus */

#ifdef __cplusplus
namespace lsp
{
    namespace dsp
    {
        /** Initialize DSP
         *
         */
        LSP_DSP_LIB_CPPIMPORT
        void init();
    }
}
#else
    /** Initialize DSP
     *
     */
    LSP_DSP_LIB_CIMPORT
    void LSP_DSP_LIB_MANGLE(init());
#endif /* __cplusplus */


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
