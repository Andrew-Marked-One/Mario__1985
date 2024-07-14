#pragma once
#include "Animation.h"

class Component {
public:
	bool has = false;
};

class CTransform : public Component {
public:
	Vec2  pos      = { 0.0, 0.0 };
	Vec2  prevPos  = { 0.0, 0.0 };
	Vec2  scale    = { 1.0, 1.0 };
	Vec2  velocity = { 0.0, 0.0 };
	float angle    = 0;

	CTransform();
	CTransform(const Vec2& p);
	CTransform(const Vec2& p, const Vec2& v, const Vec2& sc, float a);
};

class CLifespan : public Component {
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifespan();
	CLifespan(int duration, int frame);
};

class CInput : public Component {
public:
	bool up = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool shoot = false;
	CInput();
};

class CBoundingBox : public Component {
public:
	Vec2 size;
	Vec2 halfSize;
	CBoundingBox();
	CBoundingBox(const Vec2& s);
};

class CAnimation : public Component {
public:
	Animation animation;
	bool repeat = false;
	CAnimation();
	CAnimation(const Animation& animation, bool r);
};

class CGravity : public Component {
public:
	float gravity = 0;
	CGravity();
	CGravity(float g);
};

class CState : public Component {
public:
	bool inAir = false;
	bool lookingRight = true;
	bool canJump = true;
	bool canMove = true;
	std::string state = "none";

	CState();
	CState(const std::string& s);
};

class CScore : public Component {
public:
	int score = 0;
	CScore();
	CScore(int s);
};
