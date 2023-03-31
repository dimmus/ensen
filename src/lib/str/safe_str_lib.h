/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2011, 2013 by Cisco Systems, Inc
 */

#ifndef __SAFE_STR_LIB_H__
#define __SAFE_STR_LIB_H__

#include "safe_lib.h"

/*
 * The shortest string is a null string!!
 */
#define RSIZE_MIN_STR      ( 1 )

/* maximum sring length */
#define RSIZE_MAX_STR      ( 4UL << 10 )      /* 4KB */


/* set string constraint handler */
extern constraint_handler_t
set_str_constraint_handler_s(constraint_handler_t handler);

/* string copy */
extern errno_t
strcpy_s(char *dest, rsize_t dmax, const char *src);

/* fitted string copy */
extern errno_t
strncpy_s(char *dest, rsize_t dmax, const char *src, rsize_t slen);

/* string length */
extern rsize_t 
strlen_s(const char *s, size_t maxsize);

/* string length */
extern rsize_t
strnlen_s (const char *s, rsize_t smax);

/* string compare */
extern errno_t
strcmp_s(const char *dest, rsize_t dmax,
         const char *src, int *indicator);

#endif   /* __SAFE_STR_LIB_H__ */
