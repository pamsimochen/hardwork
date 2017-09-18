/* 
 *  Copyright (c) 2008 Texas Instruments. All rights reserved. 
 *  This program and the accompanying materials are made available under the 
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at 
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 * */
/*
 *  ======== System.c ========
 */

#include <xdc/runtime/Startup.h>

#include <xdc/runtime/Gate.h>

#include "package/internal/System.xdc.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* 
 * the maximum length of a string representing a 32-bit number in base 8 
 * plus 5 to accomodate the decimal point and 4 digits after the decimal 
 * point 
 */
#define OUTMAX      ((32 + 2) / 3) + 5

static const Char digtohex[] = "0123456789abcdef";

/*
 *  ======== System_Module_startup ========
 */
Int System_Module_startup(Int stat)
{
    return (Startup_DONE);
}

/*
 *  ======== System_abort ========
 */
Void System_abort(CString str)
{
    Gate_enterSystem();

    System_SupportProxy_abort(str);

    abort();
}

/*
 *  ======== System_atexit ========
 */
Bool System_atexit(System_AtexitHandler handler)
{
    IArg key;
    Bool status = TRUE;
    
    key = Gate_enterSystem();

    if (module->numAtexitHandlers < System_maxAtexitHandlers) {
        module->atexitHandlers[module->numAtexitHandlers] = handler;
        module->numAtexitHandlers++;
    }
    else {
        status = FALSE;
    }

    Gate_leaveSystem(key);
    
    return (status);
}

/*
 *  ======== System_exit ========
 */
Void System_exit(Int stat)
{
    module->exitStatus = stat;

    exit(stat);
}

/*
 *  ======== System_lastFxn ========
 */
Void System_lastFxn(Void)
{
    atexit(System_rtsExit);
}

/* 
 * rtsExit__I is an internal entry point called by target/platform boot code.
 * Boot code is not brought into a partial-link assembly. So without this pragma,
 * whole program optimizers would otherwise optimize-out these functions.
 */
#ifdef __ti__
#pragma FUNC_EXT_CALLED(xdc_runtime_System_rtsExit__I);
#endif

#ifdef __GNUC__
#if __GNUC__ >= 4
xdc_Void xdc_runtime_System_rtsExit__I(void) __attribute__ ((externally_visible));
#endif
#endif

/*
 *  ======== System_rtsExit ========
 */
Void System_rtsExit()
{
    Int i;

    Gate_enterSystem();

    for (i = module->numAtexitHandlers; i > 0; i--) {
        (module->atexitHandlers[i - 1])(module->exitStatus);
    }
    System_SupportProxy_exit(module->exitStatus);
}

/*
 *  ======== System_flush ========
 */
Void System_flush()
{
    System_SupportProxy_flush();
}


/*
 *  ======== System_putch ========
 */
Void System_putch(Char ch)
{
    if (System_SupportProxy_ready()) {
        System_SupportProxy_putch(ch);
    }
}

/*
 *  ======== System_aprintf_va ========
 */
Int System_aprintf_va(CString fmt, VaList va)
{
    return (System_avprintf(fmt, va));
}

/*
 *  ======== System_avprintf ========
 *  -1 indicates infinite output
 */
Int System_avprintf(CString fmt, VaList va)
{
    return (System_SupportProxy_ready()
            ? System_doPrint(NULL, (SizeT)-1, fmt, &va, TRUE) : -1);
}

/*
 *  ======== System_asprintf_va ========
 */
Int System_asprintf_va(Char buf[], CString fmt, VaList va)
{
    return (System_avsprintf(buf, fmt, va));
}

/*
 *  ======== System_avsprintf ========
 *  -1 indicates infinite output
 */
Int System_avsprintf(Char buf[], CString fmt, VaList va)
{
    return (System_doPrint(buf, (SizeT)-1, fmt, &va, TRUE));
}

/*
 *  ======== System_printf_va ========
 */
Int System_printf_va(CString fmt, VaList va)
{
    return (System_vprintf(fmt, va));
}

/*
 *  ======== System_vprintf ========
 *  -1 indicates infinite output
 */
Int System_vprintf(CString fmt, VaList va)
{
    return (System_SupportProxy_ready()
            ? System_doPrint(NULL, (SizeT)-1, fmt, &va, FALSE) : -1);
}

/*
 *  ======== System_sprintf_va ========
 */
Int System_sprintf_va(Char buf[], CString fmt, VaList va)
{
    return (System_vsprintf(buf, fmt, va));
}

/*
 *  ======== System_vsprintf ========
 *  -1 indicates infinite output
 */
Int System_vsprintf(Char buf[], CString fmt, VaList va)
{
    return (System_doPrint(buf, (SizeT)-1, fmt, &va, FALSE));
}

