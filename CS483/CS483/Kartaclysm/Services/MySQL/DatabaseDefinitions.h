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
			int			position;
			KartPK		kart;
			DriverPK	driver;
			bool		is_human;

			// index matches to lap order (i.e. [0] = lap 0)
			std::vector<float>	lap_times;

			InsertRacePlayer() : player_num(-1), position(-2), kart(eKartError), driver(eDriverError), is_human(true), lap_times() {}
		};

		struct InsertRace {
			bool			valid;
			TrackPK			track_id;
			std::time_t		timestamp;

			// index is irrelevant
			std::vector<InsertRacePlayer> race_players;

			InsertRace() : valid(false), track_id(eTrackError), timestamp(std::time(nullptr)), race_players() {}
		};

		struct InsertTournament {
			// index matches to race order (i.e. [0] = race 0)
			std::vector<int> tourn_race_ids;

			InsertTournament() : tourn_race_ids() {}
		};


		/**********************************************************************
		***********			Static Conversion Functions				***********
		**********************************************************************/
		static DriverPK StringToDriverPK(const std::string& p_strDriver)
		{
			if (p_strDriver == "Cleopapa")
				return eCleopapa;
			else if (p_strDriver == "Clockmaker")
				return eClockmaker;
			else if (p_strDriver == "Kingpin")
				return eKingpin;
			else
				return eDriverError;
		}
		static std::string DriverPKToString(DriverPK p_eDriver)
		{
			if (p_eDriver == eCleopapa)
				return "Cleopapa";
			else if (p_eDriver == eClockmaker)
				return "Clockmaker";
			else if (p_eDriver == eKingpin)
				return "Kingpin";
			else 
				return "";
		}

		static KartPK StringToKartPK(const std::string& p_strKart)
		{
			if (p_strKart == "Juggernaut")
				return eJuggernaut;
			else if (p_strKart == "Showoff")
				return eShowoff;
			else if (p_strKart == "Speedster")
				return eSpeedster;
			else
				return eKartError;
		}
		static std::string KartPKToString(KartPK p_eKart)
		{
			if (p_eKart == eJuggernaut)
				return "Juggernaut";
			else if (p_eKart == eShowoff)
				return "Showoff";
			else if (p_eKart == eSpeedster)
				return "Speedster";
			else
				return "";
		}

		static TrackPK StringToTrackPK(const std::string& p_strTrack)
		{
			if (p_strTrack == "Up And Over")
				return eUpAndOver;
			else if (p_strTrack == "Shift Rift")
				return eShiftRift;
			else if (p_strTrack == "Noob Zone")
				return eNoobZone;
			else
				return eTrackError;
		}
		static std::string TrackPKToString(TrackPK p_eTrack)
		{
			if (p_eTrack == eUpAndOver)
				return "Up And Over";
			else if (p_eTrack == eShiftRift)
				return "Shift Rift";
			else if (p_eTrack == eNoobZone)
				return "Noob Zone";
			else
				return "";
		}
	}
}

#endif