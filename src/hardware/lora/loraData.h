#ifndef LORADATA_INCLUDED
#define LORADATA_INCLUDED

#include "../../settings/settings.h"

struct LORADATA
{
    DATA data;
    unsigned char ttl;   // time to live (for mesh network)
};

#endif
