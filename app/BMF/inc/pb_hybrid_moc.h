/*
 * Copyright (c) 2015 - 2016, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_HYBRID_MOC_H
#define PB_HYBRID_MOC_H

/*
 * The Hybrid MOC matcher is a limited and slimmed down version of the
 * EHM Verifier. This matcher can be compiled for smartcards and
 * integrated into there CardOS to provide small sensor support via
 * multi-template techinque. The implementation also supports the
 * use-case where enrollment is made from one full-sized image
 * (however not larger than approx 252x348 pixels @ 500 dpi) while
 * verifications are made from a small sensor.
 *
 * To make the matcher versatile the feature model, threshold tables
 * etc are not built in as hardcoded constants in the code.  This
 * means that e.g ROM-based cards can still be customized and adjusted
 * to the actual use case. To support this the classical finger
 * enrollment process is extended with a pre-enrollment step
 * where the matcher is configured first.
 *
 * The usage-model determined at pre-enrollment is done using the
 * support code in BioMatchEmbedded/BioMatchMobile from a short list
 * of supported cases. All models require a small verification sensor
 * but not to small, basically the 8x8 and 10x10 mm sensor sizes is
 * expected to be supported for verification. If enrollment is made
 * from a same-sized small sensor the multi-template technique is
 * required to achieve good biometric performance whereas if the model
 * uses enrollment from large sensor one sample may be sufficent.
 *
 * The Hybrid MOC matcher includes the concept of multi-finger
 * enrollment whereas other MOC implementations often keep each
 * finger separate. By allowing the matcher to handle all enrolled
 * fingers, multi-finger genuine matching can be made much faster.
 *
 * As a consequence of this, what typically is regarded as several
 * reference templates (one per finger) there is now one reference
 * template for a person. When adding/removing enrolled fingers for
 * this person the reference template is updated, not completely
 * replaced.
 * 
 * The configuration and enrollment steps are kept completly outside
 * of the Hybrid MOC matcher and is typically handled by smartcard
 * Java applet code which basically means to receive and store
 * containers of data. These containers are then simply forwaded to
 * the match call along with the verification template.
 *
 * The very first container is the configuration container and it in
 * turn carries information of how many containers there should be in
 * total as well as the size of the the configuration and the finger
 * containers. All finger containers are reserved with the same
 * size. Containers are internally tagged with their container ID
 * which is a number between 0-10 where 0 is reserved for the
 * configuration container. All containers in an enrollment has a
 * unique number and each container can also be in either state active
 * or unused. During update of enrollment an existing active container
 * with the same ID should be replaced else an unused container is
 * used.
 *
 * Containers are always grouped together and represents any and all
 * fingers of the user. Several such sets of containers could be used
 * to represent sevral users but that is outside the concepts of this
 * matcher.
 *
 * When calling the Hybrid MOC matcher function the containers must be
 * presented either as part of one single large block of memory or
 * broken up in individual memory blocks, one for each container. When
 * one single large memory block is used to represent all fingers the
 * matcher will use the configuration found in the configuration
 * container to determine the offset to each individual
 * container. These two use-cases support different possible
 * implementations and integrations to the CardOS.
 *
 * Consequently addresses to the containers are sent in as an array of
 * pointers, where the use-case with one single large container means
 * the array only contains 1 pointer.
 *
 * When calling the matcher the entire data set must exist and have
 * been allocated, even if no fingers yet have been enrolled. This
 * allocation should be done at the time the first container is
 * enrolled which is the configuration container. The configuration
 * container itself contains the data of how many containers there
 * should be and their respective sizes. So Java applet code must be
 * able to peek inside the container retrieve this information. The
 * information is TLV taged and is the first sub-tag of the container
 * tag, more on this later.
 *
 * The sketch below illustrates the two possible use-cases regarding
 * container allocation. The example shows a configuration using
 * 4 containers; 1 configuration container and 3 finger containers.
 *
 * Example 1 - Using one large allocation for all containers.
 * 
 *  container[0] --->  +------------------+ -------- base address
 *  ncont = 1          | id=0 active      |     |
 *                     | conf container   |     |
 *                     +------------------+     v conf-size
 *                     | id=3 active      |       
 *                     | finger container |   |
 *                     |                  |   |
 *                     +------------------+   v conf-size + 1 * finger-size
 *                     | id=1 active      |  
 *                     | finger container |  |
 *                     |                  |  |
 *                     +------------------+  v  conf-size + 2 * finger-size
 *                     | id=? inactive    |
 *                     | unused container | 
 *                     |                  |
 *                     +------------------+
 *
 * Example 2 - Using several individual allocation for containers.
 *
 *  container[0] --->  +------------------+ -------- base address
 *  ncont = 4          | id=0 active      |
 *                     | conf container   |
 *                     +------------------+
 *
 * container[1] --->   +------------------+
 *                     | id=3 active      |       
 *                     | finger container |
 *                     |                  |
 *                     +------------------+
 *
 * container[2] --->   +------------------+
 *                     | id=1 active      |  
 *                     | finger container |
 *                     |                  |
 *                     +------------------+
 *
 * container[3] --->   +------------------+
 *                     | id=? inactive    |
 *                     | unused container | 
 *                     |                  |
 *                     +------------------+
 *
 * The number of containers, conf-size and finger-size parameters are
 * found in the conf container which always must be in the beginning
 * of the the first container [0] and it always has ID=0 and should be
 * marked active.
 *
 * In Example 1 the finger containers follows on certain offsets from
 * the base pointer containers[0] whereas in Example 2 each container
 * is explicitly addressed by unique pointers.
 *
 * Finger containers do not have to be in any particular order,
 * like in the example above finger=3 is in the first finger container.
 * Also not all containers need to be active as the last container
 * above shows. This container is free for adding another finger
 * with any ID different from any already existing conatiner IDs,
 * which in this case is 1 and 3. 
 *
 * To remove an enrolled finger one simply "enrolls" with an empty
 * container with the appropriate container ID and which is marked
 * as inactive.
 *
 * During an update of enrollment Java applet code needs to check
 * if there already is one container that is active with the given
 * container ID and then replace the content. This means that
 * the Java applet code also must be able to retrieve the container
 * ID and active state of a container, more on this later.
 *
 * Each Hybrid MOC implementation may be configured diffently to
 * support different use-cases. Each such implementation may alter
 * the format of the data contained in the containers. Hence a
 * generic solution needs to find out the "lib magic" number of
 * the Hybrid MOC library in question and use this magic number
 * as a parameter when creating config, enrollment and verification
 * data sent to the card.
 *
 * Either this magic number is well known in the system design and
 * can be hard-coded or one needs to query the smartcard for the
 * actual magic number.
 *
 * Such a query would be done via a dedicated APDU command which
 * forwards the request to the Hybrid MOC function "pb_hmoc_getmeta()"
 * which contains the library magic id in the response. This function
 * may be called without any containers - such as in a pre-enrollment
 * case when the card has not yet been configured (in the Javacard
 * Biometric API one can deliver this info in a request to the
 * getPublicTemplateData() function).
 *
 * When the getmeta function is called with containers the response
 * will also include e.g. a finger mask showing which active
 * containers there are along with the configuration of numbert of
 * containers and there sizes which should be used as parameter when
 * generating containers sent to the card.
 *
 *  NOTE: After a configuration or config update, call the
 *        pb_hmoc_getmeta() function and check that the response
 *        code is OK. This check includes checking that the
 *        library magic code is correct and other sanity checks.
 *
 * During matching there is no means of specifying the reqested
 * seurity level, FAR. This is a setting that is selected during
 * configuration.  This aligns better with the existing Javacad
 * biometric API which does not include such parameters.
 *
 * In general the design has been done in a way that should allow
 * implementing the Javacard biometric API (such as BioTemplate,
 * OwnerTemplate, SharedTemplat, BioManager) even though in this case
 * each reference template actually contains several fingers.
 *
 * Memory consumption; the required work memory to use the matcher
 * varies depending on the configuration and the exact size also
 * depends on the verification template sized and may also depend on
 * the enrollment templates etc. So a fixed general size is not
 * possible to give here but the size is around 3KB. Each compilation
 * needs to add such information.
 *
 * Verification template
 * ---------------------
 *
 * The verification template should be placed somewhere inside the
 * work buffer when calling the match function.
 *
 *    ver_offs +--------|
 *                0     |                     work_size
 *                |     v                         |
 *  Work buffer:  +-----<VERIFY TEMPLE>-----------+
 *
 * The size of the verification template will be deduced from the
 * TLV-tag wrapping the template and the size is verified to be
 * within the work buffer. Optionally the verification template
 * may be prefixed by C0-tag with 2 bytes finger-mask.
 *
 *  Verify  template syntax ::= [C0 finger-mask(2)] 7F2E ...
 *
 * The optional finger mask is MSB and can be used to limit matching
 * against a subset of enrolled fingers.
 *    
 * Container syntax
 * ----------------
 *
 * The containers are BER-TLV encoded and the Java applet code
 * needs to be able to get the C0 record out of the container.
 * The C0 record is the first subtag of the container.
 *
 * Config container syntax
 * -----------------------
 *
 *    EF
 *      C0 container-code lib-magic(2) num-containers(2) conf-container-size(2) finger-container-size(2)
 *      ... other tags follows
 *   
 * The C0-tag length should be >= 9 bytes and contains MSB-first encoded data:
 *
 *   container-code  - 8-bit where the least 4 significant bits encode
 *                     container number which for config conater is 0 and
 *                     the msb bit determins if config container is active
 *                     or not (which it shold be). <active:1, tbd:3, id:4>.
 *   lib-magic       - 16-bit lib magic number - must agree with library
 *                      magic version as reported by pb_hmoc_getmeta().
 *   num_containers  - 16-bit total number of containers, at least > 0 but with one
 *                     enrolled finger == 2. Max value should actually be 11.
 *   conf-container-size
 *                   - 16-bit size of the container, obviously the size
 *                     here must be at least the size of any received
 *                     config container!
 *   finger-container-size
 *                    - 16-bit size of each of the finger containers.
 *
 * Finger container syntax
 * -----------------------
 *
 *    EF
 *      C0 container-code ... other data
 *      ... other tags
 *
 * For finger containers only the container code ever needs to be
 * decoded and is used to determine which container is to be updated
 * or finding out if a container is free to use for a new finger.
 *
 *   container-code  - 8-bit where the least 4 significant bits encode
 *                     container number which for finger containers are 1-10
 *                     and the msb bit determines if container is active
 *                     or not (which it shold be). <active:1, tbd:3, id:4>.
 *
 *                     The finger-id of inactive containers is in
 *                     general not valid and should be disregarded.
 *                     The exception is when removing a finger from
 *                     the enrolled set. Then an inactive container
 *                     can be received and should replace data in the
 *                     corresponding active container with the same
 *                     finger-id which essentially disables it.
 *
 * To decode the tags described above basic TLV-decoding primitives
 * should be used.
 *
 * Any container that begins with a byte != 0xEF will be treated by the
 * Hybrid MOC matcher as invalid/inactive.
 *
 * Hence during an update scenario where containers are received it is
 * recommended that the first byte of the container is set to e.g. 00
 * while receiving all other data and once all data has been received
 * the first byte is set to EF - this will prevent using a partially
 * updated container in case of card tearing or aborted update
 * sequence.
 * 
 */

