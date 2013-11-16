#include <iostream>
#include <sstream>
#include <algorithm>

#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "TCPConnectionServer.h"
#include "../TCPMessage.h"
#include "../GameInfo.h"

class Server
{
	public:

	Server()
		:	gameCount( 0 )
		,	screenRect( { 0, 0, 500, 800 } )
		,	lastHeight( 0 )
	{
	}
	bool InitSDL()
	{
		if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
		{
			std::cout << "Server.cpp@" << __LINE__ << " : SLD_Init failed : " << SDL_GetError();
			return false;
		}
		return true;
	}
	bool CreateWindow()
	{
		window = SDL_CreateWindow(
				"DXBall Server",
				screenRect.x,
				screenRect.y,
				screenRect.w,
				screenRect.h,
				SDL_WINDOW_OPENGL
			);

		if ( !window )
		{
			std::cout << "Server.cpp@" << __LINE__ << " : SDL_CreateWindw failed : " << SDL_GetError();
			return false;
		}
		return true;
	}
	bool CreateRenderer()
	{
		renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

		if ( !renderer )
		{
			std::cout << "Server.cpp@" << __LINE__ << " : SDL_CreateRender failed : " << SDL_GetError();
			return false;
		}

		SDL_RenderSetLogicalSize( renderer, 500, 800 );

		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
		SDL_RenderClear( renderer );
		SDL_RenderPresent( renderer );

		return true;
	}
	bool InitTTF()
	{
		if ( TTF_Init() == -1 )
		{
			std::cout << "Server.cpp@" << __LINE__ << " : TTF_Init failed : " << TTF_GetError();
			return false;
		}

		return true;
	}
	bool InitFonts()
	{

		fontHeader = TTF_OpenFont( "../media/fonts/sketchy.ttf", 40  );//TTF_OpenFont( "~/Programming/DXBall/media/fonts/sketchy.ttf", 50  );

		if ( !fontHeader )
		{
			std::cout << "Server.cpp@" << __LINE__ << " : TTF_OpenFont failed : " << TTF_GetError();
			return false;
		}

		fontSubHeader = TTF_OpenFont( "../media/fonts/sketchy.ttf", 40  );

		if ( !fontSubHeader )
		{
			std::cout << "Server.cpp@" << __LINE__ << " : TTF_OpenFont failed : " << TTF_GetError();
			return false;
		}

		fontGameLine = TTF_OpenFont( "../media/fonts/sketchy.ttf", 25  );

		if ( !fontGameLine )
		{
			std::cout << "Server.cpp@" << __LINE__ << " : TTF_OpenFont failed : " << TTF_GetError();
			return false;
		}

		return true;
	}
	bool InitHeader()
	{
		SDL_Color textColor{ 255, 0, 255, 255 };
		SDL_Surface* surf = TTF_RenderText_Solid( fontHeader, "Server", textColor );
		textureHeader = SDL_CreateTextureFromSurface( renderer, surf );

		rectHeader.w = surf->clip_rect.w;
		rectHeader.h = surf->clip_rect.h;
		rectHeader.x = ( 500 * 0.5 ) - (  rectHeader.w * 0.5 );
		rectHeader.y = 10;

		SDL_FreeSurface( surf );

		return true;
	}
	bool InitSubHeader()
	{

		SDL_Color textColor{ 0, 255, 255, 255 };
		SDL_Surface* surf = TTF_RenderText_Solid( fontSubHeader, "Games : ", textColor );
		textureSubHeader = SDL_CreateTextureFromSurface( renderer, surf );

		rectSubHeader.w = surf->clip_rect.w;
		rectSubHeader.h = surf->clip_rect.h;
		rectSubHeader.x = 10;
		rectSubHeader.y = rectHeader.h + rectHeader.y + 20;
		lastHeight = rectSubHeader.y + 50;

		SDL_FreeSurface( surf );

		return true;
	}
	void AddGameLine( const std::string &IP, int32_t port )
	{
		std::stringstream ss;
		ss << "IP : " << IP << " | port : " << port;
		SDL_Color textColor{ 0, 255, 255, 255 };
		SDL_Surface* surf = TTF_RenderText_Solid( fontGameLine, ss.str().c_str(), textColor );
		SDL_Texture* textureGameLine = SDL_CreateTextureFromSurface( renderer, surf );

		SDL_Rect rectGameLine;
		rectGameLine.w = surf->clip_rect.w;
		rectGameLine.h = surf->clip_rect.h;
		rectGameLine.x = rectSubHeader.x + 20;
		rectGameLine.y = lastHeight;
		lastHeight += 30;

		SDL_FreeSurface( surf );

		rectsGameLine.push_back( rectGameLine );
		texturesGameLine.push_back( textureGameLine );
	}
	void RepositionGameLines()
	{
		lastHeight = rectSubHeader.y + 50;
		for ( auto &p : rectsGameLine )
		{
			p.y = lastHeight;
			lastHeight += 30;
		}
	}
	bool Init()
	{
		if ( !InitSDL() )
			return false;

		if ( !CreateWindow() )
			return false;

		if ( !CreateRenderer() )
			return false;

		if ( !InitTTF() )
			return false;

		if ( !InitFonts() )
			return false;

		if ( !InitHeader() )
			return false;

		if ( !InitSubHeader() )
			return false;

		if ( !InitNet( "127.0.0.1", 3113 ) )
			return false;


		return true;
	}
	bool InitNet( std::string ip, int32_t port )
	{
		if ( SDLNet_Init() < 0 )
		{
			std::cout << "Server.cpp@" << __LINE__ << " Failed to init SDLNet : " << SDLNet_GetError() << std::endl;
			return false;
		}

		if ( connection.Init( ip, port, true ) )
		{
		}
		else
			return false;

		if ( !connection.StartServer() )
			return false;

		if ( !connection.StartServer() )
			return false;

		std::cout << "Net initialized" << std::endl;

		return true;
	}
	void Run()
	{
		bool run = true;
		std::cout << "Run..\n";
		while ( run )
		{
			SDL_Event event;

			while ( SDL_PollEvent( &event ) )
			{
				if ( event.type == SDL_KEYDOWN )
				{
					switch ( event.key.keysym.sym )
					{
						case SDLK_q:
						case SDLK_ESCAPE:
							run = false;
					}
				}
				else if ( event.type == SDL_QUIT )
					run = false;
			}

			UpdateNetwork( 0 );
			UpdateNetwork( 1 );

			SDL_RenderClear( renderer );
			SDL_RenderCopy( renderer, textureHeader, nullptr, &rectHeader);
			SDL_RenderCopy( renderer, textureSubHeader, nullptr, &rectSubHeader);
			for ( int i = 0; i < texturesGameLine.size() ; ++i )
				SDL_RenderCopy( renderer, texturesGameLine[i] , nullptr, &rectsGameLine[i]);

			SDL_RenderPresent( renderer );
		}
		std::cout << "Main loop done\n";
	}
	void UpdateNetwork( int connectionNo )
	{
		std::string str = connection.ReadMessages( connectionNo );

		std::stringstream ss;
		ss << str;

		if ( str == "" )
			return;
		std::cout << "Received : " << str << std::endl;

		TCPMessage msg;
		while ( ss >> msg )
		{
			if ( msg.GetType() == MessageType::NewGame )
			{
				AddGameLine( msg.GetIPAdress(), msg.GetPort() );
				GameInfo game;
				game.Set( msg.GetIPAdress(), msg.GetPort() );
				game.SetGameID( gameCount );
				gameList.push_back( game );
				++gameCount;
			}
			else if ( msg.GetType() == MessageType::EndGame )
			{

				std::cout << "Delete message received for : " << msg.GetTypeAsString() << std::endl;
				std::string deleteIP  = msg.GetIPAdress();
				int32_t deletePort  = msg.GetPort();

				int32_t deletedGames = 0;

				for ( int32_t i = 0; i < gameList.size() ; ++i )
				{
					if ( gameList[i].GetIP() == deleteIP && gameList[i].GetPort() == deletePort )
					{
						++deletedGames;

						gameList.erase( gameList.begin() + i );
						rectsGameLine.erase( rectsGameLine.begin() + i );
						texturesGameLine.erase( texturesGameLine.begin() + i );
					}
				}

				if ( deletedGames == 0 )
				{
					std::cout << "Server.cpp@" << __LINE__
						<< " no games deleted for Game Info : "
						<< msg.GetIPAdress() << " : "
						<< msg.GetPort() << std::endl;
				}
				else if ( deletedGames > 1 )
				{
					std::cout << "Server.cpp@" << __LINE__
						<< " more than 1 games deleted for Game Info : "
						<< msg.GetIPAdress() << " : "
						<< msg.GetPort() << std::endl;
				}
				std::cout << "Game deleted!\n";

				RepositionGameLines();

			}
			else if ( msg.GetType() == MessageType::GetGameList )
			{
				SendGameList( connectionNo );
			}
		}
	}
	void SendGameList( int32_t connectionNo )
	{
		std::cout << "Get game list. From : " << connectionNo << std::endl;
		std::cout << "Size of game list : " << gameList.size() << std::endl;

		for ( const auto &p : gameList )
		{
			TCPMessage msg;
			msg.SetMessageType( MessageType::NewGame );
			msg.SetIPAdress( p.GetIP() );
			msg.SetPort( p.GetPort() );
			msg.SetObjectID( p.GetGameID() );
			std::stringstream ss;
			ss << msg;
			connection.Send( ss.str(), connectionNo );
			std::cout << "Sending : " << ss.str() << std::endl;
		}

		std::cout << "========= DONE SENDING ==========\n";
	}
	private:
	int32_t gameCount;

	std::vector< GameInfo > gameList;
	TCPConnection connection;

	// Header
	SDL_Rect rectHeader;
	SDL_Texture* textureHeader;
	TTF_Font* fontHeader;

	// Sub Header
	SDL_Rect rectSubHeader;
	SDL_Texture* textureSubHeader;
	TTF_Font* fontSubHeader;

	// Game Line
	TTF_Font* fontGameLine;
	std::vector< SDL_Rect > rectsGameLine;
	std::vector< SDL_Texture* > texturesGameLine;
	int32_t lastHeight;

	// General
	SDL_Rect screenRect;
	SDL_Window* window;
	SDL_Renderer* renderer;
};
