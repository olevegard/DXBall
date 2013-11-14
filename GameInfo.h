#pragma once

#include <sstream>

struct GameInfo
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

	int32_t GetPort() const
	{
		return host;
	}

	void Set( std::string ip_, int32_t host_)
	{
		ip = ip_;
		host = host_;
	}

	void SetGameID( int32_t gameID_ )
	{
		gameID = gameID_;
	}

	int32_t GetGameID() const
	{
		return gameID;
	}
	private:
	std::string ip;
	int32_t host;
	int32_t gameID;
};