/*
 *  ======== System_snprintf_va ========
 */
Int System_snprintf_va(Char buf[], SizeT n, CString fmt, VaList va)
{
    return (System_vsnprintf(buf, n, fmt, va));
}

/*
 *  ======== System_vsnprintf ========
 */
Int System_vsnprintf(Char buf[], SizeT n, CString fmt, VaList va)
{
    return (System_doPrint(buf, n, fmt, &va, FALSE));
}

/*
 *  ======== System_doPrint ========
 *  Internal function
 *
 *  If buf == NULL, characters are sent to System_SupportProxy_putch();
 *  otherwise, they are written into buf. Atmost `n` - 1 characters are written
 *  excluding '\0'. 
 *
 *  The return value is the number of characters that would have 
 *  been written had `n` been sufficiently large, not counting the terminating 
 *  '\0' character.
 */
Int System_doPrint(Char *buf, SizeT n, CString fmt, VaList *pva, Bool aFlag)
{
    /* temp vars */
    Int     base;
    Char    c;
    Int     res;
    Char    outbuf[OUTMAX];
   
    /* vars passed to System_extendFxn. Also keep track in while loop */
    struct System_ParseData parse;

    parse.aFlag = aFlag;

    res = 0;

    if (fmt == (Char *)NULL) {
        return (res);
    }

    while ((c = *fmt++) != '\0') {
        if (c != '%') {
            System_putchar(&buf, c, &n);
            res++;
        }
        else {
            c = *fmt++;
            /* check for - flag (pad on right) */
            if (c == '-') {
                parse.lJust = TRUE;
                c = *fmt++;
            }
            else {
                parse.lJust = FALSE;
            }
            /* check for leading 0 pad */
            if (c == '0') {
                parse.zpad = 1;
                c = *fmt++;
            }
            else {
                parse.zpad = 0;
            }

            /* allow optional field width/precision specification */
            parse.width = 0;
            parse.precis = -1;

            /* note: dont use isdigit (very large for C30) */
            if (c == '*') {
                parse.width = parse.aFlag
                    ? (int)va_arg(*pva, IArg) : (int)va_arg(*pva, int);
                c = *fmt++;
                if (parse.width < 0) {
                    parse.lJust = TRUE;
                    parse.width = -parse.width;
                }
            }
            else {
                while (c >= '0' && c <= '9') {
                    parse.width = parse.width * 10 + c - '0';
                    c = *fmt++;
                }
            }

            /* allow optional field precision specification */
            if (c == '.') {
                parse.precis = 0;
                c = *fmt++;
                if (c == '*') {
                    parse.precis = parse.aFlag ? (int)va_arg(*pva, IArg) :
                                     (int)va_arg(*pva, int);
                    if (parse.precis < 0) {
                        parse.precis = 0;
                    }
                    
                    c = *fmt++;
                }
                else {
                    while (c >= '0' && c <= '9') {
                        parse.precis = parse.precis * 10 + c - '0';
                        c = *fmt++;
                    }
                }
            }

            /* setup for leading zero padding */
            if (parse.zpad) {
                parse.zpad = parse.width;
            }
            
            /* check for presence of l flag (e.g., %ld) */
            if (c == 'l' || c == 'L') {
                parse.lFlag = TRUE;
                c = *fmt++;
            }
            else {
                parse.lFlag = FALSE;
            }

            parse.ptr = outbuf;
            parse.end = outbuf + OUTMAX;
            parse.len = 0;

            if (c == 'd' || c == 'i') {
                /* signed decimal */
                Int32 val =
                    parse.aFlag ? (Int32)va_arg(*pva, IArg) :
                    parse.lFlag ? (Int32)va_arg(*pva, long int) :
                    (Int32)va_arg(*pva, int);
				
                if (parse.precis > parse.zpad) {
                    parse.zpad = parse.precis;
                }
                parse.ptr = System_formatNum(parse.end, val, parse.zpad, -10);
                parse.len = parse.end - parse.ptr;
            }
            /* use comma operator to optimize code generation! */
            else if (((base = 10), (c == 'u')) ||       /* unsigned decimal */
                     ((base = 16), (c == 'x')) ||       /* unsigned hex */
                     ((base = 8),  (c == 'o'))) {       /* unsigned octal */

                UInt32 val =
                    parse.aFlag ? (UInt32)va_arg(*pva, IArg) :
                    parse.lFlag ? (UInt32)va_arg(*pva, unsigned long) :
                    (UInt32)va_arg(*pva, unsigned);

                if (parse.precis > parse.zpad) {
                    parse.zpad = parse.precis;
                }
                parse.ptr = System_formatNum(parse.end, val, parse.zpad, base);
                parse.len = parse.end - parse.ptr;
            }
            else if ((base = 16), (c == 'p')) {
                parse.zpad = 8;                       /* for 32 bit pointer */
                parse.ptr = System_formatNum(
                    parse.end,
                    parse.aFlag
                        ? (UInt32)va_arg(*pva, IArg) : (UInt32)(UArg)va_arg(*pva, Ptr),
                    parse.zpad, base);
                *(--parse.ptr) = '@';
                parse.len = parse.end - parse.ptr;
            }
            else if (c == 'c') {
                /* character */
                *parse.ptr = parse.aFlag
                    ? (Char)va_arg(*pva, IArg) : (Char)va_arg(*pva, int);
                parse.len = 1;
            }
            else if (c == 's') {
                /* string */
                parse.ptr = parse.aFlag ? (String)iargToPtr(va_arg(*pva, IArg)) :
                              (String)va_arg(*pva, void *);

                /* substitute (null) for NULL pointer */
                if (parse.ptr == (char *)NULL) {
                    parse.ptr = "(null)";
                }
                parse.len = strlen(parse.ptr);
                if (parse.precis != -1 && parse.precis < parse.len) {
                    parse.len = parse.precis;
                }
            }
            else {
                fmt--;

                 /* check if enough buffer space available */
                if (n > 1) {
                    /* parse.precis should account for the buffer size */
                    if ((parse.precis == -1) || ((SizeT)parse.precis >= n)) {
                         parse.precis = n; 
                    }
                    else {
                         /* Have enough space, increment to account for '\0' */
                         parse.precis++; 
                    }
                    res += System_extendFxn(&buf, &fmt, pva, &parse);
                }
            }

            /* compute number of characters left in field */
            parse.width -= parse.len;

            if (!parse.lJust) {
                /* pad with blanks on left */
                while (--parse.width >= 0) {
                    System_putchar(&buf, ' ', &n);
                    res++;
                }
            }

            /* output number, character or string */
            while (parse.len--) {
                System_putchar(&buf, *parse.ptr++, &n);
                res++;
            }
            /* pad with blanks on right */
            if (parse.lJust) {
                while (--parse.width >= 0) {
                    System_putchar(&buf, ' ', &n);
                    res++;
                }
            }
        } /* if */
    } /* while */

    if (buf) {
        *buf = '\0';
    }

    return (res);
}

