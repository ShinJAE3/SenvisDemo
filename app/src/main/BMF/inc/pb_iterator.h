/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ITERATOR_H
#define PB_ITERATOR_H

#include "pb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Note! It is the callers responsibility to make sure that 
 * all iterator input parameters are valid, i.e. not null (0). 
 * Passing a null iterator parameter to a function will result 
 * in a runtime error! */

/** A iterator object. */
typedef struct pb_iterator_st pb_iterator_t;

/** The function callback for the next function. */
typedef void* pb_iterator_next_fn_t (void* context);

/** The function callback for the delete_context function. */
typedef void pb_iterator_delete_context_fn_t (void* context);

/** Allocates an iterator object and sets the reference counter to 1. This 
  * object may be used to implement an iterator of specific elements. The user 
  * of the iterator keeps calling next until the last element in the iteration 
  * has been reached, in which case next returns 0.
  * 
  * @param[in] context is a context that may be used by the 
  *     implementations of the next and delete functions.
  * @param[in] next is a function pointer to the implementation
  *     of the next function. 
  * @param[in] delete is a function pointer to the implementation
  *     of the delete_context function. May be set to 0 if no
  *     deallocation of the context is needed.
  *
  * @return the allocated iterator object if successful, else 0. 
  */
pb_iterator_t* 
pb_iterator_create (void* context, 
                    pb_iterator_next_fn_t* next,
                    pb_iterator_delete_context_fn_t* delete_context);

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_iterator_t*
pb_iterator_retain (pb_iterator_t* iterator);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted.
  * When deleted, it will also call the delete_context function, if specified. 
  * Deleting a null pointer has no effect.
  */
void
pb_iterator_delete (pb_iterator_t* iterator);

/** Returns the next element in the iteration, or 0 if no more
  * elements are available. This is implemented by delegating
  * the call to the specified 'next' function of the iterator. 
  *
  * Note that the ownership of (who is responsible for deallocating) 
  * the returned element is undefined in this context and needs to be
  * specified by the implementation. */
void*
pb_iterator_next (pb_iterator_t* iterator);

#ifdef __cplusplus
}
#endif

#endif /* PB_ITERATOR_H */
