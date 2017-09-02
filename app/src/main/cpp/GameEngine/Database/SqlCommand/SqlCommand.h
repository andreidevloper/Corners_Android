#pragma once

//---------------------------------------------------------------------------------------------------------------------
// class SqlCommand	
//---------------------------------------------------------------------------------------------------------------------

class SqlCommand : public ISqlCommand
{
public:
	SqlCommand(Vector<String> arguments = Vector<String>());
	
	// Serializing for database's command execution
	virtual String& VSerialize();
	virtual void VDeserialize(Vector<String>& arguments);

protected:
	void CreateSqlCommand(String& command, Vector<String>& arguments);

protected:
	String m_Command;

};
