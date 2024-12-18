#ifndef __ENT_PARENT_H__
#define __ENT_PARENT_H__

#include "gfc_types.h"
#include "gf3d_model.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "gf3d_camera.h"
#include "gf3d_obj_load.h"
#include "gf3d_gltf_parse.h"
#include "skeleton.h"
#include "animation.h"
#include "gfc_config.h"

// Parent of all entites
typedef struct EntityData_S
{
	Uint8				_inuse;		/**(private) Flag for keeping track of memory useage*/
	GLTF				*file;		/**Additional data of the entity using a gltf file*/
}EntityData;

// The Entity
typedef struct Entity_S:public EntityData {

	GFC_String name = sj_object_get_gfc_string(file->json, "name");
	GFC_String tag = sj_object_get_gfc_string(file->json, "tag");

	// Transform
	GFC_Vector3D position;
	GFC_Vector3D rotation;
	GFC_Vector3D scale;

	GFC_Vector3D velocity;
	GFC_Vector3D acceleration;
	GFC_Vector3D direction;
	
}EntityThree;

typedef struct Player_S : public EntityThree {
	Model* model;
	Skeleton* skeleton;
	Animation* animation;
};

/**
*  @brief cleans up all entities
*/
//void entity_manager_system_close();

/**
*  @brief initialize the entity manager system and queues up cleanup on exit
*  @param max_ents the maximum number of entites that can exist at the same time.
*/
//void entity_manager_system_init(Uint32 max_ents);

/**
*  @brief clean up all active entites
*  @param ignore do not clean up this entity
*/
//void entity_clear_all(Entity* ignore);

/**
* @brief get a blank entity to be used
* @return NULL on no more room or error, a blank entity otherwise
**/
//EntityData* entity_new();

/*
* @brief clean up an entity and free its spot for future use.
* @param self the entity to free
*/
//void entity_free(Entity* self);

// Enemy needs
// 0. _inUse	- Use to deturnmine if the entity is there or not
// 1. Name		- A way to identify itself
// 2. Tag		- A way to tell what it is (Flying, Eletric, Boss)
// 3. Team		- Team affiliation (Friend or Foe)
// 4. GLTF File	- The gltf file it needs for 5-8
// 5. Model		- A way to display it's body
// 6. Skeleton	- A skeleton to be able to be animated
// 7. Collision	- Determines how the entity interacts with other entities

// Enemy Functions
// 1. Think		- Use for behavior and actions
// 2. Update	- To do what happens within it.

#endif