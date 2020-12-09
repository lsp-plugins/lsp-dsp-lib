/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 дек. 2020 г.
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

#include <lsp-plug.in/test-fw/ptest.h>
#include <lsp-plug.in/common/alloc.h>

static const uint16_t table[] =
{
    0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930,
    0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931,
    0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932,
    0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933,
    0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934,
    0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935,
    0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936,
    0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937,
    0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938,
    0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939
};

char* uint32_to_string0(uint32_t n, char *out_str)
{
    if ( n < 10u )
    {
        const uint64_t in = n + 0x30ull;
        memcpy( out_str, &in, 8u );
        return out_str + 1u;
    }

    const uint32_t b = n / 10u;
    if ( n < 100u )
    {
        const uint64_t in = 256ull * n - 2559ull * b + 0x3030ull;
        memcpy( out_str, &in, 8u );
        return out_str + 2u;
    }

    const uint32_t c = n / 100u;
    if ( n < 1000u )
    {
        const uint64_t in = 65536ull * n - 2559ull * ( 256ull * b + c ) + 0x303030ull;
        memcpy( out_str, &in, 8u );
        return out_str + 3u;
    }

    const uint32_t d = n / 1000u;
    if ( n < 10000u )
    {
        const uint64_t in = 16777216ull * n - 2559ull * ( 256ull * ( 256ull * b + c ) + d ) + 0x30303030ull;
        memcpy( out_str, &in, 8u );
        return out_str + 4u;
    }

    const uint32_t e = n / 10000u;
    if ( n < 100000u )
    {
        const uint64_t in = ( ( 16777216ull * n - 2559ull * ( 256ull * ( 256ull * b + c ) + d ) - 10 * e ) << 0x08ull ) + e + 0x3030303030ull;
        memcpy( out_str, &in, 8u );
        return out_str + 5u;
    }

    const uint32_t f = n / 100000u;
    if ( n < 1000000u )
    {
        const uint64_t in = ( ( 16777216ull * n - 2559ull * ( 256ull * ( 256ull * b + c ) + d ) - 10 * e ) << 0x10ull ) +
                            ( ( 256ull      * e - 2559ull * f ) + 0x303030303030ull );
        memcpy( out_str, &in, 8u );
        return out_str + 6u;
    }

    const uint32_t g = n / 1000000u;
    if ( n < 10000000u )
    {
        const uint64_t in = ( ( 16777216ull * n - 2559ull * ( 256ull * ( 256ull * b + c ) + d ) - 10 * e ) << 0x18ull ) +
                            ( ( 65536ull    * e - 2559ull * ( 256ull * f + g ) + 0x30303030303030ull ) );
        memcpy( out_str, &in, 8u );
        return out_str + 7u;
    }

    const uint32_t h = n / 10000000u;
    if ( n < 100000000u )
    {
        const uint64_t in = ( ( 16777216ull * n - 2559ull * ( 256ull * ( 256ull * b + c ) + d ) - 10 * e ) << 0x20ull ) +
                            ( ( 16777216ull * e - 2559ull * ( 256ull * ( 256ull * f + g ) + h ) ) + 0x3030303030303030ull );
        memcpy( out_str, &in, 8u );
        return out_str + 8u;
    }

    const uint32_t x = n / 100000000u;
    const uint64_t r_0 = ( ( 16777216ull * n - 2559ull * ( 256ull * ( 256ull * b + c ) + d ) - 10 * e ) << 0x20ull ) +
                         ( 16777216ull * e - 2559ull * ( 256ull * ( 256ull * f + g ) + h ) - 10 * x );

    if ( 9u < x )
    {
        const uint64_t in_1 = ( ( x % 10u ) << 8ull ) + x / 10u + 0x3030ull;
        memcpy( out_str, &in_1, 8u );
        const uint64_t in_2 = ( ( r_0 + 0x3030303030303030ull ) );
        memcpy( out_str + 2u, &in_2, 8u );
        return out_str + 9u;
    }
    else
    {
        const uint64_t in_1 = x   + 0x30u;
        memcpy( out_str, &in_1, 8u );
        const uint64_t in_2 = r_0 + 0x3030303030303030ull;
        memcpy( out_str + 1u, &in_2, 8u );
        return out_str + 10u;
    }
}

