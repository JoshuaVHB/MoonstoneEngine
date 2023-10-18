#pragma once

#include <utility>
#include <iostream>

struct Vec2 {

	union {
		struct { float x, y; };
		float body[2];
	};

	Vec2() : body{} {}
	Vec2(const float _x, const float _y) : x(_x), y(_y) {}

	template<typename Type> requires (std::convertible_to<Type, float>)
		explicit Vec2(const std::pair<Type, Type>& p) { x = p.first; y = p.second; }

	Vec2 operator+(const Vec2& other)	const { return Vec2{ x + other.x, y + other.y }; }
	Vec2 operator-(const Vec2& other)	const { return Vec2{ x - other.x, y - other.y }; }
	Vec2 operator/(const Vec2& other)	const { return Vec2{ x / other.x, y / other.y }; }
	Vec2 operator*(const Vec2& other)	const { return Vec2{ x * other.x, y * other.y }; }
	Vec2 operator*(const float scalar)	const { return Vec2{ x * scalar, y * scalar }; }
	Vec2 operator/(const float scalar)	const { const float inv = 1.f / scalar; return Vec2{ x,y }*inv; }

};

inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	os << "(" << (v.x) << "," << v.y << ")";
	return os;
}
