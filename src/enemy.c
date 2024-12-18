#include "simple_logger.h"

#include "gfc_matrix.h"
#include "gfc_input.h"

#include "enemy.h"


Entity* spawn_enemy(GFC_TextLine name, Model* model, GFC_Vector3D spawnPosition, Uint8 type) {
	Entity* enemy = entity_new();
	if (!enemy) {
		return NULL;
	}
	enemyData* eData;
	eData = gfc_allocate_array(sizeof(enemyData),1);
	if (!eData) return NULL;

	switch (type)
	{
	case 0:
		eData->health = 1;
		eData->detect_range = 10;
		eData->enemyType = 0;
	case 1:
		eData->health = 1;
		eData->detect_range = 10;
		eData->enemyType = 1;
	case 3:
		eData->health = 3;
		eData->detect_range = 15;
		eData->enemyType = 3;
	case 4:
		eData->health = 5;
		eData->detect_range = 20;
		eData->enemyType = 4;
	default:
		return NULL;
		break;
	}

}

// Make draw, think and update funct for enemey