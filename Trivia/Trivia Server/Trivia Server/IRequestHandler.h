#pragma once
#include "RequestResult.h"
#include "RequestInfo.h"

class IRequestHandler
{
public:
	virtual bool isRequestRelevant(const RequestInfo& requestInfo) = 0;
	virtual RequestResult handleRequest(const RequestInfo& requestInfo) = 0;
};