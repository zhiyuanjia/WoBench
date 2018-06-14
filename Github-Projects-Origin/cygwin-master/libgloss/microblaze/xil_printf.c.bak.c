#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/* Copyright (c) 1995-2013 Xilinx, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1.  Redistributions source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2.  Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3.  Neither the name of Xilinx nor the names of its contributors may be
 * used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ctype.h>
#include <string.h>
#include <stdarg.h>

extern void outbyte (char);

/*----------------------------------------------------*/
/* Use the following parameter passing structure to   */
/* make xil_printf re-entrant.                        */
/*----------------------------------------------------*/
typedef struct params_s {
    int len;
    int num1;
    int num2;
    char pad_character;
    int do_padding;
    int left_flag;
} params_t;

/*---------------------------------------------------*/
/* The purpose of this routine is to output data the */
/* same as the standard printf function without the  */
/* overhead most run-time libraries involve. Usually */
/* the printf brings in many kilobytes of code and   */
/* that is unacceptable in most embedded systems.    */
/*---------------------------------------------------*/

typedef char* charptr;
typedef int (*func_ptr)(int c);

/*---------------------------------------------------*/
/*                                                   */
/* This routine puts pad characters into the output  */
/* buffer.                                           */
/*                                                   */
static void padding( const int l_flag, params_t *par)
{
    int i;

    if (par->do_padding && l_flag && (par->len < par->num1))
        for (i=par->len; i<par->num1; i++)
            outbyte( par->pad_character);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine moves a string to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */
static void outs( charptr lp, params_t *par)
{
    /* pad on left if needed                         */
    par->len = strlen( lp);
    padding( !(par->left_flag), par);

    /* Move string to the buffer                     */
    while (*lp && (par->num2)--)
        outbyte( *lp++);

    /* Pad on right if needed                        */
    /* CR 439175 - elided next stmt. Seemed bogus.   */
    /* par->len = strlen( lp);                       */
    padding( par->left_flag, par);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine moves a number to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */

static void outnum( const long n, const long base, params_t *par)
{
    charptr cp;
    int negative;
    char outbuf[32];
    const char digits[] = "0123456789ABCDEF";
    unsigned long num;

    /* Check if number is negative                   */
    if (base == 10 && n < 0L) {
        negative = 1;
        num = -(n);
    }
    else{
        num = (n);
        negative = 0;
    }
   
    /* Build number (backwards) in outbuf            */
    cp = outbuf;
    do {
        *cp++ = digits[(int)(num % base)];
    } while ((num /= base) > 0);
    if (negative)
        *cp++ = '-';
    *cp-- = 0;

    /* Move the converted number to the buffer and   */
    /* add in the padding where needed.              */
    par->len = strlen(outbuf);
    padding( !(par->left_flag), par);
    while (cp >= outbuf)
        outbyte( *cp--);
    padding( par->left_flag, par);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine gets a number from the format        */
/* string.                                           */
/*                                                   */
static int getnum( charptr* linep)
{
    int n;
    charptr cp;

    n = 0;
    cp = *linep;
    while (isdigit(*cp))
        n = n*10 + ((*cp++) - '0');
    *linep = cp;
    return(n);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine operates just like a printf/sprintf  */
/* routine. It outputs a set of data under the       */
/* control of a formatting string. Not all of the    */
/* standard C format control are supported. The ones */
/* provided are primarily those needed for embedded  */
/* systems work. Primarily the floaing point         */
/* routines are omitted. Other formats could be      */
/* added easily by following the examples shown for  */
/* the supported formats.                            */
/*                                                   */

/* void esp_printf( const func_ptr f_ptr,
   const charptr ctrl1, ...) */
void xil_printf( const charptr ctrl1, ...)
{

    int long_flag;
    int dot_flag;

    params_t par;

    char ch;
    va_list argp;
    charptr ctrl = ctrl1;

    va_start( argp, ctrl1);

    for ( ; *ctrl; ctrl++) {

        /* move format string chars to buffer until a  */
        /* format control is found.                    */
        if (*ctrl != '%') {
            outbyte(*ctrl);
            continue;
        }

        /* initialize all the flags for this format.   */
        dot_flag   = long_flag = par.left_flag = par.do_padding = 0;
        par.pad_character = ' ';
        par.num2=32767;

 try_next:
        ch = *(++ctrl);

        if (isdigit(ch)) {
            if (dot_flag)
                par.num2 = getnum(&ctrl);
            else {
                if (ch == '0')
                    par.pad_character = '0';

                par.num1 = getnum(&ctrl);
                par.do_padding = 1;
            }
            ctrl--;
            goto try_next;
        }

        switch (tolower(ch)) {
            case '%':
                outbyte( '%');
                continue;

            case '-':
                par.left_flag = 1;
                break;

            case '.':
                dot_flag = 1;
                break;

            case 'l':
                long_flag = 1;
                break;

            case 'd':
                if (long_flag || ch == 'D') {
                    outnum( va_arg(argp, long), 10L, &par);
                    continue;
                }
                else {
                    outnum( va_arg(argp, int), 10L, &par);
                    continue;
                }
            case 'x':
                outnum((long)va_arg(argp, int), 16L, &par);
                continue;

            case 's':
                outs( va_arg( argp, charptr), &par);
                continue;

            case 'c':
                outbyte( va_arg( argp, int));
                continue;

            case '\\':
                switch (*ctrl) {
                    case 'a':
                        outbyte( 0x07);
                        break;
                    case 'h':
                        outbyte( 0x08);
                        break;
                    case 'r':
                        outbyte( 0x0D);
                        break;
                    case 'n':
                        outbyte( 0x0D);
                        outbyte( 0x0A);
                        break;
                    default:
                        outbyte( *ctrl);
                        break;
                }
                ctrl++;
                break;

            default:
                continue;
        }
        goto try_next;
    }
    va_end( argp);
}

/*---------------------------------------------------*/