#include "raumschach.h"
#include "error.h"
#include "configuration.h"
#include "constants.h"
#include "render.h"
#include "graphicboard.h"
#include "board.h"
#include "random_generator.h"
#include "graphicpanel.h"
#include <time.h>

Raumschach::Raumschach()
	:
	render(nullptr),
	graphicBoard(nullptr),
	graphicPanel(nullptr),
	board(nullptr),
	movePool(nullptr),
	tileState(nullptr),
	randGen(nullptr),
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
	delete graphicPanel;
	graphicPanel = nullptr;
	delete render;
	render = nullptr;
	delete randGen;
	randGen = nullptr;
}

void Raumschach::Initialize(Player * white, Player * black)
{
	if(white)
	{
		whitePlayer = white;
		playerNames[Config::WHITE] = "White Player";
	}
	else
	{
		Error("ERROR: No white player specified").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}

	if(black)
	{
		blackPlayer	= black;
		playerNames[Config::BLACK] = "Black Player";
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
	render->BeginDraw();

	graphicBoard = new GraphicBoard(render);
	if(! graphicBoard)
	{
		Error("ERROR: Failed to initalize graphic board").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}
	graphicBoard->Initialize();

	graphicPanel = new GraphicPanel(render);
	if(! graphicPanel)
	{
		Error("ERROR: Failed to initialize graphic panel").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}
	graphicPanel->Initialize();
	InitButtons();

	movePool = new BitBoardMovePool();
	if(! movePool)
	{
		Error("ERROR: Failed to initialize move pool").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}
	movePool->Initalize();

	board = new Board( DynamicArray< Piece >(Const::INITIAL_PIECES, COUNT_OF(Const::INITIAL_PIECES)), movePool);
	if(! board)
	{
		Error("ERROR: Failed to initialize the main board").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}

	tileState = new BoardTileState();
	if(! tileState)
	{
		Error("ERROR: Failed to initialize the tile state board").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}

	randGen = new RandomGenerator(time(NULL));
	if(! randGen)
	{
		Error("ERROR: Failed to initialize the random generator").Post().Exit(SysConfig::EXIT_CHESS_INIT_ERROR);
	}
}

void Raumschach::Start()
{
	IdleDrawBoard();
	PostMessage("Game started. White move first");
	render->StartEventLoop(this);
}

void Raumschach::IdleDrawBoard()
{
	graphicBoard->DrawBoard(board, tileState);
	tileState->SetChanged(false);
	graphicPanel->DrawPanel();
}

void Raumschach::MouseClick(SysConfig::MouseButton button, int x, int y)
{
	if(button == SysConfig::LEFT && graphicBoard->OnBoard(x, y))
	{
		ChessVector clickedPos = graphicBoard->ScreenToBoard(Rect(x, y));
		Piece clickedPiece = board->GetPiece(clickedPos);
		if(clickedPiece.GetType() != Config::NO_TYPE && clickedPiece.GetColour() == currentPlayer)
		{
			selectedPiece = clickedPiece;
			tileState->SetBoardTileState(Config::TILE_NORMAL, Config::BITBOARD_FULL_BOARD);
			BitBoard opposingPieces = board->GetPiecesBitBoard( Config::GetOppositePlayer(selectedPiece.GetColour()));
			tileState->SetBoardTileState(Config::TILE_SELECTED, clickedPos);
			selectedPieceMoves = movePool->GetPieceMoves(selectedPiece, board->GetPiecesBitBoard(selectedPiece.GetColour()), opposingPieces, board);
			tileState->SetBoardTileState(Config::TILE_MOVEABLE, selectedPieceMoves);
			tileState->SetBoardTileState(Config::TILE_CAPTUREABLE, selectedPieceMoves & opposingPieces);
			if(board->KingInCheck(currentPlayer))
			{
				Piece playerKing = board->GetKing(currentPlayer);
				tileState->SetBoardTileState(Config::TILE_CAPTUREABLE, playerKing.GetPositionVector());
			}
		}
		else if(selectedPiece.GetType() != Config::NO_TYPE)
		{
			// if this is a valid move make it
			if(board->ValidMove(selectedPiece, clickedPos, selectedPieceMoves))
			{
				board->MovePiece(selectedPiece, clickedPos, selectedPieceMoves, true);
				Config::PlayerColour oppositePlayerColour = Config::GetOppositePlayer(currentPlayer);
				selectedPiece = Piece();
				tileState->SetBoardTileState(Config::TILE_NORMAL, Config::BITBOARD_FULL_BOARD);
				if(board->KingInCheck(oppositePlayerColour))
				{
					Piece oppositeKing = board->GetKing(oppositePlayerColour);
					tileState->SetBoardTileState(Config::TILE_CAPTUREABLE, oppositeKing.GetPositionVector());
				}

				Config::KingState kingState = board->KingCheckState(oppositePlayerColour);

				CharString gameStateString;
				switch (kingState)
				{
				case Config::NORMAL:
					gameStateString = playerNames[oppositePlayerColour] + " is next to move";
					break;
				case Config::CHECK:
					gameStateString = playerNames[oppositePlayerColour] + " is under check!";
					break;
				case Config::CHECKMATE:
					gameStateString = "Checkmate! " + playerNames[currentPlayer] + " wins!";
					break;
				case Config::STALEMATE:
					gameStateString = "Stalemate! End of game";
					break;
				case Config::NO_KING:
					break;
				default:
					break;
				}

				PostMessage(gameStateString);

				currentPlayer = oppositePlayerColour;
			}
		}
	}
	else
	{
		bool deselect = false;
		if(button == SysConfig::LEFT && graphicPanel->OnPanel(x, y))
		{
			Button * clickedButton = graphicPanel->GetButton(x, y);
			if(clickedButton)
			{
				clickedButton->Action(this);
			}
			else
			{
				deselect = true;
			}
		}

		if(deselect)
		{
			selectedPiece = Piece();
			tileState->SetBoardTileState(Config::TILE_NORMAL, Config::BITBOARD_FULL_BOARD);
			if(board->KingInCheck(currentPlayer))
			{
				Piece oppositeKing = board->GetKing(currentPlayer);
				tileState->SetBoardTileState(Config::TILE_CAPTUREABLE, oppositeKing.GetPositionVector());
			}
		}
	}
}

void Raumschach::PostMessage(const CharString& message) const
{
	graphicPanel->PostMessage(message);
}

const Board * Raumschach::GetBoard() const
{
	return board;
}

unsigned short Raumschach::GetGameState() const
{
	unsigned short flags = 0;
	flags |= ((unsigned short) currentPlayer) << Config::BOARD_STATE_TURN_COLOUR_LSHIFT;

	return flags;
}

void Raumschach::InitializeBoard(const DynamicArray<Piece>& pieces, unsigned short flags)
{
	delete board;
	board = nullptr;
	board = new Board(pieces, movePool);

	// now initialize flags
	currentPlayer = (Config::PlayerColour) ((flags >> Config::BOARD_STATE_TURN_COLOUR_LSHIFT) & 0x0003);

	selectedPiece = Piece();
	selectedPieceMoves = BitBoard();
	tileState->SetBoardTileState(Config::TILE_NORMAL, Config::BITBOARD_FULL_BOARD);
	if(board->KingCheckState(currentPlayer))
	{
		Piece oppositeKing = board->GetKing(currentPlayer);
		tileState->SetBoardTileState(Config::TILE_CAPTUREABLE, oppositeKing.GetPositionVector());
	}

	// now refresh fully the gui
	graphicBoard->Refresh();
	graphicBoard->DrawBoard(board, tileState);
	graphicPanel->Refresh();
	graphicPanel->DrawPanel();
}

void Raumschach::InitButtons()
{
	graphicPanel->AddButton(new ResetButton("Reset game", Colour(GraphicConfig::BUTTON_COLOUR) ,Rect(GraphicConfig::POSITION_BUTTON_RESET)));
	graphicPanel->AddButton(new BoardSaveButton("Save game", Colour(GraphicConfig::BUTTON_COLOUR) ,Rect(GraphicConfig::POSITION_BUTTON_SAVE)));
	graphicPanel->AddButton(new BoardLoadButton("Load game", Colour(GraphicConfig::BUTTON_COLOUR) ,Rect(GraphicConfig::POSITION_BUTTON_LOAD)));
}