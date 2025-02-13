#pragma once
#include "..\..\cseries\cseries.h"
#include "transport_security.h"

enum e_transport_platform : long
{

};

struct s_transport_address // just transport_address in original source
{
	union {
		ulong ipv4;
		byte data[16];
	} address;
	ushort port;
	ushort address_size; // 4 for IPv4, 16 for IPv6
};
static_assert(sizeof(s_transport_address) == 0x14);

char* transport_address_get_string(s_transport_address const* address);
const char* transport_address_to_string(s_transport_address const* address, s_transport_secure_address const* secure_address, char* string, short maximum_string_length, bool append_port_info, bool append_security_info);
bool transport_address_equivalent(s_transport_address const* address1, s_transport_address const* address2);