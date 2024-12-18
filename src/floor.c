#include "simple_logger.h"

#include "gfc_matrix.h"
#include "gf3d_mesh.h"
#include "gf3d_gltf_parse.h"

#include "ent_obj.h"

void floor_think(Entity* self);
void floor_update(Entity* self);
int	 floor_draw(Entity* self);
void floor_free(Entity* self);
void floor_collider(Entity* self, Entity* other);

Entity* floor_new(GFC_TextLine name, Model* model, GFC_Vector3D spawnPosition, GFC_Vector3D size)
{
	Entity* self;

	self = entity_new();
	if (!self)
	{
		slog("Failed to spawn object entity.");
		return NULL;
	}

	gfc_line_sprintf(self->tag, "Floor");
	gfc_line_sprintf(self->name, "floor");

	self->position = spawnPosition;
	self->rotation = gfc_vector3d(0, 0, 0);
	self->scale = gfc_vector3d(2, 2, 2);
	self->model = model;
	self->velocity = gfc_vector3d(0, 0, 0);
	self->direction = gfc_vector3d(0, 0, 0);
	self->radius = 0;

	float xScale = size.x;
	float yScale = size.y;
	float zScale = size.z;

	//self->collisionX = gfc_primitive_offset(gfc_new_primitive(3, self->position.x, self->position.y, self->position.z, xScale, yScale, zScale, 0.0f, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0)), gfc_vector3d(1, -1, -1));
	self->collisionX = gfc_new_primitive(3, self->position.x - (xScale / 2.0f), self->position.y - (yScale / 2.0f), self->position.z - (zScale / 2), xScale, yScale, zScale, 0.0f, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0));

	self->think = floor_think;
	self->update = floor_update;
	self->draw = floor_draw;
	self->free = floor_free;
	self->touch = floor_collider;

	slog("%s succefully spawned.", self->name);
	//slog("%f/%f/%f", self->collisionX.s.b.xC, self->collisionX.s.b.yC, self->collisionX.s.b.zC);
	return self;
}

Entity* floor_new_gltf(const char* filename) {
	int i, c, s;
	GLTF* gltf;
	SJson* meshes, * meshData, *extraData, *extra;
	GFC_TextLine name, tag;
	Mesh* mesh;
	Model* model;
	if (!filename)return NULL;

	Entity* self;

	self = entity_new();
	if (!self)
	{
		slog("Failed to spawn floor entity.");
		return NULL;
	}

	//from here start rewriting everything around using the GLTF wrapper
	gltf = gf3d_gltf_load(filename);
	if (!gltf)
	{
		slog("GLTF file '%s' Failed to load", filename);
		return NULL;
	}
	model = gf3d_gltf_parse_model(gltf);
	if (!model)return NULL;

	// Get extra data
	//extra = sj_object_get_string(gltf->json, "nodes");
	//s = sj_array_get_count(extra);

	//extraData = sj_array_get_nth(extra, 0);
	//if (!extraData) {
	//	slog("NULL extra data"); return NULL;
	//}


	gf3d_gltf_free(gltf);

	gfc_line_sprintf(self->tag, "Floor");
	gfc_line_sprintf(self->name, "floor");

	self->position = gfc_vector3d(0,0,0);
	self->rotation = gfc_vector3d(0, 0, 0);
	self->scale = gfc_vector3d(2, 2, 2);
	self->model = model;

	//float xScale = size.x;
	//float yScale = size.y;
	//float zScale = size.z;

	//self->collisionX = gfc_primitive_offset(gfc_new_primitive(3, self->position.x, self->position.y, self->position.z, xScale, yScale, zScale, 0.0f, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0)), gfc_vector3d(1, -1, -1));
	//self->collisionX = gfc_new_primitive(3, self->position.x - (xScale / 2.0f), self->position.y - (yScale / 2.0f), self->position.z - (zScale / 2), xScale, yScale, zScale, 0.0f, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0));

	self->think = floor_think;
	self->update = floor_update;
	self->draw = floor_draw;
	self->free = floor_free;
	self->touch = floor_collider;

	slog("%s succefully spawned.", self->name);
	//slog("%f/%f/%f", self->collisionX.s.b.xC, self->collisionX.s.b.yC, self->collisionX.s.b.zC);
	return self;
}

void floor_collider(Entity* self, Entity* other){
	//slog("Object collision with %s", other->name);
}

void floor_think(Entity* self)
{
	if (!self)return;

	GFC_Vector3D dir = self->direction;

	gfc_vector3d_normalize(&dir);
	gfc_vector3d_scale(self->velocity, dir, 1);
}

void floor_update(Entity* self)
{
	if (!self)return;

	gfc_vector3d_add(self->position, self->position, self->velocity);
	//gfc_vector3d_add(self->collision, self->position, self->velocity);
	//self->collision = gfc_box(self->position.x, self->position.y, self->position.z, 3, 3, 3);
}

int	 floor_draw(Entity* self){
	if (!self)return;
	//slog("Object is drawing");
	return;
}

void floor_free(Entity* self)
{
	if (!self)return;
}

