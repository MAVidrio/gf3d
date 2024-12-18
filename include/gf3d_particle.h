#ifndef __PARTICLES_H__
#define __PARTICLES_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gfc_color.h"
#include "gfc_shape.h"
#include "gfc_primitives.h"
#include "gf3d_model.h"

typedef enum
{
	PT_POINT,
	PT_SHAPE,
	PT_MODEL,
	PT_MAX
}ParticleType;

typedef struct {
	Uint32 ttl;						/**<time to live in frames*/
	ParticleType pType;				/**<how this particle should be drawn*/

	GFC_Vector3D position;			/**<*/
	GFC_Vector3D rotation;			/**<*/
	GFC_Vector3D scale;				/**<*/
	GFC_Vector3D velocity;			/**<*/
	GFC_Vector3D acceleration;		/**<*/

	GFC_Color color;
	GFC_Color colorVelocity;
	GFC_Color colorAcceleration;

	GFC_Box primShape;
	Model* model;
	GFC_Matrix4 matrix;
	float frame;
	float frameRate;
	float frameStart;		/**<first frame of the animation*/
	float frameEnd;			/**<last frame of the animation*/
	int loops;				/**<if true, loop, else stop on last frame*/
}Particle3D;

/**
* @brief sets up the manager, pre-allocates all particles
* @param particleMax the maximum number of particles that can exist at once
*/
void particle_manager_init(Uint32 particleMax);

/**
* @brief clear all active particles
*/
void particle_manager_clear();

/**
* @brief grab an empty particle
* 
*/
Particle3D* particle_new();

/*
* @brief Spawn a spray of particles from position in direction
* @param count how many paritlces to spawn
* @param ttl how many frames each particle should live
* @param color what base color to make the particles
* @param variation, how much that color should vary
* @param dir direction of spray
* @param point where to spawn from
* @param speed how fast the particles should move
* @param speedVariation how much the speed of particles should vary
* @param spread how much the direction should vary (in radians)
* @param acceleration how much, if at all, the particles should accelerate
*/
void particle_spray(
	Uint32 count, 
	Uint32 ttl, 
	GFC_Color color, 
	GFC_Color variation, 
	GFC_Vector3D dir, 
	GFC_Vector3D point, 
	float speed, 
	float speedVariation,
	float spread,
	GFC_Vector3D acceleration);
/*
* @brief Spawn a spray of particles from position in direction
* @param count how many paritlces to spawn
* @param ttl how many frames each particle should live
* @param color what base color to make the particles
* @param variation, how much that color should vary
* @param dir direction of spray
* @param point where to spawn from
* @param speed how fast the particles should move
* @param speedVariation how much the speed of particles should vary
* @param spread how much the direction should vary (in radians)
* @param acceleration how much, if at all, the particles should accelerate
*/
void particle_spray_follow_point(
	Uint32 count,
	Uint32 ttl,
	GFC_Color color,
	GFC_Color variation,
	GFC_Vector3D dir,
	GFC_Vector3D *point,
	float speed,
	float speedVariation,
	float spread,
	GFC_Vector3D acceleration);

/*
* @brief Update and draw all particles for the new frame
*/
void particle_manager_draw();
#endif