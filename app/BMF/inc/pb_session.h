/*
 * Copyright (c) 2008 - 2014, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_SESSION_H
#define PB_SESSION_H

#include "pb_types.h"
#include "pb_returncodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Note! It is the callers responsibility to make sure that 
 * all session input parameters are valid, i.e. not null (0). 
 * Passing a null session parameter to a function will result 
 * in a runtime error! */

/* Session properties are used to pass optional parameters to
   modules. Properties can be set on a specific session, or they can
   be set globally. In the global case, they are available to modules
   also when no session is allocated. Local session values have
   priority over global values, if a property is set in both
   contexts. */

/* Session context ids. The ids are split into 3 categories:
 *
 * 1-127    Precise context ids.
 * 128-191  RFU, Reserved for Future Use.
 * 192-255  Application specific context ids, not defined. 
 *
 * There are 4 already defined external application specific context ids.
 * If these are not enough it is possible to use the other values 196-255 as 
 * well.
 *
 * Note: The already defined values must not change. Only new values may be
 * added!
 */
typedef enum pb_context_id_e {
    /** Context id:s for the RawImageSensorModule, _FOLDER specifies the folder 
      * in which the raw images are located, _WIDTH/HEIGHT specifies the size of
      * the raw images. */
    PB_CONTEXT_ID_RAW_IMAGE_FOLDER  = 1,
    PB_CONTEXT_ID_RAW_IMAGE_WIDTH = 4,
    PB_CONTEXT_ID_RAW_IMAGE_HEIGHT = 5,

    /** Context id for the public header (pb_template_t*) for the BioMatch3 
      * extractor. */
    PB_CONTEXT_ID_BIOMATCH3_PUBLIC_HEADER = 2,
    /** Context id for the requested far (pb_far_t) for the BioMatch3 extractor. */
    PB_CONTEXT_ID_BIOMATCH3_REQUESTED_FAR = 3,

    /** Context id:s for the Spectral (Slice) Extractor. */
    PB_CONTEXT_ID_IMAGE_AREA = 6,
    PB_CONTEXT_ID_IMAGE_QUALITY = 7,

    /** Context for session properties. */
    PB_CONTEXT_ID_PROPERTIES = 8,

    /** Context for multitemplate enrollment controller. */
    PB_CONTEXT_ID_MTE_CONTROLLER = 9,

    /** Context for verification controller. */
    PB_CONTEXT_ID_VERIFICATION_CONTROLLER = 10,

    /* Context for algorithm object. */
    PB_CONTEXT_ID_ALGORITHM_OBJECT = 11,

    /** Context ids for external use, application specific. */
    PB_CONTEXT_ID_EXTERNAL1     = 192,
    PB_CONTEXT_ID_EXTERNAL2     = 193,
    PB_CONTEXT_ID_EXTERNAL3     = 194,
    PB_CONTEXT_ID_EXTERNAL4     = 195
} pb_context_id_t;

/* The API:s of the modules are designed to be generic and simple to implement. 
 * But, in some cases additional information may be needed. These special cases 
 * may be solved by adding information to the session object so that a module 
 * or a controller may retrieve that special information later. 
 *
 * The contexts are created by the caller and stored in the session object using
 * key/value pairs, where the context id is the key and the context the value.
 * This also means that there may not be more than one context stored for each 
 * context id. If another context is set with the same context id the old 
 * context will be deleted and replaced by the new one.
 */
typedef struct pb_session_st pb_session_t;

/** Callback for delete of contexts. */
typedef void pb_delete_context_hook_fn (void* context);

/** Allocates a session object and sets the reference counter to 1. */
pb_session_t*
pb_session_create (void);

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_session_t*
pb_session_retain (pb_session_t* session);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted. 
  * When deleted, it will also delete those contexts with a valid delete_context
  * function. Deleting a null pointer has no effect. */
void
pb_session_delete (pb_session_t* session);

/** Returns a context from the session, if a context
  * exists with the specified id. */
void*
pb_session_get_context (const pb_session_t* session,
                        pb_context_id_t context_id);

/** Sets a context in the session. If a context already
  * exist for that context id it will be replaced with 
  * this new context. 
  *
  * @param[in] session is the session object.
  * @param[in] context is the context to set in the session object.
  * @param[in] context_id is the context id of the context, used to distinguish
  *     between different types of contexts, see PB_CONTEXT_ID_X.
  * @param[in] delete_context is a callback for deallocation of the context, 
  *     e.g. when replaced by a newer context (with the same context id) or
  *     when pb_session_delete is called. May be set to 0 if deallocation of 
  *     the context is not needed.
  */
