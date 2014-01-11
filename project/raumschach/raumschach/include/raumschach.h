#ifndef __RAUMSCHACH_H__
#define __RAUMSCHACH_H__

#include "configuration.h"
#include "piece.h"

class Player;
class Render;
class Board;
class GraphicBoard;
class BitBoardMovePool;
class BoardTileState;
class RandomGenerator;
class GraphicPanel;
class CharString;

class Raumschach
{
public:
	Raumschach();
	~Raumschach();

	// Initializes everything the chess needs to function
	// this includes boards, move pools, renderers, textures, etc.
	void Initialize(Player * white, Player * black);

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

	Piece selectedPiece;
	BitBoard selectedPieceMoves;
	Config::PlayerColour currentPlayer;

	// not owned, so no destruction
	Player * whitePlayer;
	Player * blackPlayer;
};

#endif // __RAUMSCHACH_H__