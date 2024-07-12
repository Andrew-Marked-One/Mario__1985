#pragma once
#include "Vec2.h"

Vec2::Vec2() {}
Vec2::Vec2(float xin, float yin)
		: x(xin), y(yin) {}

bool Vec2::operator == (const Vec2& rhs) const {
	return (x == rhs.x && y == rhs.y);
}
bool Vec2::operator != (const Vec2& rhs) const {
	return !(x == rhs.x && y == rhs.y);
}
Vec2 Vec2::operator + (const Vec2& rhs) const {
	return Vec2(x + rhs.x, y + rhs.y);
}
Vec2 Vec2::operator - (const Vec2& rhs) const {
	return Vec2(x - rhs.x, y - rhs.y);
}
Vec2 Vec2::operator / (const float val) const {
	return Vec2(x / val, y / val);
}
Vec2 Vec2::operator * (const float val) const {
	return Vec2(x * val, y * val);
}

void Vec2::operator += (const Vec2& rhs) {
	x += rhs.x;
	y += rhs.y;
}
void Vec2::operator -= (const Vec2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
}
void Vec2::operator *= (const float rhs) {
	x *= rhs;
	y *= rhs;
}
void Vec2::operator /= (const float rhs) {
	x /= rhs;
	y /= rhs;
}
float Vec2::distMag() const {
	return sqrt(x * x + y * y);
}
Vec2& Vec2::normalize() {
	float d = (*this).distMag();
	x /= d;
	y /= d;
	return *this;
}