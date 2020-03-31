/*
 * context.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_CONTEXT_H_
#define LSP_PLUG_IN_DSP_ARCH_GENERIC_CONTEXT_H_

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void start(dsp::context_t *ctx)
        {
            ctx->top        = 0;
        }

        void finish(dsp::context_t *ctx)
        {
// TODO
//            if (ctx->top != 0)
//                lsp_warn("DSP context is not empty");
        }

        dsp::info_t *info()
        {
            size_t size     =
                    sizeof(dsp::info_t) +
                    strlen(ARCH_STRING) + 1 +
                    strlen("native cpu") + 1 +
                    strlen("unknown") + 1;

            dsp::info_t *res = reinterpret_cast<dsp::info_t *>(malloc(size));
            if (res == NULL)
                return res;

            char *text  = reinterpret_cast<char *>(&res[1]);
            res->arch       = text;
            text            = stpcpy(text, ARCH_STRING) + 1;
            res->cpu        = text;
            text            = stpcpy(text, "native cpu") + 1;
            res->model      = text;
            text            = stpcpy(text, "unknown");
            res->features   = text; // Empty string

            return res;
        }
    }
}

#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_CONTEXT_H_ */
