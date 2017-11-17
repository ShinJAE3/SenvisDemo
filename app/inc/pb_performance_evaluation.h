/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

/**
 *  Evaluates biometric performance for the specified fingerprint database.
 *  The evaluator is a framework utilizing pluggable BMF extractors, enrollers
 *  and verifiers (xev triplet). The figure below shows the process.
 *  
 *                        +-> Extractor --> Enroller --> Gallery --v
 *  database --> sample --|                                     Verifier --> score
 *                        +-> Extractor ---------------> Probe ----^
 *  
 *    - The database is described by an index file mapping person, finger and
 *      transaction ID to a fingerprint image sample.
 *  
 *    - The Extractor creates fingerprint templates. The extractor may produce
 *      the same or different templates used for gallery and probe.
 *      It is possible to use separate extractos for gallery and probe.
 *  
 *    - The Enroller is trained with one or more samples to produce the
 *      best possible representation of the finger.
 *  
 *    - The Verifier is used to cross-match the gallery and probe templates
 *      to produce the genuine and impostor scores. These may then be analyzed
 *      with an external tool to produce e.g. a DET graph.
 *  
 *  Options select which built-in extractor, enroller, verifier and template
 *  formats is used for evaluation, not all possible combinations are valid.
 */

#ifndef PB_PERFORMANCE_EVALUATION_H
#define PB_PERFORMANCE_EVALUATION_H

#include "pb_qualityI.h"
#include "pb_extractorI.h"
#include "pb_databaseI.h"
#include "pb_verifierI.h"
#include "pb_identifier.h"
#include "pb_preprocessorI.h"
#include "pb_guiI.h"
#include "pb_algorithmI.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Matching schemes.
 * NOTE: Use the PBMT scheme with care as it will allow system faults as
 * the training set and verification sets are not completely separated.
 */
#define PB_EVAL_FVC2   0 /* Two-way FVC */
#define PB_EVAL_FVC    1 /* FVC standard */
#define PB_EVAL_ALL    2 /* Match all impostors attempts */
#define PB_EVAL_PBMT   3 /* Two-way FVC but genuines matched against any genuine not part of multi-template  */
#define PB_EVAL_FVC2_X 4 /* FVC2 + include self fingers to impostors */
#define PB_EVAL_ALL_X  5 /* All + include self fingers to impostors */

/* Evaluation types */
#define PB_EVAL_DECISION 0
#define PB_EVAL_RAW      1
#define PB_EVAL_SCORE    2
/* Update types */
#define PB_EVAL_UPDATE_NONE    0
#define PB_EVAL_UPDATE_GENUINE 1
#define PB_EVAL_UPDATE_ALL     2
#define PB_EVAL_UPDATE_DEFERED 3

/**
 * A special template type defined and implemented by the performance
 * evaluator. This is needed when when the verifier/enroller expects
 * images rather than templates.
 *
 * When the gallery or probe template types are set to this template type
 * the extractor will not be called to generate a template. Instead the
 * image will be automatically wrapped in a template. To retrieve the image
 * from the template during verification/enrollment use the function*
 * pb_eval_templ2image().
 */
#define PB_EVAL_TEMPLATE_IMAGE ((pb_template_type_t)250)
/** Template types reserved for implementation of external algorithms */
#define PB_EVAL_TEMPLATE_EXTERN1 ((pb_template_type_t)251)
#define PB_EVAL_TEMPLATE_EXTERN2 ((pb_template_type_t)252)
#define PB_EVAL_TEMPLATE_EXTERN3 ((pb_template_type_t)253)

#define PB_EVAL_OP_XONLY   (1<<0)    // Do extraction then exit
#define PB_EVAL_OP_EONLY   (2<<0)    // Do extraction and enrollment then exit
#define PB_EVAL_OP_SAVE_ALIGN (1<<4) // Save alignment visualization (only if enr_num is 1)
#define PB_EVAL_OP_NO_PROTECT (1<<5) // Force overwrite of existing results
#define PB_EVAL_OP_SKIP_PROTECT (1<<6) // Aboort without error if existing results
#define PB_EVAL_OP_CHECK   (1<<7)    // Check parameters and exit

