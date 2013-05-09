struct GamePiece
{
	GamePiece()
		:	posX( 0 )
		,	posY( 0 )
		,	textureType( 0 )
	{

	}

	~GamePiece()
	{

	}

	int posX;
	int posY;

	int textureType;

private:
	GamePiece( const GamePiece &gamePiece );
	GamePiece& operator=( const GamePiece &gamePiece );
};
