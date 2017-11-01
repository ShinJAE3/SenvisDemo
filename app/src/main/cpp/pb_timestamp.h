/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_TIMESTAMP_H
#define PB_TIMESTAMP_H

/**
 * This file declares types and functions used to handle timestamps and
 * difference in wall time between two timestamps (occations in time). 
 * 
 * Since high resolution measurement of wall time is not handled by ANSI C, 
 * platform specific implementations are possible. What implementation is 
 * used is controlled by defines, e.g. PB_USE_ANSI_C_TIME and 
 * PB_USE_POSIX_TIME which are implemented in pb_timestamp.c. You should 
 * include pb_timestamp.c in your project even if not using any of these 
 * implementations since it contains also some platform independent function
 * definitions.
 *
 * The following implementations are included (identified by corresponding
 * define that must be set when building):
 * 
 * - PB_USE_ANSI_C_TIME
 *
 *   The time() function of ANSI C allows 1s resolution only.
 *   If that is sufficient, this implementation can be used on most platforms.
 *   NOTE: The clock() function of ANSI C gives the processor time and not
 *         wall time. So although this clock often offers better resolution,
 *         it's not suitable in this context.
 *
 * - PB_USE_POSIX_TIME
 *
 *   Only avaliable on POSIX compliant platforms. A good choice if available. 
 *
 * - PB_USE_WIN_SYSTEM_TIME
 *
 *   Uses the GetTickCount () Windows function, that returns elapsed
 *   milliseconds in a 32bit value sice the system was started. The resolution
 *   is limited to the resolution of the system timer, which is typically in 
 *   the range of 10 milliseconds to 16 milliseconds.
 *
 *   Note! The timer will wrap after 49,7 days, but this is taking care of by
 *   the pb_timestamp_diff_x functions. 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "pbtypes.h"

/* Common type to represent a timestamp.
 * Note that the meaning of sec and usec may be platform specific, e.g.
 * they may refer to time elapsed since Jan 1st 1970 or since computer
 * started or since the last birthday of the almighty programmer who
 * invented the platform. */
typedef struct {
	long int sec;
	long int usec;
} pb_timestamp_t;


/*
 * Platform independent declarations.
 * Note: Defintions of these functions are platform dependent and go in
 *       pb_timestamp.c (or any other file you include in your project).
 */

/**
  * Get a timestamp.
  * @param ts A timestamp of a platform specific type to be filled in.
  */
void pb_timestamp_now (pb_timestamp_t *ts);

/**
  * Set the timestamp value to correspond to "ancient time", or at least some
  * time that is guaranteed to have occured in the past (like Jan 1 1970 0:00).
  * @param ts A timestamp of a platform specific type to be filled in.
  */
void pb_timestamp_ancient (pb_timestamp_t *ts);

/**
  * Compare two timestamps (ts2 - ts1) and get the difference in milliseconds.
  * If ts2 is newer than ts1 the result is a positive number.
  * Note: The clock resolution may very well be less than one ms.
  * Note: The int value will overflow in 596 days. If longer time intervals may
  *       occur, make sure to use pb_timestamp_diff_s() instead.
  * @param ts2 The second (newest) timestamp.
  * @param ts1 The first (oldest) timestamp.
  * @return The difference between ts2 and ts1 in ms.
  */
int32_t pb_timestamp_diff_ms (pb_timestamp_t *ts2, pb_timestamp_t *ts1);

/**
  * Compare two timestamps (ts2 - ts1) and get the difference in milliseconds.
  * If ts2 is newer than ts1 the result is a positive number.
  * Note: The int value will overflow in 68 years. If longer time intervals may
  *       occur (e.g. when comparing against ancient times), you can still find
  *       out which value is larger by using pb_timestamp_is_newer().
  * @param ts2 The second (newest) timestamp.
  * @param ts1 The first (oldest) timestamp.
  * @return The difference between ts2 and ts1 in s.
  */
int32_t pb_timestamp_diff_s (pb_timestamp_t *ts2, pb_timestamp_t *ts1);

/**
  * Compares two timestemps and decides which is the newest one.
  * The purpose of this function as compared to the diff functions, is to be
  * safe against overflows.
  * @param probe The test timestamp.
  * @param reference The "original" timestamp.
  * @return TRUE if probe is newer than reference.
  */
int pb_timestamp_is_newer (pb_timestamp_t *probe, pb_timestamp_t *reference);

/* 
 * Platform independent definitions to the following functions are available in
 * pb_timestamp.c. These implementations do only make use of the previously 
 * defined functions.
 */

/**
  * Compares a given start timestamp with current time to see if
  * more than duration_ms ms have passed.
  * @param start The start time.
  * @param duration_ms The maximum duration before expiring in ms.
  * @return TRUE if the time expired and FASLE otherwise.
  */
int pb_timestamp_expired (pb_timestamp_t *start, int duration_ms);

/**
  * Compares a given start timestamp with current time and reports
  * how many ms have passed.
  * @param start The start time.
  * @return The number of ms passed since start.
  */
int pb_timestamp_since_ms (pb_timestamp_t *start);

/**
  * Compares a given start timestamp with current time and reports
  * how many seconds have passed.
  * @param start The start time.
  * @return The number of seconds passed since start.
  */
int pb_timestamp_since_s (pb_timestamp_t *start);

#ifdef __cplusplus
}
#endif

#endif // PB_TIMESTAMP_H
