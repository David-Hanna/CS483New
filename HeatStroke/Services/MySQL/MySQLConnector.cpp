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
		m_pConnection = m_pDriver->connect(p_strAddress, p_strUsername, p_strPassword);
		printf("Database connection successful\n");

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
		ClearConnection();
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

sql::ResultSet* HeatStroke::MySQLConnector::RunQuery(const sql::SQLString& p_strSQLQuery, bool p_bThreaded /*= false*/)
{
	if (!m_bValidConnection) return nullptr;
	sql::SQLString strCurrentQuery(p_strSQLQuery);

	sql::Statement* pStatement = nullptr;
	sql::ResultSet* pResults = nullptr;
	sql::Savepoint* pSavepoint = nullptr;

	if (p_bThreaded)
	{
		if (m_bRunningQueryOnThread)
		{
			printf("Cannot run additional query: thread still in use");
			return nullptr;
		}
		m_pDriver->threadInit();
		m_bRunningQueryOnThread = true;
	}

	try
	{
		if (Reconnect())
		{
			pSavepoint = m_pConnection->setSavepoint("save_me");
			pStatement = m_pConnection->createStatement();

			size_t next = 0;
			size_t last = 0;
			while (next != sql::SQLString::npos)
			{
				next = p_strSQLQuery.find(";", last);
				strCurrentQuery = p_strSQLQuery.substr(last, next - last);
				if (strCurrentQuery.length() > 1)
				{
					// NOTE: Errors if another query is run after a SELECT query
					pStatement->execute(strCurrentQuery);
				}
				last = next + 1;
			}

			pResults = pStatement->getResultSet();
			m_pConnection->commit();
		}
	}
	catch (sql::SQLException e)
	{
		if (pSavepoint != nullptr)
		{
			m_pConnection->rollback(pSavepoint);
		}

		std::string strOutputQuery(strCurrentQuery.asStdString());
		boost::replace_all(strOutputQuery, ";", ";\n\t");
		printf("%MySQLConnector: MySQL error #%i - %s\n\t%s\n", e.getErrorCode(), e.what(), strOutputQuery.c_str());
	}

	if (p_bThreaded)
	{
		m_pDriver->threadEnd();
		m_bRunningQueryOnThread = false;
	}
	
	DELETE_IF(pStatement);
	DELETE_IF(pSavepoint);
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
			m_pConnection->setAutoCommit(false);
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