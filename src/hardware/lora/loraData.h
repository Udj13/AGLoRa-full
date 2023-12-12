/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

#ifndef LORADATA_INCLUDED
#define LORADATA_INCLUDED

#include "../../settings/settings.h"

struct LORADATA
{
    DATA data;
    unsigned char ttl;   // time to live (for mesh network)
};

#endif
