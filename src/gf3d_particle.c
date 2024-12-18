#include "simple_logger.h"

#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_buffers.h"

#include "gf3d_particle.h"

typedef struct
{
	Particle3D *particleList;
	Uint32 particleMax;
}ParticleManager;

static ParticleManager particle_manager = { 0 };

void particle_manager_close() {
	if (particle_manager.particleList) {
		free(particle_manager.particleList);
	}
	memset(&particle_manager, 0, sizeof(ParticleManager));
}

void particle_manager_init(Uint32 particleMax)
{
	if (!particleMax)
	{
		slog("cannot allocate zero particles for the particle manager");
		return;
	}

	particle_manager.particleList = gfc_allocate_array(sizeof(Particle3D), particleMax);
	if (!particle_manager.particleList)
	{
		slog("failed to allocate particles for the manager");
		return;
	}
	particle_manager.particleMax = particleMax;

	atexit(particle_manager_close);
}

void particle_manager_clear()
{
	int i;
	if (!particle_manager.particleList) return;
	for (i = 0; i < particle_manager.particleMax; i++) {
		if (particle_manager.particleList[i].model) {
			gf3d_model_free(particle_manager.particleList[i].model);
		}
	}
	memset(particle_manager.particleList, 0, sizeof(Particle3D) * particle_manager.particleMax);
}

Particle3D* particle_new() 
{
	int i;
	for (i = 0; i < particle_manager.particleMax; i++) {
		if (particle_manager.particleList[i].ttl == 0) {
			if (particle_manager.particleList[i].model) {
				gf3d_model_free(particle_manager.particleList[i].model);
			}
			memset(&particle_manager.particleList[i], 0, sizeof(Particle3D));
			particle_manager.particleList[i].ttl = 1;
			return &particle_manager.particleList[i];
		}
	}
	return NULL;	// All out of slots for new particles
}

void particle_update(Particle3D* p) 
{
	if (!p)return;
	p->ttl--;
	if (!p->ttl)return;
	gfc_vector3d_add(p->position, p->position, p->velocity);
	gfc_vector3d_add(p->velocity, p->velocity, p->acceleration);

	gfc_color_add(&p->color, p->color, p->colorVelocity);
	gfc_color_add(&p->colorVelocity, p->colorVelocity, p->colorAcceleration);

	// Animation frames for models
	if (p->pType == PT_MODEL) {
		p->frame += p->frameRate;

		if (p->frame >= p->frameEnd) {
			if (p->loops) {
				p->frame = p->frameStart;
			}
			else {
				p->frame = p->frameEnd;
				p->frameRate = 0;
			}
		}
	}
}

void particle_draw(Particle3D* p)
{
	if (!p)return;
	switch (p->pType) {
	case PT_MAX:
	case PT_POINT:
		gfc_render_box(gfc_box(p->position.x, p->position.y, p->position.z, 1, 1, 1), p->color);
		break;
	case PT_SHAPE:
		gfc_render_box(p->primShape,p->color);
		break;
	case PT_MODEL:
		gfc_matrix4_from_vectors(
			p->matrix,
			p->position,
			p->rotation,
			p->scale
		);
		gf3d_model_draw(
			p->model,
			p->matrix,
			p->color,
			(Uint32)p->frame);
		break;
	}
}

void particle_manager_draw() {
	int i;
	for (i = 0; i < particle_manager.particleMax; i++) {
		if (particle_manager.particleList[i].ttl > 0) {
			particle_update(&particle_manager.particleList[i]);
			particle_draw(&particle_manager.particleList[i]);
		}
	}
}

void particle_spray(Uint32 count, Uint32 ttl, GFC_Color color, GFC_Color variation, GFC_Vector3D dir, GFC_Vector3D point, float speed,float speedVariation, float spread, GFC_Vector3D acceleration) {
	Particle3D* p;
	int i;
	GFC_Vector3D d;
	gfc_vector3d_normalize(&dir);
	for (i = 0; i < count; i++) {
		p = particle_new();
		if (!p)return;
		p->ttl = ttl;
		gfc_color_copy(p->color, color);
		p->color.r += gfc_crandom() * variation.r;
		p->color.g += gfc_crandom() * variation.g;
		p->color.b += gfc_crandom() * variation.b;
		p->color.a += gfc_crandom() * variation.a;
		gfc_vector3d_copy(p->position, point);

		p->colorVelocity = gfc_color_hsl(0, -0.01, -0.01, -0.01);

		gfc_vector3d_rotate_about_vector(&d, dir, point, gfc_crandom() * spread);
		gfc_vector3d_scale(p->velocity, d, speed*gfc_crandom()*speedVariation);
		gfc_vector3d_copy(p->acceleration, acceleration);

		/* For models */
		//p->pType = PT_MODEL;
		//p->model = gf3d_obj_load_from_file("models/primitives/MyCube/TestCube.obj");
	}
}

void particle_spray_follow_point(Uint32 count, Uint32 ttl, GFC_Color color, GFC_Color variation, GFC_Vector3D dir, GFC_Vector3D* point, float speed, float speedVariation, float spread, GFC_Vector3D acceleration) {
	particle_spray(
		count,
		ttl,
		color,
		variation,
		dir,
		*point,
		speed,
		speedVariation,
		spread,
		acceleration);
}