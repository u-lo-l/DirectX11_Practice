#pragma once

/**
 * @link "https://en.cppreference.com/w/cpp/utility/hash/operator%28%29" @endlink
 */
namespace std
{
	template<> struct hash<Vector2D>
	{
		std::uint64_t operator()(const Vector2D & Value) const;
	};
	
	template<> struct hash<LineSegment2D>
	{
		std::uint64_t operator()(const LineSegment2D & Value) const;
	};
}