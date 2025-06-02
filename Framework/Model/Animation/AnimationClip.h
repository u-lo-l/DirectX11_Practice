#pragma once

class AnimationClip
{
public:
	explicit AnimationClip(const wstring & AnimationAssetPath);
	~AnimationClip();
private:
	string Name;
	float Duration;
	float TickPerSecond;
};
