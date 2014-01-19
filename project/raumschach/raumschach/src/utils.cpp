#include "utils.h"
#include "configuration.h"

const coord BitBoard::SIZE = sizeof(unsigned long long) * 8;

unsigned long long BitBoard::GetBits( coord offset, unsigned long long mask) const
{
	char bitn = offset / SIZE;
	char bitOffset = offset % SIZE;
	unsigned long long srcBitMask = 0xffffffffffffffff << bitOffset;
	unsigned long long srcBits = (bits[bitn] << bitOffset) & srcBitMask;
	if( bitn < Config::BITBOARD_SIZE - 1 && offset)
	{
		srcBits |= (bits[bitn + 1] >> (SIZE - bitOffset)) & ~ srcBitMask;
	}
	return srcBits & mask;
}

void BitBoard::SetBits( unsigned long long srcBits, coord offset, unsigned long long mask)
{
	char bitn = offset / SIZE;
	char bitOffset = offset % SIZE;
	unsigned long long destBitMask = mask >> bitOffset;
	unsigned long long destBits = srcBits >> bitOffset;
	bits[bitn] = (bits[bitn] & ~ destBitMask) | destBits & destBitMask;
	if( bitn < Config::BITBOARD_SIZE - 1 && offset)
	{
		destBitMask = mask << (SIZE - bitOffset);
		destBits = srcBits << (SIZE - bitOffset);
		bits[bitn + 1] = (bits[bitn + 1] & ~ destBitMask) | destBits & destBitMask;
	}
}

coord BitBoard::GetBitCount() const
{
	return BitCount(bits[0]) + BitCount(bits[1]);
}

void BitBoard::GetVectors(DynamicArray<ChessVector>& dest) const
{
	dest.Clear();
	coord count = GetBitCount();
	dest.Alloc(count);
	for(char i = 0; i < SIZE; ++i)
	{
		if(bits[0] & (Config::BITBOARD_BIT >> i))
			dest += ChessVector(i);

		if(bits[1] & (Config::BITBOARD_BIT >> i))
			dest += ChessVector(SIZE + i);
	}
}

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
				line[column] = ((bit & Config::BITBOARD_BIT) != 0UL ? '1' : '0');
			}
		}
		line[lastChar] = '\0';
		printf("%s\n", line);
	}
	printf("\n");
}
#endif // _DEBUG