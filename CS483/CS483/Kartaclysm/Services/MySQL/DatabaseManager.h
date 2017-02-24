//===========================================================================
// DatabaseManager
// Author: Bradley Cooper
//
// Intermediate manager to run queries for Kartaclysm game.
//===========================================================================

#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "Common.h"
#include "EasyXML.h"

#include <vector>
#include "MySQLConnector.h"
#include "DatabaseDefinitions.h"

namespace Kartaclysm
{
	class DatabaseManager
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static DatabaseManager* Instance();

		int InsertRaceQuery(const Database::InsertRace& p_mRace);
		int InsertTournamentQuery(const Database::InsertTournament& p_mTournament);

		std::vector<float> SelectFastestTimes(Database::TrackPK p_eTrackID, int p_iNumResults = 1);

	private:
		static DatabaseManager* s_pDatabaseManagerInstance;

		DatabaseManager();
		~DatabaseManager();
	};
}

#endif
