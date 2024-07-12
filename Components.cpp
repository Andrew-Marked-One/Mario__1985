#pragma once
#include "Components.h"

CTransform::CTransform() {}
CTransform::CTransform(const Vec2& p)
	: pos(p) {}
CTransform::CTransform(const Vec2& p, const Vec2& v, const Vec2& sc, float a)
	: pos(p), velocity(v), scale(sc), angle(a) {}


CLifespan::CLifespan() {}
CLifespan::CLifespan(int duration, int frame)
	: lifespan(duration), frameCreated(frame) {}


CInput::CInput() {}


CBoundingBox::CBoundingBox() {}
CBoundingBox::CBoundingBox(const Vec2& s)
	: size(s), halfSize(s.x / 2, s.y / 2) {}


CAnimation::CAnimation() {}
CAnimation::CAnimation(const Animation& animation, bool r)
		: animation(animation), repeat(r) {}


CGravity::CGravity() {}
CGravity::CGravity(float g)
		: gravity(g) {}


CState::CState() {}
CState::CState(const std::string& s)
		: state(s) {}


CScore::CScore() {}
CScore::CScore(int s)
		: score(s) {}