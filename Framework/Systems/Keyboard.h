#pragma once

#ifndef MAX_KEY_COUNT
# define MAX_KEY_COUNT 256
#endif

class Keyboard
{
public:
	static void Create();
	static void Destroy();
	static Keyboard * Get();
private:
	static Keyboard * Instance;
public:
	bool IsDown(int InKey);
	bool IsUp(int InKey);
	static bool IsPressed(int InKey);
	static bool IsToggled(int InKey);
private :
	Keyboard();
	~Keyboard();

private:
	bitset<MAX_KEY_COUNT> UpList;
	bitset<MAX_KEY_COUNT> DownList;
};