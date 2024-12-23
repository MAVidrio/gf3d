#include <string.h>

#include "simple_logger.h"

#include "gfc_matrix.h"

#include "gf3d_camera.h"

#include "SDL_mouse.h"

static Camera gf3d_camera = {0};

void gf3d_camera_set_move_step(float step)
{
    gf3d_camera.moveStep = step;
}

void gf3d_camera_set_rotate_step(float step)
{
    gf3d_camera.rotateStep = step;
}


void gf3d_camera_get_view_mat4(GFC_Matrix4 *view)
{
    if (!view)return;
    memcpy(view,gf3d_camera.cameraMat,sizeof(GFC_Matrix4));
}

void gf3d_camera_set_view_mat4(GFC_Matrix4 *view)
{
    if (!view)return;
    memcpy(gf3d_camera.cameraMat,view,sizeof(GFC_Matrix4));
}

void gf3d_camera_look_at(GFC_Vector3D target,const GFC_Vector3D *position)
{
    GFC_Vector3D angles,pos;
    GFC_Vector3D delta;
    if (position)
    {
        gfc_vector3d_copy(pos,(*position));
        gf3d_camera_set_position(pos);
    }
    else
    {
        pos = gf3d_camera_get_position();
    }
    gfc_vector3d_sub(delta,target,pos);
    gfc_vector3d_angles (delta, &angles);
    angles.z -= GFC_HALF_PI;
    angles.x -= GFC_PI;
    gf3d_camera_set_rotation(angles);
}

void gf3d_camera_update_view()
{
    /**
     * Adapted from tutorial:
     * https://www.3dgep.com/understanding-the-view-matrix/
     */
    
    GFC_Vector3D xaxis,yaxis,zaxis,position;
    float cosPitch = cos(gf3d_camera.rotation.x);
    float sinPitch = sin(gf3d_camera.rotation.x);
    float cosYaw = cos(gf3d_camera.rotation.z);
    float sinYaw = sin(gf3d_camera.rotation.z); 

    position.x = gf3d_camera.position.x;
    position.y = -gf3d_camera.position.z;        //inverting for Z-up
    position.z = gf3d_camera.position.y;
    gfc_matrix4_identity(gf3d_camera.cameraMat);

    gfc_vector3d_set(xaxis, cosYaw,                     0,  -sinYaw);
    gfc_vector3d_set(yaxis, sinYaw * sinPitch,   cosPitch,   cosYaw * sinPitch);
    gfc_vector3d_set(zaxis, sinYaw * cosPitch,  -sinPitch,   cosPitch * cosYaw);
    
    gf3d_camera.cameraMat[0][0] = xaxis.x;
    gf3d_camera.cameraMat[0][1] = yaxis.x;
    gf3d_camera.cameraMat[0][2] = zaxis.x;

    gf3d_camera.cameraMat[1][0] = xaxis.z;
    gf3d_camera.cameraMat[1][1] = yaxis.z;
    gf3d_camera.cameraMat[1][2] = zaxis.z;

    gf3d_camera.cameraMat[2][0] = -xaxis.y;
    gf3d_camera.cameraMat[2][1] = -yaxis.y;
    gf3d_camera.cameraMat[2][2] = -zaxis.y;

    gf3d_camera.cameraMat[3][0] = gfc_vector3d_dot_product(xaxis, position);
    gf3d_camera.cameraMat[3][1] = gfc_vector3d_dot_product(yaxis, position);
    gf3d_camera.cameraMat[3][2] = gfc_vector3d_dot_product(zaxis, position);
}

GFC_Vector3D gf3d_camera_get_position()
{
    GFC_Vector3D position;
    gfc_vector3d_negate(position,gf3d_camera.position);
    return position;
}

void gf3d_camera_set_position(GFC_Vector3D position)
{
    gf3d_camera.position.x = -position.x;
    gf3d_camera.position.y = -position.y;
    gf3d_camera.position.z = -position.z;
}

void gf3d_camera_follow_player(GFC_Vector3D dir)
{
    gfc_vector3d_sub(gf3d_camera.position, gf3d_camera.position, dir);
}

void gf3d_camera_set_offset_position(GFC_Vector3D target, GFC_Vector3D offset) 
{
    gfc_vector3d_sub(gf3d_camera.position, target, offset);
}

