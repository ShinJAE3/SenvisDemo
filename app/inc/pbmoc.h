/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef HEADER_PBMOC_H
#define HEADER_PBMOC_H

/*! \file pbmoc.h
\brief 
Precise Match on Card Application Constants.
\details
This file defines the constants for Precise MoC algorithm
which is used to enroll and verify ISO fingerprint minutiae templates.
For more information about constants see page \ref CONST.
*/


/*! \page CONST Match-on-Card Application Constants
This manual is divided in the following sections:
- \subpage FTF "Fingerprint template format"
- \subpage Std_tags "Standardized tags"
- \subpage PROP "Proprietary tags"
- \subpage unknown "Unknown tags"
- \subpage Defs "Definitions"
- \subpage SCs "Smart Cards"
- \subpage BIT "Biometric Information Template"
*/

/*! \page FTF Fingerprint template format

The matcher API accepts templates formated and structured according to 
ISO 19794-2 and 7816-11. There are a few possible supported variants
of the format in the standard. The format supported by the matcher is
therefore described here for clarity.

In terms of the standard the fingerprint is contained in the
Biometric Data Block Reference Data Template data object. The minutiae
format itself is encoded in subtag 81 or tag 90 under subtag A1 and
uses the compact size card format (that is 1 byte x, 1 byte y, 6
bits angle and 2 bits minutiae type). Depending on the matcher features
the template may also include additional standardized features as
ridge count and delta points as defined in ISO 19794-2. When used
together with the Precise Biometrics extractor the template may also
include proprietary information for enhanced performance.
*/

/*! \page Std_tags Standardized tags
The reference data template follows BER encoding rules, which is out
of the scope for this description but information can easily be found
in standardization documents.

The ISO 19794-2 and 7816-11 standards describes that the standardized
part of the template is put under tag 81/A1. The difference between
tag 81 and A1 is that the data under tag 81 is pimitive/binary while
under A1 constructed which means there are further BER tags that can
be decoded.

If the template only carries minutiae data this can be placed directy
under tag 81 as this example shows:
 
Example:
- 0x7F, 0x2E, 'LEN',   // Biometric data template
  - 0x81, 'LEN',       // Primitive Biometric data with standardised format
    - 100, 200, 45     // First minutiae; x, y, angle and type
    - ...

 NOTE 1: 'LEN' is a placeholder for BER encoded length
 
 As the example shows the compact size card format is used, i.e. 3
 bytes per minutiae.

 If additional standardized items are included the primitive tag 81 must
 be replaced with the A1, constructed, tag. The minutiae data is then placed
 under tag 90, ridge count under 91 etc as this example shows:
 
Example:
 - 0x7F, 0x2E, 'LEN',    // Biometric data template
   - 0xA1, 'LEN',        // Constructed Biometric data with standardised format
     - 0x90, 'LEN',      // Primitive Minutiae data with standardised format
       - 100, 200, 45    // First minutiae; x, y, angle&type
       - ...
     - 0x91, 'LEN', ...  // Optional. Ridge count data
     - 0x92, 'LEN', ...  // Optional. Core point data
     - 0x93, 'LEN', ...  // Optional. Delta point data
     - 0x94, 'LEN', ...  // Optional. Cell quality data
 
NOTE: The ISO 19794-2:2005 standard document is confusing regarding this matter.
 
It is also possible to use the A1 tag with only subtag 90 as an alternative
to the more compact version using 81 tag directly.
*/

/*! \page PROP Proprietary tags

In addition to the standardized items a proprietary tag may also be included
as long as it has a valid content generated by the corresponding extractor.
The propriety information shall go into the tag 82/A2. The 82 tag should be used
when the propriety data is primitive/binary while A2 is used when the data is
constructed, just as for the standardized data.

One example of a template where both propriety information has been added
at the same time core point also is included:

Example:
- 0x7F, 0x2E, 'LEN',    // Biometric data template
  - 0xA1, 'LEN',        // Constructed Biometric data with standardised format
    - 0x90, 'LEN',      // Primitive Minutiae data with standardised format
      - 100, 200, 45    // First minutiae; x, y, angle&type
      - ...
    - 0x91, 'LEN', ...  // Optional. Ridge count data
    - 0x92, 'LEN', ...  // Optional. Core point data
    - 0x93, 'LEN', ...  // Optional. Delta point data
    - 0x94, 'LEN', ...  // Optional. Cell quality data
  - 0x82, 'LEN', ...    // Primitive proprietary matcher data

Note that tag A1 and 82 are on the same level. Also note that any
combination of 81/A1 and 82/A2 is valid.

NOTE: The ISO 19794-2:2005 standard document is confusing regarding this matter.
*/

