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
	:
	m_pMySQLInstance(HeatStroke::MySQLConnector::Instance()),
	m_bTournament(false),
	m_vTournamentRaceIds()
{
	m_pMySQLInstance->SetConnection("db4free.net:3307", "kartaclysm", "upeics483", "kartaclysm");
}

Kartaclysm::DatabaseManager::~DatabaseManager()
{
	m_pMySQLInstance->ClearConnection();
}

void Kartaclysm::DatabaseManager::StartTournament()
{
	assert(!m_bTournament);
	m_bTournament = true;
}

void Kartaclysm::DatabaseManager::CancelTournament()
{
	m_bTournament = false;
	m_vTournamentRaceIds.clear();
}

int Kartaclysm::DatabaseManager::EndTournament()
{
	assert(m_bTournament);
	assert(!m_vTournamentRaceIds.empty());

	int iTournamentId = -2; // no connection

	if (m_pMySQLInstance->HasConnection())
	{
		sql::SQLString strTournamentInsertQuery =
			"INSERT INTO tournament VALUES ();"
			"SET @inserted_tourn_id = LAST_INSERT_ID();";

		int iRaceNum = 0;
		for (auto iRaceId : m_vTournamentRaceIds)
		{
			if (iRaceId < 0) continue;
			AppendInsertStringForTournamentRace(&strTournamentInsertQuery, ++iRaceNum, iRaceId);
		}

		iTournamentId = GetIdFromTransactionInsertQuery(strTournamentInsertQuery, "@inserted_tourn_id");
	}

	m_bTournament = false;
	m_vTournamentRaceIds.clear();
	return iTournamentId;
}

int Kartaclysm::DatabaseManager::InsertRaceQuery(const Database::InsertRace& p_mRace)
{
	int iRaceId = -2; // no connection

	if (m_pMySQLInstance->HasConnection())
	{
		sql::SQLString strRaceInsertQuery =
			"INSERT INTO race(track_id, timestamp) "
			"VALUES(" + std::to_string(static_cast<int>(p_mRace.track_id)) + ", "
			"FROM_UNIXTIME(" + std::to_string(p_mRace.timestamp) + "));"
			"SET @inserted_race_id = LAST_INSERT_ID();";
		
		int iPosition = 0;
		for (auto mPlayer : p_mRace.race_players)
		{
			AppendInsertStringForPlayer(&strRaceInsertQuery, mPlayer, ++iPosition);
			AppendInsertStringForPlayerLaps(&strRaceInsertQuery, mPlayer);
		}

		iRaceId = GetIdFromTransactionInsertQuery(strRaceInsertQuery, "@inserted_race_id");
	}

	if (m_bTournament)
	{
		m_vTournamentRaceIds.push_back(iRaceId);
	}

	return iRaceId;
}

std::vector<float> Kartaclysm::DatabaseManager::SelectFastestTimes(Database::TrackPK p_eTrackID, int p_iNumResults /* = 1 */)
{
	std::vector<float> vTimes;

	if (p_iNumResults > 0 && p_iNumResults <= 100 &&
		m_pMySQLInstance->HasConnection())
	{
		sql::SQLString strQuery =
			"SELECT SUM(race_lap.lap_time) AS best_time FROM race_lap "
			"JOIN race ON race.race_id = race_lap.race_id "
			"WHERE race.track_id = " + std::to_string(static_cast<int>(p_eTrackID)) + " "
			"GROUP BY CONCAT(race_lap.race_id, '-', race_lap.player_id) "
			"ORDER BY best_time DESC "
			"LIMIT " + std::to_string(p_iNumResults);

		sql::ResultSet* pResults = m_pMySQLInstance->RunQuery(strQuery);
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

void Kartaclysm::DatabaseManager::AppendInsertStringForPlayer(sql::SQLString* p_pQuery, const Database::InsertRacePlayer& p_mPlayer, int p_iPosition) const
{
	p_pQuery->append("INSERT INTO race_player(race_id, player_id, kart_id, driver_id, position, is_human) "
		"VALUES (@inserted_race_id, " +
		std::to_string(p_mPlayer.player_num + 1) + ", " +
		std::to_string(static_cast<int>(p_mPlayer.kart)) + ", " +
		std::to_string(static_cast<int>(p_mPlayer.driver)) + ", " +
		std::to_string(p_iPosition) + ", " +
		(p_mPlayer.is_human ? "TRUE" : "FALSE") +
		");"
	);
}

void Kartaclysm::DatabaseManager::AppendInsertStringForPlayerLaps(sql::SQLString* p_pQuery, const Database::InsertRacePlayer& p_mPlayer) const
{
	int iLap = 0;
	for (auto fLapTime : p_mPlayer.lap_times)
	{
		p_pQuery->append(
			"INSERT INTO race_lap(race_id, player_id, lap_num, lap_time) "
			"VALUES (@inserted_race_id, " +
			std::to_string(p_mPlayer.player_num) + ", " +
			std::to_string(++iLap) + ", " +
			std::to_string(fLapTime) +
			");"
		);
	}
}

void Kartaclysm::DatabaseManager::AppendInsertStringForTournamentRace(sql::SQLString* p_pQuery, int p_iRaceNum, int p_iRaceId) const
{
	p_pQuery->append("INSERT INTO tourn_race(tourn_id, race_num, race_id) "
		"VALUES (@inserted_tourn_id, " +
		std::to_string(p_iRaceNum) + ", " +
		std::to_string(p_iRaceId) + ", " +
		");"
	);
}

int Kartaclysm::DatabaseManager::GetIdFromTransactionInsertQuery(sql::SQLString& p_strTransactionQuery, const sql::SQLString& p_strLastInsertIdentifier) const
{
	p_strTransactionQuery.append("SELECT " + p_strLastInsertIdentifier + " AS insert_id;");
	sql::ResultSet* pResults = m_pMySQLInstance->RunQueryUsingTransaction(p_strTransactionQuery);

	int iInsertId = -1;
	if (pResults != nullptr)
	{
		iInsertId = pResults->getInt("insert_id");
	}

	DELETE_IF(pResults);
	return iInsertId;
}