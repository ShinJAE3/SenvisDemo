/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

/*****************************************************************************
 *
 * pberrno.h
 *
 * Description:
 *  This file contains definitions of the error codes used in the internal
 *  modules of the Precise Biometrics software. The error codes are specified
 *  in MAG/SPC/0037/PB.
 *
 ****************************************************************************/
#ifndef HEADER_PBERRNO_H
#define HEADER_PBERRNO_H

/** The function returned without errors. */
#define PBI_EOK               0


/** At least one buffer has an incorrect size. */
#define PBI_EBUFFER           1

/** The function returned because the caller canceled it. */
#define PBI_ECANCEL           2

/** An undefined fatal error has occurred. This error code is used for
 *  errors that "cannot happen" and isn't covered by any other error code. */
#define PBI_EFATAL            3

/** The BIR is corrupt or not recognized as a BIR of the correct type. */
#define PBI_EBIR              4

/** The data passed to the function is not of the correct format. */
#define PBI_EDATA             5

/** The reader handle does not represent any connected reader. */
#define PBI_EREADER           6

/** The session handle does not represent any open session. */
#define PBI_ESESSION          7

/** File error. (e.g. "no such file", etc.) */
#define PBI_EFILE             8

/** Cannot allocate enough memory. */
#define PBI_EMEMORY           9

/** There is no smart card in the reader. */
#define PBI_ESMARTCARD       10

/** The caller requested a version of a structure, interface etc. that is
 *  not supported by the implementation. */
#define PBI_EVERSION         11

/** A function is called before the interface being initialized. */
#define PBI_EINIT            12

/** The requested operation is not supported by the implementation. */
#define PBI_ESUPPORT         13

/** At least one of the parameters is invalid. */
#define PBI_EPARAMETER       14

/** The reader is busy (only used by GUI components). */
#define PBI_EBUSY            15

/** The operation timed-out before it could finish the operation. */
#define PBI_ETIMEOUT         16

/** The attribute is read-only. */
#define PBI_EREADONLY        17

/** There is no such attribute for this object type. */
#define PBI_EATTRIBUTE       18

/** The operation is not permitted (such as reading verification data out
 *  of a secure reader, or trying to read from a token without providing
 *  the correct read key.) */
#define PBI_EPERMISSION      19

/** The handle passed to the function is not correct. */
#define PBI_EHANDLE          20

/** Communication error. */
#define PBI_ECOMMUNICATION   21

/** Sent by the BioCore 2 at start up. */
#define PBI_ESTARTUP         22

/** Poor quality. */
#define PBI_EQUALITY         23

/** To few minutiae found by extractor. */
#define PBI_ETOOFEWMINUTIAE  24

/* MOC specific return codes. */

/** To small transient RAM to complete the operation. */
#define PBI_ELOWRAM          25

/** To small persistent memory to complete the operation. */
#define PBI_ELOWROM          26

/** Callback to write to persistent memory reported failure. */
#define PBI_EWRITEPERSISTENT 27

/** Persistent enrolled template invalid */
#define PBI_EPERSISTENTINVALID  28

/** Transient buffer not properly aligned. On architectures using
 *  16 bit pointer arithmetics the buffer cannot straddle a 64KB
 *  boundary. The start of the transient buffer must also be aligned
 *  to handle any of the types (char, short or int). */
#define PBI_ETRANSIENTALIGN  29

/** Persistent buffer not properly aligned. On architectures using
 *  16 bit pointer arithmetics the buffer cannot straddle a 64KB
 *  boundary. The start of the transient buffer must also be aligned
 *  to handle any of the types (char, short or int). */
#define PBI_EPERSISTENTALIGN 32


#if defined(_WIN32)
#ifdef PBTRIAL
/* Sent by the trial control version if trial is not valid. */
#define PBI_ETRIAL              30

/* Sent by the trial control version if a new trial is found. */
#define PBI_ENEWTRIAL           31
#endif
#endif

#endif  /* HEADER_PBERRNO_H */
