/*
 * Copyright (C) 2004  Kustaa Nyholm
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdarg.h>


void init_printf(void* putp,void (*putf) (void*,char));

void tfp_printf(char *fmt, ...);
void tfp_sprintf(char* s,char *fmt, ...);

void tfp_format(void* putp,void (*putf) (void*,char),char *fmt, va_list va);

#define Fmt tfp_sprintf

void Print( int channel, char *fmt, ...);

#endif
