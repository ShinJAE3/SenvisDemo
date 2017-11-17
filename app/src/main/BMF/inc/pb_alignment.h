/*
 * Copyright (c) 2008 - 2017, Precise Biometrics AB
 * All rights reserved.
 *
 */

#ifndef PB_ALIGNMENT_H
#define PB_ALIGNMENT_H

#include "pb_types.h"
#include "pb_returncodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Note! It is the callers responsibility to make sure that 
 * all alignment input parameters are valid, i.e. not null (0). 
 * Passing a null alignment parameter to a function will result 
 * in a runtime error! */

/** An alignment object describes translation and rotation alignment
  * primarily between two fingerprints. As such an alignment is
  * primarily created as part of a fingerprint match operation. The
  * alignment is seen as from the enrolled fingerprint to the
  * verification fingerprint which can be expressed as affine
  * transformation:
  * 
  *     V = T*R*E
  *
  * Where E is the coordinates of the enrolled template and V is the
  * coordinates of the verification template. R is rotation
  * counter-clockwise. Note that rotation is applied before
  * translation. The relation can also be expressed as
  *
  *     Vx = Ex * cos(a) - Ey * sin(a) + dx
  *     Vy = Ex * sin(a) + Ey * cos(a) + dy
  *
  * To transform the verification fingerprint into the coordinate
  * system of the enrolled fingerprint one first has to inverse
  * translate the verification fingerprint and then finally inverse
  * rotate it:
  *
  *          -1   -1
  *     E = R  * T  * V
  *
  *     Ex = (Vx - dx) * cos(-a) - (Vy - dy) * sin(-a)
  *     Ey = (Vx - dx) * sin(-a) + (Vy - dy) * cos(-a)
  *
  *     <=>
  *
  *     Ex = Vx * cos(-a) - Vy * sin(-a) - dx'
  *     Ey = Vx * sin(-a) + Vy * cos(-a) - dy'
  * 
  *     where,
  *
  *     dx' = dx * cos(-a) - dy * sin(-a)
  *     dy' = dx * sin(-a) + dy * cos(-a)
  */
typedef struct pb_alignment_st pb_alignment_t;

/** Allocates an alignment object and sets the reference counter to 1. The 
  * alignment is seen from the enrolled fingerprint to the verification 
  * fingerprint and is hence described by the affine transformation
  *   
  *     V = T*R*E
  *
  * For more information on this see the description of pb_alignment_t.
  *
  * @param dx is the translation in pixels along the x-axis, in 500dpi. 
  * @param dy is the translation in pixels along the y-axis, in 500dpi.
  * @param rotation is the rotation in binary radians [0,255], 
  *     counter-clockwise, around (0,0). 
  *
  * @return the allocated alignment object if successful, else 0.
  */
pb_alignment_t*
pb_alignment_create (int16_t dx, 
                     int16_t dy, 
                     uint8_t rotation);

/** Specifies a dpi at which alignment coordinates was specified.
  * 
  * Will internally rescale to 500 dpi.
  * 
  */
pb_alignment_t*
pb_alignment_create_ex (int16_t dx, 
                        int16_t dy, 
                        uint8_t rotation,
                        int16_t dpi);

/** Retains the object, which means that the reference counter for the object
  * will increase by 1. The caller must make sure that _delete is being called
  * when the object is not needed anymore. Retaining a null pointer has
  * no effect. */
pb_alignment_t*
pb_alignment_retain (pb_alignment_t* alignment);

/** Decreases the reference counter for the object by 1. If the reference 
  * counter reaches 0 then the object will also be deleted. Deleting a null
  * pointer has no effect. */
void
pb_alignment_delete (pb_alignment_t* alignment);

/** Creates a copy of the alignment object. */
pb_alignment_t* 
pb_alignment_copy (const pb_alignment_t* alignment);

/** Returns the translation in pixels (x-wise) of the alignment, in 500dpi. */
int16_t
pb_alignment_get_dx (const pb_alignment_t* alignment);

/** Returns the translation  in pixels (y-wise) of the alignment, in 500dpi. */
int16_t
pb_alignment_get_dy (const pb_alignment_t* alignment);

/** Returns the transformed translation in pixels (x-wise) of the alignment, in 
  * 500dpi. The transformed translation dx' = dx * cos(-a) - dy * sin(-a). */
int16_t
pb_alignment_get_dx_prim (const pb_alignment_t* alignment);

/** Returns the transformed translation in pixels (y-wise) of the alignment, in 
  * 500dpi. The transformed translation dy' = dx * sin(-a) + dy * cos(-a). */
int16_t
pb_alignment_get_dy_prim (const pb_alignment_t* alignment);

/** Returns the rotation in binary radians [0,255], counter-clockwise, 
  * of the alignment */
uint8_t
pb_alignment_get_rotation (const pb_alignment_t* alignment);

/** Returns the translation and rotation of the alignment */
void
pb_alignment_get (const pb_alignment_t* alignment, 
                  int16_t* dx, 
                  int16_t* dy, 
                  uint8_t* rotation);

