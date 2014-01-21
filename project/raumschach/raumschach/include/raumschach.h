#ifndef __RAUMSCHACH_H__
#define __RAUMSCHACH_H__

#include "configuration.h"
#include "piece.h"
#include "charstring.h"
#include "board.h"

class Player;
class Render;
class GraphicBoard;
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

	// The idle draw of the board and the panel
	void IdleDraw();

	// Mouse clicks handler
	void MouseClick(SysConfig::MouseButton button, int x, int y);

	// Registers the made move (updates game state, board, current player, etc)
	void RegisterMove();

	// Undo the last made move
	void UndoMove();

	// Call the current player GetMove(...) function
	void MakePlayerMove();

	// Outputs the provided message to the graphic panel
	void PostMessage(const CharString& message) const;

	// Getter for the exit status
	bool GetExitStatus() const;
	// Setter for the exit status
	void SetExitStatus(bool flag);

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

	DynamicStack<MadeMove> moveStack;

	bool gameEnded; // this is true if a checkmate or stalemate occured
	bool triedMove; // if the current player already tried a move (it is valid only for ai players)
	bool exitStatus; // true if the raumschach has to close

	Piece selectedPiece;
	BitBoard selectedPieceMoves;
	Config::PlayerColour currentPlayer;

	CharString playerNames[Config::PCOLOUR_COUNT];

	Player * players[Config::PCOLOUR_COUNT];
};

#endif // __RAUMSCHACH_H__