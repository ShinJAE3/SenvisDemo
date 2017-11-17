/*
 * Copyright (c) 2008 - 2014, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_IDENTIFIERI_H
#define PB_IDENTIFIERI_H

#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_identifier.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Allocates an identifier object and sets the reference counter to 1.
*
* @param[in] session is the session object.
* @param[in] nbr_of_worker_threads is the number of worker threads spawned
*     by the identifier to parallelize work. 
*
* @return the allocated identifier object if successful, else 0.
*/
typedef pb_identifier_t* pb_identifierI_create_fn_t(pb_session_t* session,
                                                    uint8_t nbr_of_worker_threads);

/** Decreases the reference counter for the object by 1. If the reference
* counter reaches 0 then the object will also be deleted.
* When deleted, it will release the database of fingers used for identification. */
typedef void pb_identifierI_delete_fn_t(pb_identifier_t* identifier);

/** The identifier object API. */
typedef struct {
    pb_identifierI_create_fn_t* create;
    pb_identifierI_delete_fn_t* delete_;
} pb_identifierI;

#ifdef __cplusplus
}
#endif

#endif /* PB_IDENTIFIERI_H */