/** Transforms the point (x,y) to a new point (x',y') using the affine 
  * transformation defined by the alignment.
  * 
  *  x' = x * cos (r) - y * sin (r) + dx
  *  y' = x * sin (r) + y * cos (r) + dy
  *
  *  , where r = the rotation of the alignment and dx/dy is the translation of 
  *    the alignment.
  */
void
pb_alignment_transform_point (const pb_alignment_t* alignment,
                              int16_t x, int16_t y,
                              int16_t* x_prim, int16_t* y_prim);

/** Transforms the point (x,y) to a new point (x',y') using the affine 
  * transformation defined by the inverse alignment.
  * 
  *  x' = x * cos (-r) - y * sin (-r) + dx'
  *  y' = x * sin (-r) + y * cos (-r) + dy'
  *
  *  , where r = the rotation of the alignment and 
  *    dx' = dx * cos(-r) - dy * sin(-r)
  *    dy' = dx * sin(-r) + dy * cos(-r)
  */
void
pb_alignment_transform_point_inverse (const pb_alignment_t* alignment,
                                      int16_t x, int16_t y,
                                      int16_t* x_prim, int16_t* y_prim);

/** Returns the extreme values for a rectangle positioned in (0, 0) and with a 
  * given size, transformed according to the alignment. */
void
pb_alignment_get_rectangle_extremes (const pb_alignment_t* alignment,
                                     int16_t rectangle_width, int16_t rectangle_height,
                                     int16_t* min_x, int16_t* max_x,
                                     int16_t* min_y, int16_t* max_y);

/** Compares two alignments. If they are within the limits then
  * the function returns 1, otherwise 0. */
int 
pb_alignment_within_limits (const pb_alignment_t* alignment1,
                            const pb_alignment_t* alignment2,
                            uint16_t translation_limit,
                            uint8_t rotation_limit);


/** Checks an alignment to see if it is with the specified limits. If they are 
  * then the function returns 1, otherwise 0. */
int 
pb_alignment_within_limits_single (const pb_alignment_t* alignment,
                                   uint16_t translation_limit,
                                   uint8_t rotation_limit);

/** Combines two alignments into one. Both input and output alignments
  * express alignment as seen from the enrollment to the verification
  * template. Alignment1 is applied first followed by alignment2.  The
  * combined alignment can be described by affine transformations based
  * on two individual transforms,
  *
  *     V1 = A1*E1
  *     V2 = A2*E2
  *
  * Where E is coordinates of the enrolled template and V is
  * coordinates of the verification template. Now rename E1=E,
  * V=V2 and combine using E2=V1,
  *
  *     V = A2*A1*E
  *     A = A2*A1
  *
  * Where E is the coordinates of the enrolled template and V is the
  * coordinates of the verification template. The new alignment A
  * describes the combined alignment over two steps. Note that the
  * affine operation A1 is applied to E before A2.
  *
  * @param[in] alignment1 is the first applied alignment, between
  *            E1 and V1 described above.
  * @param[in] alignment2 is the second applied alignment, between
  *            E2 and V2 described above.
  *
  * @return the combined alignment A describing the alignment between
  *         E and V described above.
  */
pb_alignment_t*
pb_alignment_combine (const pb_alignment_t* alignment1,
                      const pb_alignment_t* alignment2);

/** Creates the inverse of an alignment, i.e. alignment seen the other way
  * around. Normally alignment expresses alignment as seen from the enrollment
  * to the verification template, this function generates the opposite
  * alignment, as seen from the verification to the enrolled template.
  *
  * If alignment A (input to this function) describes how to transform the
  * enrollment template into the verification template's coordinate system
  *
  *     V = A * E
  *
  *       -1
  * then A   (the output of this function) describes the inverse transformation
  *
  *          -1
  *     E = A  * V
  *
  * i.e. how to transform the verification template into the enrollment template
  * coordinate system.
  *
  * @param[in] alignment is the original alignment A described above.
  *                                      -1
  * @return the inverse alignment of A, A   described above.
  */
pb_alignment_t*
pb_alignment_inverse(const pb_alignment_t* alignment);

/** Assuming two templates V1 and V2 with alignments A1 and A2 both aligned
  * against the same template E:
  *     V1 = A1 * E
  *     V2 = A2 * E
  *
  * This function then computes the alignment A so that V1 = A * V2:
  *
  *           -1
  *     E = A1  * V1
  *           -1                =>
  *     E = A2  * V2
  *
  *       -1         -1
  *     A1  * V1 = A2  * V2     =>
  *
  *                 -1
  *     V1 = A1 * A2  * V2      =>
  *
  *                -1
  *     A = A1 * A2
  */
pb_alignment_t*
pb_alignment_difference (const pb_alignment_t* alignment1,
                         const pb_alignment_t* alignment2);

#ifdef __cplusplus
}
#endif

#endif /* PB_ALIGNMENT_H */
