/*
 * init1.cpp
 *
 *  Created on: 04 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/init.h>

namespace lsp
{
    static __thread dsp::context_t *pTestContext = NULL;
}

INIT_BEGIN(dsp_initializer)

    INIT_FUNC
    {
        // Initialize DSP
        dsp::init();

        // Output information if necessary
        dsp::info_t *info = dsp::info();
        if (info != NULL)
        {
            printf("Architecture:   %s\n", info->arch);
            printf("Processor:      %s\n", info->cpu);
            printf("Model:          %s\n", info->model);
            printf("Features:       %s\n", info->features);

            ::free(info);
        }
    }

    BEFORE_FUNC
    {
        // Check test match
        if (::strstr(test, "dsp.") != test)
            return;

        // Update context
        pTestContext = reinterpret_cast<dsp::context_t *>(::malloc(sizeof(dsp::context_t)));
        if (pTestContext != NULL)
            dsp::start(pTestContext);
    }

    AFTER_FUNC
    {
        // Restore context
        if (pTestContext != NULL)
        {
            dsp::finish(pTestContext);
            ::free(pTestContext);
            pTestContext = NULL;
        }
    }

INIT_END