/* Forward declaration for Enroller, see definition below! */
typedef struct pb_eval_enrollerI_s pb_eval_enrollerI;

/**
 * ID ranges for gallery and probe samples. Each range is described
 * by a startID and endID in index position 0 and 1 respectively.
 *
 * It may be better to use pb_evaluate_parse_idrange() to initialize
 * all these values.
 */
typedef union {
    struct {
        struct {
            int person[2];
            int finger[2];
            int trans[2];
        } gallery;
        struct {
            int person[2];
            int finger[2];
            int trans[2];
        } probe;
    } r;
    int values[6*2];
} pb_eval_sample_range_t;

/**
 * Evaluation options that controls evaluation. Use pb_evaluate_reset_opt()
 * to get default sane values and then override.
 * 
 * Most fundamental options are the fingerprint database (index file),
 * extractor, enroller, verifier and template type(s).
 *
 *  - Index-file
 *    Maps personId, fingerId and transId to the samples image files
 *    to be evaluated.
 *
 *  - Enroller
 *    Each extracted gallery template is passed through an enroller
 *    for training before becoming the final gallery template.
 *    This steps allows for training from several samples to represent
 *    each gallery template.
 *
 * Enroller
 * --------
 * Each extracted gallery template is passed through an Enroller. An
 * enroller should implement the special interface pb_eval_enrollerI
 * interface which consist of three functions; create(), train() and
 * finalize().
 *
 * There are several built-in enrollers in the evaluator.
 *
 *   - Identity enroller
 *     Will simply create a gallery template from the first of all
 *     training samples. This can be used whenever training or enroller
 *     is not used such as in classical evaluation or as a baseline
 *     to compare more complex enrollers against.
 *
 *   - Score enroller
 *     Selects the template with the highest similarity score.
 *
 *   - Quality enroller
 *     Selects the template with the best image quality.
 *
 *   - Algorithm Multitemplate manager enroller
 *     Operates through the BMF Algo API to do offline enrollment.
 *
 *  General
 *  -------
 *    performace
 *        Name of this evaluation run. Result files will be saved in a
 *        subdirectory with this name. The default is 'performance'.
 *    verbose
 *        Increase verbosity level, use several to increase verbosity.
 *    match_scheme
 *        Specify matching scheme PB_EVAL_XXX:
 *  
 *        fvc  FVC standard matching scheme, may not be applicable in
 *             some cases.
 *        fvc2 Two-way FVC, this is the default, allows matching both ways
 *             but this is not always possible.
 *        all  Like fvc2 but impostor match all combinations, this causes
 *             a lot of verifications but typically without increasing
 *             statistical significance.
 *        pbmt Special fvc2 variant for Precice multi-templates where genuine
 *             matches are against any attempt not included in the multi-template.
 *             Use this scheme with care as training and verification set is
 *             not guaranteed to be separated.
 *  
 *     quality
 *        Specify a quality module to generate image quality values.
 *  
 *     score_files
 *        Control creation of individual score files, genuines.txt and impostors.txt
 *        besides just the score.txt frequency table.
 *
 *        Individual score files are often not needed and may be very large
 *        and take long time to analyze compared to the frequency table.
 *
 *        Values: -1: Use default behaviour.
 *                 0: Do not create.
 *                 1: Create
 *
 *        The current built in behaviour for default is to create the
 *        files unless multi-threading mode is used. The default
 *        setting will likely be changed in future releases.
 *
 *     num_threads
 *        If OpenMP compiled this variable may be set to change number
 *        of team threads. Default value 0 means to use number of
 *        processors. The actual number of used threads may be lower.
 *        When multiple threads are used the default is to turn off
 *        creation of individual score files.
 *
 *     operation
 *        Bitset of PB_EVAL_OP_ for modifying the standard evaluation.
 *  
 *    req_far
 *        The requested FAR level to use in verify templates. This
 *        level has impact when dynamic update or decision based
 *        verification. Default is 1:10K.
 *
 *    ver_type 
 *        One of _DECISION, _RAW or SCORE.
 *
 *    ver_update [0/1/2]
 *        Controls if and how dynamic update is done during verification.
 *        Note that this does not impact dynamic update during training part.
 *        A value of 0 means no update. A value of 1 means update from genuines
 *        and value 2 means update from impostors also. Update level will be
 *        at the specified req_far level. Update with impostors is a bit odd
 *        especially with a match-all scheme as there is statistical guarantee
 *        that one will be exposed.
 *  
 *  Enrollment training
 *  -------------------
 *  Enrollment training options control how the samples of the database are
 *  divided into training (gallery) and verification (probe) sets.
 *  There are several possible ways to dive the samples, the figure below
 *  illustrates two variants with 42 samples per finger.
 *
 *   sample| enr_num = 16          enr_enr = 4         | probes when         |
 *     #   | enr_ver = 35          enr_dyn = 8         | enr_ver = 0         |
 *   ------+---------------------+---------------------+---------------------+
 *       1 | Gallery #1            1,2,3,4             | Probe #1            |
 *       2 | Up to enr_num first | used for enrollment | Probe #2            |
 *       3 | samples forms the    (enr_enr)            | Probe #3            |
 *       4 | first gallery.      |---------------------+ ...                 |
 *       5 |                       5,6,7,8,9,10,11,12  |                     |
 *       6 | The gallery is      | used for dynamic    | Interleaved         |
 *     ... | trained from the      update (enr_dyn)    | verification set is |
 *      13 | enr_enr + enr_dyn   |---------------------+ used when dedicated |
 *      14 | first samples and     13,14,15,16 unused  | verification set    |
 *      15 | in this case 4      | (enr_enr+enr_dyn=12)| is not specified,   |
 *      16 | samples are unused.                       | enr_ver = 0.        |
 *         +---------------------+---------------------+                     |
 *      17 | Gallery #2                                | No gallery will     |
 *      18 | Samples that form     Same division       | however be verified |
 *      .. | the second gallery    as above.           | against a sample    |
 *      32 | for this finger.                          | from which it has   |
 *         +-------------------------------------------+ been trained from.  |
 *      33 | Orphaned, since too few samples           |                     |
 *      34 | to form the last gallery (< enr_min).     | Gallery #1 will     |
 *         +===========================================+ be verified against |
 *      35 | Probe #1  Dedicated verification set,     | samples 13-42.      |
 *      36 | Probe #2  begins at transId = enr_ver.                          |
 *     ... | ....                                      | Gallery #2 will     |
 *      41 |           All galleries will be verified  | be verified against |
 *      42 | Probe #8  against samples 35-42.          | samples 1-16, 29-42 |
 *   ------+-------------------------------------------+---------------------+
 *
 *    enr_num=<N>
 *       Specifies max number of samples are used as a base to form each
 *       gallery. When there are many samples several galleries per finger will
 *       be formed. The parameters enr_enr and enr_dyn may further restrict how
 *       and if all samples in each gallery are used. Galleries will not be formed
 *       in the dedicated verification set, see enr_ver. Default value is 1.
 *
 *    enr_enr=<N>
 *        How many samples the enroller shall use for classical enrollment,
 *        any further training samples should be used for dynamic template
 *        update. This division may or may not be honored by the enroller
 *        depending on the capabilities. The default value is enr_num.
 *
 *    enr_dyn=<N>
 *        How many samples the enroller shall use for dynamic template update.
 *        Such samples always follows after enrollment samples.
 *        The default value is 0. Note that enr_enr + enr_dyn <= enr_num
 *
 *    enr_ver=<transId>
 *        Reserves a dedicated verification set for probe samples. Samples with
 *        transaction ID >= transId belongs to this set, this also limits the
 *        number of samples available to form gallery templates.
 *        Without such a division all samples may be used as probe samples
 *        but verification between gallery and probe will not include those
 *        samples used to train each gallery from. Default is 0 which means
 *        no division.
 *
 *    enr_min=<N>
 *       A gallery template trained from fewer than N samples will be orphaned
 *       (removed). The default value is enr_enr with the result that all
 *       gallery templates are equally trained. A different setting may be
 *       useful in some cases.
 *  
 *  Enroller and multi-template Parameters
 *  --------------------------------------
 *  Enrollers that create templates as multi-templates or supports dynamic
 *  update may be tunable by parameters listed here or through the generic
 *  attributes. See each enroller for details which parameters it supports.
 *  
 *    attrs
 *        A comma separated name=value list of attributes that can be
 *        used to pass in evaluation options to a separate enroller e.g.
 *        See pb_evaluate_get_int().
 *  
 *    mt_size=<N>
 *        Maximum number of instances contained within each multi-template.
 *        Default value is 1.
 *  
 *    far_enr=PB_FAR_X
 *        FAR enroll suggest the security level used during classical
 *        enrollment. This security level should typically be set low
 *        unless it is important to reject samples from different fingers.
 *        The default value is 1:100.
 *
 *  Database manipulation
 *  ---------------------
 *  Database manipulation filters the sample images in some way and thus
 *  creates a virtually a different database. For this reason most manipulations
 *  will extend the given database name (given in the index file) with a suffix.
 *  Hence such evaluation runs will be saved in different subdirectories.
 *  
 *    db_suffix=<name>
 *        The database name is extended with '_<name>'. This suffix is
 *        appended after any automatic suffixes has been appended.
 *
 *    db_crop_cols
 *    db_crop_rows
 *        Each dimension (cols x rows) with a value value != 0 will center crop
 *        images. The database name is extended with '_NxM'. Note that image
 *        cropping is done on the original image regardless of the resolution.
 *  
 *    db_finger=<ID> or 0 for all
 *        Evaluate only the fingers matching the specified finger ID. Default is
 *        to evaluate all fingers in database. The database name is extended
 *        with '_f<ID>'.
 *
 *    db_prune_isize_cols
 *    db_prune_isize_rows
 *       Images smaller than either the given cols, rows will be pruned from
 *       the evaluation. There is already a built in filter for 12x12.
 *       This filter does not alter the database name as it is mainly targeting
 *       investigation on performance for swipe sensors and databases containing
 *       short swipes.
 *
 *    db_idrange p-range/f-range/t-range:p-range/f-range/t-range
 *       Samples not part of the gallery (before ':') or probe will be pruned.
 *       The three range values correspond to person, finger and transaction ID.
 *       If probe range is not given it will be same as the gallery range. A range
 *       may be left out or be the empty string which means all ID values.
 *       The symbol '-' is used as range delimiter. Examples:
 *
 *       ''      default, no pruning.
 *       1       means person 1,    all fingers, all transactions.
 *       5-      means person >= 5, all fingers, all transactions.
 *       -5      means person <= 5, all fingers, all transactions.
 *       5-9     means person 5-9,  all fingers, all transactions.
 *       1/2     means person 1,    finger 2,    all transactions.
 *       /2      means all persons, finger 2,    all transactions.
 *       /2/1-8  means all persons, finger 2,    transactions 1-8.
 *       1:      means gallery person 1, but no pruning for probe.
 *       1/2:3/4 means gallery person 1 finger 2 and probe person 3 finger 4.
 *
 *   run_part_n, run_part_m
 *       Partitioning of evaluation, suited for splitting an evaluation
 *       over several separate runs or several workstations. The first value, n,
 *       is the part to run and the second, m, is number of parts. If 0 then
 *       this parameter has no effect. The partitioning is made on gallery persons.
 *      
 *   db_rot_angle
 *   db_rot_Nprobe
 *   db_rot_Ngallery
 *       Handles source image rotation <angle:Np:Ng>.
 *       Rotate source images by the given angle. Rotation is made
 *       for every N:th source image based on its ordinal transaction number
 *       in the index file. Separate values for N is specified for probe
 *       and galleries to enable differnt kinds of test scenarious such 
 *       as rotate all probes a given angle and let enrollment training
 *       be train from from both rotated and unrotated images.
 *       Rotation should typically be done on a larger image and then combined
 *       with a suitable image crop operation to get a well defined
 *       image size with actual image data.
 *       
 *        -rot=64:1:0 Will rotate all probe images 90 degrees
 *                    but no gallery images.
 *        -rot=64:0:2 Will rotate every 2nd gallery image 90 degrees but
 *                    no probe images.
 *        -rot=64:1:2 Will rotate all probe images and every 2nd gallery image
 *                    90 degrees.
 *
 */
