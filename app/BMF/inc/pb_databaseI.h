/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_DATABASEI_H
#define PB_DATABASEI_H

#include "pb_returncodes.h"
#include "pb_session.h"
#include "pb_template.h"
#include "pb_finger.h"
#include "pb_iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Starts a new session. This function must be called before any
  * other function of the module is called. It gives the database
  * implementation a chance to allocate resources and initialize before
  * usage.
  *
  * @param[in] session is the session object.
  *
  * @return PB_RC_OK if properly initialized and ready for use.
  */
typedef pb_rc_t pb_databaseI_start_session_fn_t (pb_session_t* session);

/** Ends the session. Gives the database implementation a chance to
  * free allocated resources and terminate in a controlled manner.
  *
  * @param[in] session is the session object.
  *
  * @return PB_RC_OK if successful, or an error code.
  */
typedef pb_rc_t pb_databaseI_end_session_fn_t (pb_session_t* session);

/** Inserts a template of the specified finger in the database. If a template 
  * for the finger already exists it will be overwritten by the new template.
  *
  * @param[in] session is the session object. 
  * @param[in] template_ is the template to be inserted in the database. The 
  *     database will not inherite the ownership of the template so the caller
  *     must still delete the template. The database implementation chooses if
  *     it makes a copy of the template or just retains it.
  * @param[in] finger is the finger associated with the template. Regarding
  *     ownership the same goes for the finger as for the template, see above.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_databaseI_insert_template_fn_t (pb_session_t* session,
                                                   pb_template_t* template_, 
                                                   pb_finger_t* finger);

/** Finds a template belonging to the specified finger in the database and 
  * returns it.
  *
  * @param[in] session is the session object. 
  * @param[in] finger is the finger associated with the 
  *     template.
  * @param[out] template_ is the returned template from 
  *     the database. The caller is responsible of 
  *     deleting the returned template.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_databaseI_get_template_fn_t (pb_session_t* session,
                                                const pb_finger_t* finger, 
                                                pb_template_t** template_);

/** Deletes a previously inserted template belonging to the specified
  * finger from the database. 
  *
  * @param[in] session is the session object. 
  * @param[in] finger is the finger associated with the 
  *     template.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_databaseI_delete_template_fn_t (pb_session_t* session,
                                                   const pb_finger_t* finger);

/** Returns an finger iterator that may be used to iterate through all the 
  * enrolled fingers in the database. The user shall call pb_iterator_delete on 
  * the iterator when it is not needed anymore. The user shall also call 
  * pb_finger_delete for each iterated finger.
  *
  * Note: If the user calls insert_template or delete_template between calling 
  * get_finger_iterator and pb_iterator_delete, the behaviour of the iterator 
  * is undefined (implementation dependent). Hence, it is recommended not 
  * calling insert_template or delete_template while iterating.
  *
  * Usage: 
  * ...
  * status = my_database.get_finger_iterator (session, &my_finger_iterator);
  * if (status == PB_RC_OK) {
  *     pb_finger_t* finger = pb_iterator_next (my_finger_iterator);
  *
  *     while (finger) {
  *         // Do something with finger...         
  *         ...
  *         pb_finger_delete (finger);
  *         finger = pb_iterator_next (my_finger_iterator);
  *     }
  *     pb_iterator_delete (my_finger_iterator);
  * }
  *
  * @param[in] session is the session object. 
  * @param[out] finger_iterator is the returned finger 
  *     iterator. 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_databaseI_get_finger_iterator_fn_t (pb_session_t* session,
                                                       pb_iterator_t** finger_iterator);

/** The types of events that may occur for a database. These events will trigger
  * a call to any register listener. */
typedef enum pb_databaseI_event_e {
    /** A template has been inserted to the database. */
    PB_DATABASE_EVENT_TEMPLATE_INSERTED = 0,
    /** A template has been deleted from the database. */
    PB_DATABASE_EVENT_TEMPLATE_DELETED
} pb_databaseI_event_t;

/** Function callback for a database listener. 
  * 
  * @param[in] session is the session object. 
  * @param[in] event_ tells what kind of event that has happened.
  * @param[in] template_ is the template that has been added
  *     or removed.
  * @param[in] finger is the associated finger of the template.
  * @param[in] context is the context set when registering the 
  *     listeners.
  */
typedef void pb_databaseI_listener_fn_t (pb_session_t* session,
                                         pb_databaseI_event_t event_,
                                         const pb_template_t* template_,
                                         const pb_finger_t* finger,
                                         const void* context);

/** Registers a listener to the database. The listener will be called each time 
  * any of the specified database events (see pb_databaseI_event_t) occurs.
  *
  * @param[in] session is the session object. 
  * @param[in] listener is the listener to be registered. 
  * @param[in] context is the context that will be passed to 
  *     the callbacks. 
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_databaseI_register_listener_fn_t (pb_session_t* session,
                                                     pb_databaseI_listener_fn_t* listener,
                                                     const void* context);

/** Unregisters a listener from the database.  
  *
  * @param[in] session is the session object. 
  * @param[in] listener is the listener to be unregistered.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_databaseI_unregister_listener_fn_t (pb_session_t* session,
                                                       pb_databaseI_listener_fn_t* listener);

/** The purpose of the database interface is to allow different
  * kinds of data storage based on the needs of the application.
  * 
  * The sole purpose of the database is to store and manage templates of
  * enrolled fingers.
  * 
  * The database implemenation can be as simple as storing data in
  * files or using an existing SQL database. 
  */
typedef struct {
    pb_databaseI_start_session_fn_t* start_session;
    pb_databaseI_end_session_fn_t* end_session;
    pb_databaseI_insert_template_fn_t* insert_template;
    pb_databaseI_get_template_fn_t* get_template;
    pb_databaseI_delete_template_fn_t* delete_template;
    pb_databaseI_get_finger_iterator_fn_t* get_finger_iterator;
    pb_databaseI_register_listener_fn_t* register_listener;
    pb_databaseI_unregister_listener_fn_t* unregister_listener;
} pb_databaseI;

#ifdef __cplusplus
}
#endif

#endif /* PB_DATABASEI_H */
