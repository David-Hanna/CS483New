//===========================================================================
// DatabaseManager
// Author: Bradley Cooper
//
// Intermediate manager to run queries for Kartaclysm game.
//===========================================================================

#include "DatabaseManager.h"

Kartaclysm::DatabaseManager* Kartaclysm::DatabaseManager::s_pDatabaseManagerInstance = nullptr;

void Kartaclysm::DatabaseManager::CreateInstance()
{
	assert(s_pDatabaseManagerInstance == nullptr);
	s_pDatabaseManagerInstance = new DatabaseManager;
}

void Kartaclysm::DatabaseManager::DestroyInstance()
{
	assert(s_pDatabaseManagerInstance != nullptr);
	delete s_pDatabaseManagerInstance;
	s_pDatabaseManagerInstance = nullptr;
}

Kartaclysm::DatabaseManager* Kartaclysm::DatabaseManager::Instance()
{
	assert(s_pDatabaseManagerInstance != nullptr);
	return s_pDatabaseManagerInstance;
}

Kartaclysm::DatabaseManager::DatabaseManager()
{
	HeatStroke::MySQLConnector::Instance()->SetConnection("db4free.net:3307", "kartaclysm", "upeics483", "kartaclysm");
}

Kartaclysm::DatabaseManager::~DatabaseManager()
{
	HeatStroke::MySQLConnector::Instance()->ClearConnection();
}

int Kartaclysm::DatabaseManager::InsertRaceQuery(const Database::InsertRace& p_mRace)
{
	int iRaceId = -1;

	return iRaceId;
}

int Kartaclysm::DatabaseManager::InsertTournamentQuery(const Database::InsertTournament& p_mTournament)
{
	int iTournamentId = -1;

	return iTournamentId;
}

std::vector<float> Kartaclysm::DatabaseManager::SelectFastestTimes(Database::TrackPK p_eTrackID, int p_iNumResults /* = 1 */)
{
	std::vector<float> vTimes;

	if (p_iNumResults > 0 && p_iNumResults <= 100 &&
		HeatStroke::MySQLConnector::Instance()->HasConnection())
	{
		sql::SQLString strQuery =
			"SELECT SUM(race_lap.lap_time) AS best_time FROM race_lap "
			"JOIN race ON race.race_id = race_lap.race_id "
			"WHERE race.track_id = " + std::to_string(static_cast<int>(p_eTrackID)) + " "
			"GROUP BY race_lap.race_id "
			"ORDER BY best_time DESC "
			"LIMIT " + std::to_string(p_iNumResults);

		sql::ResultSet* pResults = HeatStroke::MySQLConnector::Instance()->RunQuery(strQuery);
		if (pResults != nullptr)
		{
			for (int i = 1; pResults->next(); ++i)
			{
				float fTime = static_cast<float>(pResults->getDouble("best_time"));
				vTimes.push_back(fTime);
			}
		}

		DELETE_IF(pResults);
	}

	return vTimes;
}