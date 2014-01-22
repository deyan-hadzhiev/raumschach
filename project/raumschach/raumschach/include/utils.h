#ifndef __UTILS_H__
#define __UTILS_H__

#include "configuration.h"

#define	COUNT_OF(x) sizeof((x)) / sizeof((x[0]))

namespace Utils
{
	template<class Type>
	Type Min(Type a, Type b)
	{
		return (a < b ? a : b);
	}

	template<class Type>
	Type Max(Type a, Type b)
	{
		return (a > b ? a : b);
	}

	template<class Type>
	Type Abs(Type a)
	{
		return (a < 0 ? -a : a);
	}

	const unsigned char BITBOARD_PHYS_SIZE = sizeof(unsigned long long) * 8;

	const unsigned char BITBOARD_PHYS_SIZE_OFFSET = 6; // for division with right shifts
	const unsigned char BITBOARD_PHYS_SIZE_MOD_MASK = 0x40 - 1; // for modulo with bit operations
};

template< class Type >
class DynamicArray
{
public:
	DynamicArray(int arrSize = 20) : arr(nullptr), size(arrSize), count(0)
	{
		arr = new Type[size];
	}

	DynamicArray(const DynamicArray<Type>& copy) : arr(nullptr), size(copy.size), count(copy.count)
	{
		arr = new Type[size];
		for(int i = 0; i < count; ++i)
		{
			arr[i] = copy.arr[i];
		}
	}

	DynamicArray(const Type srcArr[], int srcSize) : arr(nullptr), size(srcSize), count(srcSize)
	{
		arr = new Type[srcSize];
		for(int i = 0; i < srcSize; ++i)
		{
			arr[i] = srcArr[i];
		}
	}

	virtual ~DynamicArray()
	{
		delete[] arr;
		arr = nullptr;
	}

	DynamicArray<Type>& operator=(const DynamicArray<Type>& assign)
	{
		if(this != &assign)
		{
			delete[] arr;
			arr = nullptr;
			size = assign.size;
			count = assign.count;
			arr = new Type[size];
			for(int i = 0; i < count; ++i)
			{
				arr[i] = assign.arr[i];
			}
		}
		return *this;
	}

	DynamicArray<Type>& Append(const DynamicArray<Type>& rhs)
	{
		if(count + rhs.count < size)
		{
			Realloc(count + rhs.count + 1); // add just one more element
		}

		for(int i = 0; i < rhs.count; ++i)
		{
			arr[count] = rhs.arr[i];
			++count;
		}
		return *this;
	}

	// adds an item to the end of the array ( rellocates current size * 2 if needed )
	DynamicArray<Type>& operator+=(Type item)
	{
		if(count >= size)
		{
			Realloc( size * 2);
		}
		arr[count] = item;
		count++;
		return *this;
	}

	// removes an item with given index by replacing it with the last element
	void RemoveItem( int index)
	{
		count--;
		arr[index] = arr[count];
	}

	// clear all elements ( not physically )
	void Clear()
	{
		count = 0;
	}

	// get the count of elements
	int Count() const { return count; }

	// element accessor
	Type& operator[](int index)
	{
		return arr[index];
	}

	// const element accessor
	const Type& operator[](int index) const
	{
		return arr[index];
	}

	// sorts the array in place using quicksort for the sorting
	// NOTE: the class of the array must have operator< predefined
	void Sort()
	{
		if(count > 1)
		{
			QuickSort(0, count - 1);
		}
	}

	// insertion sorts the array in place
	// NOTE: this should be faster for nearly sorted array than the quicksort
	void InsertionSort()
	{
		for(int i = 1; i < count; ++i)
		{
			Type x = arr[i];
			int j = i;
			while(j > 0 && x < arr[j - 1])
			{
				arr[j] = arr[j - 1];
				--j;
			}
			arr[j] = x;
		}
	}


	// if there isn't 'n' in size currently free, the array allocates them
	void Alloc(int n)
	{
		if(size - count < n)
		{
			Realloc(size + n);
		}
	}

protected:
	void Realloc(int newSize)
	{
		if( newSize > size)
		{
			size = newSize;
			Type* tmp = new Type[size];
			for(int i = 0; i < count; ++i)
			{
				tmp[i] = arr[i];
			}
			delete[] arr;
			arr = nullptr;
			arr = tmp;
		}
	}