/*
 *  ======== System_formatNum ========
 *  Internal function
 *
 *  Format unsigned long number in specified base, returning pointer to
 *  converted output.
 *
 *  Note: ptr points PAST end of the buffer, and is decremented as digits
 *  are converted from right to left!
 *
 *  Note: base is negative if n is signed else n unsigned!
 *
 *  ptr  - Pointer to the end of the working buffer where the string version
 *         of the number will be placed.
 *  un   - The unsigned number to be formated
 *  base - The base to format the number into. TODO - signed?
 */
Char *System_formatNum(Char *ptr, UInt32 un, Int zpad, Int base)
{
    Int i = 0;
    Char sign = 0;

    UInt32 n;
    n = un;

    if (base < 0) {
        /* handle signed long case */
        base = -base;
        if ((Int32)n < 0) {
            n = -(Int32)n;

            /* account for sign '-': ok since zpad is signed */
            --zpad;
            sign = '-';
        }
    }

    /* compute digits in number from right to left */
    do {
        *(--ptr) = digtohex[(Int) (n % base)];
        n = n / base;
        ++i;
    } while (n);

    /* pad with leading 0s on left */
    while (i < zpad) {
        *(--ptr) = '0';
        ++i;
    }

    /* add sign indicator */
    if (sign) {
        *(--ptr) = sign;
    }
    return (ptr);
}

/*
 *  ======== System_putchar ========
 *  Internal function
 *
 *  Write character `c` to the buffer and, if the buffer pointer is
 *  non-NULL, update the buffer pointer.
 *  
 *  Keeps track of the number of characters written into the buffer by 
 *  modifying bufsize `n`. Atmost, `n` - 1 characters are written.  
 */
Void System_putchar(Char **bufp, Char c, SizeT *n)
{
    /* if the size == 1, don't write so we can '\0' terminate buffer */
    if ((*n) <= 1) {
        return;
    }
         
    /* decrement n to keep track of the number of chars written */
    (*n)--;

    /*
     *  If the buffer is non-NULL, use it, otherwise call the
     *  proxy's putch function (if it is ready).
     */
    if (*bufp) {
        *((*bufp)++) = c;
        return;
    }

    if (System_SupportProxy_ready()) {
        System_SupportProxy_putch(c);
    }
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,438; 12-19-2013 19:53:37; /db/ztree/library/trees/xdc/xdc-z63x/src/packages/
 */

