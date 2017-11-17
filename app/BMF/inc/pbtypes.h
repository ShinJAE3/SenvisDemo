/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

/*****************************************************************************
 *
 * pbtypes.h
 *
 * Description:
 *  This file contains definitions of types that should be used in order to
 *  get the software as portable as possible. It does also contain specific
 *  "Precise Biometrics" types that are common for multiple modules.
 *  The specification of this file is in MAG/SPC/0019/PB.
 *
 ****************************************************************************/
#ifndef HEADER_PBTYPES_H
#define HEADER_PBTYPES_H

/* Definition of size_t. */
#include <stddef.h>

/* Definition of malloc */
#include <stdlib.h>

/* C99 integer types */

#if defined(_MSC_VER) && _MSC_VER < 1600
# include "win32/stdint.h"
#else
# include <stdint.h>
#endif

#ifdef __APPLE__
#ifndef __OBJC__
typedef int BOOL;
#endif

#ifdef UNICODE
typedef wchar_t *LPTSTR;
#else
typedef char *LPTSTR;
#endif
#endif

#ifndef ALLOC_TYPE
#define ALLOC_TYPE(type)    (type*) malloc(sizeof(type))
#else
#error ALLOC_TYPE already defined!
#endif



#ifndef TRUE
#define TRUE       (1 == 1)
#define FALSE      (0 == 1)
#endif /* TRUE */

/* Set character used as delimiter in paths, this differs on different OSes */
#ifndef PATH_DELIMITER_STR
# ifdef WIN32
#  define PATH_DELIMITER_STR    "\\"
#  define PATH_DELIMITER_CHAR   '\\'
# else
#  define PATH_DELIMITER_STR    "/"
#  define PATH_DELIMITER_CHAR   '/'
# endif
#endif /* PATH_DELIMITER_STR */


/*
 * Global Precise types and definitions specified in the "internal layer"
 * (MAG/SPC/0016/PB)
 */
typedef uint32_t  pbi_handle_t;

#define PBI_TRUE   1
#define PBI_FALSE  0

/*
 * Macro to be used in functions that have unused arguments.
 * This is used to suppress compiler warnings and makes it clear that the
 * argument was left unused on purpose.
 */
#define UNUSED_ARGUMENT(x) (void)x

#endif  /* HEADER_PBTYPES_H */
