#include "simple_logger.h"

#include "gfc_matrix.h"
#include "gfc_input.h"

#include "player.h"

void player_think(Entity *self);
void player_update(Entity *self);
int player_draw(Entity* self);
void player_free(Entity *self);
void player_collider(Entity* self, Entity* other, GFC_Vector3D* collision);

// Flag for collision
Bool collide = false;
/*
* @brief Type of collision being detected
* @param [0]-Objects
* @param [1]-Enemy Collision
* @param [2]-Enemy Hurtbox
* @param [3]-Enviormental
* @param [4]-Floor
*/
int typeCollision[] = { 0,0,0,0,0 };
/*
* @brief Entities that are touching this entity
* @param [0]-Objects
* @param [1]-Enemy Collision
* @param [2]-Enemy Hurtbox
* @param [3]-Enviormental
* @param [4]-Floor
*/
Entity entCollision[] = { 0,0,0,0,0 };
float gravity = 1.0f;

// Used for checking if the player is on the ground or in the air
enum groundState {
	onGround,
	inAir
};

// Used for universal states. Idle, attacking, moving, ex.
enum playerState {
	idle,
	attack,
	moving,

};



int state = idle;
int ground = onGround;

Entity* player_new(GFC_TextLine name, Model* model, GFC_Vector3D spawnPosition)
{
	Entity *self;

	self = entity_new();
	if (!self)
	{
		slog("Failed to spawn player entity.");
		return NULL;
	}

	gfc_line_sprintf(self->tag, "Player");
	gfc_line_sprintf(self->name, name);
	self->position = spawnPosition;
	self->rotation = gfc_vector3d(0, 0, 0);
	self->scale = gfc_vector3d(1, 1, 1);
	self->model = model;
	self->velocity = gfc_vector3d(0, 0, 0);
	self->direction = gfc_vector3d(0, 0, 0);
	self->cameraMode = 0;
	self->camera;
	self->radius = 0;
	//self->collision = gfc_box(self->position.x, self->position.y, self->position.z, 4, 4, 4);

	float xScale = 6.0f;
	float yScale = 6.0f;
	float zScale = 11.0f;

	self->collisionX = gfc_new_primitive(3, self->position.x-(xScale/2.0f), self->position.y-(yScale/2.0f), self->position.z-(zScale/2)-1, xScale, yScale, zScale, 0.0f, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0));

	self->think = player_think;
	self->update = player_update;
	self->draw = player_draw;
	self->free = player_free;
	self->touch = player_collider;

	slog("Player succefully spawned.");
	return self;
}

void update_Camera_Mode(Entity *self, int camera)
{
	self->cameraMode = camera;
}

int get_Camera_Mode(Entity *self) 
{
	return self->cameraMode;
}

void player_collider(Entity* self, Entity* other, GFC_Vector3D* collision) {
	if (!self)return;

	GFC_Vector3D dir = self->direction;
	if (strcmp(other->tag, "Object") == 0) {
		//slog("Touching object");
		collide = true;
		typeCollision[0] = 1;
		entCollision[0] = *other;
	}	
	if (strcmp(other->tag, "Floor") == 0) {
		//slog("Touching floor");
		collide = true;
		typeCollision[4] = 1;
		slog("Collision: %f/%f/%f", collision->x, collision->y, collision->z);
		//entCollision[4] = *other;
	}
}

void player_to_object(Entity* self, Entity* object) {
	//slog("Touching Object");
}

void player_think(Entity* self) {

	if (!self)return;

	float fallMultiplier = 0.5f;
	float jumpMultiplier = 15.0f;
	const Uint8* keys;
	GFC_Vector3D velocityDir = gfc_vector3d(0,0,0);
	GFC_Vector3D finalY;
	GFC_Vector3D finalX;
	float movementX = 0.0f;
	float movementY = 0.0f;
	GFC_Vector3D camForward;
	GFC_Vector3D camRight;

	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

	// Floor Collision
	if (typeCollision[4] == 1) {
		ground = onGround;
		self->velocity.z = 0;
		typeCollision[4] = 0;
	}
	// Gravity
	else {
		self->velocity.z -= 0.1;
		ground = inAir;
	}

	// Jump
	if (keys[SDL_SCANCODE_SPACE] && ground == onGround) self->velocity.z += 2.0f;

	// Player Control mode
	gf3d_camera_get_view_vectors(&camForward, &camRight, NULL);

	camForward.z = 0;
	camRight.z = 0;

	if (keys[SDL_SCANCODE_W]) { movementY += -1.0f; }	// Press W
	else if (keys[SDL_SCANCODE_S]) { movementY += 1.0f; }	// Press S
	else { movementY = 0.0f; }
	if (keys[SDL_SCANCODE_D]) { movementX += -1.0f; }	// Press D
	else if (keys[SDL_SCANCODE_A]) { movementX += 1.0f; }	// Press A
	else { movementX = 0.0f; }

	gfc_vector3d_scale(finalY, camForward, movementY);
	gfc_vector3d_scale(finalX, camRight, movementX);

	self->velocity.x = -finalY.x + -finalX.x;
	self->velocity.y = -finalY.y + -finalX.y;

	// X limit
	if (self->velocity.x > 2.0f) self->velocity.x -= 0.3f;
	else if (self->velocity.x < -2.0f) self->velocity.x += 0.3f;

	// Y limit
	if (self->velocity.y > 2.0f) self->velocity.y -= 0.3f;
	else if (self->velocity.y < -2.0f) self->velocity.y += 0.3f;

	// Z limit
	if (self->velocity.z < -5.0f) self->velocity.z += 0.3f;

	// DEBUG: check velocity 
	if (gfc_input_command_pressed("cancel")) slog("Velocity: %f/%f/%f", self->velocity.x, self->velocity.y, self->velocity.z);
}

void player_update(Entity *self)
{
	if (!self)return;

	gfc_vector3d_add(self->position, self->position, self->velocity);
	gfc_vector3d_add(self->collisionX.s.b, self->collisionX.s.b, self->velocity);

	//gfc_primitive_offset(self->collisionX, self->velocity);
	

	//int collided = ent_collid_action(self);

	//if (collided != 0) {
	//	slog("Colliding with object.");
	//}

	/*
	float direction;
	//gfc_vector3d_normalize(&self->velocity);

	if (self->velocity.y == 0 && self->velocity.x == 0) direction = 0;
	else {
		// Forward and back
		if (self->velocity.x) direction = (atan2(self->velocity.y, self->velocity.x));  
		else if (self->velocity.y > 0)  direction = GFC_HALF_PI;	//Left
		else { direction = -GFC_HALF_PI; }							//Right

		if (direction < 0) { direction += GFC_2PI; }
	}

	//slog("%f", direction);

	//Forward = 0
	//Right = One and a half PI
	//Left = Half PI
	//Down = PI

	// Forward
	if (direction == 0.0f) self->rotation.z = 0.0f;
	else if (direction == GFC_PI) self->rotation.z = GFC_PI;
	else if (direction == GFC_HALF_PI) self->rotation.z = GFC_HALF_PI;
	else if (direction == -GFC_HALF_PI) self->rotation.z = -GFC_HALF_PI;*/
}

int player_draw(Entity* self)
{
	if (!self)return;
	//slog("Player is drawing");
	return;
}

void player_free(Entity *self)
{
	if (!self)return;
}

