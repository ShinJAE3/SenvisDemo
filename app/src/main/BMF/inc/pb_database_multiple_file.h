/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_DATABASE_MULTIPLE_FILE_H
#define PB_DATABASE_MULTIPLE_FILE_H

#include "pb_databaseI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** This database stores multiple templates in multiple files,
  * depending on the finger passed to the enroll_template
  * function. This allows for storing of multiple templates per user
  * or finger and storing of templates from different users.
  *
  * The database files are stored in the current working directory
  * unless configured with multiple_file_database_init().
  *
  * Currently the implementation does not support more than
  * one concurrent database session.
  *
  * NOTE: The index file of all templates is saved only at the
  * end of the database session even though the templates
  * for each fingers are save (this should be fixed).
  */
extern pbif_const pb_databaseI multiple_file_database;

typedef struct {
    const char* db_path;
} multiple_file_database_conf_t;

/** Optionally configures the database directory.
 *
 * Configuration of the database must be done before start_session()
 * is called. If the directory does not exist it is created as long
 * as all parent directories exist.
 */
pb_rc_t multiple_file_database_init(pb_session_t* session, 
                                    const multiple_file_database_conf_t* conf);

#ifdef __cplusplus
}
#endif

#endif /* PB_DATABASE_MULTIPLE_FILE_H */
