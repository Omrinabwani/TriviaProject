#pragma once

#include <ctime>
#include "Requests.h"
#include "Buffer.h"

struct RequestInfo
{
	RequestId id{}; // This represents whether the request is of signup, login, etc.
	time_t receivalTime{};
	Buffer buffer{};

};
