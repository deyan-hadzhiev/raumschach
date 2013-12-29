#include "utils.h"
#include "configuration.h"

ChessVector::ChessVector()
	: x(0), y(0), z(0)
{}

ChessVector::ChessVector(const coord coordArray[])
	: x(coordArray[0]), y(coordArray[1]), z(coordArray[2])
{}

ChessVector::ChessVector(coord x, coord y, coord z)
	: x(x), y(y), z(z)
{}

ChessVector::ChessVector( const ChessVector& copy)
	: x(copy.x), y(copy.y), z(copy.z)
{}

ChessVector::ChessVector(coord singleCoord)
{
	z = singleCoord / (Config::BOARD_SIDE * Config::BOARD_SIDE);
	y = (singleCoord % (Config::BOARD_SIDE * Config::BOARD_SIDE)) / Config::BOARD_SIDE;
	x = singleCoord % Config::BOARD_SIDE;
}

ChessVector& ChessVector::operator=(const ChessVector& assign)
{
	x = assign.x;
	y = assign.y;
	z = assign.z;
	return *this;
}

ChessVector& ChessVector::operator+=(ChessVector rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

ChessVector& ChessVector::operator-=(ChessVector rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

ChessVector& ChessVector::operator*=(coord scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}

const ChessVector operator+(ChessVector lhs, ChessVector rhs)
{
	return ChessVector(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

const ChessVector operator-(ChessVector lhs, ChessVector rhs)
{
	return ChessVector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

const ChessVector operator*(ChessVector lhs, coord scale)
{
	return ChessVector(lhs.x * scale, lhs.y * scale, lhs.z * scale);
}

const ChessVector operator*(coord scale, ChessVector rhs)
{
	return ChessVector(rhs.x * scale, rhs.y * scale, rhs.z * scale);
}

bool operator==(ChessVector lhs, ChessVector rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

bool operator!=(ChessVector lhs, ChessVector rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

ChessVector& ChessVector::operator-()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

coord ChessVector::GetVectorCoord() const
{
	using Config::BOARD_SIDE;
	return z * BOARD_SIDE * BOARD_SIDE + y * BOARD_SIDE + x;
}

BitBoard::BitBoard()
{
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		bits[i] = 0UL;
	}
}

BitBoard::BitBoard(const unsigned long long sBits[])
{
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		bits[i] = sBits[i];
	}
}

BitBoard::BitBoard(const BitBoard& copy)
{
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		bits[i] = copy.bits[i];
	}
}

BitBoard& BitBoard::operator=(const BitBoard& assign)
{
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		bits[i] = assign.bits[i];
	}
	return *this;
}

BitBoard& BitBoard::operator~()
{
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		bits[i] = ~ bits[i];
	}
	return *this;
}

BitBoard& BitBoard::operator&=(const BitBoard& rhs)
{
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		bits[i] &= rhs.bits[i];
	}
	return *this;
}

BitBoard& BitBoard::operator|=(const BitBoard& rhs)
{
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		bits[i] |= rhs.bits[i];
	}
	return *this;
}

BitBoard& BitBoard::operator^=(const BitBoard& rhs)
{
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		bits[i] ^= rhs.bits[i];
	}
	return *this;
}

const BitBoard operator&(const BitBoard& lhs, const BitBoard& rhs)
{
	BitBoard result;
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		result.bits[i] = lhs.bits[i] & rhs.bits[i];
	}
	return result;
}

const BitBoard operator|(const BitBoard& lhs, const BitBoard& rhs)
{
	BitBoard result;
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		result.bits[i] = lhs.bits[i] | rhs.bits[i];
	}
	return result;
}

const BitBoard operator^(const BitBoard& lhs, const BitBoard& rhs)
{
	BitBoard result;
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		result.bits[i] = lhs.bits[i] ^ rhs.bits[i];
	}
	return result;
}

bool BitBoard::operator==(const BitBoard& rhs) const
{
	bool equal = true;
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		equal &= (bits[i] == rhs.bits[i]);
	}
	return equal;
}

bool BitBoard::operator!=(const BitBoard& rhs) const
{
	return ! (*(this) == rhs);
}

BitBoard::operator bool() const
{
	bool nonZero = false;
	for(char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		nonZero |= (bits[i] != 0UL);
	}
	return nonZero;
}

const coord BitBoard::SIZE = sizeof(unsigned long long) * 8;

unsigned long long BitBoard::GetBits( coord offset, unsigned long long mask) const
{
	char bitn = offset / SIZE;
	char bitOffset = offset % SIZE;
	unsigned long long srcBitMask = 0xffffffffffffffff << bitOffset;
	unsigned long long srcBits = (bits[bitn] << bitOffset) & srcBitMask;
	if( bitn < Config::BITBOARD_SIZE)
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
	if( bitn < Config::BITBOARD_SIZE)
	{
		destBitMask = mask << (SIZE - bitOffset);
		destBits = srcBits << (SIZE - bitOffset);
		bits[bitn + 1] = (bits[bitn + 1] & ~ destBitMask) | destBits & destBitMask;
	}
}

coord BitBoard::GetBitCount() const
{
	coord count = 0;
	for( char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		count += BitCount( bits[i]);
	}
	return count;
}