	// quicksorts the array in place
	void QuickSort(int left, int right)
	{
		if(left < right)
		{
			int pivotIndex = (left + right) >> 1; // (left + right) / 2
			// get the new pivot index after partitioning
			pivotIndex = Partition(left, right, pivotIndex);

			// recursively quicksort the other two parts
			QuickSort(left, pivotIndex - 1);
			QuickSort(pivotIndex + 1, right);
		}
	}

	// used by the quicksort algorithm for partitioning
	int Partition(int left, int right, int pivotIndex)
	{
		Type pivotValue = arr[pivotIndex];
		arr[pivotIndex] = arr[right];
		arr[right] = pivotValue;
		int storeIndex = left;
		for(int i = left; i < right; ++i)
		{
			if(arr[i] < pivotValue)
			{
				Type tmp = arr[i];
				arr[i] = arr[storeIndex];
				arr[storeIndex] = tmp;
				++storeIndex;
			}
		}
		arr[right] = arr[storeIndex];
		arr[storeIndex] = pivotValue;
		return storeIndex;
	}

	Type* arr;
	int size;
	int count;
};

template<class Type>
class DynamicStack : protected DynamicArray<Type>
{
public:
	DynamicStack(int stackSize = 40) : DynamicArray<Type>(stackSize) {}

	DynamicStack(const DynamicArray<Type>& copy) : DynamicArray<Type>(copy) {}

	DynamicStack(const DynamicStack<Type>& copy) : DynamicArray<Type>(copy) {}

	DynamicStack<Type>& operator=(const DynamicStack<Type>& assing)
	{
		if(this != &assign)
		{
			delete[] arr;
			arr = nullptr;
			size = assign.size;
			count = assign.count;
			arr = new Type[size];
			for(int i = 0; i < count; ++i)
			{
				arr[i] = assign.arr[i];
			}
		}
		return *this;
	}

	void Push(Type element)
	{
		if(count >= size)
		{
			Realloc(size * 2);
		}
		arr[count] = element;
		count++;
	};

	Type Pop()
	{
		if(count)
		{
			count--;
			return arr[count];
		}
		return Type();
	}

	Type& Top()
	{
		if(count)
		{
			return arr[count - 1];
		}
	}

	const Type& Top() const
	{
		if(count)
		{
			return arr[count - 1];
		}
	}

	bool Empty() const
	{
		return count == 0;
	}

	void Clear()
	{
		count = 0;
	}
};

class ChessVector
{
public:
	coord x,y,z;

	ChessVector() : x(0), y(0), z(0) {}
	explicit ChessVector(const coord coordArray[]) : x(coordArray[0]), y(coordArray[1]), z(coordArray[2]) {}
	explicit ChessVector(coord x, coord y, coord z) : x(x), y(y), z(z) {}
	ChessVector(const ChessVector& copy) : x(copy.x), y(copy.y), z(copy.z) {}
	explicit ChessVector(coord singleCoord)
	{
		z = singleCoord / (Config::BOARD_SIDE * Config::BOARD_SIDE);
		y = (singleCoord % (Config::BOARD_SIDE * Config::BOARD_SIDE)) / Config::BOARD_SIDE;
		x = singleCoord % Config::BOARD_SIDE;
	}

	inline ChessVector& operator=(const ChessVector& assign)
	{
		x = assign.x;
		y = assign.y;
		z = assign.z;
		return *this;
	}