typedef struct {
    int verbose;        // Request more or less logging
    int operation;      // Operation modifier bitfield, see PB_EVAL_OP_XXXX
    char db_path[512];  // Database index file to be evaluated
    char *run_name;     // Name of the run, base directory of output
    int  match_scheme;  // Matching scheme PB_EVAL_XXX
    int  match_gen;     // No genuine matching if set to 0
    int  match_imp;     // No impostor matching if 0

    int  ver_imp;       // Maximum impostor transaction id. Set to zero by default.

    pb_algorithm_t* algorithm;          // The algorithm object to evaluate
    pb_algorithm_t* algorithm_ver;      // If set this is used during verify

    /* Algorithm options. */
    pb_sensor_type_t   sensor_type; // If 0/unknown eval will choose
    pb_sensor_size_t   sensor_size; // depening on image size.
    int                ver360;      // 1(default):enable 360, 0:disable

    /* Enroller to use to produce the enrolled (mt) templates */
    const pb_eval_enrollerI* enroller;

    pb_identifier_t*         identifier;
    pb_template_type_t       gallery_template_type;
    pb_template_type_t       probe_template_type;

    /* Identification parameters*/
    uint8_t id_rank;  // Number of score/id values to log

    /* Verify req level has impact when decision or dynamic update. */
    pb_far_t req_far;

    /* Dynamic update req level has impact when decision or dynamic update. */
    pb_far_t req_far_dyn;

    /* Evaluation type, one of PB_EVAL_DECISION, PB_EVAL_RAW or
     * PB_EVAL_SCORE(default). */
    int ver_type;

    /* Dynamic update during verification, default=0 means no. */
    int ver_update;

    /* Enroller training parameters */
    int enr_num;  // Number of samples to train from
    int enr_enr;  // Suggested number of sampled for classical enroll
    int enr_dyn;  // Suggested number of sampled for dynamic update
    int enr_ver;  // transId to divide samples into of training and verification set
    int enr_min;  // Min number of training samples allowed to form a gallery

    /* GUI to log information to */
    const pb_guiI*       log_gui;    // null logs to stdout
    /* Quality module to calculate image quality values */
    const pb_qualityI*   quality;    // null means no quality

    /* Optional image pre-processing hooks, any non-null pointer is run in sequence */
#   define PB_EVAL_MAXPPF 8
    const pb_preprocessorI* img_ppf[PB_EVAL_MAXPPF];

    /* Control when genuines.txt and impostors.txt files will be created */
    int score_files;

    /* Controls whether match_results.txt will be created. */       //#BS-intern
    int match_files;  //#BS-intern
    int feature_vectors; //#BS-intern
    /* Controls whether link_stats_{genuines, impostors}.tx will be created. */ //#BS-intern
    int link_stats_files; //#BS-intern

    /* Request number of threads for verification, requires an OpenMP
     * build option to have any effect. */
    int num_threads;

    /* Enroller and multi-template configuration parameters */
    char     attrs[512];  // Generic name=value[,...] attribute values
    int      mt_size;     // Max kept templates per multi-template 
    pb_far_t far_enr;     // Suggested FAR level for classical enrollment DEPRICATED

    /* Database manipulation attributes.
     * Database manipulation will also rename the result-direcory
     * database directory to reflect the change of the database.
     */

    /* Appends a suffix at the end of database name separated by a '_'. */
    char db_suffix[128];
    /* Image cropping */
    short db_crop_cols, db_crop_rows;
    /* Image cropping for probe images */
    short db_crop_probe_cols, db_crop_probe_rows;

    /*Positon on where to crop the image */
    short db_crop_position;  // 0 = Center, 1 = Random 2 = along border random
    short db_crop_position_seed;  // seed for the random position of the crop

    /* Evaluate only the specfied finger ID */
    int   db_finger_id;
    /* Prune small images */
    short db_prune_isize_cols, db_prune_isize_rows;
    /* Prune on ID ranges, prunes gallery and probe separately. */
    pb_eval_sample_range_t db_idrange;
    /* Partition of run into several parts N:M */
    int run_part_n, run_part_m;
    /* Rotation of ecery N:th probe and/or gallery. N ==0 means no rotation */
    uint8_t db_rot_angle;    // Brads
    int     db_rot_Nprobe;   // Rotates probes    where transOrdinal%N == (N-1)
    int     db_rot_Ngallery; // Rotates galleries where transOrdinal%N == (N-1)
    /* Permute order of images for each finger in the database. Interpretation
     *   0  original order
     *   1  shuffle enrollment images only
     *   2  shuffle verification images only
     *   3  shuffle both */
    int db_random_order;
    unsigned int db_random_order_seed;

    /** The size of the queue used for dynamic update to mitigate template 
      * poisoning. Default 0 equal to old behavior without the queue. */
    int dynamic_update_queue_size;

    /* Selects multi-finger verification mode if > 0. The value itself
     * specifies max number of fingers to use from each individual.
     * Only decision based verifications are supported, ver_update
     * support. The n parameter specifies number of repeats. */
    int ver_multi, ver_multi_n;

    /** Command line arguments, will only be logged in info file
     * if specified */
    int argc;
    char** argv;
} pb_eval_opt_t;

