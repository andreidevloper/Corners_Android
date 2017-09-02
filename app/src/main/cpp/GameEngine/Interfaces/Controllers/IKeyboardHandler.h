#pragma once

class IKeyboardHandler : public virtual Object
{
public:
	IKeyboardHandler(Context* context);
	virtual ~IKeyboardHandler();
	virtual bool VOnKeyDown(const int c) = 0;
	virtual bool VOnKeyUp(const int c) = 0;
};