char* uint32_to_string1(uint32_t n, char *out_str)
{
    uint8_t bytes;
    uint64_t div;
    uint32_t mod;

    // 4294967295
    if (n <= 9999)
    {
        if (n <= 99)
            bytes = (n <= 9) ? 1 : 2;
        else
            bytes = (n <= 999) ? 3 : 4;
    }
    else if (n <= 99999999)
    {
        if (n <= 999999)
            bytes = (n <= 99999) ? 5 : 6;
        else
            bytes = (n <= 9999999) ? 7 : 8;
    }
    else
        bytes = (n <= 999999999) ? 9 : 10;

    char *ret = &out_str[bytes+1];
    if (bytes & 1)
    {
        out_str    += bytes - 1;
        div         = n / 10;
        mod         = n % 10;
        out_str[0]  = 0x30 + mod;
        out_str[1]  = '\0';

        n           = div;
        bytes      &= ~1;
    }
    else
    {
        out_str    += bytes;
        out_str[0]  = '\0';
    }

    uint16_t *dst   = (uint16_t *)out_str;
    while (n)
    {
        --dst;
        div     = n / 100;
        mod     = n % 100;

        *dst    = table[mod];
        n       = div;
    }

    return ret;
}

typedef char* (* uint32_to_string_t)(uint32_t n, char *out_str);

static const uint32_t values[] =
{
    0, 1, 2, 3,
    4, 5, 6, 7,
    8, 9, 0, 1,
    2, 3, 4, 5,

    11, 22, 33, 44,
    55, 66, 77, 88,
    99, 45, 54, 36,
    63, 27, 72, 81,

    100, 212, 323, 434,
    545, 656, 767, 878,
    989, 191, 272, 363,
    454, 999, 153, 709,

    1234, 2345, 3456, 5678,
    7890, 8901, 9012, 9876,
    8765, 7654, 6543, 5432,
    4321, 3210, 2109, 1098,

    12345, 23456, 34567, 45678,
    56789, 67890, 78901, 89012,
    90123, 76543, 65432, 54321,
    43210, 32109, 21098, 10987,

    568947, 689471, 894713, 947134,
    471341, 713412, 134120, 341205,
    568947, 689671, 814713, 957134,
    471641, 712412, 634127, 241205,

    7654321, 6543210, 5432109, 4321098,
    3210987, 2109876, 1098765, 9876543,
    8765432, 7591274, 5912747, 9127475,
    1928374, 9283741, 2837419, 8374192,

    19283746, 92837461, 28374619, 83746192,
    37461928, 74619283, 46192837, 61928374,
    13822473, 38224731, 82247313, 22473138,
    24731382, 47313822, 73138224, 31382247,

    104837519, 483751910, 837519104, 375191048,
    751910483, 519104837, 191048375, 910483751,
    884772641, 847726418, 477264188, 772641884,
    726418847, 264188477, 641884772, 418847726,

    1498673324, 1437674341, 1123456789, 1987654321,
    2581275057, 2302857683, 2476301756, 2762307694,
    3278574027, 3495817583, 3459371495, 3048658387,
    4168205863, 4258672947, 4148592495, 4027845728
};

PTEST_BEGIN("dsp", number, 5, 1000)

    void call(const char *label, char *out, uint32_to_string_t func)
    {
        printf("Testing %s ...\n", label);

        PTEST_LOOP(label,
            for (size_t i=0; i<sizeof(values)/sizeof(uint32_t); ++i)
                func(values[i], out);
        );
    }

    PTEST_MAIN
    {
        char buf[0x20];

        #define CALL(func) \
            call(#func, buf, func)

        CALL(uint32_to_string0);
        CALL(uint32_to_string1);
        PTEST_SEPARATOR;
    }

PTEST_END

