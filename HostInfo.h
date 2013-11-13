#pragma once

#include <sstream>

struct HostInfo
{
	public:
	std::string GetAsSrting() const
	{
		std::stringstream ss;
		ss << ip << " : " << host;
		return ss.str();
	}

	std::string GetIP() const
	{
		return ip;
	}

	int32_t GetPort()
	{
		return host;
	}

	void Set( std::string ip_, int32_t host_)
	{
		ip = ip_;
		host = host_;
	}
	private:
	std::string ip;
	int32_t host;
};
