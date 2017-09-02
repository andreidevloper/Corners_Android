#pragma once

class IPointerHandler : public virtual Object
{
public:
	IPointerHandler(Context* context);
	virtual ~IPointerHandler();
	virtual bool VOnPointerMove(const IntVector2& pos, const int radius) = 0;
	virtual bool VOnPointerButtonDown(const IntVector2& pos, const int radius, const String& buttonName) = 0;
	virtual bool VOnPointerButtonUp(const IntVector2& pos, const int radius, const String& buttonName) = 0;
};