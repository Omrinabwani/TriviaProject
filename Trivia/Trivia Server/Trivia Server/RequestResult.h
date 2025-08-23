#pragma once
#include "Buffer.h"

class IRequestHandler;

struct RequestResult
{
	Buffer response{};
	IRequestHandler* newHandler{};

};
