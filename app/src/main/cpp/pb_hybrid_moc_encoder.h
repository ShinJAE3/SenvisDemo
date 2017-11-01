/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

/*
 * Functions for representing and converting EHM and EHM
 * multi-templates to the Hybrid MOC format that is used by the card
 * matcher.
 *
 * These functions are used on the rich side to configure,
 * enroll and convert verification templates.
 *
 * Some basics about Hybrid MoC
 *
 *   * An Hybrid MOC matcher runs on a smartcard or SE.  See the
 *     pb_hybrid_moc.h header for more details.
 *
 *   * There exist several variations of the matcher on the card. Each
 *     such variant is identified by a library magic number.
 *
 *   * At enrollment and verification traditional BME functions should
 *     be used to create enrollment and verification templates.

 *   * Only the EHM format is supported and is further limited to
 *     basically the XL and L sensor sizes.
 *
 *     There is a usage-model where enrollment can be made using
 *     full sized images (up to 252x348 pixels @ 500 dpi) and
 *     verified using the XL/L sensor sizes. In this usage case the
 *     multi-template size may even be as low as 1 sub-template.
 *
 *     This use-case is less resource demanding compared to
 *     the traditional use-case when it comes to verification time
 *     and template storage sizes.
 *
 *   * Before templates are sent to the card they must be converted
 *     to a format suitable for the card and this conversion differers
 *     based on each library variant as defined by the library magic
 *     number.
 *
 *   * The converting application either needs to query the card for
 *     the magic number or know it by other means such as using a
 *     dedicated APDU command to retrieve it.
 *
 *   * Before enrollment of fingers the card needs to be configured
 *     based on the use-case model and other parameters.  The use-case
 *     model e.g. describes the sensor sizes used and other parameters
 *     define the requested FAR level that will be used during
 *     verification or if full 360-verification should be done or not.
 *     At configuration it needs to be decided how many fingers should
 *     be supported and how large the containers shall be for
 *     representing each finger.
 *
 *   * The matcher does support the concept of multi-finger matching
 *     compared to the traditional card principle of either match
 *     against just one selected finger or match against all in a
 *     sequence.
 *
 *   * Data sent to card for configuration and enrollment is done via
 *     containers. The first container should carry the configuration
 *     and a subsequent container may carry enrollment for finger
 *     1. Later the finger 1 container may be replaced or a container
 *     for finger 2 is added. To the card only one reference template
 *     is seen. To remove an enrolled finger an empty container is
 *     sent to the card.
 *
 *   * During verification one typically match against all enrolled
 *     fingers but one may also specify a set of fingers to match
 *     against.
 *
 *   * Besides retrieving the matchers library magic code a card
 *     should also allow retrieving the metadata describing the
 *     number of containers, there size and which fingers are
 *     enrolled.
 */

#ifndef PB_HYBRID_MOC_ENCODER_H
#define PB_HYBRID_MOC_ENCODER_H

