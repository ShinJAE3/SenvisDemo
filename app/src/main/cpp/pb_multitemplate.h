/*
 * Copyright (c) 2008 - 2013, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_MULTITEMPLATE_H
#define PB_MULTITEMPLATE_H

#include "pb_template.h"
#include "pb_returncodes.h"


#define PB_MULTITEMPLATE_NO_TAG    (-1 << 31)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A multi-template object is used to enclose several templates in one
 * container. Enclosed templates can be fetched, replaced, removed and meta
 * data may be attached to them or to the multi-template as a whole.
 *
 * Multi-templates can be converted to/from (encoded/decoded) pb_template_t
 * objects of type PB_TEMPLATE_PB_MULTIPLE.
 *
 * NOTE: This is not an immutable type! There are no reference counters and
 *       objects may be altered during their life time.
 */
typedef struct pb_multitemplate_st pb_multitemplate_t;

/**
 * Meta data types.
 *
 * This is to allow each application to store meta data with templates in the
 * multi-template container. Each application making use of multi-templates can
 * register types with this enumerator.
 *
 * Numbers are split into these categories:
 *         0   Reserved. Represents "no type" when initializing iteration.
 *   1 -  63   Reserved for Precise Biometrics' BMF modules and controllers.
 *  64 - 255   Available for other applications.
 * (Type is represented using one byte of data, max type value is thus 255)
 */
typedef enum pb_multitemplate_metadata_type_e {

    /** Numbers reserved for PB (1-63) */
    PB_MULTITEMPLATE_METADATA_DYNAMIC_MULTIENROLL      = 0x01,
    PB_MULTITEMPLATE_METADATA_ISLAND                   = 0x02

    /** Numbers available for other applications (64-255) */

} pb_multitemplate_metadata_type_t;

/**
 * Creates a new empty multi-template.
 * @return The created multi-template.
 */
pb_multitemplate_t *
pb_multitemplate_create(void);

/**
 * Deletes the multi-template and frees all resources it has allocated.
 * @param multi The multi-template to delete. Deleting a null pointer
 * has no effect.
 */
void
pb_multitemplate_delete(pb_multitemplate_t *multi);

/**
 * Adds a new template to the multi-template.
 * NOTE: A maximum number of 255 templates may be saved with a multi-template.
 * @param[in] multi The multi-template container to add a template to.
 * @param[in] tag A tag to associate the template with. All future references
 *    of the template will be addressed using this tag value. The tag value
 *    must be unique within the multi-template and the value
 *    PB_MULTITEMPLATE_NO_TAG is reserved.
 *    If the multi-template already contains an instance with this tag value,
 *    the operation will fail.
 * @param[in] template_ The template to add. retain() will be called on this
 *    object and delete() once it's removed from the container.
 * @return PB_RC_OK on success or an PB_RC_X error code on error.
 */
pb_rc_t
pb_multitemplate_add_instance(pb_multitemplate_t *multi,
                              int32_t tag,
                              pb_template_t *template_);

/**
 * Adds a new template to the multi-template without specifiying a tag. The tag
 * will automatically be set to 'the highest tag value in the multitemplate' + 1.
 * NOTE: A maximum number of 255 templates may be saved with a multi-template.
 * @param[in] multi The multi-template container to add a template to.
 * @param[in] template_ The template to add. retain() will be called on this
 *    object and delete() once it's removed from the container.
 * @param[out] tag is the tag received for the added instance, which can be used
 *             to attach metadata to the instance. May be set to 0 if not needed.
 * @return PB_RC_OK on success or an PB_RC_X error code on error.
 */
pb_rc_t
pb_multitemplate_add_instance_no_tag (pb_multitemplate_t* multi,
                                      pb_template_t* template_,
                                      int32_t* tag);

/**
 * Remove a template and any associated meta data from the container.
 * @param[in] multi The multi-template container from which to remove.
 * @param[in] tag The tag value a of the template to remove.
 * @return PB_RC_OK on success or PB_RC_NOT_FOUND if no instance with the
 *     provided tag was found.
 */
pb_rc_t
pb_multitemplate_remove_instance(pb_multitemplate_t *multi, int32_t tag);

/**
 * Removes all templates and any associated meta data from the container.
 * @param[in] multi The multi-template container from which to remove.
 * @return PB_RC_OK on success or an error code.
 */
pb_rc_t
pb_multitemplate_remove_all_instances(pb_multitemplate_t *multi);

