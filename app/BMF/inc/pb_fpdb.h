/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_FPDB_H
#define PB_FPDB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/* Precise Biometrics Fingerprint Database abstraction.
 *
 * The functions in this module is intended for test application
 * that iterates over one of the many collected fingerprint databases.
 *
 * A fundamental part of such database is that it is composed of
 * many fingerpint images or templates structured like this:
 *
 *    Person #1
 *       Finger #1
 *         Attempt #1   Afile.png
 *         Attempt #2   AnotherFile.png
 *         Attempt #3   ...
 *       Finger #2
 *         Attempt #1   ...
 *         Attempt #2   ...
 *         Attempt #3   ...
 *    Person #2
 *
 * But the details of how this structure is implemented such as naming
 * each file as well as metainformation such as image resolution etc is
 * not standarized.
 *
 * To mitagate this a per database index.txt file is created that allows
 * this module to retrieved and iterate over all items (the resource need
 * not be a PNG file, hence called item instead).
 *
 * The index file should be named index.txt and placed in the base
 * directory of the database. The file need not be called index.txt as
 * long as its full name is provided, but must be stored in base directory.
 *
 * Several versions of the index file may be created featureing only
 * parts of the database, this may e.g. be very usefull when studying a
 * biometric algorithm as only a fraction of the problematic images may
 * be collected in such an index file. This special index file may be
 * placed anywhere in the file system through using the special dbpath
 * attribute.
 *
 * The syntax of the index file is simple and composed of a few types
 * of lines as described below:
 *
 *  ## <attr> = <value>  
 *     Assignes a string value to an attribute. The attribute name
 *     cannot contain whitespace. Whitespace surronding '=' is striped
 *     of
 *   #
 *     Any other line starting with # is just a comment
 *
 *   <int> <int> <int> <int> filename
 *     A line consising of four integers and a string is interpreted
 *     as an item consisting of:
 *         personId    Any numner, typically 1..hundreds
 *         fingerId    A finger id for the person, typicaly 1..10 or so.
 *         fingerType  The finger position type according to ISO 19794-2:2005
 *                     0 = unknown, 1=rigth thumb, 2=rigth index etc..
 *         transId     Also called attempt, typically 1..10 or so
 *                     Note: An attribute named transOrdinal will also
 *                     be synthesized numbering the transactions from 0
 *                     and up.
 *         filename    Relative filename to the items resource file
 *                     which often is a PNG image with the fingerprint.
 *
 * Although an index file can be created manually it is more practical
 * to use some script with regular expression syntax to generate it
 * based on the existing names of the files and add some attributes
 * that are needed.
 *
 * Although this module do not enforce an index file to contain any
 * attributes it is wise to include at least:
 *
 *  name  - A unique name of the database as an identifier, this
 *          means no whitespace or characters not allowed in filenames.
 *          This name should be used whenever there is a need to
 *          state or declare what database is referred to.
 *  itemType
 *        - Describes the item type. Unless this is specified an 
 *          application should assume the file resource is a PNG
 *          file unless other information is available as well.
 *          To explicitly specify PNG file specify "png".
 *          Databases in proprietary format needs some means
 *          of reading / converting the content to a suitable
 *          format, this the hook to let the application know
 *          what is needed.
 *  resolution
 *        - For fingerrint images specify the DPI of the images, if
 *          missing application should assume 500 DPI if not
 *          other information is available as well.
 *          It is assumed than images has uniform resolution
 *          but in case not then two values can be specified
 *          delimeted by ",". Example: 381,508 can be used to
 *          specify 381 DPI horizontal resolution an 508
 *          vertically.
 *
 * For testing it may be practical to create an index file that
 * is not located in the database base directory. In this case
 * the following attribute has special meaning.
 *
 *   dbpath
 *        - Points to the actual datbase location. This attribute
 *          shall normally not be included but is very usefull
 *          when the index file cannot/should not reside in the
 *          database bsae directory, e.g. when evaluationg a
 *          subportion of a large database.
 *
 * 
 * Sample index file
 * -----------------
 * The sample below is for example only, not normative.
 * 
 * # This file contains information about a fingerprint database.
 * # It is intended to help when iterating over all images of a database.
 * #
 * # Database attributes:
 * ## fingerIdsRegistered=0,1,2,3,4,5
 * ## fingerTypesRegistered=0
 * ## idPersonBottom=1
 * ## idPersonTop=12
 * ## idTransactionBottom=0
 * ## idTransactionTop=5
 * ## name=FPC1080AInt01
 * ## nbrOfFingers=72
 * ## nbrOfMatchingFiles=431
 * ## nbrOfPersons=12
 * ## resolution=508
 * #
 * # Columns (tab separated):
 * # Person ID (0 if unknown)
 * #       Finger ID (= Finger Type if unspecified or 0 if unknown/unused)
 * #               Finger Type (according to ISO/IES 19794-2:2005 table 2)
 * #                       Transaction ID (sometimes referred to as "Attempt")
 * #                               Image file relative path
 * #
 * 1       0       0       0       0001/20101220092709_00_0.bir
 * 1       0       0       1       0001/20101220092709_00_1.bir
 * 1       0       0       2       0001/20101220092709_00_2.bir
 * 1       0       0       3       0001/20101220092709_00_3.bir
 * 1       0       0       4       0001/20101220092709_00_4.bir
 * 1       0       0       5       0001/20101220092709_00_5.bir
 * 1       1       0       0       0001/20101220092709_01_0.bir
 * 1       1       0       1       0001/20101220092709_01_1.bir
 * 1       1       0       2       0001/20101220092709_01_2.bir
 * 1       1       0       3       0001/20101220092709_01_3.bir
 * 1       1       0       4       0001/20101220092709_01_4.bir
 * 1       1       0       5       0001/20101220092709_01_5.bir
 * 1       2       0       0       0001/20101220092709_02_0.bir
 * ...
 *
 */