/** Reset the options to a default sane values.
 * Use this function on an option struct before filling in values
 * since it increases backwards compatibility when new options
 * are added to the struct.
 *
 * @param[inout] opt  the eval options to reset.
 */
void pb_evaluate_reset_opt(pb_eval_opt_t* opt);

/** Returns an integer value attribute from the attribute list of the
 * current evaluation option attributes.
 *
 * All enroller special parameters such as mt_size and far_enr is
 * also accessible though this function when using null attrs pointer
 * for enrollers to be able to get all evaluation parameters.
 *
 * @param[in] session the session used for the evaluation.
 * @param[in] name    name of the attribute to return.
 * @param[in] def     default value to return if the attribute is missing.
 *
 * @returns the integer value of the attribute or value def if missing.
 */
int pb_evaluate_get_int(pb_session_t* session, const char* name, int def);

/** Returns a string value attribute from the attribute list of the
 * current evaluation option attributes.
 *
 * @param[in] session the session used for the evaluation.
 * @param[in] name    name of the attribute to return.
 * @param[in] def     default value to return if the attribute is missing.
 * @param[in] buff    Work area, result may point here.
 *
 * @returns the integer value of the attribute or value def if missing.
 */
const char* pb_evaluate_get_str(pb_session_t* session, const char* name, const char* def, char buff[64]);