/*! \page unknown Unknown tags
Any extra unknown tags in the template will be silently discarded.
*/

/*! \page Defs Definitions

In the API there are a number of constants defined to describe the
matcher in an ISO 19784-2 and ISO 7816 context. These constants 
do not take direct part in the API functions but should be viewed as
required parts of the glue to interconnect the matcher with an extractor
to make sure the properly formated templates are being generated.

Each constant is documented by itself but an overview follows.

- PB_MOC_BDB_FORMAT_TYPE
   - Defines which of the possible format types of ISO 19794-2 compact
   card format is used.
- PB_MOC_BDB_PROPRIETY_FORMAT_OWNER
   - Together with the PB_MOC_BDB_PROPRIETY_FORMAT_TYPE this identifies
   the content of the proprietary data tags in the template.
   These values should typically be conveyed to the extractor in order
   for it to include proprietary data in the template.
- PB_MOC_PARAM_MIN_MINUTIAE_ENROLL  / PB_MOC_PARAM_MIN_MINUTIAE_VERIFY
   - The minimum number of minutiae that must be sent to the matcher
   during enrollment and verify respectively.
- PB_MOC_PARAM_MAX_MINUTIAE_ENROLL / PB_MOC_PARAM_MAX_MINUTIAE_VERIFY
   - The maximum number of minutiae that the matcher can handle
   during enrollment and verify respectively.
- PB_MOC_PARAM_ORDER_ENROLL / PB_MOC_PARAM_ORDER_VERIFY
   - If possible the minutias in the template should be sorted
   according to this parameter during enrollment and verify
   respectively.
- PB_MOC_FEAT_RIDGE_COUNT_SUPPORT
- PB_MOC_FEAT_CORE_SUPPORT
- PB_MOC_FEAT_DELTA_SUPPORT
- PB_MOC_FEAT_QUALITY_SUPPORT
   - Boolean flags specifying which additional items can be used by
   the matcher if they are included in the template.
- PB_MOC_PARAM_FEATURES
   - An ISO 19794-2 feature indicator of the supported additional
   features (ridge count, core, delta and quality).
*/

/*! \page SCs Smart Cards

When the matcher is used on a smart card a number of standards may
apply like the ISO 7816. Since the matcher takes BER encoded card
templates as input it may be easy to integrate the matcher due to
the standardized format. There are however other formats that may
apply as well like the BIT, Biometric Information Template defined
in ISO/ 7816-11. The BIT is kept together with the reference template
to identify the type and format of it. An interface terminal typically
reads the BIT from a card to deduce the type of biometric verifications
offered. As illustrated in the standard the following steps are involved.

Enrollment:
  1. Create biometric data block reference data (potentially with
     proprietary data for the specific on-card matcher when applicable).
  2. Save reference data to card using CHANGE REFERENCE DATA command
  3. Construct BIT corresponding to the biometric data
  4. Save BIT to card.

Verify:
  1. Read the BIT from the card
  2. Select biometric method/type based on BIT
  3. Create verification biometric reference data (potentially with
     proprietary data matching the format indicated by the BIT).
  4. Issue VERIFY command identifying the algorithm indicated in the BIT

From these scenarios it is unclear how the enrollment process know
what matcher and its capabilities are available on the card, this information
however also needs to be transfered in some way.
*/

