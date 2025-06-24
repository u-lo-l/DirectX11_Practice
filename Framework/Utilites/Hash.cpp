#include "framework.h"
#include "Hash.h"

std::uint32_t hash<pair<int, int>>::operator()(const pair<int, int>& Value) const noexcept
{
	constexpr std::uint32_t FnvPrime = 0x01000193; // 16777619
	constexpr std::uint32_t FnvOffsetBasis = 0x811C9DC5; // 2166136261
	std::uint32_t Result = FnvOffsetBasis;
	
	constexpr auto IntHash = hash<int>();
	const uint32_t H1 = IntHash(Value.first);
	const uint32_t H2 = IntHash(Value.second);
	Result ^= H1;
	Result *= FnvPrime;
	Result ^= H2;
	Result *= FnvPrime;
	return Result;
}

std::uint32_t hash<Vector2D>::operator()(const Vector2D& Value) const noexcept
{
	constexpr std::uint32_t FnvPrime = 0x01000193;
	constexpr std::uint32_t FnvOffsetBasis = 0x811C9DC5;
	std::uint32_t Result = FnvOffsetBasis;
	
	constexpr auto FloatHash = hash<float>();
	const size_t H1 = FloatHash(Value.X);
	const size_t H2 = FloatHash(Value.Y);
	Result ^= H1;
	Result *= FnvPrime;
	Result ^= H2;
	Result *= FnvPrime;
	return Result;
}

std::uint32_t hash<LineSegment2D>::operator()(const LineSegment2D& Value) const noexcept
{
	constexpr std::uint32_t FnvPrime = 0x01000193;
	constexpr std::uint32_t FnvOffsetBasis = 0x811C9DC5;
	std::uint32_t Result = FnvOffsetBasis;
	
	constexpr auto Vector2DHash = hash<Vector2D>();
	std::uint32_t H1 = Vector2DHash(Value.GetEndPoint(0));
	std::uint32_t H2 = Vector2DHash(Value.GetEndPoint(1));

	if (H1 > H2)
		std::swap(H1, H2);
	
	Result ^= H1;
	Result *= FnvPrime;
	Result ^= H2;
	Result *= FnvPrime;
	return Result;
}

