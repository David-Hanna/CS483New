//===========================================================================
// MySQLConnector
// Author: Bradley Cooper
//
// Provides interface to interact with MySQL database.
//===========================================================================

#include "MySQLConnector.h"

HeatStroke::MySQLConnector* HeatStroke::MySQLConnector::s_pMySQLConnectorInstance = nullptr;

void HeatStroke::MySQLConnector::CreateInstance()
{
	assert(s_pMySQLConnectorInstance == nullptr);
	s_pMySQLConnectorInstance = new MySQLConnector;
}

void HeatStroke::MySQLConnector::DestroyInstance()
{
	assert(s_pMySQLConnectorInstance != nullptr);
	delete s_pMySQLConnectorInstance;
	s_pMySQLConnectorInstance = nullptr;
}

HeatStroke::MySQLConnector* HeatStroke::MySQLConnector::Instance()
{
	assert(s_pMySQLConnectorInstance != nullptr);
	return s_pMySQLConnectorInstance;
}

bool HeatStroke::MySQLConnector::SetConnection(const sql::SQLString& p_strAddress, const sql::SQLString& p_strUsername, const sql::SQLString& p_strPassword, const sql::SQLString& p_strSchema)
{
#ifdef _DEBUG
	printf("Database connection disabled for Debug mode\n");
	return false;
#endif

	ClearConnection();

	try
	{
		printf("Connecting:  %s\nUsername:    %s\nPassword:    %s\nDatabase:    %s\n", p_strAddress.c_str(), p_strUsername.c_str(), p_strPassword.c_str(), p_strSchema.c_str());
		m_pConnection = m_pDriver->connect(p_strAddress, p_strUsername, p_strPassword);
		printf("Successfully connected\n");

		m_strAddress = p_strAddress;
		m_strUsername = p_strUsername;
		m_strPassword = p_strPassword;

		if (p_strSchema != "")
		{
			m_pConnection->setSchema(p_strSchema);
			m_strSchema = p_strSchema;
		}
		m_bValidConnection = m_pConnection->isValid();
	}
	catch (sql::SQLException e)
	{
		printf("%MySQLConnector: s\n", e.what());
	}
	
	CloseConnection();
	return m_bValidConnection;
}

void HeatStroke::MySQLConnector::ClearConnection()
{
	m_strAddress = "";
	m_strUsername = "";
	m_strPassword = "";
	m_strSchema = "";
	CloseConnection();
	DELETE_IF(m_pConnection);
	m_bValidConnection = false;
}

sql::PreparedStatement* HeatStroke::MySQLConnector::CreateNewPreparedStatement(const sql::SQLString& p_strPreparedSQL)
{
	if (!m_bValidConnection) return nullptr;

	sql::PreparedStatement* pPrepStatement = m_pConnection->prepareStatement(p_strPreparedSQL);

	return pPrepStatement;
}

sql::ResultSet* HeatStroke::MySQLConnector::RunQuery(const sql::SQLString& p_strSQLQuery)
{
	if (!m_bValidConnection) return nullptr;

	sql::Statement* pStatement = nullptr;
	sql::ResultSet* pResults = nullptr;
	try
	{
		if (Reconnect())
		{
			pStatement = m_pConnection->createStatement();
			pResults = pStatement->executeQuery(p_strSQLQuery);
		}
	}
	catch (sql::SQLException e)
	{
		printf("%MySQLConnector: Error with query - %s\n%s\n", p_strSQLQuery.c_str(), e.what());
	}
	
	DELETE_IF(pStatement);
	CloseConnection();
	return pResults;
}

sql::ResultSet* HeatStroke::MySQLConnector::RunQuery(sql::PreparedStatement* p_pPrepStatement)
{
	if (!m_bValidConnection) return nullptr;

	sql::ResultSet* pResults = nullptr;

	try
	{
		if (Reconnect())
		{
			if (p_pPrepStatement->execute())
			{
				pResults = p_pPrepStatement->getResultSet();
			}
		}
	}
	catch (sql::SQLException e)
	{
		printf("%MySQLConnector: Error with prepared statement\n%s\n", e.what());
	}

	CloseConnection();
	return pResults;
}

bool HeatStroke::MySQLConnector::Reconnect()
{
	if (!m_bValidConnection) return false;
	assert(m_pConnection != nullptr);

	if (m_pConnection->isValid() || m_pConnection->reconnect())
	{
		return true;
	}
	else
	{
		m_pConnection = m_pDriver->connect(m_strAddress, m_strUsername, m_strPassword);
		if (m_strSchema != "")
		{
			m_pConnection->setSchema(m_strSchema);
		}
		return m_pConnection->isValid();
	}
}

void HeatStroke::MySQLConnector::CloseConnection()
{
	if (m_bValidConnection && !m_pConnection->isClosed())
	{
		m_pConnection->close();
	}
}

void HeatStroke::MySQLConnector::Test()
{
	if (m_bValidConnection)
	{
		sql::SQLString quer = "SELECT Hello.Key FROM Hello";
		printf("Testing query: %s\n", quer.c_str());
		sql::ResultSet* res = RunQuery(quer);

		if (res != nullptr)
		{
			for (int i = 1; res->next(); ++i)
			{
				printf("\tResult %i -> %s\n", i, res->getString("Key").c_str());
			}
			printf("End query results\n");
		}
		else
		{
			printf("Error: No results\n", quer.c_str());
		}

		DELETE_IF(res);
	}
}