/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

/*****************************************************************************
 *
 * pbbir.h
 *
 * Description:
 *  This file contains definitions of constants used in the Precise
 *  Biometrics BIR format as specified in TEC/SPC/0005/PB.
 *
 ****************************************************************************/
#ifndef HEADER_PBBIR_H
#define HEADER_PBBIR_H

/* Tags. */
#ifndef PB_BIR_TAG_PAYLOAD
#define PB_BIR_TAG_PAYLOAD                 0xc0
#define PB_BIR_TAG_TEMPLATE                0xc1
#define PB_BIR_TAG_IMAGE                   0xc3
#define PB_BIR_TAG_BIOMETRIC_HEADER        0xc5
#define PB_BIR_TAG_REFERENCE_DATA          0xc6
#define PB_BIR_TAG_VERIFICATION_DATA       0xc7
#define PB_BIR_TAG_FINGER_CONTAINER        0xe3
#define PB_BIR_TAG_BIOMETRIC_SUBTYPE       0xc2
#endif /* PB_BIR_TAG_PAYLOAD */

/* Deprecated TAGs. */
#define PB_BIR_TAG_PRECISE                 0xe1

/* Deprecated TAG names. */
#define PB_BIR_TAG_EFD                     0xc1
#define PB_BIR_TAG_PUBLIC_TEMPLATE         0xc5
#define PB_BIR_TAG_PRIVATE_TEMPLATE        0xc6

/* Finger constants. */
#ifndef PB_BIR_FINGER_LEFT_LITTLE
#define PB_BIR_FINGER_LEFT_LITTLE          54
#define PB_BIR_FINGER_LEFT_RING            50
#define PB_BIR_FINGER_LEFT_MIDDLE          46
#define PB_BIR_FINGER_LEFT_POINTER         42
#define PB_BIR_FINGER_LEFT_THUMB           38
#define PB_BIR_FINGER_RIGHT_THUMB          37
#define PB_BIR_FINGER_RIGHT_POINTER        41
#define PB_BIR_FINGER_RIGHT_MIDDLE         45
#define PB_BIR_FINGER_RIGHT_RING           49
#define PB_BIR_FINGER_RIGHT_LITTLE         53
#endif /* PB_BIR_FINGER_LEFT_LITTLE */

/* Sensor types (images). */
#define PB_BIR_SENSOR_UNKNOWN              0
#define PB_BIR_SENSOR_VERIDICOM_FPS110     1
#define PB_BIR_SENSOR_INFINEON_FT1         2
#define PB_BIR_SENSOR_AUTHENTEC_AES4000    3
#define PB_BIR_SENSOR_POLAROID_PFS100      4
#define PB_BIR_SENSOR_VERIDICOM_FPS200     5
#define PB_BIR_SENSOR_INFINEON_FT2         6
#define PB_BIR_SENSOR_AUTHENTEC_AES3500    7
#define PB_BIR_SENSOR_PAC_BIOCORE_AES4000  8
#define PB_BIR_SENSOR_INFINEON_FT3         9
#define PB_BIR_SENSOR_AUTHENTEC_AFS8500   10
#define PB_BIR_SENSOR_AUTHENTEC_AFS2      11
#define PB_BIR_SENSOR_AUTHENTEC_AES2500   12
#define PB_BIR_SENSOR_ST_TCS1             13
#define PB_BIR_SENSOR_ST_TCS2             14
#define PB_BIR_SENSOR_ST_TCS3B            16
#define PB_BIR_SENSOR_AUTHENTEC_AES2501   17
#define PB_BIR_SENSOR_VALIDITY_VFS101     18
#define PB_BIR_SENSOR_TESTECH_BIOI        19
#define PB_BIR_SENSOR_UPEK_TCS1_TURBO     20
#define PB_BIR_SENSOR_FUJITSU_MBF110       1 /* Same sensor as Veridicom. */
#define PB_BIR_SENSOR_FUJITSU_MBF200       5 /* Same sensor as Veridicom. */
#define PB_BIR_SENSOR_UPEK_TCS3           21
#define PB_BIR_SENSOR_UPEK_TCS4           22
#define PB_BIR_SENSOR_CROSSMATCH_300LC    23
#define PB_BIR_SENSOR_AUTHENTEC_AES2550   24
#define PB_BIR_SENSOR_AUTHENTEC_AES2810   25
#define PB_BIR_SENSOR_UPEK_TCS5           26
#define PB_BIR_SENSOR_UNKNOWN_LIVE_PLAIN       27
#define PB_BIR_SENSOR_UNKNOWN_LIVE_ROLLED      28
#define PB_BIR_SENSOR_UNKNOWN_NONLIVE_PLAIN    29
#define PB_BIR_SENSOR_UNKNOWN_NONLIVE_ROLLED   30
#define PB_BIR_SENSOR_UNKNOWN_LATENT           31
#define PB_BIR_SENSOR_UNKNOWN_SWIPE            32
#define PB_BIR_SENSOR_UNKNOWN_LIVE_CONTACTLESS 33
#define PB_BIR_SENSOR_LUMIDIGM                 34
#define PB_BIR_SENSOR_AUTHENTEC_AES2660        35
/* Sensor types (slice images).  Note that the BIR header should be
 * interpreted slightly differently for these images. */
#define PB_BIR_SENSOR_UNKNOWN_SLICES      64

/* Image encoding. */
#define PB_BIR_ENCODE_BM8                  0
#define PB_BIR_ENCODE_BM1                  1
#define PB_BIR_ENCODE_BM4                  2
#define PB_BIR_ENCODE_BM1REV               3
#define PB_BIR_ENCODE_BM4REV               4

/* Image orientation types. */
#ifndef PB_BIR_ORIENTATION_ROT_0
#define PB_BIR_ORIENTATION_ROT_0           0
#define PB_BIR_ORIENTATION_ROT_90          1
#define PB_BIR_ORIENTATION_ROT_180         2
#define PB_BIR_ORIENTATION_ROT_270         3
#define PB_BIR_ORIENTATION_ROT_0_MIRROR    4
#define PB_BIR_ORIENTATION_ROT_90_MIRROR   5
#define PB_BIR_ORIENTATION_ROT_180_MIRROR  6
#define PB_BIR_ORIENTATION_ROT_270_MIRROR  7
#endif /* PB_BIR_ORIENTATION_ROT_0 */

#endif  /* HEADER_PBBIR_H */