#include "pb_types.h"
#include "pb_returncodes.h"
#include "pb_verifierI.h"
#include "pb_template.h"
#include "pb_finger.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Hybrid MOC matchers comes in a few different variants each is
 * represented with a magic number. Either the magic number is known
 * or an on-card matcher should be asked which magic number it
 * supports. The magic number will affect how templates are encoded
 * before sending them to the card.
 *
 * Processing power and implementation choices determine which
 * library variant is put on the card.
 *
 * Currently the Hybrid MOC only supports sensor sizes of XL and L.
 * 
 * Magic 0x0000
 * ------------
 * This is the legacy first variant of Hybrid MOC. This variant does
 * not use the new container format, dynamic configuration etc.  This
 * variant also did kind of support M-sized rectangular sensors but
 * took very very long time to match so it was practically not
 * feasible.
 *
 * Creating enrollment and verification templates for this variant is
 * still supported (for sensor sizes XL and L) but will likely be
 * removed in future variants.
 *
 * ISO-cc preprocessing is done on card and hence requires more
 * RAM and processing power, similar as as the 0x1000 variant.
 *
 * Magic 0x1000, 0x1100
 * --------------------
 * This variant uses on-card ISO-cc pre-processing step. The benefit
 * of this is that enrollment templates will be a bit smaller but
 * processing time will be longer. This mode requires an additional
 * 1428 bytes of working RAM memory (46 minutiae).
 *
 * It is expected that this variant is used only on more powerful
 * 32-bit smartcards such as the SC300.
 *
  * Magic 0x1001, 0x1101
 * ---------------------
 * This variant expects that ISO pre-processing is done outside the
 * card and included in the enrollment container. Thus enrollment data
 * will be a bit larger but less processing power and RAM memory is
 * needed compared to the 0x1000 variant.
 *
 * This variant uses the large configuration of the ISO matcher which
 * uses up to 64 minutiae. This variant may require up to 2370
 * additional bytes for the pre-processed template, but actual size
 * depends on number of found minutiae.
 *
 * This variant should not be needed unless full-sized images
 * are enrolled.
 *
 * Magic 0x1002, 0x1102
 * --------------------
 * Like the 0x1001 variant but uses a memory conservative
 * configuration of the ISO matcher using only a maximum of 46
 * minutiae. This variant may require up to 1428 additional bytes for
 * the pre-processed template, but actual size depends on number of
 * found minutiae. This should be the default choice for XL or L sized
 * sensors.
 *
 * Typically the number of minutiae on 10x10 mm sized sensor is less
 * that 46 in 95% of all samples (and the high end is likely most
 * false minutiae due to image noise).
 *
 *
 * IMPLEMENTATION NOTE:
 *    Value 0x0000 is for legacy first variant of Hybrid MOC.
 *    it has no config container e.g. Newer variants have
 *    number 0x0100 or larger.
 *
 *    Bits 0-7 (lowest 2 hex digits) are reserved to represent
 *    generic implementation features while bits 7-15 (highest
 *    2 hex digits) are used to separate other variants.
 *
 *    The lowest 2 bits determine pre-processing model.
 *    The 3:rd bit represents which spectral matcher is used.
 *    The 4:th bit, RFU.
 *
 */
typedef enum {
    PB_HMOC_MAGIC_NONE = 0,
    PB_HMOC_MAGIC_0000 = 0x0000,  /* First hmoc variant, On-card ISO-cc preprocessing */
    PB_HMOC_MAGIC_1000 = 0x1000,  /* On-card ISO-cc preprocessing, small config */
    PB_HMOC_MAGIC_1001 = 0x1001,  /* Enrollment preprocessed, large config */
    PB_HMOC_MAGIC_1002 = 0x1002,  /* Enrollment preprocessed, small config */
    PB_HMOC_MAGIC_1100 = 0x1100,  /* Second version of 0x1000. */
    PB_HMOC_MAGIC_1101 = 0x1101,  /* Second version of 0x1001. */
    PB_HMOC_MAGIC_1102 = 0x1102,  /* Second version of 0x1002. */
    PB_HMOC_MAGIC_1108 = 0x1108,  /* 0x1100 with verlimit */
    PB_HMOC_MAGIC_1109 = 0x1109,  /* 0x1101 with verlimit */
    PB_HMOC_MAGIC_110A = 0x110A   /* 0x1102 with verlimit */
} pb_hmoc_magic_t;

/**
 * Hybrid matchers are configured for different sensors and different
 * use-cases and each configuration is denoted by the model. One
 * model may be when enrollment and verification images both are
 * from a 10x10 mm sensor while another when enrollment is from a
 * full-sized image.
 *
 * Different models will generate different data in the configuration
 * sent to the card during personalization - before enrolling any
 * fingers for the card owner.
 */
typedef enum {
    PB_HMOC_MODEL_UNKNOWN,
    PB_HMOC_MODEL_L,        // L sized sensor both for enrollment and verification
    PB_HMOC_MODEL_XL,       // XL sized sensor both for enrollment and verification
    PB_HMOC_MODEL_FULL__L,  // Single image full sized sensor enrolled against L sized
    PB_HMOC_MODEL_FULL__XL  // Single image full sized sensor enrolled against XL sized
    //...
} pb_hmoc_model_t;

