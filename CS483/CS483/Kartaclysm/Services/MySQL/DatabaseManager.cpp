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
		bool bValidTournament = true;
		for (auto iRaceId : m_vTournamentRaceIds)
		{
			if (iRaceId < 0)
			{
				bValidTournament = false;
				break;
			}
			AppendInsertStringForTournamentRace(&strTournamentInsertQuery, ++iRaceNum, iRaceId);
		}

		if (bValidTournament)
		{
			iTournamentId = RunQueryAndReturnInsertId(strTournamentInsertQuery, "@inserted_tourn_id");
		}
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
		assert(p_mRace.track_id != Database::eTrackError);
		
		unsigned int uiLapCount = p_mRace.race_players.at(0).lap_times.size();
		if (uiLapCount > 1) // ignore debugging races with 0 or 1 laps
		{
			sql::SQLString strRaceInsertQuery =
				"INSERT INTO race(track_id, timestamp) "
				"VALUES(" + std::to_string(static_cast<int>(p_mRace.track_id)) + ", "
				"FROM_UNIXTIME(" + std::to_string(p_mRace.timestamp) + "));"
				"SET @inserted_race_id = LAST_INSERT_ID();";

			int iPosition = 0;
			bool bValidRace = true;
			for (auto mPlayer : p_mRace.race_players)
			{
				if (uiLapCount != mPlayer.lap_times.size())
				{
#ifdef _DEBUG
					assert(false && "One player has different lap count than another");
#endif
					iRaceId = -1;
					bValidRace = false;
					break;
				}
				uiLapCount = mPlayer.lap_times.size();

				AppendInsertStringForPlayer(&strRaceInsertQuery, mPlayer, ++iPosition);
				AppendInsertStringForPlayerLaps(&strRaceInsertQuery, mPlayer);
			}

			if (bValidRace)
			{
				iRaceId = RunQueryAndReturnInsertId(strRaceInsertQuery, "@inserted_race_id");
			}
		}
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
			"SELECT race_times.race_time AS best_time "
			"FROM race_times " // race_time is a created view, not a table FYI
			"WHERE race_times.track_id = " + std::to_string(static_cast<int>(p_eTrackID)) + " "
			"AND race_times.is_human = true "
			"ORDER BY best_time ASC "
			"LIMIT " + std::to_string(p_iNumResults) + ";";

		sql::ResultSet* pResults = m_pMySQLInstance->RunQuery(strQuery);
		if (pResults != nullptr)
		{
			while(pResults->next())
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
	assert(p_mPlayer.driver != Database::eDriverError);
	assert(p_mPlayer.kart != Database::eKartError);

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
			std::to_string(p_mPlayer.player_num + 1) + ", " +
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
		std::to_string(p_iRaceId) +
		");"
	);
}

int Kartaclysm::DatabaseManager::RunQueryAndReturnInsertId(sql::SQLString& p_strTransactionQuery, const sql::SQLString& p_strLastInsertIdentifier) const
{
	p_strTransactionQuery.append("SELECT " + p_strLastInsertIdentifier + " AS insert_id;");
	sql::ResultSet* pResults = m_pMySQLInstance->RunQuery(p_strTransactionQuery);

	int iInsertId = -1;
	if (pResults != nullptr)
	{
		pResults->next();
		iInsertId = pResults->getInt("insert_id");
	}

	printf("%s: %i\n", p_strLastInsertIdentifier.c_str(), iInsertId);
	DELETE_IF(pResults);
	return iInsertId;
}