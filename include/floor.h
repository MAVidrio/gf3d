#ifndef __FLOOR_H__
#define __FLOOR_H__

#include "entity.h"

Entity* floor_new(GFC_TextLine name, Model* model, GFC_Vector3D spawnPosition, GFC_Vector3D size);

Entity* floor_new_gltf(const char* filename);

#endif