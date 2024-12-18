#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"
#include "gfc_audio.h"
#include "simple_logger.h"
#include "gfc_types.h"


typedef struct {
	int health;

	float detect_range;

	Uint8 enemyType;
}enemyData;

//Entity* spawn_enemy(GFC_TextLine name, Model* model, GFC_Vector3D spawnPosition, Skeleton3D* armature);

Entity* spawn_enemy(GFC_TextLine name, Model* model, GFC_Vector3D spawnPosition, Uint8 type);

#endif