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
	Server();

	bool InitSDL();
	bool CreateWindow();
	bool CreateRenderer();

	bool Init();
	bool InitNet( std::string ip, uint16_t port );
	bool InitTTF();
	bool InitFonts();
	bool InitHeader();
	bool InitSubHeader();
	bool InitPlayerCount( int32_t playerCount = 0 );

	void AddGameLine( const std::string &IP, int32_t port );
	void RepositionGameLines();

	void Run();
	void UpdateNetwork( int connectionNo );
	void SendGameList( int32_t connectionNo );

	private:
	int32_t gameCount;

	void RecieveGameJoinedMessage(const TCPMessage &msg );

	void SendMessageToAll( const TCPMessage &msg );

	std::vector< GameInfo > gameList;
	TCPConnectionServer connection;

	// Header
	SDL_Rect rectHeader;
	SDL_Texture* textureHeader;
	TTF_Font* fontHeader;

	// Sub Header
	SDL_Rect rectSubHeader;
	SDL_Texture* textureSubHeader;
	TTF_Font* fontSubHeader;

	// Playe count
	SDL_Rect rectPlayerCount;
	SDL_Texture* texturePlayerCount;
	TTF_Font* fontPlayerCount;

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
