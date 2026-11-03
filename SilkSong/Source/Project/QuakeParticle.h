#pragma once
#include "Objects/Actor.h"


class QuakeParticle :public Actor
{
public:
	QuakeParticle();
private:
	class ParticleSystem* particle;
	Timer DestroyTimerHandle;
};