#pragma once

class ISqlCommand;
typedef SharedPtr<ISqlCommand> ISqlCommandPtr;

class ISqlCommand : public Object
{
	URHO3D_OBJECT(ISqlCommand, Object);
public:
	ISqlCommand(Context* context);
	virtual ~ISqlCommand() {}
	
	virtual String& VSerialize() = 0;
	virtual void VDeserialize(Vector<String>& arguments) = 0;
};
