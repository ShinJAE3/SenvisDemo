/*
 * Copyright (c) 2008 - 2015, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_FINGER_H
#define PB_FINGER_H

#include "pb_types.h"
#include "pb_user.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Note! It is the callers responsibility to make sure that 
 * all finger input parameters are valid, i.e. not null (0). 
 * Passing a null finger parameter to a function will result 
 * in a runtime error! */

/** Finger positions. */
typedef enum pb_finger_position_e {
    PB_FINGER_POSITION_UNKNOWN      = 0,
    PB_FINGER_POSITION_RIGHT_THUMB  = 1,
    PB_FINGER_POSITION_RIGHT_INDEX  = 2,
    PB_FINGER_POSITION_RIGHT_MIDDLE = 3,
    PB_FINGER_POSITION_RIGHT_RING   = 4,
    PB_FINGER_POSITION_RIGHT_LITTLE = 5,
    PB_FINGER_POSITION_LEFT_THUMB   = 6,
    PB_FINGER_POSITION_LEFT_INDEX   = 7,
    PB_FINGER_POSITION_LEFT_MIDDLE  = 8,
    PB_FINGER_POSITION_LEFT_RING    = 9,
    PB_FINGER_POSITION_LEFT_LITTLE  = 10
} pb_finger_position_t;

/** Undefined acquisition. */
#define PB_ACQUISITION_UNKNOWN            0

/** A finger object represents the identity of an individual
  * finger of a certain user. A finger is always related to
  * one user but a user may have several fingers relating to
  * it. Once created the object is immutable.
  *
  * More generally a finger object may represent a finger
  * acquisition object. A finger acquisition is a finger
  * object with an additional acquisition ID. 
  */
typedef struct pb_finger_st pb_finger_t;

/** An anonymous finger. May be used e.g. when a finger is required
  * but finger and user id is not relevant. The constant may be
  * deleted but is special since it will never be destroyed.
  */
extern pb_finger_t* PB_FINGER_ANONYMOUS;

/** Allocates a finger object and sets the reference counter to 1. The finger 
  * will be uniquely defined by it's user and the finger position. The finger 
  * will call _retain on the user which means that it is safe for the caller to 
  * call _delete on the user immediately after this call.
  * 
  * @param[in] position is the finger postion, see 
  *     PB_FINGER_POSITION_X. 
  * @param[in] user is the user of the finger. Use PB_USER_ANONYMOUS
  *     if user is unknown (anonymous). 
  *
  * @return the allocated finger object if successful, else 0. 
  */
pb_finger_t* 
pb_finger_create (pb_finger_position_t position, 
                  pb_user_t* user);

/** Allocates a finger object and sets the reference counter to 1. The finger 
  * will be uniquely defined by it's user, the finger position and the 
  * acquisition. The finger will call _retain on the user which means that it 
  * is safe for the caller to call _delete on the user immediately after this 
  * call.
  * 
  * @param[in] position is the finger postion, see 
  *     PB_FINGER_POSITION_X. 
  * @param[in] user is the user of the finger. Use PB_USER_ANONYMOUS
  *     if user is unknown (anonymous). 
  * @param[in] aqusition is the acquisition of the finger, 
  *     PB_ACQUISITION_UNKNOWN (0) means that value is unknown. 
  *
  * @return the allocated finger object if successful, else 0. 
  */
pb_finger_t* 
pb_finger_create_acquisition (pb_finger_position_t position, 
                              pb_user_t* user,
                              uint32_t acquisition);

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_finger_t*
pb_finger_retain (pb_finger_t* finger);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted. 
  * When deleted, it will also call _delete on the user object, since _retain 
  * was called on the user object when the finger was created.
  * Deleting a null pointer has no effect. */
void
pb_finger_delete (pb_finger_t* finger);

/** Creates a copy of the finger. Will also make a copy
  * of the user of the finger. */
pb_finger_t* 
pb_finger_copy (const pb_finger_t* finger);

/** Tells if finger 1 is a copy of finger 2. */
int
pb_finger_is_copy (const pb_finger_t* finger1, 
                   const pb_finger_t* finger2);

/** Returns the finger position. */
pb_finger_position_t
pb_finger_get_position (const pb_finger_t* finger);

/** Returns a unique finger id. As long as two fingers
  * have different user ids and finger positions their
  * finger ids will differ. */
uint64_t
pb_finger_get_id (const pb_finger_t* finger);

/** Returns the user of the finger. */
pb_user_t*
pb_finger_get_user (const pb_finger_t* finger);

/** Returns the user id of the user of the finger. 
  * Note that there might not be any associated user, 
  * in which case this function will return 0. */
uint32_t
pb_finger_get_user_id (const pb_finger_t* finger);

/** Returns the acquisition of the finger, or 
  * PB_ACQUISITION_UNKNOWN if undefined. */
uint32_t
pb_finger_get_acquisition (const pb_finger_t* finger);

#ifdef __cplusplus
}
#endif

#endif /* PB_FINGER_H */
