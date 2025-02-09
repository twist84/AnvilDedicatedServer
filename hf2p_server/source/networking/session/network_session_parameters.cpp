#include "network_session_parameters.h"
#include "..\..\dllmain.h"
#include "network_session.h"
#include <stdio.h>

void c_network_session_parameters::check_to_send_updates()
{
	void(__thiscall * check_to_send_updates)(c_network_session_parameters* parameters) = reinterpret_cast<decltype(check_to_send_updates)>(module_base + 0x1A390);
	check_to_send_updates(this);
}

void c_network_session_parameters::check_to_send_change_requests()
{
	void(__thiscall * check_to_send_change_requests)(c_network_session_parameters* parameters) = reinterpret_cast<decltype(check_to_send_change_requests)>(module_base + 0x1A7B0);
	check_to_send_change_requests(this);
}

void c_network_session_parameters::update()
{
	auto session = this->session;
	if (session->established())
	{
		if (session->is_host())
			this->check_to_send_updates();
		else
			this->check_to_send_change_requests();
	}
}

bool c_network_session_parameters::handle_change_request(bool is_leader, s_network_message_parameters_request const* message)
{
	if (this->session->compare_session_id(&message->session_id))
	{
		if (this->session->is_host())
		{
			byte* message_parameters = (byte*)&message->parameters;
			printf("MP/NET/STUB_LOG_PATH,STUB_LOG_FILTER: c_network_session_parameters::handle_change_request: [%s] handling session parameter change request [requested 0x%08X]\n",
				this->get_session_description(),
				message->change_request_parameters);
			long total_size = 0;
			for (long i = 0; i < k_network_session_parameter_type_count; i++)
			{
				if ((message->change_request_parameters & i) != 0) // assuming this is a mask of all the params - TODO
				{
					c_network_session_parameter_base* parameter = (c_network_session_parameter_base*)&message->parameters[i];
					long change_request_size = parameter->get_change_request_size();
					if (total_size + change_request_size > k_network_session_parameter_type_count * 0x400)
					{
						printf("MP/NET/SESSION,PARAMS: c_network_session_parameters::handle_change_request: [%s] parameter %d [%s] change request size out of bounds of payload size [0x%8X 0x%8X 0x%8X]\n",
							this->get_session_description(),
							i,
							parameter->m_name,
							change_request_size,
							total_size,
							k_network_session_parameter_type_count * 0x400);
						return false;
					}
					else
					{
						if (parameter->handle_change_request(message_parameters, change_request_size))
						{
							message_parameters += change_request_size;
							total_size += change_request_size;
						}
						else
						{
							printf("MP/NET/SESSION,PARAMS: c_network_session_parameters::handle_change_request: [%s] parameter %d failed to handle change request\n",
								this->get_session_description(),
								i);
							return false;
						}
					}
				}
			}
		}
		else
		{
			printf("MP/NET/STUB_LOG_PATH,STUB_LOG_FILTER: c_network_session_parameters::handle_change_request: [%s] not host, can't handle change request\n",
				this->get_session_description());
		}
	}
	else
	{
		printf("MP/NET/STUB_LOG_PATH,STUB_LOG_FILTER: c_network_session_parameters::handle_change_request: [%s] session id mismatch handling change request\n",
			this->get_session_description());
	}
	return true;
}

const char* c_network_session_parameters::get_session_description()
{
	const char* description_str = "UNKNOWN";
	if (this->session && this->session->get_session_id(nullptr))
		description_str = this->session->get_id_string();
	return description_str;
}