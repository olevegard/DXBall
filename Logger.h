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
	{
		inited = false;
	}
	void Log( const std::string &fileName, int32_t line, const std::string &msg )
	{
		std::stringstream ss;
		ss << std::left << std::setw( 20 ) << fileName << " @ " << std::setw( 5 ) << line << " : " << msg << std::endl;
		Write( ss.str() );
	}

	void Init( const std::string &filename )
	{
		file.open( filename, std::fstream::out );
		file << "======================== DX BALL ======================== \n";
		file << std::left << std::setw( 20 ) << "File"  << " @ " << std::setw( 5 ) << "Line" << " : " << "Message " << std::endl;
		inited = true;
	}

	void Init( const std::string &filename, const std::string &playername )
	{
		std::stringstream ss;
		ss << filename << "_" << playername << ".txt";
		Init( ss.str() );
	}
	private:

	void Write( const std::string &line )
	{
		std::cout << line;
		if ( inited )
			file << line;
	}
	std::fstream file;
	bool inited;
};
