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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <private/utest/dsp/3d/helpers.h>

namespace lsp
{
    namespace test
    {
        bool point3d_ck(const dsp::point3d_t *p1, const dsp::point3d_t *p2)
        {
            return
                (float_equals_relative(p1->x, p2->x)) &&
                (float_equals_relative(p1->y, p2->y)) &&
                (float_equals_relative(p1->z, p2->z));
        }

        bool point3d_sck(const dsp::point3d_t *p1, const dsp::point3d_t *p2)
        {
            return
                (float_equals_relative(p1->x, p2->x)) &&
                (float_equals_relative(p1->y, p2->y)) &&
                (float_equals_relative(p1->z, p2->z)) &&
                (float_equals_relative(p1->w, p2->w));
        }

        bool point3d_ack(const dsp::point3d_t *p1, const dsp::point3d_t *p2, float tolerance)
        {
            return
                (float_equals_adaptive(p1->x, p2->x, tolerance)) &&
                (float_equals_adaptive(p1->y, p2->y, tolerance)) &&
                (float_equals_adaptive(p1->z, p2->z, tolerance)) &&
                (float_equals_adaptive(p1->w, p2->w, tolerance));
        }

        bool vector3d_sck(const dsp::vector3d_t *v1, const dsp::vector3d_t *v2)
        {
            return
                (float_equals_relative(v1->dx, v2->dx)) &&
                (float_equals_relative(v1->dy, v2->dy)) &&
                (float_equals_relative(v1->dz, v2->dz)) &&
                (float_equals_relative(v1->dw, v2->dw));
        }

        bool vector3d_ack(const dsp::vector3d_t *v1, const dsp::vector3d_t *v2, float tolerance)
        {
            return
                (float_equals_adaptive(v1->dx, v2->dx, tolerance)) &&
                (float_equals_adaptive(v1->dy, v2->dy, tolerance)) &&
                (float_equals_adaptive(v1->dz, v2->dz, tolerance)) &&
                (float_equals_adaptive(v1->dw, v2->dw, tolerance));
        }

        bool matrix3d_ck(const dsp::matrix3d_t *m1, const dsp::matrix3d_t *m2)
        {
            for (size_t i=0; i<16; ++i)
                if (!float_equals_relative(m1->m[i], m2->m[i]))
                    return false;
            return true;
        }

        void dump_point(const char *text, const dsp::point3d_t *p)
        {
            printf("%s: { %e, %e, %e, %e }\n", text, p->x, p->y, p->z, p->w);
        }

        void dump_vector(const char *text, const dsp::vector3d_t *v)
        {
            printf("%s: { %e, %e, %e, %e }\n", text, v->dx, v->dy, v->dz, v->dw);
        }
    }
}

