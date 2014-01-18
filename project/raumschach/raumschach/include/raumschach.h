#ifndef __RAUMSCHACH_H__
#define __RAUMSCHACH_H__

#include "configuration.h"
#include "piece.h"
#include "charstring.h"

class Player;
class Render;
class Board;
class GraphicBoard;
class BitBoardMovePool;
class BoardTileState;
class RandomGenerator;
class GraphicPanel;

class Raumschach
{
public:
	Raumschach();
	~Raumschach();

	// Initializes everything the chess needs to function
	// this includes boards, move pools, renderers, textures, etc.
	void Initialize();

	// Main loop
	void Start();

	void IdleDrawBoard();
	// Mouse clicks handler
	void MouseClick(SysConfig::MouseButton button, int x, int y);

	// Outputs the provided message to the graphic panel
	void PostMessage(const CharString& message) const;

	// functions used by the buttons to define the game state

	// Returns the current board
	const Board * GetBoard() const;
	// Returns formatted flag, containing various game states, needed for saving and loading
	unsigned short GetGameState() const;
	// Initialize the board and state with the given pieces and flags
	// NOTE: this includes refresh of gui and all states
	void InitializeBoard(const DynamicArray<Piece>& pieces, unsigned short flags);
	// Initialize a new player of the specified colour (deletes the current player)
	void InitializeNewPlayer(Config::PlayerType type, Config::PlayerColour colour);

private:
	// helper function for button initialization
	void InitButtons();

	Render * render;
	GraphicBoard * graphicBoard;
	GraphicPanel * graphicPanel;
	Board * board;
	BitBoardMovePool * movePool;
	BoardTileState * tileState;
	RandomGenerator * randGen;
	bool gameEnded; // this is true if a checkmate or stalemate occured

	Piece selectedPiece;
	BitBoard selectedPieceMoves;
	Config::PlayerColour currentPlayer;

	CharString playerNames[Config::PCOLOUR_COUNT];

	Player * players[Config::PCOLOUR_COUNT];
};

#endif // __RAUMSCHACH_H__