/**
 * Represents any extra values associated with each item.
 * To use this opaque struct declare the struct localy where
 * used.
 */
typedef struct pb_fpdb_extra_st pb_fpdb_extra_t;

/**
 * Holds an attribute name/value pair.
 */
typedef struct {
    char name[64];
    char value[512];
} pb_fpdb_attr_t;

/**
 * Holds a resource item
 */
typedef struct  {
    int personId;
    int fingerId;
    int fingerType; // alias finger position
    int transId;
    int personOrdinal; // person ordinal [0 .. NumPersons[
    int fingerOrdinal; // per person finger ordinal [0 .. NumPersonFingers[
    int transOrdinal;  // per finger transaction ordinal [0 .. NumFingerTrans[
    int fingerNumber;  // unique running person-finger ordinal [0 .. NumFingers[
    char filename[1024];
    // Extra data, externally handled
    pb_fpdb_extra_t* e;
} pb_fpdb_item_t;

/**
 * Holds the entire database index once read
 * This is the principal object that an application
 * holds and passes to the functions in the API.
 */
typedef struct {    
    char dbpath[1024]; // always ends with / 
    int siz_attrs;
    int num_attrs;    
    pb_fpdb_attr_t* attrs;
    int siz_items;
    int num_items;
    pb_fpdb_item_t* items;
    int nPersons;
    int nFingers;
    int nTrans;
    int* index; // Sorted index into items
} pb_fpdb_t;

/**
 * Item iterator allows iterating through all the items
 * of the database using the iterator functions.
 */
typedef struct {
    pb_fpdb_t* fpdb;
    int  current;
    int* index;
} pb_fpdb_iter_t;


/**
 * Reads the index of a database.
 *
 * The filename should be the full filename naming the index.txt file
 * or similar name. The file must be located in the base directory of
 * the database.
 *
 * Although deprecated the filename may also just specify the base
 * diectory of the database and then index.txt is added to the filename.
 *
 * The object returned represents the data in the index file and
 * should be freed using the pb_fpdb_free() to reclaim memory when
 * not used any longer.
 *
 * In case the index file cannot be read or lacks any attributes
 * or items NULL is returned. A message is printed on stderr.
 */
pb_fpdb_t* pb_fpdb_read(const char* filename);

/**
 * Creates an empty database with the given size for attributes
 * and items.
 */
pb_fpdb_t* pb_fpdb_create(int num_attrs, int num_items);

/**
 * Frees allocated resorces
 */
void pb_fpdb_free(pb_fpdb_t* fpdb);

/**
 * Adds an item. The newly added item is returned.
 */
pb_fpdb_item_t* pb_fpdb_add(pb_fpdb_t* fpdb, int personId, int fingerId, int fingerType, int transId);

/**
 * Reads an integer attribute value. In case the attribute is missing
 * or not an integer the supplied default value is returned instead.
 */
int pb_fpdb_getint(pb_fpdb_t* fpdb, const char* name, int def);

/**
 * Reads a string attribute value. In case the attribute is missing
 * the default value is returned instead.
 */
const char* pb_fpdb_getstr(pb_fpdb_t* fpdb, const char* name, const char* def);

/**
 * Returns number of items in database.
 */
int pb_fpdb_numitems(pb_fpdb_t* fpdb);

/**
 * Initializes an iterator over all items in the database sorted by
 * personId, fingerId and transId.
 *
 * Upon initialization the current item is the first item
 * unless there is no items at all. 
 *
 * Hence the return value is that of pb_fpdb_iter_cur().
 *
 */ 
pb_fpdb_item_t* pb_fpdb_iter_init(pb_fpdb_t* fpdb, pb_fpdb_iter_t* iter);

/**
 * Returns the current item of the iterator or 0 if there
 * is no more items left to iterate on.
 */
pb_fpdb_item_t* pb_fpdb_iter_cur(pb_fpdb_iter_t* iter);

/**
 * Advances the iterator to the next item. Returns the
 * new curremt item or NULL if there is no more items.
 */
pb_fpdb_item_t* pb_fpdb_iter_next(pb_fpdb_iter_t* iter);

/**
 * Peaks on the next item without moving to the next
 * item. Returns the next item or NULL if no more items.
 */
pb_fpdb_item_t* pb_fpdb_iter_peak(pb_fpdb_iter_t* iter);

/**
 * Complementary function to retrieve the full filename
 * path to the items resource file as the filename property
 * of the item is just the relative path.
 */
const char* pb_fpdb_iter_fullname(pb_fpdb_iter_t* iter, char buf[512]);

/**
 * Complementary function to return just the basename
 * of the file (name without directory path). Also the
 * file suffix can optionaly be striped of
 */
const char* pb_fpdb_iter_basename(pb_fpdb_iter_t* iter, int strip_suffix, char buf[512]);

/**
 * Complementary function to read the item resource file
 * data. The data is alloced with malloc() and should be
 * freed by the caller. The size of the item is set in the
 * item_size parameter unless NULL is specified.
 */
char* pb_fpdb_iter_read(pb_fpdb_iter_t* iter, size_t* item_size);

/**
 * Numbers all items with an ordinal person/finger/transaction number
 * based on the normal sorted order. The ordinal numbers starts at 0.
 */
void pb_fpdb_number_ordinals(pb_fpdb_t* fpdb);

/**
 * Shuffles the transactions in |range| for all fingers in |fpdb|.
 */
void pb_fpdb_shuffle_each_finger(pb_fpdb_t* fpdb, int* range);

#ifdef __cplusplus
}
#endif

#endif
