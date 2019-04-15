#ifndef __STRING_HELPER
#define __STRING_HELPER

/**
 * @brief      Convert signed integer to string
 *
 * @param      data  The string to write to
 * @param[in]  i     The signed integer to convert
 *
 * @return     Returns the new end pointer of the string
 */
static inline char* itos(char *data, int32_t i) {
	if (i < 0) {
		i = -i;
		*data++ = '-';
	}
	char buf[14] = {0};
	char* ptr = &buf[1];
	do {
		*ptr++ = '0' + (i%10);
		i /= 10;
	} while(i);
	ptr--;
	while (*ptr) {
		*data++ = *ptr--;
	}
	*data = '\0';
	return data;
}

/**
 * @brief      Convert signed integer to string
 *
 * @param      data  The string to write to
 * @param[in]  i     The signed integer to convert
 *
 * @return     Returns the new end pointer of the string
 */
static inline char* ltos(char *data, int64_t i) {
	if (i < 0) {
		i = -i;
		*data++ = '-';
	}
	char buf[24] = {0};
	char* ptr = &buf[1];
	do {
		*ptr++ = '0' + (i%10);
		i /= 10;
	} while(i);
	ptr--;
	while (*ptr) {
		*data++ = *ptr--;
	}
	*data = '\0';
	return data;
}

/**
 * @brief      Convert unsigned integer to string
 *
 * @param      data  The string to write to
 * @param[in]  i     The unsigned integer to convert
 *
 * @return     Returns the new end pointer of the string
 */
static inline char* utos(char *data, uint32_t i) {
	char buf[14] = {0};
	char* ptr = &buf[1];
	do {
		*ptr++ = '0' + (i%10);
		i /= 10;
	} while(i);
	ptr--;
	while (*ptr) {
		*data++ = *ptr--;
	}
	*data = '\0';
	return data;
}

/**
 * @brief      Convert signed integer to string
 * 
 * This version will not write anything if the number is zero
 *
 * @param      data  The string to write to
 * @param[in]  i     The signed integer to convert
 *
 * @return     Returns the new end pointer of the string
 */
static inline char* itos2(char *data, int32_t i) {
	if (i < 0) {
		i = -i;
		*data++ = '-';
	}
	char buf[20] = {0};
	char* ptr = &buf[1];
	while(i) {
		*ptr++ = '0' + (i%10);
		i /= 10;
	}
	ptr--;
	while (*ptr) {
		*data++ = *ptr--;
	}
	*data = '\0';
	return data;
}

/**
 * @brief      Convert specific number of (most significant first) digits of an unsigned integer to a string
 * 
 * If the number has less digits than specified then it is right-padded with zeros
 *
 * @param      data    The string to write to
 * @param[in]  i       The unsigned integer
 * @param[in]  digits  The number of digits
 *
 * @return     Returns the new end pointer of the string
 */
static inline char* utos0(char* data, uint32_t i, uint32_t digits) {
	char buf[14] = {0};
	char* ptr = &buf[1];
	do {
		*ptr++ = '0' + (i%10);
		i /= 10;
	} while(i);
	ptr--;
	while (*ptr && digits--) {
		*data++ = *ptr--;
	}
	while (digits--) {
		*data++ = '0';
	}
	*data = '\0';
	return data;
}

/**
 * @brief      Converts a float to a string with a certain number of decimal places
 * 
 * Warning: will fail if the integer part of the number cannot fit in a int32_t
 * 
 * @param      data    The string to write to
 * @param[in]  i       The float
 * @param[in]  digits  The number of decimal places
 *
 * @return     Returns the new end pointer of the string
 */
static inline char* ftos(char *data, float a, uint32_t dp) {
	int32_t i = a;
	if (a < 0) { *data++ = '-'; i=-i; a=-a-i; }
	else { a=a-i; }
	data = itos(data, i);
	*data++ = '.';
	float d = a;
	while (dp--) {
		d*=10;
	}
	data = utos(data, (uint32_t)(d+0.5f));
	return data;
}

/**
 * @brief      Converts a float to a string with a certain number of decimal places
 * 
 * Warning: will fail if the integer part of the number cannot fit in a int32_t
 * 
 * This version will leave out the 0 before the decimal place for 0.xxx..
 * 
 * @param      data    The string to write to
 * @param[in]  i       The float
 * @param[in]  digits  The number of decimal places
 *
 * @return     Returns the new end pointer of the string
 */
static inline char* ftos2(char *data, float a, uint32_t dp) {
	int32_t i = a;
	if (a < 0) { *data++ = '-'; i=-i; a=-a-i; }
	else { a=a-i; }
	data = itos2(data, i);
	*data++ = '.';
	uint32_t d = a*100000;
	data = utos0(data, d, dp);
	return data;
}

/*
	Copyright 2001, 2002 Georges Menie (www.menie.org)
	stdarg version contributed by Christian Ettinger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdarg.h>

__forceinline void printchar(char **str, int c)
{
	**str = (char)c;
	++(*str);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
			if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = (unsigned int)i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = (unsigned int)-i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = (unsigned int)u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = (char)(t + '0');
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

static int print( char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
			out:
			printchar (out, *format);
			++pc;
		}
	}
	va_end( args );
	return pc;
}

static inline int sprintf(char *out, const char *format, ...)
{
	va_list args;

	va_start( args, format );
	return print( &out, format, args );
}

#endif