/*! \page BIT Biometric Information Template

When the matcher is used with e.g. smartcards it may be appropriate
to construct an corresponding Biometric Information Template (
based on the parameters of the matcher. The BIT is typically constructed
during enrollment, saved to the card and read by the terminal during
verification in order to be able to generically detect the features.

The BIT may be constructed in different ways depending on various features.
The following example shows one such BIT for on-card matching incorporating
the matcher parameters appropriately and to also allow inclusion of the
proprietary matcher specific data in the BDB Reference Data Template.

The following symbols refers to unbound values that must be set by
the implementation.

- _ALGORITHM_REFERENCE, _REFERENCE_DATA
    - Any suitable value so the card can pick the corresponding matcher for
    this BIT. This value is arbitrary but must be chosen in concert with
    the card software. If there is only one biometric matcher on the card
    the tags 0x80 and 0x81 are not needed at all.
- _POSITION
    - The finger associated with this BIT (what finger the user is supposed
    to present), as defined in  ISO 19785-3 (not ISO 19794-2!).
- _BDB_CREATOR
    - The creator identity of the reference data.

Example:
- 0x7F, 0x60, 'LEN', Biometric Information Template (BIT)
        - 0x80, 'LEN', _ALGORITHM_REFERENCE, Optional, used in VERIFY command to identify the matcher to be used
        - 0x83, 'LEN', _REFERENCE_DATA,      Optional, used in VERIFY command to identify the biometric reference template
        - 0x06, 'LEN',                       Optional, OID of standard body; default is
            - 0x28, 0x81, 0x9A, 0x49,        ISO/IEC JTC 1/SC 37 (default tag allocation
            - 0x03, 0x01, 0x00,              authority for the Data Objects nested in the Biometric Header Template, BHT) oid = 1.0.19785.3.1.0  
        - 0xA1, 'LEN', Biometric Header Template (BHT) level 1. Tag allocation authority ISO/IEC JTC 1/SC 37, see tag 0x06 above
            - 0x80, 'LEN', 0x01, 0x01, Patron Header Version. Optional, this is the default value if omitted.
            - 0x81, 'LEN', 0x08,  Biometric type: finger
            - 0x82, 'LEN', '_POSITION',  Finger position as defined in 19785-3 (NOT 19794-2)
            - 0xA1, 'LEN'   // BHT level 2 (standardized part)
                - 0x86, 'LEN',               IBIA assigned creator of biometric data block 2 or 4 bytes ?
                    - _BDB_CREATOR ...,
                - 0x87, 'LEN',               IBIA assigned biometric data block format owner;
                    - 0x01, 0x01,            ISO/IEC JTC1/SC37
                - 0x88, 'LEN',               Biometric data block format type; ISO 19794-2
                    - 0x00, PB_MOC_BDB_FORMAT_TYPE,
                - 0xB1, 'LEN',               Biometric matching algorithm parameters; ISO 19794-2
                    - 0x81, 'LEN',           MIN and MAX number of minutiae
                        - PB_MOC_PARAM_MIN_MINUTIAE, PB_MOC_PARAM_MAX_MINUTIAE,
                    - 0x82, 'LEN',           Minutiae ordering
                        - PB_MOC_PARAM_ORDER_VERIFY,
                    - 0x83, 'LEN',           Feature handling indicator
                        - PB_MOC_PARAM_FEATURES
            - 0xA2, 'LEN'   BHT level 2 (proprietary part)
                - 0x87, 'LEN',               IBIA assigned biometric data block format owner;
                                             ... precise biometrics ...
                    - MSB(PB_MOC_BDB_PROPRIETY_FORMAT_OWNER), LSB(PB_MOC_BDB_PROPRIETY_FORMAT_OWNER),
                - 0x88, 'LEN',               Biometric data block format type; ISO 19794-2
                    - 0x00, PB_MOC_BDB_PROPRIETY_FORMAT_TYPE,
*/

/*!
\anchor FAR
\name Security settings
@{
Depending on what the system is going to be used for, different FAR levels can
be chosen. The FAR level is specified during enrollment and the specified 
level is stored on the smart card as a part of the secure reference data. 

For example, PB_FAR_10000 means that 10,000 imposter attempts, i.e. 10,000 
fingers, are on average needed to produce one (1) false accept.
\n\n
The FAR is inversely related to the FRR such that a decreasing FAR will result
in an increasing FRR, e.g. PB_FAR_100 in the table will result in a much lower
FRR than PB_FAR_1000000. Therefore it is recommended not to choose a lower FAR
level than required by the system. A FAR value of 1:10,000 (PB_FAR_10000) is 
recommended unless the customer explicitly wants a lower value. The FRR is not
only dependent on the FAR, but also on a lot of abstract parameters, such as:
\n\n
- The users experience of the fingerprint reader.\n
- The quality of the enrolled fingerprint template.\n
- The skin condition of the users fingerprint.\n
- When the last cleaning of the fingerprint sensor was made.\n
\n

\def PB_FAR_5
False Acceptance Rate 1:5
\def PB_FAR_10
False Acceptance Rate 1:10
\def PB_FAR_50
False Acceptance Rate 1:50
\def PB_FAR_100
False Acceptance Rate 1:100
\def PB_FAR_500
False Acceptance Rate 1:500
\def PB_FAR_1000
False Acceptance Rate 1:1000
\def PB_FAR_5000
False Acceptance Rate 1:5000
\def PB_FAR_10000
False Acceptance Rate 1:10000
\def PB_FAR_50000
False Acceptance Rate 1:50000
\def PB_FAR_100000
False Acceptance Rate 1:100000
\def PB_FAR_500000
False Acceptance Rate 1:500000
\def PB_FAR_1000000
False Acceptance Rate 1:1000000
*/
#define PB_FAR_5                1
#define PB_FAR_10               2
#define PB_FAR_50               3
#define PB_FAR_100              4
#define PB_FAR_500              5
#define PB_FAR_1000             6
#define PB_FAR_5000             7
#define PB_FAR_10000            8
#define PB_FAR_50000            9
#define PB_FAR_100000           10
#define PB_FAR_500000           11
#define PB_FAR_1000000          12
/*
@}*/

