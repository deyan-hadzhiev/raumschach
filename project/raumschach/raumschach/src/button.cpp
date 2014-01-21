#include "configuration.h"
#include "button.h"
#include "raumschach.h"
#include "board.h"
#include <stdio.h>
#include "constants.h"

Button::Button(const CharString& lbl, Colour col, Rect position)
	:
	label(lbl),
	colour(col),
	pos(position),
	needsRedraw(true)
{}

Button::~Button()
{}

void Button::SetLabel(const CharString& lbl)
{
	needsRedraw = true;
	label = lbl;
}

const CharString& Button::GetLabel() const
{
	return label;
}

void Button::SetPosition(Rect position)
{
	needsRedraw = true;
	pos = position;
}

Rect Button::GetPosition() const
{
	return pos;
}

void Button::SetColour(Colour col)
{
	needsRedraw = true;
	colour = col;
}

Colour Button::GetColour() const
{
	return colour;
}

bool Button::NeedsRedraw() const
{
	return needsRedraw;
}

BoardLoadButton::BoardLoadButton(const CharString& lbl, Colour col, Rect position)
	: Button(lbl, col, position)
{}

void BoardLoadButton::Action(Raumschach * controller)
{
	FILE * input = fopen(Config::BOARD_SAVE_FILENAME, "rb");

	if(input)
	{
		unsigned short flags = 0;
		fread(&flags, sizeof(flags), 1, input);

		Piece inputPiece;
		DynamicArray<Piece> pieces;
		while(fread(&inputPiece, sizeof(Piece), 1, input))
		{
			pieces += inputPiece;
		}

		fclose(input);

		controller->InitializeBoard(pieces, flags);
		controller->InitializeNewPlayer(Config::PLAYER_HUMAN, Config::WHITE);
		controller->InitializeNewPlayer(Config::PLAYER_HUMAN, Config::BLACK);

		CharString msg = CharString("Successfully loaded the game from file : ") + CharString(Config::BOARD_SAVE_FILENAME);
		controller->PostMessage(msg);
	}
	else
	{
		controller->PostMessage("Could not load game from file");
	}
}

BoardSaveButton::BoardSaveButton(const CharString& lbl, Colour col, Rect position)
	: Button(lbl, col, position)
{}

void BoardSaveButton::Action(Raumschach * controller)
{
	FILE * output = fopen(Config::BOARD_SAVE_FILENAME, "wb");

	bool saved = false;
	if(output)
	{
		unsigned short flags = controller->GetGameState();

		fwrite(&flags, sizeof(flags), 1, output);

		const Board * board = controller->GetBoard();
		DynamicArray<Piece> pieces;
		board->GetPiecesArray(Config::BOTH_COLOURS, pieces);

		for(int i = 0; i < pieces.Count(); ++i)
		{
			fwrite(&pieces[i], sizeof(Piece), 1, output);
		}

		fclose(output);
		saved = true;
	}

	if(saved)
	{
		CharString message = CharString("Successfully created save file : ") + CharString(Config::BOARD_SAVE_FILENAME);
		controller->PostMessage(message);
	}
	else
	{
		controller->PostMessage("Could not save game!");
	}
}

ResetButton::ResetButton(const CharString& lbl, Colour col, Rect position)
	: Button(lbl, col, position)
{}

void ResetButton::Action(Raumschach * controller)
{
	DynamicArray< Piece > newBoard(Const::INITIAL_PIECES, COUNT_OF(Const::INITIAL_PIECES));
	unsigned short flags = 0;

	controller->InitializeBoard(newBoard, flags);
	controller->InitializeNewPlayer(Config::PLAYER_HUMAN, Config::WHITE);
	controller->InitializeNewPlayer(Config::PLAYER_HUMAN, Config::BLACK);

	controller->PostMessage("Reset the game. White start!");
}

NewPlayerButton::NewPlayerButton(const CharString& lbl, Colour col, Rect position, Config::PlayerType type, Config::PlayerColour colour)
	:
	Button(lbl, col, position),
	type(type),
	colour(colour)
{}

void NewPlayerButton::Action(Raumschach * controller)
{
	controller->InitializeNewPlayer(type, colour);
}

ExitButton::ExitButton(const CharString& lbl, Colour col, Rect position)
	:
	Button(lbl, col, position)
{}

void ExitButton::Action(Raumschach * controller)
{
	controller->SetExitStatus(true);
}

UndoButton::UndoButton(const CharString& lbl, Colour col, Rect position)
	:
	Button(lbl, col, position)
{}

void UndoButton::Action(Raumschach * controller)
{
	controller->UndoMove();
}
