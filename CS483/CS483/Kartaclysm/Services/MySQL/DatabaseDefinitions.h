//====================================================================================
// DatabaseDefinitions
// Author: Bradley Cooper
//
// Provides enums and structs to match database schematics.
//====================================================================================

#ifndef DATABASE_DEFINITIONS_H
#define DATABASE_DEFINITIONS_H

#include <vector>
#include <ctime>

namespace Kartaclysm
{
	namespace Database
	{
		enum DriverPK
		{
			eCleopapa = 0,
			eClockmaker = 1,
			eKingpin = 2
		};
		enum KartPK
		{
			eJuggernaut = 0,
			eShowoff = 1,
			eSpeedster = 2
		};
		enum TrackPK
		{
			eNoobZone = 0,
			eShiftRift = 1,
			eUpAndOver = 2
		};

		// Breaking naming convention but variable names will try to match database fields
		struct InsertRacePlayer {
			int			player_num;
			KartPK		kart;
			DriverPK	driver;
			int			position;
			bool		is_human;

			// index matches to lap order (i.e. [0] = lap 0)
			std::vector<float>	lap_times;
		};

		struct InsertRace {
			TrackPK			track_id;
			std::time_t		timestamp;

			// index matches to player ID (i.e. [0] = Player0)
			std::vector<InsertRacePlayer>	race_players;

			InsertRace() : timestamp(std::time(nullptr)) {}
		};

		struct InsertTournament {
			// index matches to race order (i.e. [0] = race 0)
			std::vector<int> tourn_race_ids;
		};
	}
}

#endif