#if defined(_MSC_VER) && _MSC_VER < 1600
# include <win32/stdint.h>
#else
# include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//NOTE: these features are replicated in BMF; pb_hmoc_encoder.h
#define PB_HMOC_FEAT_360      (1<<0) // Do full 360 verification

/* RFU. More or less internal hybrid featurs below, these are reserved for
 * a possible future extension of matcher that supports smaller sensor
 * sizes and does full search. This would likely use the traditional
 * spectral verifier and thus require more ram, speed and a 32-bit
 * processor. */
#define PB_HMOC_FEAT_NOCOARSE (1<<8)  // RFU: Spectral search feature
#define PB_HMOC_FEAT_SEARCH   (1<<9)  // RFU: Spectral search feature
#define PB_HMOC_FEAT_SQCANVAS (1<<10) // RFU: Spectral search feature

/**
 * Returns library metadata, formatted as a C0 TLV tag:
 *
 *   C0 container-code lib-magic num-containers conf-container-size finger-container-size finger-mask
 *
 * Where container-code is one byte and remaing parameters are 2-byte
 * big-endian integers.  Hence the total size is 13 bytes (at least).
 *
 *   NOTE: To decode this use the BME/BMM function pb_hmoc_decode_public().
 *
 * If contp is NULL (no containers) then the result will only contain
 * the library magic code and remaining parameters set to 0.
 *
 * If ncontp >= 1 then the contp[0] should point to metadata
 * conatainer from which other data in C0 tag is extracted;
 * e.g. num_containers, metadata_size, container size. If the
 * metacontainer lib-magic number is not exacly the same as the
 * library lib magic an error code is returned.
 *
 * If ncontp == 1 then it is assumed that each finger container is
 * located at regular offsets as described by the metadata.
 *
 * If ncontp > 1 then ncontp MUST be same as num_containers in
 * metadata and each contp[i] pointer points to respective container
 * where [0] is metadata. Else an error is returned.
 *
 * When a card accepts a new configuration it should call this function
 * to verify that it is correct and that lib_magic number is correct
 * (indicated by a failure code) and only enable the container if valid.
 *
 * @param contp     Container pointers, there should be ncontp container pointers.
 *                  If contp is NULL (and ncontp 0) no containers exist, else
 *                  these will be investigated to form metadata.
 *                  See description above.
 * @param ncontp    Number of container pointers.
 *
 * @param resp      Response buffer.
 * @param resp_size Length of metadata respons buffer. At least
 *                  13 bytes is needed.
 * @param resp_len  Length of metadata response output.
 *
 * @return Status of the function where PBI_EOK means OK,
 *         PB_EBUFFER if response buffer to small for any
 *         response. If the configuration container magic number
 *         is not same as the library magic number then the
 *         response will be coded and PB_EVERSION is returned.
 */