/**
 * Parses a textual range representation into a ranges datatype. A
 * range specifies range for both gallery and probes, separated
 * by a ":"
 *
 *     galleryID-range:probeID-range.
 *
 * Each such range is further broken down into three parts;
 * 
 *     personID-range/fingerID-range/transID-range
 *
 * Where each range part is further broken down into the following
 * possible combinations.
 *
 *     N    // range N..N
 *     -N   // range 0..N
 *     N-   // range N..MAX
 *     N-M  // range N..M
 *     -    // range 0..MAX
 *     ""   // range 0..MAX ("" means empty string)
 *
 * It is not required for all parts to be specified which means
 * they default to unlimited range. If however the probeID-range
 * is not specified it will be defined as the same range as the
 * gallery range.
 *
 * Some examples shows the possibilities.
 *
 *     1/1/1:2/2/2
 *     1/1/1        // same as  1/1/1:1/1/1
 *     5:           // same as 5//://
 *     5-:5-10      // same as 5-//:5-10// 
 *
 * @param[in] spec the range specification to be parsed.
 * @param[out] ranges the output containing  the parsed ranges.
 *
 * @return 0 success an 1 on failure.
 */
int pb_evaluate_parse_idrange(const char* spec, pb_eval_sample_range_t* sample_range);

/** Guess appropriate sensor size and type.
 *
 * The first image of the database is read and used to determine
 * the sensor size and type. The crop, and preprocessor filters
 * are applied on the image.
 *
 * In case the size cannot be determined, e.g. cannot read
 * database then default unknow type and size.
 *
 * Returns PB_RC_FILE_OPEN_FAILED if no image could be read.
 */
