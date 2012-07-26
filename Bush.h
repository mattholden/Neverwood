#ifndef BUSH_H
#define BUSH_H

#include "base.h"

class Bush : public Base
{
public:
#if _DEBUG
	ManageMemory (Bush, BASETYPE_BUSH);
#endif
};

#endif