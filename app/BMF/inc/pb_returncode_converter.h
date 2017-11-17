/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_RETURNCODE_CONVERTER_H
#define PB_RETURNCODE_CONVERTER_H

#include "pb_returncodes.h"


#ifdef __cplusplus
extern "C" {
#endif


/** Converts a PBI_E<ERROR> error code to a pb_rc_t error code. */
pb_rc_t
pb_rc_convert_from_pbi (int pbi_error_code);


#ifdef __cplusplus
}
#endif

#endif /* PB_RETURNCODE_CONVERTER_H */
