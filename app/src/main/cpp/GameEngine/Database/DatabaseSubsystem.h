#pragma once

class DatabaseSubsystem : public Object
{
	URHO3D_OBJECT(DatabaseSubsystem, Object)

	typedef HashMap<String, String> SqlCommands;

public:
	DatabaseSubsystem(Context* context);

	DbResult CreateTable(String table);
	void DropTable(String tableName);

	void AddRecord(String tableName, String columns, String data);	
	void AddRecord(String tableName, String data);

	DbResult ExecuteSql(const String& sqlCommand, bool useCursorEvent = false);
	
	String GetSqlCommand(const String& commandName);
	DbResult ExecuteSqlFromXML(const String& commandName, Vector<String>& arguments, bool useCursorEvent = false);
	DbResult ExecuteInsertSqlFromXML(const String& commandName, Vector<String>& arguments, bool useCursorEvent = false);
	bool LoadSqlCommands(String path);
	bool ReloadSqlCommands(String path = String::EMPTY);

	inline void SetDatabase(Database* pDatabase) {m_pDatabase = pDatabase; }
	DbConnection* Connect(String pathToDb);

protected:
	inline static void ToQueryString(String& value) { value = ("'" + value).Append("'"); }
	
	static void CreateSqlCommand(String& command, Vector<String>& arguments);
	static void CreateInsertSqlCommand(String& command, Vector<String>& arguments);

protected:
	Database* m_pDatabase;			// Urho3D database

	String m_MainTable;
	String m_AbstrTable;

	DbConnection* m_pDbConnection;
	SqlCommands m_SqlCommands;

	String m_SqlCommandsXMLPath;
};