/**
 * Hybrid MOC (official) verifier features.
 */
#define PB_HMOC_FEAT_360      (1<<0)  // Do full 360 verification

/**
 * Creates a Hybrid MOC configuration container. The configuration
 * container is be the first container sent to the card during
 * personalization and is typically never updated. This prepares
 * the card for the intended use-case.
 *
 * The configuration container is typically sent as a regular
 * "enrollment" in classical APIs but the container contains only
 * configuration data.
 *
 * To create the correct configuration container the matcher library
 * magic number must be known. Also the use-case model must be specified
 * as this tunes the algorithms to the actual sensor sizes and images
 * used at enrollment and verification time. The selected model also
 * affects e.g. which set of thresholds are used at verification.
 *
 * The type model should be known from the system context.
 *
 * The selected security FAR level that should be used at verifications
 * is also specified here (verification APIs on JavaCards do not include
 * such parameters). Additional verification tuning parameters can
 * be set, see PB_HMOC_FEAT_xxx.
 *
 * The configuration also specifies how many and how large the
 * fingerprint containers should be. When the configuration container
 * is sent to the card during personalization all containers are
 * created at the same time and cannot typically be changed. The
 * size of the fingerprint containers should be selected based on
 * the expected average or maximal template size for the use-case
 * model (single-template size times number of sub-templates).
 *
 * @param lib_magic      The actual library variant on the card
 * @param model          The use-case model reflects which sensors sizes
 *                       and sensor specific tuning are used.
 * @param num_containers Total number of containers including the config
 *                       container. Hence n >= 2 should be given to allow
 *                       at least 1 enrolled finger.
 * @param config_size    The size to reserve for the configuration container,
 *                       i.e. the container created by this function. If
 *                       set to 0 the actual size will be used rounded
 *                       up to even multiple of 16. Note that in some
 *                       cases the size must be an even multiple, this
 *                       will not make the returned container itself larger.
 * @param container_size The size of the fingerprint containers. The selected
 *                       size must be set properly for the use-case
 *                       Note that in some cases the size must be an even
 *                       multiple of e.g 16.
 * @param req_far        The selected security level for single finger
 *                       verification. Note that if enrolling many fingers
 *                       the practical security level will become a bit lower
 *                       in practise as there are several to match against.
 * @param features       Specifies hybrid moc verification features, such as
 *                       enable/disable full 360 degree search. This is a
 *                       bitset combined from PB_HMOC_FEAT_xxx values.
 * @param rfu            (rfu & 0x000000FF) is verlimit. This is the maximum number
 *                       of sub-templates per enrolled finger that will be verified
 *                       for any given match attempt. A low verlimit will speed up
 *                       verification and increase FRR. 0 means no verlimit.
 *                       (rfu & 0xFFFFFF00) reserved for future. Must specify 0.
 * @param container      The resulting configuration container, represented
 *                       as a BMF template object.
 */
pb_rc_t pb_hmoc_encode_conf(
    uint16_t        lib_magic,
    pb_hmoc_model_t model,
    uint16_t        num_containers,
    uint16_t        config_size,
    uint16_t        container_size,
    pb_far_t        req_far,
    uint32_t        features,
    uint32_t        rfu,
    pb_template_t** container);

/**
 * Creates a Hybrid MOC finger enrollment container. The container is
 * created from a multi-template and represents one particular finger
 * (1-10). The container is typically transfered to the card using
 * traditional enrollment APIs and the designated finger is either
 * updated or added by claiming the appropriate container on the card.
 *
 * The multi-template must contain EHM templates and should be of
 * the appropriate model type as specified in the configuration
 * container.
 *
 * @param lib_magic      The actual library variant on the card.
 * @param enr_template   The enrollment multi-template to be encoded.
 * @param finger_code    Identification of which finger container
 *                       should be updated, only values 1-10 allowed.
 * @param max_size       If not 0 this specifies a maximum allowed container
 *                       size. This means that one or more sub-templates
 *                       may be removed to make it fit.
 * @param container      The resulting enrollment container, represented
 *                       as a BMF template object.
 * @param ntemplates     Optional output parameter. If non null it will
 *                       be he number of included sub-templates, this may
 *                       be less than the total number of templates
 *                       due to max_size restriction on the container
 *                       size. This can be of interest so that
 *                       container sized can be tuned.
 */