uint8_t pb_hmoc_getmeta(const uint8_t* const contp[], uint8_t ncontp,
                        uint8_t* resp, uint8_t resp_size, uint8_t* resp_len);

/**
 * Matches a verification template agains one or more enrolled fingers
 * as defined by the configuration and enrollment containers.
 *
 * A description of the container parameters can be found in the
 * header of this file but contains the config and finger enrollment
 * data.
 *
 * When called the verification template should be stored in the
 * work buffer at the given offset.
 *
 * Normally score should not be requested for any other purpose of
 * testing or evaluation. Requesting score will force a complete match
 * to be done and quick matching is disabled. Quick matching means
 * that the operation can end as soon as matching finger/subtemplate
 * is found.  Matching is done at the configured requested FAR level
 * in the configuration container. Quick match may also implement
 * a sorting order among all subtemplates to increase changce of a
 * quicker match of genuines.
 *
 * Upon entry the verification template should be stored in the
 * work buffer at the given offset.
 *
 * @param contp     Container pointers.
 * @param ncont     Number of container pointers.
 * @param work      Work buffer, needed to complete the operation.
 *                  The buffer must be properly aligned to handle
 *                  uint32_t access.
 *                  At call the verification template is located
 *                  at some offset inside this buffer.
 * @param work_size The size of the work buffer.
 *                  The required size depends on how the library
 *                  has been configured and also on how large
 *                  the largest verification template may be as it
 *                  occupies a part of the work buffer as well.
 * @param ver_offs  The offset to the verification template which
 *                  should be stored inside the work area upon entry.
 *                  The size of the template is determined from the
 *                  TLV-length. The tag type must be 7F2E or
 *                  a C0-tag directly followd by the 7F2E tag and
 *                  the length of data must be within the limits
 *                  of the work buffer to be accepted.
 * @param ver_offs  The size the verification template.
 * @param decision  The outcome of the match, decision is made
 *                  at the configured requested FAR. A value of 0
 *                  means no match was achieved. A value > 0 indicates
 *                  a match and the value itself will indicate the
 *                  finger code (container code) of the matching finger.
 * @param score     Optional request matching score, normally set this to
 *                  null, else quick matching will be sisabled.
 *
 * @return Status of  the function where PBI_EOK means OK.
 */
