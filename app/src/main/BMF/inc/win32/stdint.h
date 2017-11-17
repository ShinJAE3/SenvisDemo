/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef _STDINT_H
#define _STDINT_H

/*
 * ISO C9x compliant basic types for Windows VisualC.
 *
 * This file currently only has rudimentary support for the
 * stdint.h file. It can/should be extended when needed to be
 * in accordance with C9x.
 *
 */

#ifndef _MSC_VER 
#error "Use this header only with Microsoft Visual C++ compilers!"
#endif

typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;
typedef signed __int8       int8_t;
typedef signed __int16      int16_t;
typedef signed __int32      int32_t;
typedef signed __int64      int64_t;

typedef unsigned __int8     uint_fast8_t;
typedef unsigned __int16    uint_fast16_t;
typedef unsigned __int32    uint_fast32_t;
typedef unsigned __int64    uint_fast64_t;
typedef signed __int8       int_fast8_t;
typedef signed __int16      int_fast16_t;
typedef signed __int32      int_fast32_t;
typedef signed __int64      int_fast64_t;


#endif
