/*
 * init1.cpp
 *
 *  Created on: 04 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/init.h>

INIT_BEGIN(dsp_initializer)

    INIT_FUNC
    {
        // Initialize DSP
        dsp::init();

        dsp::info_t *info = dsp::info();
        if (info != NULL)
        {
            printf("Architecture:   %s\n", info->arch);
            printf("Processor:      %s\n", info->cpu);
            printf("Model:          %s\n", info->model);
            printf("Features:       %s\n", info->features);
        }
    }

INIT_END