pb_rc_t  pb_evaluate_guess_sensor(pb_session_t* session, pb_eval_opt_t* opt, 
                                  pb_sensor_type_t* sensor_type,
                                  pb_sensor_size_t* sensor_size);

/** Evaluates the biometric performance of the extractor and 
  * verifier combination.
  *
  * The modules and various options used for the evaluation is
  * specifed in the opt option. New options may be added in future
  * releases. To stay reasonable backwards compatible initialize the
  * struct using pb_evaluate_reset_opt() before assigning values.
  *
  * @param[in]   opt the options controlling the evaluation.
  *
  * @return PB_RC_OK if successful, or an error code. 
  */
pb_rc_t
pb_evaluate_performance_ext (pb_session_t* session, pb_eval_opt_t* opt);

/**
 * Creates enroller instance. See also general description for
 * the EnrollerI interface.
 *
 * NOTE: There may be several instances created at the same time.
 *       This will however only happen when evaluation is done using
 *       more than one thread. In cases where only one instance can
 *       be realized the enroller must fail the operation and the
 *       evaluation be started with only one thread.
 *
 * The instance will eventually be released via a call to the
 * finalize function.
 *
 * Note that there may be more enr_enr samples to train from,
 * these samples are then suggested to be used as dynamic update
 * of the template formed from the enrollment samples. This
 * is controlled via the enr_dyn evalaluation parameter. There
 * may potentially be a large number of such samples.
 *
 * If dynamic update is not supported those samples should be
 * skipped to enable comparing results with other enrollers that
 * supports it.
 *
 * It is suggested that the enr_enr parameter is honored as a
 * classical enrollment and optionally dynamic update for the
 * remaining samples.
 *
 * The far_enr parameter should also be honored when possible even if
 * the exact value cannot not be ensured there may at least be a
 * secure/unsecure variant via a threshold. DEPRICATED.
 *
 * @param{in] session  The session object used during evaluation.
 * @param[in] finger   The finger identity of the gallery template
 *                     to be created. Retain this object to keep it.
 * @param[in] enr_enr  The evaluation parameter enr_enr suggests
 *                     using the first enr_enr samples for enrollment
 *                     and the rest for dynamic update.
 *                     This parameter will be at least 1.
 *
 * @return A handle that will passed in to subsequent calls to the
 * train and finalize functions. This value must be != 0 on success.
 *
 */