/** The decision that the two templates matched. */
#define PB_MOC_DECISION_MATCH        1

/** The decision that the two templates did not match. */
#define PB_MOC_DECISION_NO_MATCH     0

/*!
\anchor BDB_FORMAT_TYPE
\name Biometric Data Block format type
@{
\def PB_MOC_BDB_FORMAT_TYPE 
ISO 19794-2 compact card format type 5; 
Finger minutiae card format - compact size, with;
- ridge endings (valley skeleton bifurcation points)
- ridge bifurcations (ridge skeleton bifurcation points)
\n
\n
NOTE: This is the only supported BDB FORMAT TYPE. 
Any other value will cause return of \ref PB_EPARAMETER
*/
#define PB_MOC_BDB_FORMAT_TYPE   0x05
/*@}*/

/*!
\anchor BDB_FORMAT_OWNER
\name Biometric Data Block format owner
@{
\def PB_MOC_BDB_FORMAT_OWNER
IBIA registered format owner id for ISO/IEC JTC 1/SC 37
\n
\n
NOTE: This is the only supported BDB FORMAT OWNER.
Any other value will cause return of \ref PB_EPARAMETER
*/
#define PB_MOC_BDB_FORMAT_OWNER   0x0101
/*@}*/

/*!
\anchor BDB_PROPRIETARY_FORMAT_OWNER
\name Biometric Proprietary Data Block format owner
@{
\def PB_MOC_BDB_PROPRIETARY_FORMAT_OWNER
Proprietary biometric data block format owner, IBIA assigned.
This number is used to identify who created the proprietary part
of the template. Typically this number should be conveyed to the
extractor in order for it to be able to include additional proprietary
data in the template.
\n
\n
NOTE: This is the only supported BDB PROPRIETARY FORMAT OWNER.
Any other value will cause return of \ref PB_EPARAMETER
*/
#define PB_MOC_BDB_PROPRIETARY_FORMAT_OWNER 0x000B
/*@}*/

/*!
\anchor BDB_PROPRIETARY_FORMAT_TYPE
\name Biometric Proprietary Data Block format type
@{
\def PB_MOC_BDB_PROPRIETARY_FORMAT_TYPE
Proprietary biometric data block format type.
This number together with the PB_MOC_BDB_PROPRIETARY_FORMAT_OWNER is 
used to identify the exact format of the proprietary data in the template.
\n
\n
NOTE: This is the only supported BDB PROPRIETARY FORMAT TYPE.
Any other value will generate a template without propretary data.
*/
#define PB_MOC_BDB_PROPRIETARY_FORMAT_TYPE  0x00
/*@}*/

/*!
\anchor Card_capabilities
\name Indication of card capabilities
@{
\def PB_MOC_FEAT_RIDGE_COUNT_SUPPORT
Feature indication of supported additional ISO 19794-2
standard feature for ridge count. Value 1 means supported
0 means not supported.
\n
\def PB_MOC_FEAT_CORE_SUPPORT
Feature indication of supported additional ISO 19794-2
standard feature for core points. Value 1 means supported
0 means not supported.
\n
\def PB_MOC_FEAT_DELTA_SUPPORT
Feature indication of supported additional ISO 19794-2
standard feature for delta points. Value 1 means supported
0 means not supported.
\n
\def PB_MOC_FEAT_QUALITY_SUPPORT
Feature indication of supported additional ISO 19794-2
standard feature for cell quality. Value 1 means supported
0 means not supported.
\n
\def PB_MOC_PARAM_FEATURES
Feature indication of supported additional ISO 19794-2
standard features coded as ISO 19794-2 feature handling indicator. 
The feature indicator has an 8-bit coding with the four highest bits
RFU and should have default value 0.
*/
#define PB_MOC_FEAT_RIDGE_COUNT_SUPPORT 0
#define PB_MOC_FEAT_CORE_SUPPORT 1
#define PB_MOC_FEAT_DELTA_SUPPORT 1
#define PB_MOC_FEAT_QUALITY_SUPPORT 0

