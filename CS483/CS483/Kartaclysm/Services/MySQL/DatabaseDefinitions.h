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
			eDriverError = -1,
			eCleopapa = 0,
			eClockmaker = 1,
			eKingpin = 2
		};
		enum KartPK
		{
			eKartError = -1,
			eJuggernaut = 0,
			eShowoff = 1,
			eSpeedster = 2
		};
		enum TrackPK
		{
			eTrackError = -1,
			eNoobZone = 0,
			eShiftRift = 1,
			eUpAndOver = 2
		};

		// Breaking naming convention but variable names will try to match database fields
		struct InsertRacePlayer {
			int			player_num;
			KartPK		kart;
			DriverPK	driver;
			bool		is_human;

			// index matches to lap order (i.e. [0] = lap 0)
			std::vector<float>	lap_times;

			InsertRacePlayer() : player_num(-1), kart(eKartError), driver(eDriverError), is_human(true), lap_times() {}
		};

		struct InsertRace {
			TrackPK			track_id;
			std::time_t		timestamp;

			// index matches to position (i.e. [0] = first place)
			std::vector<InsertRacePlayer>	race_players;

			InsertRace() : track_id(eTrackError), timestamp(std::time(nullptr)), race_players() {}
		};

		struct InsertTournament {
			// index matches to race order (i.e. [0] = race 0)
			std::vector<int> tourn_race_ids;

			InsertTournament() : tourn_race_ids() {}
		};
	}
}

#endif