typedef void* pb_eval_enrollerI_create_fn_t(pb_algorithm_t* algorithm, 
                                            pb_finger_t* finger,
                                            int enr_enr);
/**
 * Called to submit another training sample to the enroller.
 * 
 * @param[in] handle   The handle returned from enroller create,
 * @param[in] template_ The template to train from. This template
 *                     has the type of enr_template_type and has been
 *                     created using the extactor passed in to the
 *                     evaluation. In case only images are possible
 *                     specify the PB_EVAL_IMAGE_TEMPLATE_TYPE and
 *                     retrieve the image using the function
 *                     pb_eval_templ2image().
 *                     Retain this object to keep it.
 * @param[in] finger   The finger beeing trained from, mostly usefull
 *                     debugging purpose.
 * @param[in] quality  Optional image quality, this parameter may be
 *                     null in case quality is not available. To get
 *                     quality values evaluiation must specify a quality
 *                     module.
 *
 * @return PB_RC_OK if successfull.
 */
typedef pb_rc_t pb_eval_enrollerI_train_fn_t(void* handle, 
                                             pb_template_t* template_, 
                                             pb_finger_t* finger, 
                                             pb_quality_t* quality);

/**
 * Called to end the training for this gallery template. The final
 * template should be created and returned. Any allocated resources
 * should be released.
 *
 * In case no training samples has been received or if for other
 * reasons no enrollment is possible return NULL template to fail
 * enrollment for this gallery template.
 *
 * @param[in] handle   The handle returned from enroller create,
 * @param[out] template_ The final gallery template. This may be
 *             set to NULL in case enrollment failed.
 *
 * @return PB_RC_OK on success, only return failed code on
 *   severe errors, with failed enrollment return NULL template.
 */
