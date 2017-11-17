/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_SENSORI_H
#define PB_SENSORI_H

#include "pb_returncodes.h"
#include "pb_types.h"
#include "pb_session.h"
#include "pb_reader.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Callback for notifying that a reader has been inserted to or 
  * removed from the system (e.g. the PC). 
  *
  * @param[in] session is the session object.
  * @param[in] reader is the reader that has been inserted or
  *     removed.
  * @param[in] inserted tells if the reader has been inserted
  *     (1) or removed (0).
  * @param[in] context is the context set when registering the
  *     callback.
  */
typedef void pb_sensorI_plugnplay_callback_fn_t (pb_session_t* session,
                                                 pb_reader_t* reader,
                                                 int inserted,
                                                 void* context);

/** Starts a sensor session. Must be called before calling any
  * other sensor interface functions.
  *
  * @param[in] session is the session object.
  * @param[in] plugnplay_callback is the callback to be called
  *     to notify that a reader has been inserted or removed
  *     from the system. May be set to 0 if no notifications
  *     are needed.
  * @param[in] plugnplay_context is the context that will be 
  *     passed to the callback. May be set to 0.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_sensorI_start_session_fn_t (pb_session_t* session, 
                                               pb_sensorI_plugnplay_callback_fn_t* plugnplay_callback,
                                               void* plugnplay_context);

/** Ends a sensor session. Deletes all readers associated with 
  * the sensor, which means that any reader returned by 
  * list_readers may no longer be used.
  *
  * @param[in] session is the session object.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_sensorI_end_session_fn_t (pb_session_t* session);

/** Returns the number of currently available readers for
  * this type of sensor. 
  *
  * @param[in] session is the session object.
  * @param[out] nbr_of_readers is the returned number of readers.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_sensorI_get_nbr_of_readers_fn_t (pb_session_t* session,
                                                    uint16_t* nbr_of_readers);

/** Returns an array of the currently available readers for this
  * type of sensor. 
  *
  * @param[in] session is the session object.
  * @param[out] readers is the returned array of readers. The caller is 
  *     responsible for allocating the array and for deallocating the returned
  *     readers.
  * @param[in,out] nbr_of_readers is the requested maximum number of readers to 
  *     be returned. Will be set to the actual number of returned readers, in 
  *     case there was not as many readers available as requested.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
typedef pb_rc_t pb_sensorI_list_readers_fn_t (pb_session_t* session,
                                              pb_reader_t* readers[],
                                              uint16_t* nbr_of_readers);

/** The sensor interface allows different sensors to be implemented.
  * 
  * A system may have one ore more sensor implementations. Each implementation 
  * handles one or more types of sensors.
  * 
  * The sensors handled by one implementation is handled via the sensor 
  * interface but also via the common reader object (pb_reader_t).
  * 
  * While the reader object is used for the process of capturing images the 
  * sensor interface is used to initilize the sensors and act as a factory to 
  * create reader objects.
  * 
  * Since the reader object itself does not know how to implement the capture of 
  * images for various images, each created reader object must delegate this to 
  * the corresponding sensor implementation. This is managed via the 
  * pb_reader_create() function specifying the implementation of 
  * pb_reader_captureI interface to be used for each reader.
  * 
  * Since each sensor implementation is responsible for creating the reader 
  * objects they can pass in the corresponding functions for the capture 
  * functions. This way a reader object abstracts the details of the capturing 
  * process.
  * 
  * This means that a typical sensor implementation needs to implement both this 
  * interface and the pb_reader_captureI interface.
  *
  * Sensor session
  *
  * A sensor implementation remains completely idle until it is started in what 
  * is called a sensor session. During a sensor session the sensor 
  * implementation will be able to list and manage sensors and the corresponding 
  * reader objects, discover new and detached sensors etc.
  * 
  * At the end of a sensor session all resources are released and this also 
  * means that any created reader objects are unusable (calling any methods on 
  * a released reader will result in an error code).
  * 
  * A session may last for a long time, possible throughout the lifecycle of the 
  * running application. During this period new readers may be connected and can 
  * be discovered by listing readers again or by using the plug and play 
  * notification. Readers may also be disconnected and reconnected during the 
  * session. Reconnected readers can be identified via the unique reader id, 
  * retrieved by calling pb_reader_get_id.
  * 
  * It is not required to actually implement the plug and play support in a 
  * sensor implementation, e.g. if there only is one fixed sensor in an
  * embedded system there is no point in such a function.
  */
typedef struct {
    pb_sensorI_start_session_fn_t* start_session;
    pb_sensorI_end_session_fn_t* end_session;
    pb_sensorI_get_nbr_of_readers_fn_t* get_nbr_of_readers;
    pb_sensorI_list_readers_fn_t* list_readers;
} pb_sensorI;

#ifdef __cplusplus
}
#endif

#endif /* PB_SENSORI_H */
