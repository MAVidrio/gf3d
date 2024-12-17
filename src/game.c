#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"
#include "gf2d_menu.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_draw.h"
#include "gf3d_obj_load.h"
#include "gf3d_gltf_parse.h"
#include "skeleton.h"

#include "entity.h"
#include "player.h"
#include "ent_obj.h"
#include "floor.h"
#include "gf3d_particle.h"

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;
float deltaTime = 0.0f;

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}

void draw_origin()
{
    // x axes
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(-100,0,0),gfc_vector3d(100,0,0)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(1,0,0,1));
    // y axes
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(0,-100,0),gfc_vector3d(0,100,0)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(0,1,0,1));
    // z axes
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(0,0,-100),gfc_vector3d(0,0,100)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(0,0,1,1));
}


int main(int argc,char *argv[])
{
    //local variables
    ObjData *square;
    Model *sky,*dino, *cylinder;
    GLTF  *playerData;
    GFC_Matrix4 skyMat,dinoMat;
    Mix_Music *music;
    int menu = 1;
    int mainMenu = 0;
    GFC_Vector2D prevMousePoint;            // Used for camera movement and pausing it when menu is open
    int *menuType = 0;                      // Used for determining what menu to open
    Skeleton3D* playerArma;                 // Player armature
    //initializtion    
    parse_arguments(argc,argv);
    init_logger("gf3d.log",0);
    slog("gf3d begin");
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
    gfc_audio_init(16,8,0,1,1,0);
    entity_system_init(1024);                 // Entity limit 1024
    particle_manager_init(10000);
    gf3d_armature_system_init(1024);          // Armature limit 1024


    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg");
    gf3d_materials_init();
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);
    gf3d_draw_init();//3D
    gf2d_draw_manager_init(1000);//2D

    //game init
    srand(SDL_GetTicks());
    slog_sync();

    //game setup
    gf2d_mouse_load("actors/mouse.actor");
    //dog = gf3d_model_load_full();
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix4_identity(skyMat);
    //dino = gf3d_model_load("models/dino.model");

    /* Use obj to get model.
    * Use gtfl to move the model.
    */
    playerData = gf3d_gltf_load("models/enemies/Bat/Bat.gltf");
    playerArma = gf3d_armature_parse(playerData);
    //dino = gf3d_model_load("models/Bat.model");
    dino = gf3d_gltf_parse_model("models/enemies/Bat/Bat.gltf");
    gfc_matrix4_identity(dinoMat);
    cylinder = gf3d_model_load("models/primitives/icylinder.model");
    gfc_matrix4_identity(cylinder);
    music = gfc_sound_load_music("music/Persona 3 Reload - It's Going Down Now (Extended Version).mp3");
    square = gf3d_obj_load_from_file("models/primitives/MyCube/TestCube.obj");

    // Player
    GFC_Vector3D playerSpawn = gfc_vector3d(0, 0, 0);
    Entity* player = player_new("player", dino, playerSpawn, playerArma);
    
    // Floor
    GFC_Vector3D spawnFloor = gfc_vector3d(-100, -200, -20);
    GFC_Vector3D sizeFloor = gfc_vector3d(1000, 1000, 10);
    GFC_Vector3D spawnFloor2 = gfc_vector3d(0, 20, 10);
    GFC_Vector3D sizeFloor2 = gfc_vector3d(10, 10, 10);
    Entity* floor = floor_new("cylinder", cylinder, spawnFloor, sizeFloor);
    Entity* floor2 = floor_new("cylinder", cylinder, spawnFloor2, sizeFloor2);
    //GFC_Primitive shape = gfc_new_primitive(3, spawnFloor.x, spawnFloor.y, spawnFloor.z, 10, 10, 1, 0.0f, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0));
    //Entity* floor = floor_new("floor");

    // Object
    GFC_Vector3D spawn = gfc_vector3d(0, 0, 0);
   
    Entity* cylind = obj_new("cylinder", square, spawn);

    float offset = 45.0f;
    entity_set_radius(player, &offset);
    
    //camera
    gf3d_camera_set_scale(gfc_vector3d(1,1,1));
    SDL_CaptureMouse(SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    // Camera Setup
    GFC_Vector3D offsetPos = gfc_vector3d(0.0f, offset, 20.0f);
    GFC_Vector3D offsetLook = gfc_vector3d(0.0f, 0.0f, 10.0f);
    gf3d_camera_set_offset_position(player->position, offsetPos);
    gf3d_camera_set_offset_rotation(player->position, offsetLook);
    
    //GFC_Box testBox = gfc_box(0, -10, 0, 5, 5, 5);

    //gf3d_camera_look_at(gfc_vector3d(player->position.x, player->position.y, player->position.z + 10), NULL);
    //gf3d_camera_set_move_step(1.0);
    //gf3d_camera_set_rotate_step(0.1);
    
    //gf3d_camera_enable_free_look(1);
    gf3d_camera_enable_player_look(1);
    //gf3d_camera_set_auto_pan(1);
    entity_set_camera(player, gf3d_camera_get_mode(), gf3d_get_camera());
    //windows

    // Music
    gfc_music_play(music, -1);      // Play Music

    // main game loop    
    while(!_done)
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();

        //camera updates
        GFC_Vector2D mousePos = gf2d_mouse_get_movement();
        if (menu == 1) { 
            prevMousePoint = gf2d_mouse_get_movement(); 

            entity_system_collision();
            entity_system_think();
            entity_system_update();
        }
        else { prevMousePoint = gfc_vector2d(0, 0); }

        gf3d_camera_move_mouse(prevMousePoint, player->position, offset);
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

            //3D draws
        
                gf3d_model_draw_sky(sky,skyMat,GFC_COLOR_WHITE);

                //gf3d_model_draw();
                
                particle_spray_follow_point(
                    10,
                    30,
                    GFC_COLOR_BLUE,
                    GFC_COLOR_GREEN,
                    gfc_vector3d(player->position.x + gfc_crandom(), player->position.y + gfc_crandom(), player->position.z + gfc_crandom()),
                    &player->position,
                    0.2,
                    0.1,
                    0.5,
                    gfc_vector3d(0,0,0));
                
                entity_system_draw();
                particle_manager_draw();
                gf3d_draw_plane_3d(gfc_plane3d(0,0,0,0),gfc_vector3d(0,0,0), gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1), GFC_COLOR_WHITE);
                entity_system_collision_visible(1);
                
                
                
                //gf3d_model_draw();

                /*gf3d_model_draw(
                    dino,
                    dinoMat,
                    GFC_COLOR_WHITE,
                    0);*/
                draw_origin();
            //2D draws
                //gf2d_mouse_draw();
                gf2d_font_draw_line_tag("ALT+F4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));

                if (menu == -1) {
                    int menuchange = gf2d_draw_menu(menuType, gf2d_mouse_get_position(), gf2d_mouse_button_pressed(0));

                    // Return to menu
                    if (menuchange == 0) { menuType = 0; }
                    // Options menu
                    if (menuchange == 1) { menuType = 1; }
                    // Bestiary 
                    if (menuchange == 2) { menuType = 2; }
                    // Exit menu from "resume"
                    if (menuchange == -1) { menuType = 0; menu = 1; }
                    // Exit condition from "exit"
                    else if (menuchange == -2) { _done = 1; }

                    gf2d_mouse_draw();
                }

        gf3d_vgraphics_render_end();

        entity_set_camera(player,gf3d_camera_get_mode(), gf3d_get_camera());
        //gf3d_camera_third_person(mousePos, player->position);
        //gf3d_camera_update_view();

        // Open menu when pressing ESCAPE
        if (gfc_input_command_pressed("cancel")) menu = -menu;

        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }    
    
    entity_system_close();
    
    gfc_music_free(music);
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());  
    //cleanup
    slog("gf3d program end");
    exit(0);
    slog_sync();
    return 0;
}

void parse_arguments(int argc,char *argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }    
}

void game_frame_delay()
{
    Uint32 diff;
    static Uint32 now;
    static Uint32 then;
    then = now;
    slog_sync();// make sure logs get written when we have time to write it
    now = SDL_GetTicks();
    diff = (now - then);

    // Calculate delta time by converting ticks into seconds
    deltaTime = diff / 1000.0f;

    if (diff < frame_delay)
    {
        SDL_Delay(frame_delay - diff);
    }
    fps = 1000.0/MAX(SDL_GetTicks() - then,0.001);
//     slog("fps: %f",fps);
}
/*eol@eof*/
