/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2012 Jonathan Toppins <jtoppins@users.sourceforge.net>
 *  Copyright (c) 2008-2013 by Cisco Systems, Inc
 *  Copyright (c) 2021-2022 by Intel Corp
 */

#ifndef __SAFE_M_LIB_H__
#define __SAFE_M_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "str/safe_types.h"
#include "str/safe_lib_errno.h"

/* C11 appendix K types - specific for bounds checking */
typedef size_t  rsize_t;

#define LEN ( 128 )

/*
 * We depart from the standard and allow memory and string operations to
 * have different max sizes. See the respective safe_mem_lib.h or
 * safe_str_lib.h files.
 */
/* #define RSIZE_MAX (~(rsize_t)0)  - leave here for completeness */

typedef void (*constraint_handler_t) (const char * /* msg */,
                                      void *       /* ptr */,
                                      errno_t      /* error */);

extern void abort_handler_s(const char *msg, void *ptr, errno_t error);
extern void ignore_handler_s(const char *msg, void *ptr, errno_t error);

#define sl_default_handler ignore_handler_s

#include "safe_mem_lib.h"

#ifdef __cplusplus
}
#endif
#endif /* __SAFE_LIB_H__ */
