#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "gfc_types.h"
#include "gfc_string.h"
#include "gf3d_gltf_parse.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "gf3d_gltf_parse.h"
#include "gfc_list.h"
#include "gfc_matrix.h"
#include "gfc_config.h"
#include "skeleton.h"

/*
* @brief Defines what is being animated and which sampler provides the keyframes.
*/
typedef struct AnimeChannel_S {
    // In gltf -> "node" under "target", under the "channels", which is under "animations"
    int targetNode;                 // Index of the node (joint) being animated

    // In gltf -> "path" under "target", under the "channels", which is under "animations"
    GFC_String targetPath;          // Property being animated: "translation", "rotation", or "scale"

    // In gltf -> "sampler" under the "channels", which is under "animations"
    int samplerIndex;               // Index of the sampler providing the keyframes
}AnimationChannel;

/*
* @brief Defines when and how the animation progresses.
*/
typedef struct AnimeSampler_S {
    // In gltf -> "input" under the "samplers", which is under "animations"
    float input;                 // Keyframe times

    // In gltf -> "output" under the "samplers", which is under "animations"
    GFC_Vector3D translations;          // Translation values (if animating position)
    GFC_Vector3D rotations;             // Rotation values (if animating orientation)
    GFC_Vector3D scales;                // Scale values (if animating size)

    // In gltf -> "interpolation" under the "samplers", which is under "animations"
    GFC_String interpolation;       // Interpolation type: "LINEAR"         - Smoothly Interpolates between keyframe values.
                                    //                      "STEP"          - Holds the value constant until the next keyframe.
                                    //                      "CUBICSPLINE"   - Provides a smooth cubic spline interpolation
}AnimationSampler;

/*
* @brief An animation from gltf is made up of three different things 
* @param name - The name of the animation
* @param channel_list - A list of channels for refrencing the bone(target) to it's movement(sampler).
* @param sampler_list - A list of samplers for giving the infomration need to move the bone.
*/
typedef struct Animation_S {
    GFC_String name;                             // Name of the animation
    AnimationChannel* channel_list;              // List of animation channels
    AnimationSampler* sampler_list;              // List of animation samplers
}Animation;

#endif