void gf3d_camera_set_offset_rotation(GFC_Vector3D target, GFC_Vector3D offset) 
{
    GFC_Vector3D angles, cameraPos, targetPos;
    GFC_Vector3D delta;
    gfc_vector3d_negate(cameraPos, gf3d_camera.position);
    gfc_vector3d_add(targetPos, target, offset);

    gfc_vector3d_sub(delta, targetPos, cameraPos);
    gfc_vector3d_angles(delta, &angles);
    angles.z -= GFC_HALF_PI;
    angles.x -= GFC_PI;
    gfc_angle_clamp_radians(&angles.x);
    gf3d_camera.rotation.x = -angles.x;
    gf3d_camera.rotation.y = -angles.y;
    gf3d_camera.rotation.z = -angles.z;;
}

void gf3d_camera_move_mouse(GFC_Vector2D mousePos, GFC_Vector3D target, float radius) 
{
    // 1. Find angle
    // 
    // angle = sensitivity * mousePos.x; 
    // cameraRotaion += (angle * mousePos.x);
    // position.x += target.x + (radius * cosine(angle.xy);
    // position.y += target.y + (radius * sin(angle.xy);

    //float angleX = GFC_HALF_PI * 0.005 * mousePos.x;
    //float angleY = GFC_HALF_PI * 0.005 * mousePos.y;
    //gf3d_camera.rotation.z += angleX;
    //gf3d_camera.rotation.x += angleY;

    gf3d_camera.angle -= 0.005 * mousePos.x;
    gf3d_camera.height += 0.005 * mousePos.y;
    
    gf3d_camera.position.x = -target.x + radius * cos(gf3d_camera.angle);
    gf3d_camera.position.y = -target.y + radius * sin(gf3d_camera.angle);

    gfc_angle_clamp_radians(&gf3d_camera.position.z);
    gf3d_camera.position.z = -target.z + radius * cos(gf3d_camera.height);
    //slog("Z: %f", cos(gf3d_camera.height));

    GFC_Vector3D angles, pos;
    GFC_Vector3D delta;
    
    pos = gf3d_camera_get_position();
    
    gfc_vector3d_sub(delta, gfc_vector3d(target.x, target.y, target.z + 10.0f), pos);
    gfc_vector3d_angles(delta, &angles);
    angles.z -= GFC_HALF_PI;
    angles.x -= GFC_PI;
    gf3d_camera_set_rotation(angles);
    
}


void gf3d_camera_move(GFC_Vector3D translation)
{
    gfc_vector3d_sub(gf3d_camera.position,gf3d_camera.position,translation);
}

