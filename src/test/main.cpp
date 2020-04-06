/*
 * main.cpp
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/main.h>

#ifndef LSP_BUILTIN_MODULE
    int main(int argc, const char **argv)
    {
        lsp::test::main(argc, argv);
    }
#endif


