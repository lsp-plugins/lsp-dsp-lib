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

#ifndef PRIVATE_DSP_ARCH_AARCH64_FEATURES_H_
#define PRIVATE_DSP_ARCH_AARCH64_FEATURES_H_

#include <lsp-plug.in/common/types.h>

#ifdef ARCH_AARCH64

    #if defined(PLATFORM_MACOSX)
        #define HWCAP_ASIMD             (1 << 0)
    #elif defined(PLATFORM_POSIX)
        #include <sys/auxv.h>
    #endif /* PLATFORM_POSIX */

    // Override some constants if they were not defined previously
    #ifndef HWCAP_FP
        #define HWCAP_FP                0
    #endif /* HWCAP_FP */

    #ifndef HWCAP_ASIMD
        #define HWCAP_ASIMD             0
    #endif /* HWCAP_ASIMD */

    #ifndef HWCAP_EVTSTRM
        #define HWCAP_EVTSTRM           0
    #endif /* HWCAP_EVTSTRM */

    #ifndef HWCAP_AES
        #define HWCAP_AES               0
    #endif /* HWCAP_AES */

    #ifndef HWCAP_PMULL
        #define HWCAP_PMULL             0
    #endif /* HWCAP_PMULL */

    #ifndef HWCAP_SHA1
        #define HWCAP_SHA1              0
    #endif /* HWCAP_SHA1 */

    #ifndef HWCAP_SHA2
        #define HWCAP_SHA2              0
    #endif /* HWCAP_SHA2 */

    #ifndef HWCAP_CRC32
        #define HWCAP_CRC32             0
    #endif /* HWCAP_CRC32 */

    #ifndef HWCAP_ATOMICS
        #define HWCAP_ATOMICS           0
    #endif /* HWCAP_ATOMICS */

    #ifndef HWCAP_FPHP
        #define HWCAP_FPHP              0
    #endif /* HWCAP_FPHP */

    #ifndef HWCAP_ASIMDHP
        #define HWCAP_ASIMDHP           0
    #endif /* HWCAP_ASIMDHP */

    #ifndef HWCAP_CPUID
        #define HWCAP_CPUID             0
    #endif /* HWCAP_CPUID */

    #ifndef HWCAP_ASIMDRDM
        #define HWCAP_ASIMDRDM          0
    #endif /* HWCAP_ASIMDRDM */

    #ifndef HWCAP_JSCVT
        #define HWCAP_JSCVT             0
    #endif /* HWCAP_JSCVT */

    #ifndef HWCAP_FCMA
        #define HWCAP_FCMA              0
    #endif /* HWCAP_FCMA */

    #ifndef HWCAP_LRCPC
        #define HWCAP_LRCPC             0
    #endif /* HWCAP_LRCPC */

    #ifndef HWCAP_DCPOP
        #define HWCAP_DCPOP             0
    #endif /* HWCAP_DCPOP */

    #ifndef HWCAP_SHA3
        #define HWCAP_SHA3              0
    #endif /* HWCAP_SHA3 */

    #ifndef HWCAP_SM3
        #define HWCAP_SM3               0
    #endif /* HWCAP_SM3 */

    #ifndef HWCAP_SM4
        #define HWCAP_SM4               0
    #endif /* HWCAP_SM4 */

    #ifndef HWCAP_ASIMDDP
        #define HWCAP_ASIMDDP           0
    #endif /* HWCAP_ASIMDDP */

    #ifndef HWCAP_SHA512
        #define HWCAP_SHA512            0
    #endif /* HWCAP_SHA512 */

    #ifndef HWCAP_SVE
        #define HWCAP_SVE               0
    #endif /* HWCAP_SVE */

    #ifndef HWCAP_ASIMDFHM
        #define HWCAP_ASIMDFHM          0
    #endif /* HWCAP_ASIMDFHM */

    #ifndef HWCAP_DIT
        #define HWCAP_DIT               0
    #endif /* HWCAP_DIT */

    #ifndef HWCAP_USCAT
        #define HWCAP_USCAT             0
    #endif /* HWCAP_USCAT */

    #ifndef HWCAP_ILRCPC
        #define HWCAP_ILRCPC            0
    #endif /* HWCAP_ILRCPC */

    #ifndef HWCAP_FLAGM
        #define HWCAP_FLAGM             0
    #endif /* HWCAP_FLAGM */

    // Define LSP-defined AARCH-64 specific macros
    #define HWCAP_AARCH64_FP                HWCAP_FP
    #define HWCAP_AARCH64_ASIMD             HWCAP_ASIMD
    #define HWCAP_AARCH64_EVTSTRM           HWCAP_EVTSTRM
    #define HWCAP_AARCH64_AES               HWCAP_AES
    #define HWCAP_AARCH64_PMULL             HWCAP_PMULL
    #define HWCAP_AARCH64_SHA1              HWCAP_SHA1
    #define HWCAP_AARCH64_SHA2              HWCAP_SHA2
    #define HWCAP_AARCH64_CRC32             HWCAP_CRC32
    #define HWCAP_AARCH64_ATOMICS           HWCAP_ATOMICS
    #define HWCAP_AARCH64_FPHP              HWCAP_FPHP
    #define HWCAP_AARCH64_ASIMDHP           HWCAP_ASIMDHP
    #define HWCAP_AARCH64_CPUID             HWCAP_CPUID
    #define HWCAP_AARCH64_ASIMDRDM          HWCAP_ASIMDRDM
    #define HWCAP_AARCH64_JSCVT             HWCAP_JSCVT
    #define HWCAP_AARCH64_FCMA              HWCAP_FCMA
    #define HWCAP_AARCH64_LRCPC             HWCAP_LRCPC
    #define HWCAP_AARCH64_DCPOP             HWCAP_DCPOP
    #define HWCAP_AARCH64_SHA3              HWCAP_SHA3
    #define HWCAP_AARCH64_SM3               HWCAP_SM3
    #define HWCAP_AARCH64_SM4               HWCAP_SM4
    #define HWCAP_AARCH64_ASIMDDP           HWCAP_ASIMDDP
    #define HWCAP_AARCH64_SHA512            HWCAP_SHA512
    #define HWCAP_AARCH64_SVE               HWCAP_SVE
    #define HWCAP_AARCH64_ASIMDFHM          HWCAP_ASIMDFHM
    #define HWCAP_AARCH64_DIT               HWCAP_DIT
    #define HWCAP_AARCH64_USCAT             HWCAP_USCAT
    #define HWCAP_AARCH64_ILRCPC            HWCAP_ILRCPC
    #define HWCAP_AARCH64_FLAGM             HWCAP_FLAGM

    namespace lsp
    {
        namespace aarch64
        {
            typedef struct cpu_features_t
            {
                size_t      implementer;
                size_t      architecture;
                size_t      variant;
                size_t      part;
                size_t      revision;
                uint64_t    hwcap;
                char        cpu_name[64];
            } cpu_features_t;

            void detect_cpu_features(cpu_features_t *f);

            void dsp_init(const cpu_features_t *f);
        } /* namespace aarch64 */
    } /* namespace lsp */

    #define LSP_DSP_CPU_NAMESPACE           aarch64

#endif /* ARCH_AARCH64 */

#endif /* PRIVATE_DSP_ARCH_AARCH64_FEATURES_H_ */
