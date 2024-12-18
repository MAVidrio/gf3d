#include "simple_logger.h"

#include "gfc_matrix.h"

#include "animation.h"

typedef struct
{
    Animation* skeleton_list;
    Uint32		skeleton_max;
}AnimationManager;

static AnimationManager _animation_manager = { 0 };

/*void applyAnimation(Animation* animation, Skeleton* skeleton, float time) {
    if (!animation || !skeleton) return;

    // Apply the animation to each joint
    for (int i = 0; i < animation->channels.count; i++) {

        // TODO: Have this be able to get animation channels from the gltf file by animation channels
        AnimationChannel* channel = animation->channels[i];

        // TODO: Add a gfc_skeleton_set_translation() to skeleton.c
        // TODO: Change this to be like how gltf finds the data.
        if (strcmp(channel->targetPath, "translation") == 0) {
            skeleton_set_translation(skeleton, channel->targetNode, gfc_animation_interpolate_vec3(channel, time));
        }
        else if (strcmp(channel->targetPath, "rotation") == 0) {
            skeleton_set_rotation(skeleton, channel->targetNode, gfc_animation_interpolate_quat(channel, time));
        }
        else if (strcmp(channel->targetPath, "scale") == 0) {
            skeleton_set_scale(skeleton, channel->targetNode, gfc_animation_interpolate_vec3(channel, time));
        }
    }

}
*/