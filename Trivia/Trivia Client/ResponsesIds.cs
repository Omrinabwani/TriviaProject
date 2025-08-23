using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Trivia_Client
{
    enum ResponseId : uint
    {
        // The values represent the version of Trivia (10, 11... for 1.0.x, 20, 21.. for for 2.0.x, etc.)
        // They are also sorted by the order they appear in the UML
        ERROR_RESPONSE = 10,
        LOGIN_RESPONSE = 11,
        SIGNUP_RESPONSE = 12,

        LOGOUT_RESPONSE = 20,
        GET_ROOMS_RESPONSE = 21,
        GET_PLAYERS_IN_ROOM_RESPONSE = 22,
        GET_HIGHSCORE_RESPONSE = 23,
        GET_PERSONAL_STATS_RESPONSE = 24,
        JOIN_ROOM_RESPONSE = 25,
        CREATE_ROOM_RESPONSE = 26,

        LEAVE_ROOM_RESPONSE = 30,
        CLOSE_ROOM_RESPONSE = 31,
        START_GAME_RESPONSE = 32,
        GET_ROOM_STATE_RESPONSE = 33
    };
}
