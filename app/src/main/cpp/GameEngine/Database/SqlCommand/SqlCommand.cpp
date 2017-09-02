#include "GameEngineStd.h"
#include "SqlCommand.h"

SqlCommand::SqlCommand(Vector<String> arguments) : ISqlCommand(g_pApp->GetContext())
{
	VDeserialize(arguments);
}

String& SqlCommand::VSerialize()
{
	return m_Command;
}

void SqlCommand::VDeserialize(Vector<String>& arguments)
{

}

void SqlCommand::CreateSqlCommand(String& command, Vector<String>& arguments)
{

}
