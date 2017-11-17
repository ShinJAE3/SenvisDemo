/*
 * Copyright (c) 2008 - 2014, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_PRODUCT_INFORMATION_H
#define PB_PRODUCT_INFORMATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns the product number. The product number
 * is a string of letters and numbers which uniquely identify
 * the product that build the library.
 */
const char* pb_prod_number(void);

/**
 * Returns the product revision. The revision is a string of the form
 * R2A01/4 where major number is 2, minor is A, release candidate
 * number is 01 and patch is 4. The letter R indicates an official
 * release whereas a P means an inofficial, preliminary, build.
 * Inofficial builds also always use major number 0.
 **/
const char* pb_prod_revision(void);

/**
 * Returns revison number in decimal notation. The number contains
 * four parts each occupying one byte. For the revision R2A01/4
 * the following describes the content.
 *    MSB   major   (2)
 *          minor   (0, A=first)
 *          release (1)
 *    LSB   patch   (4)
 *
 * See also the PB_PROD_MAJOR, _MINOR, ... macros.
 */
unsigned int pb_prod_rev(void);

/**
 * Macros for extracting the distinctive parts from the
 * product revision.
 */
#define PB_PROD_MAJOR(rev) (((rev)>>24)&0xFF)
#define PB_PROD_MINOR(rev) (((rev)>>16)&0xFF)
#define PB_PROD_RELEA(rev) (((rev)>>8)&0xFF)
#define PB_PROD_PATCH(rev) (((rev)>>0)&0xFF)

/**
 * Macros for compile time testing of encoded product revision in
 * header files. Note that these macros evaluate at compile time while
 * the pb_prod_rev() gives the version of the actually linked library.
 *
 * These macros may e.g. be used to handle API changes in different
 * releases by using preprocessor directives like this:
 *
 * #if (PB_PROD_REV_MAJOR == 1 && PB_PROD_REV_MINOR <= 3)
 *   .... Old stuff.
 * #else
 *   .... New stuff
 * #endif
 *
 * For released products the release and patch numbers should not be
 * significant and may be better not to be tested against.
 *
 */
#define PB_PROD_REV() PB_PROD_DECREV_TOREV4(PB_PROD_DECREV)
#define PB_PROD_REV_MAJOR (((PB_PROD_REV())>>24)&0xFF)
#define PB_PROD_REV_MINOR (((PB_PROD_REV())>>16)&0xFF)
#define PB_PROD_REV_RELEA (((PB_PROD_REV())>>8)&0xFF)
#define PB_PROD_REV_PATCH (((PB_PROD_REV())>>0)&0xFF)

#define PB_PROD_DECREV_TOREV4(x) PB_PROD_DECREV_TOREV(x)
#define PB_PROD_DECREV_TOREV(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|(d))

#ifndef PB_PROD_DECREV
# define PB_PROD_DECREV 7,0,0,0
#endif

#ifdef __cplusplus
}
#endif

#endif /* PB_PRODUCT_INFORMATION_H */

