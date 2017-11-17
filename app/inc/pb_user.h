/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_USER_H
#define PB_USER_H

#include "pb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Note! It is the callers responsibility to make sure that 
 * all user input parameters are valid, i.e. not null (0). 
 * Passing a null user parameter to a function will result 
 * in a runtime error! */

/* User genders. */
typedef enum pb_gender_e {
    PB_GENDER_UNKNOWN = 0,
    PB_GENDER_MALE,   
    PB_GENDER_FEMALE
} pb_gender_t;

/* Unknown year of birth of user. */
#define PB_USER_YEAR_OF_BIRTH_UNKNOWN         0


/** A user object represents the entity of an invidual person.
  * A user object is related to 0 or more finger objects but
  * there is no navigability from a user to the fingers. 
  */
typedef struct pb_user_st pb_user_t;

/** An anonymous user. May be used e.g. when creating a finger
  * where the user is unknown (anonymous). The anonymous user 
  * has user id = 0. The constant may be deleted but is special
  * since it will never be destroyed.
  */
extern pb_user_t* PB_USER_ANONYMOUS;

/** Allocates a user object  and sets the reference counter to 1. The user will
  * have unknown year of birth and gender (PB_USER_YEAR_OF_BIRTH_UNKNOWN and 
  * PB_GENDER_UNKNOWN, respectively).
  * 
  * @param[in] id is the id of the user.
  * 
  * @return the allocated user object if successful, else 0. 
  */
pb_user_t* 
pb_user_create (uint32_t id);

/** Allocates a user object and sets the reference counter to 1. 
  * 
  * @param[in] id is the id of the user.
  * @param[in] year_of_birth is the year of birth of the user, in YYYY format,
  *     e.g. 1978. Use PB_USER_YEAR_OF_BIRTH_UNKNOWN if unknown.
  * @param[in] gender is the gender of the user. Use PB_GENDER_UNKNOWN if 
  *     unknown.
  * 
  * @return the allocated user object if successful, else 0. 
  */
pb_user_t* 
pb_user_create_ext (uint32_t id,
                    uint16_t year_of_birth,
                    pb_gender_t gender);

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_user_t*
pb_user_retain (pb_user_t* user);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted. Deleting a null
  * pointer has no effect. */
void
pb_user_delete (pb_user_t* user);

/** Creates a copy of the user. */
pb_user_t*
pb_user_copy (const pb_user_t* user);

/** Tells if user 1 is a copy of user 2. */
int
pb_user_is_copy (const pb_user_t* user1, 
                 const pb_user_t* user2);

/** Returns the user id. */
uint32_t
pb_user_get_id (const pb_user_t* user);

/** Returns the year of birth of the user, in YYYY format. */
uint16_t
pb_user_get_year_of_birth (const pb_user_t* user);

/** Returns the gender of the user. */
pb_gender_t
pb_user_get_gender (const pb_user_t* user);

#ifdef __cplusplus
}
#endif

#endif /* PB_USER_H */