pb_rc_t pb_hmoc_encode_enr(
    uint16_t                lib_magic,
    const pb_template_t*    enr_template,
    pb_finger_position_t    finger_code,
    uint16_t                max_size,
    pb_template_t**         container,
    uint8_t*                ntemplates);

/**
 * Encodes a single EHM template suited for Hybrid MOC matcher on the
 * card. Different matcher implementations may require different
 * encodings and is indicated via the library magic number.
 *
 * The optional finger mask can be used to select a subset of fingers
 * to match against but is typically set to 0 to match against all
 * enrolled fingers.
 *
 * NOTE 1 - Limit max number of minutiae at extraction.
 *
 *    When extraction of verification template is made it
 *    is recommended to set a limit on max number of minutiae
 *    corresponding to the configuration of the matcher library.
 *    This prevents unnecessary large templates from being
 *    created with more minutiae than will be used any way
 *    (E.g. coming from image noise). Set this with:
 *
 *      pb_session_set_int(session, "pb.max_minutiae", nn);
 *
 *    Where nn is maximum, default value is 64.
 *
 *    The memory conservative matcher variants is configured
 *    for a maximum of 46 so this is typically the appropriate
 *    configuration to specify.
 *
 * NOTE 2 - Limit image size
 *
 *   In the use-case where full sized images are used for enrollment
 *   the enrollment image size should be limited to the active area
 *   of the fingerint before images are further processed. The
 *   active size should not be larger than 252x348 at 500/508 dpi, 
 *   else memory constrains in the matcher may prevent it from being
 *   used.
 *
 *   TODO: In such a use-case the number of minutiae should not
 *   be limited during extraction and one maybe should select a
 *   special dedicated Hybrid MOC verifier implementatation to
 *   reach best possible biometric perormance.
 *
 * @param lib_magic      The actual library variant on the card.
 * @param ehm_template   The EHM verification template.
 * @param hmoc_template  The encoded verification template that should
 *                       be sent to the card for verification.
 * @param finger_mask    Selection of which fingers to match, a zero
 *                       means match against all enrolled finger codes, else
 *                       it is a bitset of finger codes (1-10)
 *                       (1 << finger_code).
 */
pb_rc_t pb_hmoc_encode_ver(
    uint16_t             lib_magic,
    const pb_template_t* ehm_template,
    pb_template_t**      hmoc_template,
    uint16_t             finger_mask);


/**
 * Decodes Hybrid MOC library public metadata.
 *
 * It should be possible to retrive metadata from the card, e.g.
 * using JavaCard getPublicTemplateData() on the enrolled reference
 * data, this metadata is a TLV encoded set of parameters that can
 * be deoced by this function.
 *
 * Retrieving metadata from the card prior to personalisation may be
 * done to learn the magic numer of the verifier, and after enrollment
 * it can also be used to find out which fingers are enrolled.
 *
 * The output arguments are optional any may be given as null.
 *
 * @param data           The metdata from the card (a TVL encoding).
 * @param length         Length of metadata
 * @param lib_magic      The library magic number.
 * @param num_containers Corresponing value from the configuration container
 *                       or 0 if not configured.
 * @param metadata_size  Corresponing value from the configuration container
 *                       or 0 if not configured.
 * @param container_size Corresponing value from the configuration container
 *                       or 0 if not configured.
 * @param finger_mask    A bitset of active enrolled finger containers
 *                       each encoded like (1 << finger_code)
  */
pb_rc_t pb_hmoc_decode_metadata(
    const uint8_t* data, uint32_t length,
    uint16_t* lib_magic,
    uint16_t* num_containers,
    uint16_t* config_size,
    uint16_t* container_size,
    uint16_t* finger_mask);

#ifdef __cplusplus
}
#endif

#endif
