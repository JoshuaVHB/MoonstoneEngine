#pragma once

#include <utility>
#include <iostream>

template<class _type>
struct Vec2 {

	union {
		struct { _type x, y; };
		_type body[2];
	};

	Vec2() : body{} {}
	Vec2(const _type _x, const _type _y) : x(_x), y(_y) {}

	template<typename Type> requires (std::convertible_to<Type, float>)
		explicit Vec2(const std::pair<Type, Type>& p) { x = p.first; y = p.second; }

	Vec2 operator+(const Vec2& other)	const { return Vec2{ x + other.x, y + other.y }; }
	Vec2 operator-(const Vec2& other)	const { return Vec2{ x - other.x, y - other.y }; }
	Vec2 operator/(const Vec2& other)	const { return Vec2{ x / other.x, y / other.y }; }
	Vec2 operator*(const Vec2& other)	const { return Vec2{ x * other.x, y * other.y }; }
	Vec2 operator*(const float scalar)	const { return Vec2{ x * scalar, y * scalar }; }
	Vec2 operator/(const float scalar)	const { const float inv = 1.f / scalar; return Vec2{ x,y }*inv; }
	_type& operator[](int i)	 { return body[i]; }
	const _type& operator[](int i)	const { return body[i]; }

};


namespace std {
	template <>
	struct hash<Vec2<int>>
	{
		int w;
		size_t operator()(const Vec2<int>& x) const
		{
			return x.x + x.y * w;
		}
	};
}

template<class _type>
inline std::ostream& operator<<(std::ostream& os, const Vec2<_type>& v) {
	os << "(" << (v.x) << "," << v.y << ")";
	return os;
}
