#include "raumschach.h"
#include "error.h"
#include "configuration.h"
#include "constants.h"
#include "render.h"
#include "graphicboard.h"
#include "board.h"

Raumschach::Raumschach()
	:
	render(nullptr),
	graphicBoard(nullptr),
	board(nullptr),
	movePool(nullptr),
	tileState(nullptr),
	whitePlayer(nullptr),
	blackPlayer(nullptr),
	selectedPiece(),
	selectedPieceMoves(),
	currentPlayer(Config::WHITE)
{}

Raumschach::~Raumschach()
{
	delete tileState;
	tileState = nullptr;
	delete board;
	board = nullptr;
	delete movePool;
	movePool = nullptr;
	delete graphicBoard;
	graphicBoard = nullptr;
	delete render;
	render = nullptr;
}

void Raumschach::Initialize(Player * white, Player * black)
{
	if(white)
	{
		whitePlayer = white;
	}
	else
	{
		Error("ERROR: No white player specified").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}

	if(black)
	{
		blackPlayer	= black;
	}
	else
	{
		Error("ERROR: No black player specified").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}

	render = new Render(SysConfig::SCREEN_WIDTH, SysConfig::SCREEN_HEIGHT);
	if(! render)
	{
		Error("ERROR: Failed to initialize graphic render").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}

	graphicBoard = new GraphicBoard(render);
	if(! graphicBoard)
	{
		Error("ERROR: Failed to initalize graphic board").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}
	graphicBoard->Initialize();

	movePool = new BitBoardMovePool();
	if(! movePool)
	{
		Error("ERROR: Failed to initialize move pool").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}
	movePool->Initalize();

	board = new Board( DynamicArray< Piece >(Const::INITIAL_PIECES, COUNT_OF(Const::INITIAL_PIECES)));
	if(! board)
	{
		Error("ERROR: Failed to initialize the main board").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}

	tileState = new BoardTileState();
	if(! tileState)
	{
		Error("ERROR: Failed to initialize the tile state board").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}
}

void Raumschach::Start()
{
	render->StartEventLoop(this);
}

void Raumschach::IdleDrawBoard()
{
	graphicBoard->DrawBoard(board, tileState);
	tileState->SetChanged(false);
}

void Raumschach::MouseClick(SysConfig::MouseButton button, int x, int y)
{
	if(button == SysConfig::LEFT && GraphicBoard::OnBoard(x, y))
	{
		ChessVector clickedPos = GraphicBoard::ScreenToBoard(Rect(x, y));
		Piece clickedPiece = board->GetPiece(clickedPos);
		if(clickedPiece.GetType() != Config::NO_TYPE && clickedPiece.GetColour() == currentPlayer)
		{
			selectedPiece = clickedPiece;
			tileState->SetBoardTileState(Config::TILE_NORMAL, Config::BITBOARD_FULL_BOARD);
			BitBoard opposingPieces = board->GetPiecesBitBoard( Config::GetOppositePlayer(selectedPiece.GetColour()));
			tileState->SetBoardTileState(Config::TILE_SELECTED, clickedPos);
			selectedPieceMoves = movePool->GetPieceMoves(selectedPiece, board->GetPiecesBitBoard(selectedPiece.GetColour()), opposingPieces);
			tileState->SetBoardTileState(Config::TILE_MOVEABLE, selectedPieceMoves);
			tileState->SetBoardTileState(Config::TILE_CAPTUREABLE, selectedPieceMoves & opposingPieces);
		}
		else if(selectedPiece.GetType() != Config::NO_TYPE && board->MovePiece(selectedPiece, clickedPos, selectedPieceMoves))
		{
			selectedPiece = Piece();
			tileState->SetBoardTileState(Config::TILE_NORMAL, Config::BITBOARD_FULL_BOARD);
			currentPlayer = Config::GetOppositePlayer(currentPlayer);
		}
	}
	else
	{
		selectedPiece = Piece();
		tileState->SetBoardTileState(Config::TILE_NORMAL, Config::BITBOARD_FULL_BOARD);
	}
}