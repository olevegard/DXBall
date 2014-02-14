#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>

class Logger
{
	public:
	Logger()
		:	logCout( true )
		,	logFile( true )
		,	inited( false )
	{

	}
	void Log( const std::string &fileName, int32_t line, const std::string &msg )
	{
		if ( !logCout && !logFile )
			return;

		std::stringstream ss;
		ss << std::left << std::setw( 20 ) << fileName << " @ " << std::setw( 5 ) <<  line << " : " << msg;
		std::string str = ss.str();

		if ( logFile )
			Write( str  );

		if ( logCout )
			std::cout << str << std::endl;
	}
	template < class T >
	void Log( const std::string &fileName, int32_t line, const std::string &msg, T object )
	{
		if ( !logCout && !logFile )
			return;

		std::stringstream ss;
		ss  << msg << " : " << object;

		Log( fileName, line, ss.str() );
	}
	void Init( const std::string &fileName = "local" )
	{
		std::stringstream ss;

		ss << "log_" << fileName << ".txt";
		CreateFile( ss.str() );
	}
	private:

	void Write( const std::string &line )
	{
		if ( inited )
			file << line;
	}
	void CreateFile( const std::string &fileName )
	{
		file.open( fileName, std::fstream::out );
		file << "======================== DX BALL ======================== \n";
		file << std::left << std::setw( 20 ) << "File"  << " @ " << std::setw( 5 ) << "Line" << " : " << "Message " << std::endl;
		inited = true;
	}

	std::fstream file;

	bool logCout;
	bool logFile;
	bool inited;
};