void gf3d_camera_walk_forward(float magnitude)
{
    GFC_Vector2D w;
    GFC_Vector3D forward = {0};
    w = gfc_vector2d_from_angle(-gf3d_camera.rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    gfc_vector3d_set_magnitude(&forward,magnitude);
    gf3d_camera_move(forward);

}

void gf3d_camera_walk_right(float magnitude)
{
    GFC_Vector2D w;
    GFC_Vector3D right = {0};
    w = gfc_vector2d_from_angle(-gf3d_camera.rotation.z - GFC_HALF_PI);
    right.x = w.x;
    right.y = w.y;
    gfc_vector3d_set_magnitude(&right,magnitude);
    gf3d_camera_move(right);

}

void gf3d_camera_move_up(float magnitude)
{
    GFC_Vector3D up = {0,0,magnitude};
    gf3d_camera_move(up);
}

void gf3d_camera_yaw(float magnitude)
{
    gf3d_camera.rotation.z -= magnitude;
}

void gf3d_camera_pitch(float magnitude)
{
    gf3d_camera.rotation.x -= magnitude;
    if (gf3d_camera.rotation.x >= -GFC_HALF_PI)
    {
        gf3d_camera.rotation.x = -GFC_HALF_PI - GFC_EPSILON;
    }
    if (gf3d_camera.rotation.x  <= -GFC_PI_HALFPI)
    {
        gf3d_camera.rotation.x = -GFC_PI_HALFPI + GFC_EPSILON;
    }
}

void gf3d_camera_roll(float magnitude)
{
    gf3d_camera.rotation.y -= magnitude;
}

GFC_Vector3D gf3d_camera_get_direction()
{
    GFC_Vector3D forward = {0};
    gf3d_camera_get_view_vectors(&forward, NULL, NULL);
    return forward;
}

GFC_Vector3D gf3d_camera_get_right()
{
    GFC_Vector3D right = {0};
    gf3d_camera_get_view_vectors(NULL,&right, NULL);
    return right;
}

GFC_Vector3D gf3d_camera_get_up()
{
    GFC_Vector3D up = {0};
    gf3d_camera_get_view_vectors(NULL, NULL,&up);
    return up;
}


void gf3d_camera_fly_forward(float magnitude)
{
    GFC_Vector3D forward;
    gf3d_camera_get_view_vectors(&forward, NULL, NULL);
    gfc_vector3d_set_magnitude(&forward,magnitude);
    gf3d_camera_move(forward);
}

void gf3d_camera_fly_right(float magnitude)
{
    GFC_Vector3D right;
    gf3d_camera_get_view_vectors(NULL, &right, NULL);
    gfc_vector3d_set_magnitude(&right,magnitude);
    gf3d_camera_move(right);
}

void gf3d_camera_fly_up(float magnitude)
{
    GFC_Vector3D up;
    gf3d_camera_get_view_vectors(NULL, NULL, &up);
    gfc_vector3d_set_magnitude(&up,magnitude);
    gf3d_camera_move(up);
}

void gf3d_camera_calc_view_vectors()
{
    gf3d_camera.forward = gfc_vector3d(0,-1,0);
    gf3d_camera.right   = gfc_vector3d(1,0,0);
    gf3d_camera.up      = gfc_vector3d(0,0,-1);
    //first rotate by roll
    gfc_vector3d_rotate_about_y(&gf3d_camera.right, -gf3d_camera.rotation.y);
    gfc_vector3d_rotate_about_y(&gf3d_camera.up, -gf3d_camera.rotation.y);
    //then rotate by pitch
    gfc_vector3d_rotate_about_x(&gf3d_camera.forward, gf3d_camera.rotation.x);
    gfc_vector3d_rotate_about_x(&gf3d_camera.right, gf3d_camera.rotation.x);
    gfc_vector3d_rotate_about_x(&gf3d_camera.up, gf3d_camera.rotation.x);
    //then by yaw
    gfc_vector3d_rotate_about_z(&gf3d_camera.forward, -gf3d_camera.rotation.z);
    gfc_vector3d_rotate_about_z(&gf3d_camera.right, -gf3d_camera.rotation.z);
    gfc_vector3d_rotate_about_z(&gf3d_camera.up, -gf3d_camera.rotation.z);
}

void gf3d_camera_get_view_vectors(GFC_Vector3D *forward, GFC_Vector3D *right, GFC_Vector3D *up)
{
    gf3d_camera_calc_view_vectors();
    if (forward)*forward = gf3d_camera.forward;
    if (right)*right = gf3d_camera.right;
    if (up)*up = gf3d_camera.up;
}

GFC_Vector3D gf3d_camera_get_angles()
{
    return gfc_vector3d(-gf3d_camera.rotation.x,-gf3d_camera.rotation.y,-gf3d_camera.rotation.z);
}

void gf3d_camera_set_rotation(GFC_Vector3D rotation)
{
    gfc_angle_clamp_radians(&rotation.x);
    gf3d_camera.rotation.x = -rotation.x;
    gf3d_camera.rotation.y = -rotation.y;
    gf3d_camera.rotation.z = -rotation.z;
}

void gf3d_camera_set_scale(GFC_Vector3D scale)
{
    if (!scale.x)gf3d_camera.scale.x = 0;
    else gf3d_camera.scale.x = 1/scale.x;
    if (!scale.y)gf3d_camera.scale.y = 0;
    else gf3d_camera.scale.y = 1/scale.y;
    if (!scale.z)gf3d_camera.scale.z = 0;
    else gf3d_camera.scale.z = 1 / scale.z;
}

void gf3d_camera_set_look_target(GFC_Vector3D target)
{
    gfc_vector3d_copy(gf3d_camera.lookTargetPosition, target);
    gf3d_camera_look_at(target, NULL);
}

GFC_Vector3D gf3d_camera_get_look_target()
{
    return gf3d_camera.lookTargetPosition;
}

void gf3d_camera_toggle_free_look()
{
    gf3d_camera_enable_free_look(!gf3d_camera.freeLook);
}

void gf3d_camera_enable_free_look(Uint8 enable)
{
    gf3d_camera.freeLook = enable;
}

void gf3d_camera_toggle_player_look()
{
    gf3d_camera_enable_player_look(!gf3d_camera.playerLook);
}

void gf3d_camera_enable_player_look(Uint8 enable)
{
    gf3d_camera.playerLook = enable;
}

void gf3d_camera_set_auto_pan(Bool enable)
{
    gf3d_camera.autoPan = enable;
}

Bool gf3d_camera_free_look_enabled()
{
    return gf3d_camera.freeLook;
}

int gf3d_camera_get_mode() 
{
    if (gf3d_camera.playerLook == 1) return 0;
    else if (gf3d_camera.freeLook == 1) return 1;
    else if (gf3d_camera.autoPan == 1) return 2;
    else if (gf3d_camera.cameraTargetLock == 1) return 3;
}

void gf3d_camera_third_person(GFC_Vector2D mousePos, GFC_Vector3D playerPos) {
    

    return;
}

void gf3d_camera_controls_update(GFC_Vector2D mousePos, GFC_Vector3D playerPos)
{
    //GFC_Vector3D dir = { 0,0,0 };
    float moveSpeed = gf3d_camera.moveStep;
    GFC_Vector3D position, rotation;
    const Uint8* keys;

    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    // Camera Mode
    if (gf3d_camera.freeLook)
    {
        if (keys[SDL_SCANCODE_R])
        {
            rotation = gf3d_camera_get_angles();
            slog("rotation: %f,%f,%f", rotation.x, rotation.y, rotation.z);
        }
        if (keys[SDL_SCANCODE_P])
        {
            position = gf3d_camera_get_position();
            slog("position: %f,%f,%f", position.x, position.y, position.z);
        }
        // Press W
        if (keys[SDL_SCANCODE_W])
        {
            gf3d_camera_walk_forward(moveSpeed);
        }
        // Press S
        if (keys[SDL_SCANCODE_S])
        {
            gf3d_camera_walk_forward(-moveSpeed);
        }
        // Press D
        if (keys[SDL_SCANCODE_D])
        {
            gf3d_camera_walk_right(moveSpeed);
        }
        // Press A
        if (keys[SDL_SCANCODE_A])
        {
            gf3d_camera_walk_right(-moveSpeed);
        }
        if (keys[SDL_SCANCODE_SPACE])gf3d_camera_move_up(moveSpeed);            // Press Space
        if (keys[SDL_SCANCODE_Z])gf3d_camera_move_up(-moveSpeed);               // Press Z

        if (keys[SDL_SCANCODE_UP])gf3d_camera_pitch(-gf3d_camera.rotateStep);   // Press Arrow Up
        if (keys[SDL_SCANCODE_DOWN])gf3d_camera_pitch(gf3d_camera.rotateStep);  // Press Arrow Down
        if (keys[SDL_SCANCODE_RIGHT])gf3d_camera_yaw(-gf3d_camera.rotateStep);  // Press Arrow Right
        if (keys[SDL_SCANCODE_LEFT])gf3d_camera_yaw(gf3d_camera.rotateStep);    // Press Arrow Left

        return;
    }
    // Cinema Mode
    if (gf3d_camera.autoPan)
    {
        gf3d_camera_set_look_target(gfc_vector3d(playerPos.x, playerPos.y, playerPos.z + 10));
        gf3d_camera_look_at(gf3d_camera.lookTargetPosition, NULL);

        gf3d_camera_walk_right(moveSpeed / 5);
    }
    // Lock On Mode
    if (gf3d_camera.cameraTargetLock)
    {
        gf3d_camera_look_at(gf3d_camera.lookTargetPosition, NULL);
    }
    // Player Mode
    if (gf3d_camera.playerLook) {

        gf3d_camera_set_look_target(gfc_vector3d(playerPos.x, playerPos.y, playerPos.z + 10));

        // Right and Left
        if (mousePos.x > 0) gf3d_camera_walk_right(-moveSpeed * 2);
        else if (mousePos.x < 0) gf3d_camera_walk_right(moveSpeed * 2);

        // Down and Up
        //if (mousePos.y > 0) gf3d_camera_fly_up(moveSpeed * 2);
        //else if (mousePos.y < 0) gf3d_camera_fly_up(-moveSpeed * 2);

        if (keys[SDL_SCANCODE_R])
        {
            rotation = gf3d_camera_get_right();
            slog("rotation: %f,%f,%f", rotation.x, rotation.y, rotation.z);
        }
        if (keys[SDL_SCANCODE_P])
        {
            position = gf3d_camera_get_position();
            slog("position: %f,%f,%f", position.x, position.y, position.z);
        }
        
        return;
    }
}

/*eol@eof*/
