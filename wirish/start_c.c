/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/*
 * This file is a modified version of a file obtained from
 * CodeSourcery Inc. (now part of Mentor Graphics Corp.), in which the
 * following text appeared:
 *
 * Copyright (c) 2006, 2007 CodeSourcery Inc
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <stddef.h>

extern void __libc_init_array (void);

extern int main (int, char **, char **);

extern void exit (int) __attribute__ ((noreturn, weak));

extern char _data, _edata;
extern char _bss, _ebss;

struct rom_img_cfg {
  long long *img_start;
};

extern char _lm_rom_img_cfgp;

void __attribute__((noreturn)) start_c(void) {
  struct rom_img_cfg *img_cfg = (struct rom_img_cfg*)&_lm_rom_img_cfgp;
  long long *src;
  long long *dst;
  int exit_code;

  /* Initialize .data, if necessary. */
  src = img_cfg->img_start;
  dst = (long long*)&_data;
  if (src != dst) {
    while (dst < (long long*)&_edata) {
      *dst++ = *src++;
    }
  }

  /* Zero .bss. */
  dst = (long long*)&_bss;
  while (dst < (long long*)&_ebss) {
    *dst++ = 0;
  }

  /* Run initializers.  */
  __libc_init_array ();

  exit_code = main (0, NULL, NULL);
  if (exit)
    exit (exit_code);
  /* If exit is NULL, make sure we don't return. */
  for (;;)
    continue;
}