void
pb_session_set_context (pb_session_t* session,
                        void* context,
                        pb_context_id_t context_id,
                        pb_delete_context_hook_fn* delete_context);

/** Removes a context from the session, if a context with the specified id
  * exists. The removed context will also be deleted using the specified
  * callback, if any. */
void
pb_session_remove_context (pb_session_t* session,
                           pb_context_id_t context_id);

/** Clear a specific property in the given session, or in the global
  * context by setting session = 0.
  *
  * @param session is the session object.
  * @param id is the property identifier.
  */
void
pb_session_clear_property(pb_session_t* session, const char* id);

/** Clear all properties in the given session, or in the global
  * context by setting session = 0.
  *
  * @param session is the session object.
  */
void
pb_session_clear_properties(pb_session_t* session);

/** Set an int property in the given session, or in the global context
  * by setting session = 0.
  *
  * @param[in] session is the session object.
  * @param[in] id is the property identifier.
  * @param[in] value is the value to set.
  * @returns PB_RC_OK on success.
  *          PB_RC_MEMORY_ALLOCATION_FAILED if no memory is available.
  *          PB_RC_WRONG_DATA_FORMAT if the property has been set earlier
  *                                  using a different type.
  */
pb_rc_t
pb_session_set_int(pb_session_t* session,
                   const char* id,
                   int value);

/** Get an int property from the given session, or from the global
  * context by setting session = 0. If the property is undefined in
  * the session, the property will be read from the global context.
  *
  * @param[in] session is the session object.
  * @param[in] id is the property identifier.
  * @param[out] default_value is returned when the property is undefined.
  * @returns The int value, or default_value if it is undefined.
  */
int
pb_session_get_int(pb_session_t* session,
                   const char* id,
                   int default_value);



/** Fetches the selected algorithm object from the session.
  *
  * The returned object is a loner and should not be deleted.
  */
struct pb_algorithm_st;
struct pb_algorithm_st* pb_session_get_algorithm(pb_session_t* session);

/** Helper functions that may be used when not using the algorithm API 
  * directly, i.e. when not creating an algorithm object from an 
  * algorithm implementation.
  *
  * NOTE: SEVERAL SETTERS FUNCTIONS ARE NOW DEPRICATED AND
  *       RENAMED AS ALGORITHM API IS NOW REQUIRED. 
  *
  * The get-functions are still supported. The setters have been
  * renamed but should not be used in production code. Instead of
  * using setters use the corresponding algorithm functions.
  */

/** Sets the sensor type in the session object. Depricated in favor of
  * algorithm API but if used will change what
  * pb_session_get_sensor_type() reports. */
void
pb_session_set_sensor_type_ (pb_session_t* session,
                             pb_sensor_type_t sensor_type);
/** Sets the sensor size in the session object.  Depricated but if
  * used will change what pb_session_get_sensor_size() reports. */
void
pb_session_set_sensor_size_ (pb_session_t* session,
                             pb_sensor_size_t sensor_size);

/** Sets 360 support on/off in the session object. Depricated in favor
  * of algorithm API but if used will change what
  * pb_session_get_360_state() reports.
  *
  * Note: Default state will be on, so this function will primarily 
  * be used for disabling the 360 support, but may of course also be
  * used to enable the 360 support again after it has been disabled. */
void
pb_session_set_360_state_ (pb_session_t* session,
                           int state);

/** Set number of slice rows to be used by the slice
  * extractors. Depricated in favor of algorithm API but if used
  * will change what pb_session_get_360_state() reports. */
void
pb_session_set_slice_rows_(pb_session_t* session, int slice_rows);

/** Returns the sensor type from the session object.
  * Delegates to the active algorithm object. */
pb_sensor_type_t
pb_session_get_sensor_type (pb_session_t* session);

/** Returns the sensor type from the session object.
    Delegates to the active algorithm object. */
pb_sensor_size_t
pb_session_get_sensor_size (pb_session_t* session);

/** Returns the state for 360 support from the session object. If
  * state has not been set, the default value returned will be on (1).
  * Delegates to the active algorithm object. */
int
pb_session_get_360_state (pb_session_t* session);

/** Returns number of slice rows used by the slice extractors. */
int
pb_session_get_slice_rows(pb_session_t* session);

#ifdef __cplusplus
}
#endif

#endif /* PB_SESSION_H */