/**
 * Changes the instance associated with tag to the provided template parameter.
 *
 * Any metadata associated with the tag will be untouched.
 * 
 * @param[in] multi The multi-template container to update template data in.
 * @param[in] template_ The template to replace with.
 * @param[in] tag is the tag of the template to replace/update.
 * 
 * @return PB_RC_OK on success or an PB_RC_X error code on error.
 */
pb_rc_t
pb_multitemplate_update_instance (pb_multitemplate_t* multi,
                                  pb_template_t* template_,
                                  int32_t tag);

/**
 * Used to iterate through the instances of a multi-template.
 * Gives a reference (the tag value) to the next instance in the iteration once
 * the current is known.
 *
 * Example:
 * int32_t tag = PB_MULTITEMPLATE_NO_TAG;
 * while (pb_multitemplate_next_tag(multi, &tag)) {
 *   pb_template_t *t = pb_multitemplate_get_instance(multi, tag);
 *   // Work on t
 * }
 *
 * @param multi The multi-template to iterate through.
 * @param tag[in|out] In: The tag value of the current instance. To get the
 *    first value of iteration tag should be set to PB_MULTITEMPLATE_NO_TAG.
 *    Out: Tag value of the next instance.
 * @return 1 if a tag was fetched or 0 if no was found or at end of iteration.
 */
int
pb_multitemplate_next_tag(const pb_multitemplate_t *multi, int32_t* tag);

/**
 * Get a template from a multi-template.
 * NOTE: It's the callers responsibility to delete the returned object.
 * @param[in] multi The multi-template container.
 * @param[in] tag The tag value of the instance to fetch.
 * @return The template or 0 if no instance was found for the given tag value.
 */
pb_template_t *
pb_multitemplate_get_instance(const pb_multitemplate_t *multi, int32_t tag);

/**
 * Get a template at a specific index (beginning from 0) from a multi-template.
 * NOTE: It's the callers responsibility to delete the returned object.
 * @param[in] multi The multi-template container.
 * @param[in] index The index at which the requested template should be found.
 * @param[out] tag The tag of the returned template, may be set to 0 if not needed.
 * @return The template or 0 if no instance was found for the given index.
 */
pb_template_t* 
pb_multitemplate_get_instance_for_index (const pb_multitemplate_t* multi, 
                                         int index, 
                                         int32_t* tag);

/**
 * Decodes (de-serializes) a template (pb_template_t) of type
 * PB_TEMPLATE_PB_MULTIPLE into a multi-template (pb_multitemplate_t).
 * NOTE: It's the callers responsibility to delete the created object by
 *       calling pb_multitemplate_delete().
 * @param[in] template_ The binary representation of the template as a
 *    pb_template_t object.
 * @param[out] multi A pointer to the created pb_multitemplate_t.
 * @return PB_RC_OK on success or an PB_RC_X error code on error.
 */
pb_rc_t
pb_multitemplate_decode(const pb_template_t *template_,
                        pb_multitemplate_t **multi);

/**
 * Encodes (serializes) a multi-template (pb_multitemplate_t) into a
 * pb_template_t object, e.g. in order to prepare it for permanent storage.
 * NOTE: It's the caller's responsibility to delete the returned object.
 * @param[in] multi The multi-template to serialize.
 * @param[out] template_ A pointer to the created pb_template_t object.
 * @return PB_RC_OK on success or PB_RC_MEMORY_ALLOCATION_FAILED in case
 *     not enough memory was available.
 */
pb_rc_t
pb_multitemplate_encode(const pb_multitemplate_t *multi,
                        pb_template_t **template_);

/**
 * Returns the size an encoded template would be
 */
uint32_t
pb_multitemplate_encode_size(const pb_multitemplate_t *multi);

/**
 * Returns the number of templates currently enclosed in the multi-template.
 * @param[in] multi The multi-template to check.
 * @return The number of enclosed templates.
 */
uint8_t
pb_multitemplate_get_nbr_of_instances(const pb_multitemplate_t *multi);

