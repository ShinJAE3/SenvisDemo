/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_IDENTIFIER_AFIS_H
#define PB_IDENTIFIER_AFIS_H

#include "pb_identifierI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The AFIS identifier. This identifier is a further development of the ISO
  * Compact Card verifier. It is capable of matching ~100000 fingerprints in 
  * a second. 
  *
  * The AFIS identifier implementation is dependent on the LinkedClusterAFIS 
  * component that can be found in the BMF_SupportLibrary. */
extern pbif_const pb_identifierI afis_identifier;


/** The configuration parameters for the identifier. */
typedef struct pb_identifier_afis_config_st {
    /** The maximum number of minutiae used for each template. Using a larger 
      * number of minutiae improves biometric performance but increases the 
      * identification time. The average amount of minutiae present in a 
      * fingerprint depending on the image size is shown below. Default 32. 
      *
      *     Image size (@ 500dpi)   The average number of minutiae
      *     250x350 (full size)     40
      *     200x200                 18
      *     150x150                 10
      *     200x100                  9
      *
      * Doubling the maximum number of minutiae will quadruple the identification 
      * time. Note that this is only true for values less than or equal to the 
      * average number of minutiae. Going above that will result in that the 
      * majority of the images will not have that many minutiae anyways and the 
      * increase in identification time will decline. 
      */
    uint8_t max_nbr_of_minutiae;

    /** The maximum rotation (in 360/256 degrees) allowed between the enrolled
      * and the verification templates. Accepting a larger rotation improves 
      * usability but increases the identification time. Default 32 (+-45 degrees). 
      *
      * Doubling the maximum rotation will double the identification time. 
      */
    uint8_t max_rotation;

} pb_identifier_afis_config_t;

/** The default configuration parameters. 
  * The default parameters are optimized for good biometric performance with full size
  * (250x350) images. */
static const pb_identifier_afis_config_t pb_identifier_afis_default_config = {32, 32};

/** Set configuration parameters. Note: The default configuration parameters are set 
  * automatically, when creating the identifier. */
void
pb_identifier_afis_set_config (pb_identifier_t* identifier, 
                               const pb_identifier_afis_config_t* config);
#ifdef __cplusplus
}
#endif

#endif /* PB_IDENTIFIER_AFIS_H */
