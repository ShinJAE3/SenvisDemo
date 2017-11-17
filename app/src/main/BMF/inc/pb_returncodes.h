/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_RETURNCODES_H
#define PB_RETURNCODES_H

/* The possible return codes from the PB interfaces. */
typedef enum pb_rc_e {
    /** The function returned without errors. */
    PB_RC_OK = 0,

    /** The requested operation is not supported by the implementation. */
    PB_RC_NOT_SUPPORTED = 1,

    /** At least one of the parameters is invalid. */
    PB_RC_INVALID_PARAMETER = 2,

    /** The data passed to the function has the wrong format. */
    PB_RC_WRONG_DATA_FORMAT = 3,

    /** At least one buffer has an incorrect size. */
    PB_RC_WRONG_BUFFER_SIZE = 4,

    /** A function is called before the interface being initialized. */
    PB_RC_NOT_INITIALIZED = 5,

    /** The requested item was not found. */
    PB_RC_NOT_FOUND = 6,

    /** The function returned because the caller canceled it. */
    PB_RC_CANCELLED = 7,

    /** The operation timed out before it could finish the operation. */
    PB_RC_TIMED_OUT = 8,

    /** Cannot allocate enough memory. */
    PB_RC_MEMORY_ALLOCATION_FAILED = 9,

    /** Unable to open a file. */
    PB_RC_FILE_OPEN_FAILED = 10,
    /** Unable to read from a file. */
    PB_RC_FILE_READ_FAILED = 11,
    /** Unable to write to a file. */
    PB_RC_FILE_WRITE_FAILED = 12,

    /** Reader is not connected or not started. */
    PB_RC_READER_NOT_AVAILABLE = 13,

    /** Reader has been locked by another user. */
    PB_RC_READER_BUSY = 14,

    /** The enrollment failed because none of the images matched each
      * other. This only applies if multiple images are required for
      * enrollment. */
    PB_RC_ENROLLMENT_VERIFICATION_FAILED = 15,

    /** The finger is temporarily blocked due to brute force attack attempt. 
      * As long as the finger is blocked, all verification attempts will fail. */
    PB_RC_FINGER_BLOCKED = 16,

    /** An undefined fatal error has occurred. This error code is used
      * for errors that "cannot happen" and isn't covered by any other
      * error code. */
    PB_RC_FATAL = 17,

    /** The selected sensor type and size is not supported by the
      * verifier to calculate score or make decisions. Choose one
      * of the supported algorithm combinations. */
    PB_RC_SENSOR_NOT_SUPPORTED = 18,

    /** The function has exceeded maximum capacity and cannot accept
     * more data. */
    PB_RC_CAPACITY = 19,

    /** No algorithm as been selected. This is a requirement for newer
     * versions of the API.
     */
    PB_RC_NO_ALGORITHM = 20,

} pb_rc_t;

#endif /* PB_RETURNCODES_H */
