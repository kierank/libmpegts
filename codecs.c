/*****************************************************************************
 * ts_codecs.c :
 *****************************************************************************
 * Copyright (C) 2010 Kieran Kunhya
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *****************************************************************************/

#include "codecs.h"

const h264_level_t h264_levels[] =
{
    { 10,     64,     64 },
    {  9,    128,    350 }, /* 1b */
    { 11,    192,    500 },
    { 12,    384,   1000 },
    { 13,    768,   2000 },
    { 20,   2000,   2000 },
    { 21,   4000,   4000 },
    { 22,   4000,   4000 },
    { 30,  10000,  10000 },
    { 31,  14000,  14000 },
    { 32,  20000,  20000 },
    { 40,  20000,  25000 },
    { 41,  50000,  62500 },
    { 42,  50000,  62500 },
    { 50, 135000, 135000 },
    { 51, 240000, 240000 },
    { 0 }
};
