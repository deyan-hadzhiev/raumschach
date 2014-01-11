#ifndef __GRAPHICPANEL_H__
#define __GRAPHICPANEL_H__

#include "configuration.h"
#include "utils.h"
#include "error.h"
#include "render.h"
#include "button.h"

class TextRender
{
public:
	TextRender(const char * filename, Render * render);
	~TextRender();

	// Renders the message to the specified destination
	void RenderMessage(const CharString& message, Rect dest);
	bool IsLoaded() const;

	// Returns the calculated message size
	Rect GetMessageSize(const CharString& message) const;

private:
	// disable copy and assignment
	TextRender(const TextRender& copy);
	TextRender& operator=(const TextRender& assign);

	Rect charDimension;
	Texture * asciiChart;
	Texture * cachedTextures[GraphicConfig::ASCII_CHART_COUNT];

	// not owned objects -> no destruction
	Render * render;
};

class GraphicPanel
{
public:
	GraphicPanel(Render * render, Rect position = Rect(GraphicConfig::PANEL_POSITION_X, GraphicConfig::PANEL_POSITION_Y, GraphicConfig::PANEL_SIZE_WIDTH, GraphicConfig::PANEL_SIZE_HEIGHT));
	~GraphicPanel();

	void Initialize();

	// takes possesion of the button (deletes it at destruction)
	void AddButton(Button * button);

	// posts a message throught the text render
	void PostMessage(const CharString& message);

	bool OnPanel(int x, int y) const;

	// returns the button handle of the button on the designated coordinates
	// NOTE: x and y are screen coordinates
	Button * GetButton(int x, int y) const;

	// drawing funciton - it may be called fairly rare
	void DrawPanel();

	void Refresh(bool flag = true);

private:
	// disable copy and assignment
	GraphicPanel(const GraphicPanel& copy);
	GraphicPanel& operator=(const GraphicPanel& assign);

	Rect pos;
	Rect textRect;
	TextRender * textRender;
	DynamicArray<Button*> buttons;

	Colour backgroundColour;

	bool fullRefresh;

	// not owned objects
	Render * render;
};

#endif // __GRAPHICPANEL_H__