#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "configuration.h"
#include "charstring.h"
#include "utils.h"
#include "render.h"

class Raumschach;

class Button
{
public:
	Button(const CharString& lbl, Colour col, Rect position);
	virtual ~Button();

	virtual void SetLabel(const CharString& lbl);
	virtual const CharString& GetLabel() const;

	virtual void SetPosition(Rect position);
	virtual Rect GetPosition() const;

	virtual void SetColour(Colour col);
	virtual Colour GetColour() const;

	virtual bool NeedsRedraw() const;

	// this will be the derived buttons action handler that the raumschash will call to execute their actions
	virtual void Action(Raumschach * controller) = 0;

private:
	// disable copy and assign
	Button(const Button& copy);
	Button& operator=(const Button& assign);

	bool needsRedraw;
	Rect pos;
	CharString label;
	Colour colour;
};

// Loads the game board and state from a file
class BoardLoadButton : public Button
{
public:
	BoardLoadButton(const CharString& lbl, Colour col, Rect position);

	void Action(Raumschach * controller);
};

// Saves the game board and state to a file
class BoardSaveButton : public Button
{
public:
	BoardSaveButton(const CharString& lbl, Colour col, Rect position);

	void Action(Raumschach * controller);
};

// Resets the game board
class ResetButton : public Button
{
public:
	ResetButton(const CharString& lbl, Colour col, Rect position);

	void Action(Raumschach * controller);
};

#endif // __BUTTON_H__