#define PB_MOC_PARAM_FEATURES (PB_MOC_FEAT_RIDGE_COUNT_SUPPORT << 0 | \
	PB_MOC_FEAT_CORE_SUPPORT << 1 |  \
	PB_MOC_FEAT_DELTA_SUPPORT << 2 | \
	PB_MOC_FEAT_QUALITY_SUPPORT << 3)

/*@}*/

/*!
\anchor Minutiae_Order
\name Minutiae order
@{
Minutiae ordering as defined in ISO 19794-2
\n
\def PB_MOC_PARAM_ORDER_NO_ORDERING
No ordering (default value)
\def PB_MOC_PARAM_ORDER_ASC
Ordered ascending (not supported)
\def PB_MOC_PARAM_ORDER_DEC
Ordered decending (not supported)
\def PB_MOC_PARAM_ORDER_CARTESIAN_X_Y
Ordered cartesian X-Y
\def PB_MOC_PARAM_ORDER_CARTESIAN_Y_X
Ordered cartesian Y-X
\def PB_MOC_PARAM_ORDER_ANGLE
Ordered angle (not supported)
\def PB_MOC_PARAM_ORDER_POLAR
Ordered polar (not supported)
\def PB_MOC_PARAM_ORDER_X_Y_EXT_CC
Ordered X or Y coordinate extension for compact format (not supported)
\def PB_MOC_PARAM_ORDER
Order as defined by any of the defines above
\n
\n
NOTE: PB_MOC_PARAM_ORDER_X_Y_EXT_CC is currently not supported.
*/
#define PB_MOC_PARAM_ORDER_NO_ORDERING		0x00
#define PB_MOC_PARAM_ORDER_ASC				0x01
#define PB_MOC_PARAM_ORDER_DEC				0x02
#define PB_MOC_PARAM_ORDER_CARTESIAN_X_Y	0x04
#define PB_MOC_PARAM_ORDER_CARTESIAN_Y_X	0x08
#define PB_MOC_PARAM_ORDER_ANGLE			0x0B
#define PB_MOC_PARAM_ORDER_POLAR			0x10
#define PB_MOC_PARAM_ORDER_X_Y_EXT_CC		0x20

#define PB_MOC_PARAM_ORDER PB_MOC_PARAM_ORDER_NO_ORDERING
/*@}*/


/*!
\anchor Number_of_minutiae
\name Number of minutiae
@{
These defines sets the number of minutiae during enroll or verfiy.
\n
\def PB_MOC_PARAM_MIN_MINUTIAE_ENROLL
Minimum number of minutiae required by matcher during enroll.
Default value is 0.
\n
\def PB_MOC_PARAM_MAX_MINUTIAE_ENROLL
Maximum number of minutiae handled by the matcher during enroll.
Default value is 64.

NOTE: This parameter may not be appropriate to convey to the
extractor as is due to e.g. other limiting factors as PDU
sizes of the transmission. It simply states the maximum that
will be accepted.

NOTE: If the extractor finds more minutiae than the maximum
that can be handles (considering all constraints) then a
minutiae pruning should take place e.g. as described in
ISO 19794-2.
\n
\def PB_MOC_PARAM_MIN_MINUTIAE_VERIFY
Minimum number of minutiae required by matcher during verify.
Default value is 0.
\n
\def PB_MOC_PARAM_MAX_MINUTIAE_VERIFY
Maximum number of minutiae handled by the matcher during verify.
Default value is 64.

NOTE: This parameter may not be appropriate to convey to the
extractor as is due to e.g. other limiting factors as PDU
sizes of the transmission. It simply states the maximum that
will be accepted.

NOTE: If the extractor finds more minutiae than the maximum
that can be handles (considering all constraints) then a
minutiae pruning should take place e.g. as described in
ISO 19794-2.
*/
#define PB_MOC_PARAM_MIN_MINUTIAE_ENROLL 0
#define PB_MOC_PARAM_MAX_MINUTIAE_ENROLL 128
#define PB_MOC_PARAM_MIN_MINUTIAE_VERIFY 0
#define PB_MOC_PARAM_MAX_MINUTIAE_VERIFY 128
/*@}*/


#endif