/**
 * Adds application specific meta data to the multi-template. Meta data types
 * must be registered with the enum pb_multitemplate_metadata_type_t above.
 * Meta data can be associated with a specific instance in the multi-template or
 * with the multi-template as a whole (general meta data). Each instance can
 * only hold one piece of meta data of the same type (and only one piece of meta
 * data of the same type can be stored as general meta data).
 * @param[in] multi The multi-template to store meta data with.
 * @param[in] tag Tag value of the instance to associate the meta data with. A
 *    value of PB_MULTITEMPLATE_NO_TAG indicates that the meta data should not
 *    be associated with a specific instance but be saved as general meta data.
 * @param[in] type The meta data type (PB_MULTITEMPLATE_METADATA_X).
 * @param[in] data Arbitrary data (that the application has to decodes itself)
 * @param[in] data_size Size of the provided data in bytes.
 * @return PB_RC_OK on success or an PB_RC_X error code on error (e.g. if there
 *    is already meta data of the same type registered with the instance).
 */
pb_rc_t
pb_multitemplate_add_metadata(pb_multitemplate_t *multi,
                              int32_t tag,
                              pb_multitemplate_metadata_type_t type,
                              const uint8_t *data,
                              uint16_t data_size);

/**
 * Removes meta data from the multi-template.
 * @param[in] multi The multi-template to remove meta data from.
 * @param[in] tag The tag value of the instance to remove meta data from. A
 *    value of PB_MULTITEMPLATE_NO_TAG indicates that the meta data to remove is
 *   general meta data.
 * @param[in] type The type of meta data to remove. (Other types associated with
 *    the same multi-template and instance remains untouched.)
 */
pb_rc_t
pb_multitemplate_remove_metadata(pb_multitemplate_t *multi,
                                 int32_t tag,
                                 pb_multitemplate_metadata_type_t type);

/**
 * Used to iterate through meta data types stored with a specific multi-template
 * instance or to iterate through general meta data. Gives a reference (the
 * meta data type) to the next meta data in the iteration once the current
 * is known.
 *
 * Example:
 * int32_t tag = PB_MULTITEMPLATE_NO_TAG;
 * pb_multitemplate_metadata_type_t type = (pb_multitemplate_metadata_type_t)0;
 * while (pb_multitemplate_next_metadata(multi, tag, &type)) {
 *   uint8_t *data = pb_multitemplate_get_metadata(multi, tag, type);
 *   // Work on data
 * }
 * @param[in] multi The multi-template to iterate through for meta data.
 * @param[in] tag The tag value of the instance to iterate through. A value of
 *    PB_MULTITEMPLATE_NO_TAG indicates that multi-template general meta data
 *    should be iterated.
 * @param[in|out] type In: The current meta data type in the iteration. Set type
 *    to 0 to get the first entry of the iteration. Out: The next meta data type
 * @return 1 if a meta data type was fetched or 0 if no was found or at end of
 *    iteration.
 */
int
pb_multitemplate_next_metadata(pb_multitemplate_t *multi,
                               int32_t tag,
                               pb_multitemplate_metadata_type_t *type);

/**
 * Returns meta data from the multi-template.
 * NOTE: The returned data is owned by the multi-template and may not be
 *       deleted. The data as such is never interpreted by the mutli-template
 *       and may thus be altered by applications as long as data_size is
 *       respected.
 * @param[in] multi The multi-template to fetch meta data from.
 * @param[in] tag Tag value of the instance to fetch meta data from. A value of
 *    PB_MULTITEMPLATE_NO_TAG indicates that general (multi-template global)
 *    meta data should be fetched.
 * @param[in] type The type of meta data to get.
 * @param[out] data_size Will be set to the size of the returned meta data. Can
 *    be set to 0 if not size information is requested.
 * @return The found meta data or 0 if none was found.
 */
uint8_t *
pb_multitemplate_get_metadata(pb_multitemplate_t *multi,
                              int32_t tag,
                              pb_multitemplate_metadata_type_t type,
                              uint16_t *data_size);

/** 
 * Copies metadata from one multitemplate to another. 
 * 
 * @param[in] from_multitemplate is the multitemplate to copy metadata from.
 * @param[in] to_multitemplate is the multitemplate to copy metadata to.
 * @param[in] tag is the tag of the template containing the metadata to copy 
 *            or the PB_MULTITEMPLATE_NO_TAG if it is the global metadata that 
 *            is to be copied.
 * @param[in] metadata_type is the type of the metadata to be copied.
 */
pb_rc_t
pb_multitemplate_copy_metadata (pb_multitemplate_t* from_multitemplate, 
                                pb_multitemplate_t* to_multitemplate,
                                int32_t tag, 
                                pb_multitemplate_metadata_type_t metadata_type);

#ifdef __cplusplus
}
#endif

#endif // PB_MULTITEMPLATE_H
