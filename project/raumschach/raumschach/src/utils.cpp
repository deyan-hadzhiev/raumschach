#include "utils.h"
#include "configuration.h"

#ifdef _DEBUG
#include <stdio.h>
void BitBoard::PrintBitBoard(const char* str) const
{
	if(str) printf("%s\n", str);
	char line[(Config::BOARD_SIDE + 1) * Config::BOARD_SIDE + 1] = "";
	const int lastChar = (Config::BOARD_SIDE + 1) * Config::BOARD_SIDE;
	line[lastChar] = '\0';
	const int columnSize = Config::BOARD_SIDE + 1;
	const int levelSize = Config::BOARD_SIDE * Config::BOARD_SIDE;
	for(int l = 0; l < Config::BOARD_SIDE; ++l)
	{
		for(int column = 0; column < lastChar; ++column)
		{
			if((column + 1) % columnSize == 0)
			{
				line[column] = ' ';
			}
			else
			{
				int level = column / columnSize;
				int ch = (column - level) % Config::BOARD_SIDE;
				int offset = level * levelSize + l * Config::BOARD_SIDE + ch;
				unsigned long long bit = GetBits(offset);
				line[column] = ((bit & Config::BITBOARD_BIT) != 0ULL ? '1' : '0');
			}
		}
		line[lastChar] = '\0';
		printf("%s\n", line);
	}
	printf("\n");
}
#endif // _DEBUG