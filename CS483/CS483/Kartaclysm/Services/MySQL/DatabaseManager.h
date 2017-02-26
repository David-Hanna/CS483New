//===========================================================================
// DatabaseManager
// Author: Bradley Cooper
//
// Intermediate manager to run queries for Kartaclysm game.
//===========================================================================

#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "EasyXML.h"

#include <vector>
#include "DatabaseDefinitions.h"
#include "MySQLConnector.h"

namespace Kartaclysm
{
	class DatabaseManager
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static DatabaseManager* Instance();

		void StartTournament();
		void CancelTournament();
		int EndTournament();

		int InsertRaceQuery(const Database::InsertRace& p_mRace);

		std::vector<float> SelectFastestTimes(Database::TrackPK p_eTrackID, int p_iNumResults = 1);

	private:
		static DatabaseManager* s_pDatabaseManagerInstance;

		DatabaseManager();
		~DatabaseManager();

		HeatStroke::MySQLConnector* m_pMySQLInstance;
		bool m_bTournament;
		std::vector<int> m_vTournamentRaceIds;

		void AppendInsertStringForPlayer(sql::SQLString* p_pQuery, const Database::InsertRacePlayer& p_mPlayer, int p_iPosition) const;
		void AppendInsertStringForPlayerLaps(sql::SQLString* p_pQuery, const Database::InsertRacePlayer& p_mPlayer) const;
		void AppendInsertStringForTournamentRace(sql::SQLString* p_pQuery, int p_iRaceNum, int p_iRaceId) const;

		int RunQueryAndReturnInsertId(sql::SQLString& p_strTransactionQuery, const sql::SQLString& p_strLastInsertIdentifier) const;
	};
}

#endif
