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
	
	~DynamicArray()
	{
		delete[] arr;
		arr = nullptr;
	}

	DynamicArray& operator=(const DynamicArray<Type>& assign)
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

	// adds an item to the end of the array ( rellocates current size * 2 if needed )
	DynamicArray& operator+=(Type item)
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

	// if there isn't 'n' in size currently free, the array allocates them
	void Alloc(int n)
	{
		if(size - count < n)
		{
			Realloc(size + n);
		}
	}

private:
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

class ChessVector
{
public:
	coord x,y,z;

	ChessVector();
	explicit ChessVector(const coord coordArray[]);
	explicit ChessVector(coord x, coord y, coord z);
	ChessVector(const ChessVector& copy);
	explicit ChessVector(coord singleCoord);

	ChessVector& operator=(const ChessVector& assign);

	ChessVector& operator+=(ChessVector rhs);
	ChessVector& operator-=(ChessVector rhs);
	ChessVector& operator*=( coord scale);

	friend const ChessVector operator+(ChessVector lhs, ChessVector rhs);
	friend const ChessVector operator-(ChessVector lhs, ChessVector rhs);
	friend const ChessVector operator*(ChessVector lhs, coord scale);
	friend const ChessVector operator*(coord scale, ChessVector rhs);
	friend bool operator==(ChessVector lhs, ChessVector rhs);
	friend bool operator!=(ChessVector lhs, ChessVector rhs);

	ChessVector& operator-();

	coord GetVectorCoord() const;

	// Returns the Manhattan distance to the vector 'to'
	coord GetManhattanDistance(ChessVector to) const;
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

	Rect& operator=(const Rect& assign)
	{
		x = assign.x;
		y = assign.y;
		width = assign.width;
		height = assign.height;
		return *this;
	}

	bool Inside(int sx, int sy) const
	{
		return sx >= x && sx < (width + x) && sy >= y && sy < (height + y);
	}

};

class BitBoard
{
public:
	// only for debug
	friend void PrintBitBoard( const BitBoard& bb);

	BitBoard();
	BitBoard(const unsigned long long sBits[]);
	BitBoard(const BitBoard& copy);
	explicit BitBoard(ChessVector pos);

	BitBoard& operator=(const BitBoard& assign);

	BitBoard& operator~();
	BitBoard& operator&=(const BitBoard& rhs);
	BitBoard& operator|=(const BitBoard& rhs);
	BitBoard& operator^=(const BitBoard& rhs);

	friend const BitBoard operator~(const BitBoard& lhs);
	friend const BitBoard operator&(const BitBoard& lhs, const BitBoard& rhs);
	friend const BitBoard operator|(const BitBoard& lhs, const BitBoard& rhs);
	friend const BitBoard operator^(const BitBoard& lhs, const BitBoard& rhs);

	bool operator==(const BitBoard& rhs) const;
	bool operator!=(const BitBoard& rhs) const;
	operator bool() const;

	void Zero(); // makes all bits zeroes
	unsigned long long GetBits( coord offset, unsigned long long mask = Config::BITBOARD_BIT) const; // returns the bits with the given offset, and the specified mask ( 1 is the default mask)
	void SetBits( unsigned long long srcBits, coord offset, unsigned long long mask = Config::BITBOARD_BIT); // sets the bits with the given offset using only those from the specified mask
	coord GetBitCount() const; // take the current object's bit count

	void GetVectors(DynamicArray<ChessVector>& dest) const; // sets all bits to proper vectors in the specified DynamicArray

	// Template static function for taking any variable's number of bits
	template < class T >
	static coord BitCount( T src)
	{
		coord count = 0;
		for( coord i = 0; i < sizeof(T) * 8; ++i)
		{
			count += (src >> i) & 1;
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
	static const coord SIZE;
	unsigned long long bits[Config::BITBOARD_SIZE];
};

#endif // __UTILS_H__