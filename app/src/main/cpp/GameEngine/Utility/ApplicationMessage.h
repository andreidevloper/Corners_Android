#pragma once

enum APP_EVENT 
{
	KEY_UP, KEY_DOWN,
	TEXT_INPUT,
	MOUSE_MOVE, MOUSE_LBUTTONDOWN, MOUSE_RBUTTONDOWN, MOUSE_LBUTTONUP, MOUSE_RBUTTONUP, MOUSE_MIDDLEDOWN, MOUSE_MIDDLEUP,
	TOUCH_BEGIN
};

enum APP_SYSCOMMAND { SYS_MAXIMIZE, SYS_CLOSE, SYS_ALTLEFT, SYS_ALTRIGHT, SYS_UNKNOWN };
enum SPECIFIC_MSG { SPC_QUITNOPROMT };


struct APP_WPARAM
{
	APP_SYSCOMMAND m_syscommand;
	SPECIFIC_MSG m_specificMsg;
};

struct APP_LPARAM
{
	IntVector2 m_position;
	IntVector2 m_delta;
	int m_keycode;
	int m_scancode;
	int m_mode;
	unsigned int m_unicode;
};


struct AppMsg
{
	APP_EVENT uEvent;
	APP_LPARAM lParam;
	APP_WPARAM wParam;
};
