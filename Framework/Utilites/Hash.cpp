#include "framework.h"
#include "Hash.h"

std::uint64_t hash<Vector2D>::operator()(const Vector2D& Value) const
{
	constexpr std::uint64_t prime{0x100000001B3};
	std::uint64_t result{0xcbf29ce484222325};
	
	constexpr auto FloatHash = hash<float>();
	const size_t h1 = FloatHash(Value.X);
	const size_t h2 = FloatHash(Value.Y);
	result = result ^ h1;
	result = result * prime;
	result = result ^ h2;
	result = result * prime;
	return result;
}

std::uint64_t hash<LineSegment2D>::operator()(const LineSegment2D& Value) const
{
	constexpr std::uint64_t prime{0x100000001B3};
	std::uint64_t result{0xcbf29ce484222325};

	constexpr auto Vector2DHash = hash<Vector2D>();
	size_t h1 = Vector2DHash(Value.GetEndPoint(0));
	size_t h2 = Vector2DHash(Value.GetEndPoint(1));

	if (h1 > h2)
		std::swap(h1, h2);
	
	result = result ^ h1;
	result = result * prime;
	result = result ^ h2;
	result = result * prime;
	return result;
}