typedef pb_rc_t pb_eval_enrollerI_finalize_fn_t(void* handle, pb_template_t** template_);

/**
 * Evaluation Enroller.
 *
 * The enroller is used as an intermediate step to create the gallery
 * templates used during evaluation. The enroller will be called zero
 * or more times with samples to train from. The purpose of the
 * enroller is to create the best possible gallery template from all
 * the training samples.
 *
 * The enroller has three phases which corresponds to the functions
 * of the 
 *
 *   - create, called once at start
 *   - train, called zero or more times
 *   - finalize, called last
 *
 * During evaluation the following will be repeated for all
 * gallery templates that shall be created.
 *
 *   1. Create enroller instance with the purpose of creating a new
 *      gallery template
 *   2. Use the supplied extractor to create a template of type
 *      enr_template_t.
 *   3. Call enroller train for this template.
 *   4. Reapeat 3 depending on enrollment parameters.
 *   5. Finalize enroller to get the final template.
 *
 * Some comments:
 *   - The controller will do the basic extraction before
 *     calling the enroller using the supplied extractor.
 *   - The final template type returned by the enroller is typically
 *     not the same as enr_template_type, rather e.g. a sort
 *     of multi-template.
 *   - In cases where the enroller must operate in images
 *     as input set enr_template_type to PB_EVAL_IMAGE_TEMPLATE_TYPE.
 *     To decode such a template to get the image use the
 *     function pb_eval_templ2image().
 */
struct pb_eval_enrollerI_s {
    pb_eval_enrollerI_create_fn_t* create;
    pb_eval_enrollerI_train_fn_t* train;
    pb_eval_enrollerI_finalize_fn_t* finalize;
};

/**
 * Algorithm XEV; extractor, enroller anv verifier. This modules
 * should be used when evaluating a selected algorithm via
 * the algorithm API.
 *
 * The enroller supports parameter mt_size, far_enr. The
 * far_enr is not directly supported but is mapped to the manager
 * parameter 'prevent_enrollment_of_multiple_fingers'. This parameter
 * is enabled if far_enr is set higher than the PB_MTM_ENROLL_FAR
 * security level (1:1000).
 */
extern pbif_const pb_eval_enrollerI pb_eval_algomt_enroller;
    
/**
 * Basic multi-template container enroller. This enroller uses
 * the pb_multitemplate_t object to create a multi-template.
 * Dynamic update training is not supported. Traning samples
 * after initial enroll or over max template size is discarded.
 */
extern pbif_const pb_eval_enrollerI pb_eval_mtcontainer_enroller;

/**
 * The identity enroller is a simplistic enroller which is intended
 * to be used when the enrollment process is just to accept whatever
 * sample is given. It may e.g. be used in classic evaluation where
 * no training is made.
 *
 * If used with several samples to train from the final gallery
 * template will be the first. 
 */
extern pbif_const pb_eval_enrollerI pb_eval_identity_enroller;

/**
 * The quality enroller picks the sample with the best image
 * quality. This enroller requires quality values and hence a
 * Quality module must be used during evaluation, else the first
 * sample is used.
 */
extern pbif_const pb_eval_enrollerI pb_eval_quality_enroller;

/**
 * The best score enroller picks the sample with the best matching
 * score between all of the enr_enr samples. It does not handle
 * dynamic update samples.
 */
extern pbif_const pb_eval_enrollerI pb_eval_score_enroller;

/**
 * Internal test function for algorithms.
 */
void pb_eval_algo_depth(pb_algorithm_t* algo, int n);

#ifdef __cplusplus
}
#endif

#endif /* PB_PERFORMANCE_EVALUATION_H */
