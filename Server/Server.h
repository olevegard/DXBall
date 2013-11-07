#include <iostream>
#include <sstream>

#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "TCPConnectionServer.h"

class Server
{
	public:

	Server()
		:	screenRect( { 0, 0, 500, 800 } )
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
		fontHeader = TTF_OpenFont( "../media/fonts/sketchy.ttf", 50  );

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

		if ( connection.StartServer() )
		{
			std::string ipConnected;
			uint32_t port = 0;
			connection.GetServerInfo( ipConnected, port );

			std::cout << "Port : " << port << std::endl;
			AddGameLine( ipConnected, port );
		}
		else
			return false;

		if ( connection.StartServer() )
		{
			std::string ipConnected;
			uint32_t port = 0;
			connection.GetServerInfo( ipConnected, port );

			AddGameLine( ipConnected, port );
		}
		else
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
				if ( event.type == SDL_QUIT )
					run = false;
			}

			SDL_RenderCopy( renderer, textureHeader, nullptr, &rectHeader);
			SDL_RenderCopy( renderer, textureSubHeader, nullptr, &rectSubHeader);
			for ( int i = 0; i < texturesGameLine.size() ; ++i )
				SDL_RenderCopy( renderer, texturesGameLine[i] , nullptr, &rectsGameLine[i]);

			SDL_RenderPresent( renderer );
		}
	}
	private:
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
