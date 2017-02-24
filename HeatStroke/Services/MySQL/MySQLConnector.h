//===========================================================================
// MySQLConnector
// Author: Bradley Cooper
//
// Provides interface to interact with MySQL database.
//===========================================================================

#ifndef MYSQL_CONNECTOR_H
#define MYSQL_CONNECTOR_H

#include "Common.h"
#include "EasyXML.h"

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

namespace HeatStroke
{
	class MySQLConnector
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static MySQLConnector* Instance();

		bool SetConnection(const sql::SQLString& p_strAddress, const sql::SQLString& p_strUsername, const sql::SQLString& p_strPassword, const sql::SQLString& p_strSchema = "");
		void ClearConnection();
		bool HasConnection() const { return m_bValidConnection; };

		sql::ResultSet* RunQuery(const sql::SQLString& p_strSQLQuery);
		sql::ResultSet* RunQuery(sql::PreparedStatement* p_pPrepStatement);
		sql::PreparedStatement* CreateNewPreparedStatement(const sql::SQLString& p_strPreparedSQL);

	private:
		static MySQLConnector* s_pMySQLConnectorInstance;

		MySQLConnector() :
			m_bValidConnection(false),
			m_pDriver(get_driver_instance()), // memory is auto-managed, do not delete
			m_pConnection(nullptr),
			m_strAddress(""),
			m_strUsername(""),
			m_strPassword(""),
			m_strSchema("") {} 
		~MySQLConnector() { ClearConnection(); }

	private:

		bool Reconnect();
		void CloseConnection();

		bool m_bValidConnection;
		sql::Driver* m_pDriver;
		sql::Connection* m_pConnection;

		sql::SQLString m_strAddress;
		sql::SQLString m_strUsername;
		sql::SQLString m_strPassword;
		sql::SQLString m_strSchema;
	};
}

#endif
