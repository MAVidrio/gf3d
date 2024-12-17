#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gf3d_model.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "gf3d_camera.h"
#include "gf3d_obj_load.h"
#include "skeleton.h"

/**
* @purpose Make and manage entity
*/

typedef struct Entity_S
{
	Uint8				_inuse;		/**(private) Flag for keeping track of memory useage*/
	GFC_TextLine		tag;		/**What team am I*/
	GFC_TextLine		name;		/**Name of the entity*/
	GFC_Vector3D		position;	/**Where I am in space*/
	GFC_Vector3D		rotation;	/**How I should rotate*/
	GFC_Vector3D		scale;		/**Stretching*/
	GFC_Vector3D		velocity;	/**How much I should move in space*/
	GFC_Vector3D		direction;	/**Where am I faceing in x and y*/
	Model* model;					/**My graphics*/
	int*				cameraMode;	/**What mode is the camera in?*/
	Camera*				camera;		/**My camera object*/
	float*				radius;		/**Offset for my camera*/
	GFC_Primitive		collisionX;	/**What is the shape of my body collision?*/
	Skeleton3D*			armature;	/**My skeleton for aniimations*/

	// behavior
	void (*think)(struct Entity_S* self);		/**Function to call to make decisions*/
	void (*update)(struct Entity_S* self);		/**Function to call to execute think's decisions*/
	int  (*draw)(struct Entity_S* self);		/**Function to call to draw it's model*/
	void (*free)(struct Entity_S* self);		/**clean up any custom data*/
	void (*touch)(struct Entity_S* self, 
		struct Entity_S* other, 
		GFC_Vector3D collision);			/**Function to call when checking for collisions and actions*/
	void* data;									/**For ad hoc addition data for the entity*/
}Entity;

/**
*  @brief cleans up all entities
*/
void entity_system_close();

/**
*  @brief initialize the entity manager system and queues up cleanup on exit
*  @param max_ents the maximum number of entites that can exist at the same time.
*/
void entity_system_init(Uint32 max_ents);

/**
*  @brief clean up all active entites
*  @param ignore do not clean up this entity
*/
void entity_clear_all(Entity* ignore);

/**
* @brief get a blank entity to be used
* @return NULL on no more room or error, a blank entity otherwise
**/
Entity* entity_new();

/*
* @brief clean up an entity and free its spot for future use.
* @param self the entity to free
*/
void entity_free(Entity* self);

/**
* @brief run the think functions for all active entities
*/
void entity_system_think();

/**
* @brief run the think functions for all active entities
*/
void entity_system_update();

/**
* @brief draw all active entities
*/
void entity_system_draw();

/*
* @brief Use to draw anything
*/
void entity_draw(Entity *self);

/*
* @brief Use to set camera's current mode
* @param cameraMode - Camera's current mode
* @param camera - The camera object
*/
void entity_set_camera(Entity* self, int cameraMode, Camera* camera);

/*
* @brief Use to update the camera for the player
* @param camera - Camera object
*/
void entity_update_camera(Entity* self, Camera* camera);

/*
* @brief Use to get camera mode
* @return Camera's current mode
*/
void entity_get_camera(Entity* self);

/*
* @brief Use to set the radius around the entity (Used for camera offset)
* @param radius
*/
void entity_set_radius(Entity* self, float *radius);

/*
* @brief Used to see if these two entities collided with each other or not
* @param self
* @param other - The other entity
* @param collision - Point of collision
* @return - 1 if collided, else 0 if not collided.
*/
void ent_collider(Entity* self, Entity* other, GFC_Vector3D collision);

/*
* @brief if there is a collision in the game, run collision physics of those entities.
*/
void entity_system_collision();

/*
* @brief if an entity has collision, render the collision
* @param toggle - 0 is no visibility, 1 is visible.
*/
void entity_system_collision_visible(int toggle);

/*
* @brief Finds and puts all entites that are objects into a list
* @return - A list of entites that are objects
*/
//GFC_List* get_Object_Entities(Entity* self);

#endif