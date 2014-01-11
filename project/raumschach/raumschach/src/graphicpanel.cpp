#include "configuration.h"
#include "graphicpanel.h"

TextRender::TextRender(const char * filename, Render * render)
	:
	render(render),
	charDimension()
{
	asciiChart = new Texture(filename);
	if(asciiChart && asciiChart->IsLoaded())
	{
		bool error = false;
		error |= ! asciiChart->SetPixelFormat(render->GetPixelFormat());
		asciiChart->InitTransparent(Colour(GraphicConfig::ASCII_CHART_TRANSPARENT_COLOUR[0]), Colour(GraphicConfig::ASCII_CHART_TRANSPARENT_COLOUR[1]));
		error |= ! asciiChart->RenderTexture(render);
		if(error) Error(SDL_GetError()).Post().Exit(SysConfig::EXIT_SDL_ERROR);

		charDimension.width = asciiChart->GetWidth() / GraphicConfig::ASCII_CHART_CHAR_WIDTH;
		charDimension.height = asciiChart->GetHeight() / GraphicConfig::ASCII_CHART_CHAR_HEIGHT;
	}

	for(int i = 0; i < COUNT_OF(cachedTextures); ++i)
	{
		cachedTextures[i] = nullptr;
	}
}

TextRender::~TextRender()
{
	delete asciiChart;
	for(int i = 0; i < COUNT_OF(cachedTextures); ++i)
	{
		delete cachedTextures[i];
		cachedTextures[i] = nullptr;
	}
}

void TextRender::RenderMessage(const CharString& message, Rect dest)
{
	int destX = dest.x;
	int destY = dest.y;
	for(int i = 0; i < message.Length(); ++i)
	{
		unsigned char destChar = (unsigned char) message[i];
		if(!cachedTextures[destChar])
		{
			Rect blitSrc = charDimension;
			blitSrc.x = (destChar % GraphicConfig::ASCII_CHART_COUNT_ROW) * charDimension.width;
			blitSrc.y = (destChar / GraphicConfig::ASCII_CHART_COUNT_COLUMN) * charDimension.height;
			cachedTextures[destChar] = new Texture(*asciiChart, blitSrc, render);
		}
		render->DrawTexture(cachedTextures[destChar], destX, destY);
		destX += GraphicConfig::ASCII_CHART_CHAR_RENDER_WIDTH[destChar];
	}
}

bool TextRender::IsLoaded() const
{
	return asciiChart->IsLoaded();
}

Rect TextRender::GetMessageSize(const CharString& message) const
{
	Rect retval;
	retval.height = GraphicConfig::ASCII_CHART_CHAR_HEIGHT;
	for(int i = 0; i < message.Length(); ++i)
	{
		unsigned char src = message[i];
		retval.width += GraphicConfig::ASCII_CHART_CHAR_RENDER_WIDTH[src];
	}
	return retval;
}

GraphicPanel::GraphicPanel(Render * render, Rect position)
	:
	render(render),
	pos(position),
	textRect(position.x + GraphicConfig::PANEL_PADDING, position.y + GraphicConfig::PANEL_PADDING, position.width - GraphicConfig::PANEL_PADDING * 2, GraphicConfig::ASCII_CHART_CHAR_HEIGHT),
	textRender(nullptr),
	buttons(10),
	fullRefresh(true)
{}

GraphicPanel::~GraphicPanel()
{
	delete textRender;
	textRender = nullptr;
	for(int i = 0; i < buttons.Count(); ++i)
	{
		delete buttons[i];
	}
}

void GraphicPanel::Initialize()
{
	backgroundColour = Colour(GraphicConfig::PANEL_BACKGROUND_COLOUR);

	textRender = new TextRender(GraphicConfig::ASCII_CHART_FILENAME, render);
	if(!textRender || !textRender->IsLoaded())
	{
		CharString errmsg = CharString("ERROR: Error loading ascii texture chart") + CharString(GraphicConfig::ASCII_CHART_FILENAME);
		Error(errmsg).Post().Exit(SysConfig::EXIT_PANEL_INIT_ERROR);
	}
}

void GraphicPanel::AddButton(Button * button)
{
	buttons += button;
}

void GraphicPanel::PostMessage(const CharString& message)
{
	// first draw background to clear the message area
	render->DrawRectangle(backgroundColour, textRect);
	// then render the message
	textRender->RenderMessage(message, textRect);
	render->EndDraw();
}

bool GraphicPanel::OnPanel(int x, int y) const
{
	return pos.Inside(x, y);
}

Button * GraphicPanel::GetButton(int x, int y) const
{
	Button * retval = nullptr;
	if(OnPanel(x, y))
	{
		int rx = x - pos.x;
		int ry = y - pos.y;
		bool found = false;
		for(int i = 0; i < buttons.Count() && !found; ++i)
		{
			if(buttons[i]->GetPosition().Inside(rx, ry))
			{
				retval = buttons[i];
				found = true;
			}
		}
	}
	return retval;
}

void GraphicPanel::DrawPanel()
{
	if(fullRefresh)
	{
		// draw the background
		render->DrawRectangle(backgroundColour, pos.x, pos.y, pos.width, pos.height);
	}

	//draw the buttons
	for(int i = 0; i < buttons.Count(); ++i)
	{
		if(fullRefresh || buttons[i]->NeedsRedraw())
		{
			Rect labelPos = textRender->GetMessageSize(buttons[i]->GetLabel());
			Rect buttonPos = buttons[i]->GetPosition();
			buttonPos.x += pos.x;
			buttonPos.y += pos.y;

			labelPos.x = buttonPos.x + (buttonPos.width - labelPos.width) / 2;
			labelPos.y = buttonPos.y + (buttonPos.height - labelPos.height) / 2;

			render->DrawRectangle(buttons[i]->GetColour(), buttonPos);
			textRender->RenderMessage(buttons[i]->GetLabel(), labelPos);
		}
	}

	fullRefresh = false;

	render->EndDraw();
}

void GraphicPanel::Refresh(bool flag)
{
	fullRefresh = flag;
}