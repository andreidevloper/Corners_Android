#include "GameEngineStd.h"
#include <iostream>
#include "DatabaseSubsystem.h"

DatabaseSubsystem::DatabaseSubsystem(Context* context) : Object(context)
{
	m_MainTable = "account"
		"("
		"Id SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY, "
		"Username varchar(45), "
		"Firstname varchar(45), "
		"Lastname varchar(45), "
		"Email varchar(45), "
		"Password varchar(45), "
		"Admin boolean"
		")";

	m_AbstrTable = "hero"
		"("
		    "Id int NOT NULL AUTO_INCREMENT PRIMARY KEY, "
		    "Class varchar(45), "
		    "Name varchar(45), "
		    "Level int, "
			"Account_Id SMALLINT UNSIGNED NOT NULL, "
		    "CONSTRAINT `Hero_Owner` FOREIGN KEY(Account_Id) REFERENCES account(Id) ON DELETE CASCADE ON UPDATE RESTRICT"
			")";
}

DbResult DatabaseSubsystem::CreateTable(String table)
{
	String input = "CREATE TABLE IF NOT EXISTS " + table;
	URHO3D_LOGINFO(input);
	return m_pDbConnection->Execute(input, true);
}

void DatabaseSubsystem::DropTable(String tableName)
{
	String input = "DROP TABLE IF EXISTS " + tableName;
    URHO3D_LOGINFO(input);
	m_pDbConnection->Execute(input, true);
}

void DatabaseSubsystem::AddRecord(String tableName, String data)
{
	String input = "INSERT INTO" + tableName + "VALUES(" + data + ")";
    URHO3D_LOGINFO(input);
	m_pDbConnection->Execute(input, true);
}

void DatabaseSubsystem::AddRecord(String tableName, String columns, String data)
{
	String input = "INSERT INTO" + tableName + "VALUES(" + data + ")";
    URHO3D_LOGINFO(input);
	m_pDbConnection->Execute(input, true);
}


String DatabaseSubsystem::GetSqlCommand(const String& commandName)
{
	assert(m_pDbConnection);

	String sqlCommand;
	if (m_SqlCommands.Contains(commandName))
	{
		sqlCommand = m_SqlCommands[commandName];
	}
	else
	{
        URHO3D_LOGINFO("Failed to find sql command");
		return String::EMPTY;
	}

	return sqlCommand;
}

DbResult DatabaseSubsystem::ExecuteSqlFromXML(const String& commandName, Vector<String>& arguments, bool useCursorEvent)
{
	String sqlCommand = GetSqlCommand(commandName);

	if (sqlCommand.Empty())
	{
		return DbResult();
	}

    // Copy of command
    String sqlCommandExecute;

    sqlCommandExecute.Append(sqlCommand);

    sqlCommandExecute.Replace(";lth;","<>0", false);

	CreateSqlCommand(sqlCommandExecute, arguments);

    URHO3D_LOGINFO(sqlCommandExecute);

	return m_pDbConnection->Execute(sqlCommandExecute, useCursorEvent);
}

DbResult DatabaseSubsystem::ExecuteInsertSqlFromXML(const String& commandName, Vector<String>& arguments, bool useCursorEvent)
{
	String sqlCommand = GetSqlCommand(commandName);

	if (sqlCommand.Empty())
	{
		return DbResult();
	}

    // Copy of command
    String sqlCommandExecute;

    sqlCommandExecute.Append(sqlCommand);

    sqlCommandExecute.Replace(";lth;","<>0", false);

	CreateInsertSqlCommand(sqlCommandExecute, arguments);

    URHO3D_LOGINFO(sqlCommandExecute);

	return m_pDbConnection->Execute(sqlCommandExecute, useCursorEvent);
}

DbResult DatabaseSubsystem::ExecuteSql(const String& sqlCommand, bool useCursorEvent)
{
	assert(m_pDbConnection);
    URHO3D_LOGINFO(sqlCommand);
	return m_pDbConnection->Execute(sqlCommand, useCursorEvent);
}

bool DatabaseSubsystem::LoadSqlCommands(String path)
{
	XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(path);

	if (!file)
	{
		URHO3D_LOGERROR("Failed to load sql commands from xml file");
		return false;
	}

	XMLElement root = file->GetRoot();
	for (auto it = root.GetChild(); it; it = it.GetNext())
	{
		String commandName = it.GetAttribute("name");
		String command = it.GetValue();
		m_SqlCommands[commandName] = command;
	}

	return true;
}


bool DatabaseSubsystem::ReloadSqlCommands(String path)
{
	if (!path.Empty())
	{
		m_SqlCommandsXMLPath = path;
	}

	m_SqlCommands.Clear();

	if (!LoadSqlCommands(path))
	{
		return false;
	}

	return true;
}

void DatabaseSubsystem::CreateSqlCommand(String& command, Vector<String>& arguments)
{
	String var = "VAR";
	String countStr = "1";

	// Counter
	int count = 1;

	// Counter previous number
	int oldCount = count;

	// String VAR1, VAR2, VAR3 ...
	var = var + countStr;

	for (auto it = arguments.Begin(); it != arguments.End(); it++)
	{
		ToQueryString(*it);
		command.Replace(var, (*it));
		count++;
		countStr = String(count);
		var.Replace(String(oldCount), countStr);
		oldCount++;
	}
}

void DatabaseSubsystem::CreateInsertSqlCommand(String& command, Vector<String>& arguments)
{
	String values;
	int size = arguments.Size() - 2;
	for (int i = 0; i <= size; i++)
	{
		ToQueryString(arguments[i]);
		values += arguments[i].Append(',');
	}

	size++;
	ToQueryString(arguments[size]);
	values += (arguments[size]);

	command.Replace("VARIABLES", values);
}

DbConnection* DatabaseSubsystem::Connect(String pathToDb)
{
    m_pDbConnection = m_pDatabase->Connect(pathToDb);
    return m_pDbConnection;
}

