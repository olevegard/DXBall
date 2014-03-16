#pragma once

#include <sstream>

struct GameInfo
{
	public:
	std::string GetAsSrting() const
	{
		std::stringstream ss("");
		ss << playerName << " [ " << ip << " : " << host << " ] ";
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

	void Set( std::string ip_, int32_t host_, std::string playerName_ )
	{
		ip = ip_;
		host = host_;
		playerName = playerName_;
	}

	void SetGameID( int32_t gameID_ )
	{
		gameID = gameID_;
	}

	int32_t GetGameID() const
	{
		return gameID;
	}

	std::string GetPlayerName( ) const
	{
		return playerName;
	}

	private:
	std::string ip;
	int32_t host;
	int32_t gameID;
	std::string playerName;
};
