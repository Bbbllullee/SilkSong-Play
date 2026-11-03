#pragma once
#include "Objects/Actor.h"


class Movie :public Actor
{
public:
	Movie();
	virtual void Update(float deltaTime)override;
protected:
	class VideoPlayer* player;
	bool bSkipped = false;
};
