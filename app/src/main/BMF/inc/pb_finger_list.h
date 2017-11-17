/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_FINGER_LIST_H
#define PB_FINGER_LIST_H

#include "pb_types.h"
#include "pb_finger.h"
#include "pb_iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DEPRECATED!
 * Note that the full finger_list API is deprecated. 
 * It might be removed in future releases without further notice.
 */
 
/* Note! It is the callers responsibility to make sure that 
 * all finger_list input parameters are valid, i.e. not null (0). 
 * Passing a null finger_list parameter to a function will result 
 * in a runtime error! */

/** A finger list object. */
typedef struct pb_finger_list_st pb_finger_list_t;

/** Deprecated!
  * Allocates a finger list object and sets the reference counter to 1. 
  *
  * @return the allocated finger list object if successful, else 0. 
  */
pb_finger_list_t* 
pb_finger_list_create (void);

/** Deprecated!
  * Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_finger_list_t*
pb_finger_list_retain (pb_finger_list_t* finger_list);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted. Deleting a null
  * pointer has no effect. */
void
pb_finger_list_delete (pb_finger_list_t* finger_list);

/** Deprecated!
  * Inserts a finger into the list. The finger list will retain the finger. */
void
pb_finger_list_insert_finger (pb_finger_list_t* finger_list, 
                              pb_finger_t* finger);

/** Deprecated!
  * Tells if the finger list contains the specified finger. */
int
pb_finger_list_contains_finger (const pb_finger_list_t* finger_list, 
                                const pb_finger_t* finger);

/** Deprecated!
  * Removes a finger from the list. */
void
pb_finger_list_remove_finger (pb_finger_list_t* finger_list, 
                              const pb_finger_t* finger);

/** Deprecated!
  * Returns an iterator for the finger list. The iterator returned will start
  * at the first finger in the list. It is the callers responsability to delete
  * the iterator when not needed anymore. */
pb_iterator_t*
pb_finger_list_get_iterator (const pb_finger_list_t* finger_list);

#ifdef __cplusplus
}
#endif

#endif /* PB_FINGER_LIST_H */