	inline ChessVector& operator+=(ChessVector rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	inline ChessVector& operator-=(ChessVector rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	inline ChessVector& operator*=( coord scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}

	inline friend const ChessVector operator+(ChessVector lhs, ChessVector rhs)
	{
		return ChessVector(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	inline friend const ChessVector operator-(ChessVector lhs, ChessVector rhs)
	{
		return ChessVector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	inline friend const ChessVector operator*(ChessVector lhs, coord scale)
	{
		return ChessVector(lhs.x * scale, lhs.y * scale, lhs.z * scale);
	}

	inline friend const ChessVector operator*(coord scale, ChessVector rhs)
	{
		return ChessVector(rhs.x * scale, rhs.y * scale, rhs.z * scale);
	}

	inline friend bool operator==(ChessVector lhs, ChessVector rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	inline friend bool operator!=(ChessVector lhs, ChessVector rhs)
	{
		return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
	}

	inline ChessVector& operator-()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	inline coord GetVectorCoord() const
	{
		return z * Config::BOARD_SIDE * Config::BOARD_SIDE + y * Config::BOARD_SIDE + x;
	}

	// Returns the Manhattan distance to the vector 'to'
	inline coord GetManhattanDistance(ChessVector to) const
	{
		return Utils::Abs(x - to.x) + Utils::Abs(y - to.y) + Utils::Abs(z - to.z);
	}
};

class Rect
{
public:
	int x, y, width, height;

	Rect() : x(0), y(0), width(0), height(0) {}
	Rect(int x, int y) : x(x), y(y), width(0), height(0) {}
	Rect(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}
	Rect(const Rect& copy) : x(copy.x), y(copy.y), width(copy.width), height(copy.height) {}
	Rect(const int vec[4]) : x(vec[0]), y(vec[1]), width(vec[2]), height(vec[3]) {}

	inline Rect& operator=(const Rect& assign)
	{
		x = assign.x;
		y = assign.y;
		width = assign.width;
		height = assign.height;
		return *this;
	}

	inline bool Inside(int sx, int sy) const
	{
		return sx >= x && sx < (width + x) && sy >= y && sy < (height + y);
	}

};

class BitBoard
{
public:

	BitBoard()
	{
		bits[0] = 0ULL;
		bits[1] = 0ULL;
	}

	BitBoard(const unsigned long long sBits[])
	{
		bits[0] = sBits[0];
		bits[1] = sBits[1];
	}

	// this is specifically for the implementation with two bit chains
	BitBoard(unsigned long long lbit, unsigned long long rbit)
	{
		bits[0] = lbit;
		bits[1] = rbit;
	}

	BitBoard(const BitBoard& copy)
	{
		bits[0] = copy.bits[0];
		bits[1] = copy.bits[1];
	}

	explicit BitBoard(ChessVector pos)
	{
		bits[0] = 0ULL;
		bits[1] = 0ULL;
		SetBit(true, pos.GetVectorCoord());
	}

	inline BitBoard& operator=(const BitBoard& assign)
	{
		if(this != &assign)
		{
			bits[0] = assign.bits[0];
			bits[1] = assign.bits[1];
		}
		return *this;
	}

	inline BitBoard& operator~()
	{
		bits[0] = ~ bits[0];
		bits[1] = ~ bits[1];
		return (*this);
	}

	inline BitBoard& operator&=(const BitBoard& rhs)
	{
		bits[0] &= rhs.bits[0];
		bits[1] &= rhs.bits[1];
		return *this;
	}

	inline BitBoard& operator|=(const BitBoard& rhs)
	{
		bits[0] |= rhs.bits[0];
		bits[1] |= rhs.bits[1];
		return *this;
	}

	inline BitBoard& operator^=(const BitBoard& rhs)
	{
		bits[0] ^= rhs.bits[0];
		bits[1] ^= rhs.bits[1];
		return *this;
	}

	inline friend const BitBoard operator~(const BitBoard& lhs)
	{
		return BitBoard(~lhs.bits[0], ~lhs.bits[1]);
	}

	inline friend const BitBoard operator&(const BitBoard& lhs, const BitBoard& rhs)
	{
		return BitBoard(lhs.bits[0] & rhs.bits[0], lhs.bits[1] & rhs.bits[1]);
	}

	inline friend const BitBoard operator|(const BitBoard& lhs, const BitBoard& rhs)
	{
		return BitBoard(lhs.bits[0] | rhs.bits[0], lhs.bits[1] | rhs.bits[1]);
	}

	inline friend const BitBoard operator^(const BitBoard& lhs, const BitBoard& rhs)
	{
		return BitBoard(lhs.bits[0] ^ rhs.bits[0], lhs.bits[1] ^ rhs.bits[1]);
	}

	inline bool operator==(const BitBoard& rhs) const
	{
		return bits[0] == rhs.bits[0] && bits[1] == rhs.bits[1];
	}

	inline bool operator!=(const BitBoard& rhs) const
	{
		return bits[0] != rhs.bits[0] || bits[1] != rhs.bits[1];
	}

	inline operator bool() const
	{
		return bits[0] != 0ULL || bits[1] != 0ULL;
	}

	// makes all bits zeroes
	inline void Zero()
	{
		bits[0] = 0ULL;
		bits[1] = 0ULL;
	}

	// fast function for getting a single bit
	inline bool GetBit(coord offset) const
	{
		unsigned char bitn = offset >> Utils::BITBOARD_PHYS_SIZE_OFFSET;
		unsigned char bitOffset = offset & Utils::BITBOARD_PHYS_SIZE_MOD_MASK;
		return (bits[bitn] & (Config::BITBOARD_BIT >> bitOffset)) != 0;
	}

	// returns the bits with the given offset, and the specified mask ( 1 is the default mask)
	inline unsigned long long GetBits(coord offset, unsigned long long mask = Config::BITBOARD_BIT) const
	{
		unsigned char bitn = offset >> Utils::BITBOARD_PHYS_SIZE_OFFSET;
		unsigned char bitOffset = offset & Utils::BITBOARD_PHYS_SIZE_MOD_MASK;
		unsigned long long srcBitMask = 0xffffffffffffffff << bitOffset;
		unsigned long long srcBits = (bits[bitn] << bitOffset) & srcBitMask;
		if( bitn < Config::BITBOARD_SIZE - 1 && offset)
		{
			srcBits |= (bits[bitn + 1] >> (Utils::BITBOARD_PHYS_SIZE - bitOffset)) & ~ srcBitMask;
		}
		return srcBits & mask;
	}

	inline void SetBit(bool flag, coord offset)
	{
		unsigned char bitn = offset >> Utils::BITBOARD_PHYS_SIZE_OFFSET;
		unsigned char bitOffset = offset & Utils::BITBOARD_PHYS_SIZE_MOD_MASK;
		unsigned long long destBit = (flag ? Config::BITBOARD_BIT >> bitOffset : 0ULL);
		unsigned long long destMask = Config::BITBOARD_BIT >> bitOffset;
		bits[bitn] = (bits[bitn] & ~ destMask) | (destBit & destMask);
	}

	// sets the bits with the given offset using only those from the specified mask
	inline void SetBits(unsigned long long srcBits, coord offset, unsigned long long mask = Config::BITBOARD_BIT)
	{
		unsigned char bitn = offset >> Utils::BITBOARD_PHYS_SIZE_OFFSET;
		unsigned char bitOffset = offset & Utils::BITBOARD_PHYS_SIZE_MOD_MASK;
		unsigned long long destBitMask = mask >> bitOffset;
		unsigned long long destBits = srcBits >> bitOffset;
		bits[bitn] = (bits[bitn] & ~ destBitMask) | destBits & destBitMask;
		if( bitn < Config::BITBOARD_SIZE - 1 && offset)
		{
			destBitMask = mask << (Utils::BITBOARD_PHYS_SIZE - bitOffset);
			destBits = srcBits << (Utils::BITBOARD_PHYS_SIZE - bitOffset);
			bits[bitn + 1] = (bits[bitn + 1] & ~ destBitMask) | destBits & destBitMask;
		}
	}

	// take the current object's bit count
	coord GetBitCount() const
	{
		return BitCount(bits[0]) + BitCount(bits[1]);
	}

	// sets all bits to proper vectors in the specified DynamicArray
	inline void GetVectors(DynamicArray<ChessVector>& dest) const
	{
		dest.Clear();
		coord count = GetBitCount();
		dest.Alloc(count);
		for(char i = 0; i < Utils::BITBOARD_PHYS_SIZE; ++i)
		{
			if(bits[0] & (Config::BITBOARD_BIT >> i))
				dest += ChessVector(i);

			if(bits[1] & (Config::BITBOARD_BIT >> i))
				dest += ChessVector(Utils::BITBOARD_PHYS_SIZE + i);
		}
	}

	// Template static function for taking any variable's number of bits
	template < class T >
	static coord BitCount( T src)
	{
		coord count = 0;
		while(src)
		{
			src &= src - 1;
			++count;
		}
		return count;
	}

	//debuging purposes
#ifdef _DEBUG
#define DEBUG_BB(x,str) (x).PrintBitBoard((str));

	// prints the bit board to the console
	void PrintBitBoard(const char* str) const;

#else
#define DEBUG_BB(x,str)
#endif // _DEBUG

private:
	unsigned long long bits[Config::BITBOARD_SIZE];
};

#endif // __UTILS_H__