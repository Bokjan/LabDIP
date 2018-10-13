#include "DisplayAgent.h"

DisplayAgent DisplayAgent::displayAgent;

DisplayAgent * DisplayAgent::GetInstance()
{
	return &DisplayAgent::displayAgent;
}
