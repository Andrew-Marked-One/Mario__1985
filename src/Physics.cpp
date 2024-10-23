#include "Physics.h"
#include "Components.h"

Vec2 Physics::getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
	auto& aT = a->getComponent<CTransform>().pos;
	auto& bT = b->getComponent<CTransform>().pos;
	Vec2& aB = a->getComponent<CBoundingBox>().halfSize;
	Vec2& bB = b->getComponent<CBoundingBox>().halfSize;
	float dx = std::abs(aT.x - bT.x);
	float dy = std::abs(aT.y - bT.y);
	return Vec2(aB.x + bB.x - dx, aB.y + bB.y - dy);
}
Vec2 Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
	auto& aT = a->getComponent<CTransform>().prevPos;
	auto & bT = b->getComponent<CTransform>().prevPos;
	Vec2& aB = a->getComponent<CBoundingBox>().halfSize;
	Vec2& bB = b->getComponent<CBoundingBox>().halfSize;
	float dx = std::abs(aT.x - bT.x);
	float dy = std::abs(aT.y - bT.y);
	return Vec2(aB.x + bB.x - dx, aB.y + bB.y - dy);
}
