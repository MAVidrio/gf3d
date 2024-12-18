#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "gfc_audio.h"
#include "simple_logger.h"

typedef struct {
	int health;
	Uint8 super_mode;

	GFC_Sound footsteps;
	GFC_Sound attack1;
	GFC_Sound attack2;
	GFC_Sound attack3;
}playerData;

/**
* @brief spawn a player
* @return NULL on error, or a pointer of the player otherwise
*/
Entity *player_new(GFC_TextLine name, Model* model, GFC_Vector3D spawnPosition, Skeleton3D* armature);

Uint8 player_Trigger(Entity* self);
#endif