uint8_t pb_hmoc_match(const uint8_t* const contp[], uint8_t ncontp,
                      uint32_t* work, uint16_t work_size,
                      uint16_t ver_offs,
                      uint8_t* decision,
                      uint16_t* score);

/**
 * Decodes data in the 0xC0 container header tag. It can also be used
 * for matcher metadata decoding (pb_hmoc_getmeta()).
 *
 * When decoding a configuration container all available output data
 * is returned. When decoding a finger container only the active and
 * finger_code is returned and the remaining data is set to 0.
 *
 * Any output parameter that is not if interest may be specified as 0.
 *
 * When decoding not all data of the container need to be available,
 * hence the length parameter must only be long enough to decode the
 * C0 tag. With 32 bytes available it is guaranteed to be possible to
 * decode all data. The intended purpose is to be able to decode
 * incoming containers sent in chunks.
 *
 * @param container             The container to decode.
 * @param length                Lenth in bytes of container.
 * @parma active                1 if container is active, 0 otherwise.
 * @param finger_code           Container finger code between 1 and 10
 *                              and 0 for config container.
 * @param lib_magic             The library magic number (config container).
 * @param num_containers        Number of containers (config container).
 * @param conf_container_size   Size in bytes for config container
 *                              (config container).
 * @param finger_container_size Size in bytes for finger containers
 *                              (config container).
 * @param finger_mask           A bitset of active enrolled finger containers
 *                              each encoded like (1 << finger_code). Only
 *                              available in matcher metadata.
 */
uint8_t pb_hmoc_decode_container_header(
    const uint8_t* container,
    uint32_t length,
    uint8_t* active,
    uint8_t* finger_code,
    uint16_t* lib_magic,
    uint16_t* num_containers,
    uint16_t* conf_container_size,
    uint16_t* finger_container_size,
    uint16_t* finger_mask);

#ifdef __cplusplus
}
#endif

#endif /* PB_HYBRID_MOC_H */
