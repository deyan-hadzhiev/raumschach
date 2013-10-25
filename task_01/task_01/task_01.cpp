#include <iostream>
#include <algorithm>
#include <queue>
#include <random>
#include <string>
#include <ctime>

static const int MAX_MATRIX_SIZE = 4096;
static const char PASSABLE_TERRAIN = '1';
static const char IMPASSABLE_TERRAIN = '0';
static const char PATH = '*';

struct Coord
{
	friend Coord operator+( Coord a, Coord b); 
	Coord( int a = 0, int b = 0): row(a), column(b) {}
	bool operator==( Coord a) const { return row == a.row && column == a.column;}
	bool operator!=( Coord a) const { return row != a.row || column != a.column;}
	int row, column;
};

Coord operator+( Coord a, Coord b)
{
	return Coord( a.row + b.row, a.column + b.column);
}

template< class ContentType>
class Matrix
{
public:
	Matrix ()
		:
		table( nullptr),
		rows( 0),
		columns( 0)
	{}

	Matrix( unsigned numRows , unsigned numColumns = 0)
		:
		table( nullptr),
		rows(numRows),
		columns(numColumns)
	{
		if( numRows > 0 && numColumns > 0)
		{
			Allocate( numRows, numColumns);
		}
	}

	virtual ~Matrix()
	{
		clearTable();
	}

	// Allocates new table and clears the previous
	bool Allocate( unsigned numRows, unsigned numColumns)
	{
		bool retval = false;
		if( numRows > 0 && numColumns > 0)
		{
			clearTable();
			table = new ContentType[ numRows * numColumns ];
			rows = numRows;
			columns = numColumns;
			retval = true;
		}
		return retval;
	}

	unsigned GetRows() const { return rows;}
	unsigned GetColumns() const { return columns;}

	ContentType* operator[]( unsigned row) { return table != nullptr ? &table[row * columns] : nullptr;}
	const ContentType* operator[]( unsigned row) const { return table != nullptr ? &table[row * columns] : nullptr;}

	ContentType& operator[]( Coord c) 
	{ 
		if( table == nullptr)
		{
			throw std::runtime_error("ERROR: Access to unallocated table!");
		}
		return table[ columns * c.row + c.column];
	}
	const ContentType& operator[]( Coord c) const 
		{ 
		if( table == nullptr)
		{
			throw std::runtime_error("ERROR: Access to unallocated table!");
		}
		return table[ columns * c.row + c.column];
	}

	bool ValidCoord( int row, int column) const
	{
		return row >= 0 && column >= 0 && row < rows && column < columns;
	}

	bool ValidCoord( const Coord& c) const
	{
		return c.row >= 0 && c.column >= 0 && c.row < rows && c.column < columns;
	}


protected:

	ContentType* table;

	unsigned rows;
	unsigned columns;

	// for clearing the current table
	void clearTable()
	{
		if( table != nullptr)
		{
			delete[] table;
			table = nullptr;
		}
	}

private:
	// Disable copy and assignment
	Matrix( const Matrix& copy);
	const Matrix& operator= ( const Matrix& assign);
};

template< class Outputable>
void OutputMatrix( const Matrix< Outputable>& matrix)
{
	for( int i = 0; i < matrix.GetRows(); ++i)
	{
		for( int j = 0; j < matrix.GetColumns() - 1; ++j)
		{
			std::cout << matrix[i][j] << " ";
		}
		std::cout << matrix[i][ matrix.GetColumns() - 1] << std::endl;
	}
}

Coord InputCoord( const std::string& msg)
{
	Coord retval;

	std::cout << msg;
	int row, column;
	std::cin >> row >> column;

	return Coord( row, column);
}

void RandomizeMatrix( Matrix< char>& matrix, int number)
{
	srand( time(nullptr));

	unsigned row = 0;
	unsigned column = 0;

	for( int i = 0; i < number; ++i)
	{
		row = rand() % matrix.GetRows();
		column = rand() % matrix.GetColumns();
		if( matrix[row][column] == IMPASSABLE_TERRAIN)
		{
			--i;
		}
		else
		{
			matrix[row][column] = IMPASSABLE_TERRAIN;
		}
	}
}

void InputMatrix( Matrix<char>& matrix)
{
	int n = 0;
	std::cout << "Enter the dimension of the square Matrix" << std::endl;
	std::cout << "N = ";

	std::cin >> n;

	while( n <= 0 || n > MAX_MATRIX_SIZE)
	{
		std::cout << "ERROR: Invalid dimension (max: " << MAX_MATRIX_SIZE << "). Pleasy try again:" << std::endl;
		std::cout << "N = ";

		std::cin >> n;
	}

	matrix.Allocate( n, n);
	
	for( int i = 0; i < matrix.GetRows(); ++i){
		for( int j = 0; j < matrix.GetColumns(); ++j)
		{
			matrix[i][j] = PASSABLE_TERRAIN;
		}
	}

	int k = 0;
	std::cout << "Enter the number of impassible tiles:" << std::endl;
	std::cout << "K = ";

	std::cin >> k;

	while( k < 0 || k >= n*n)
	{
		std::cout << "ERROR: Invalid number of impassible tiles. Try again:" << std::endl;
		std::cout << "K = ";

		std::cin >> k;
	}

	char answer = 'n';
	std::cout << "Whould you like to enter the impassible coordinates? (y/n)" << std::endl;
	std::cin >> answer;
	if( answer == 'y')
	{
		std::cout << "Please enter the impassible coordinates:" << std::endl << " Format: X Y" << std::endl;
		for( int i = 0; i < k; ++i)
		{
			char number[10] = "";
			itoa( i, number, 10);
			Coord impassibleCoord = InputCoord( std::string(number) + " = ");
			if( matrix.ValidCoord( impassibleCoord))
			{
				if( matrix[ impassibleCoord.row][ impassibleCoord.column] == IMPASSABLE_TERRAIN)
				{
					std::cout << "Already impassible terrain" << std::endl;
					--i;
				}
				else
				{
					matrix[ impassibleCoord.row][impassibleCoord.column] = IMPASSABLE_TERRAIN;
				}
			}
			else
			{
				std::cout << "Invalid Coordinate. Dimensions are " << matrix.GetRows() << " " << matrix.GetColumns() << std::endl;
				--i;
			}
		}
	}
	else
	{
		RandomizeMatrix( matrix, k);
	}

	//OutputMatrix( matrix);
}

template < class ContentType >
inline bool IsPassable( const Matrix< ContentType>& matrix, Coord c)
{
	return matrix.ValidCoord( c) && matrix[c.row][c.column] == PASSABLE_TERRAIN;
}

void InputEndingCoordinates( const Matrix< char>& matrix , Coord& start, Coord& end)
{
	Coord input = InputCoord( "Start = ");
	
	while( ! IsPassable( matrix, input))
	{
		if ( ! matrix.ValidCoord( input))
		{
			std::cout << "You have entered invalid coordinate. Try again:" << std::endl;
		}
		else if( matrix[ input] == IMPASSABLE_TERRAIN)
		{
			std::cout << "This tile is impassable. Try again:" << std::endl;
		}

		input = InputCoord( "Start = ");
	}

	start = input;

	input = InputCoord( "End = ");

	while( ! IsPassable( matrix, input))
	{
		if ( ! matrix.ValidCoord( input))
		{
			std::cout << "You have entered invalid coordinate. Try again:" << std::endl;
		}
		else if( matrix[ input] == IMPASSABLE_TERRAIN)
		{
			std::cout << "This tile is impassable. Try again:" << std::endl;
		}

		input = InputCoord( "End = ");
	}

	end = input;
}

//This is the actual path finder, it uses BFS to find the best path
template< class ContentType>
std::vector< Coord> FindPath( const Matrix< ContentType>& table, Coord start, Coord end)
{
	// initialization of variables, directions, etc
	enum Parent{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		START,
		UNREACHED,
	};

	Matrix< Parent> pathTracer( table.GetRows(), table.GetColumns());

	for( unsigned i = 0; i < pathTracer.GetRows(); ++i)
		for( unsigned j = 0; j < pathTracer.GetColumns(); ++j)
			pathTracer[i][j] = UNREACHED;

	pathTracer[ start.row][ start.column] = START;

	Coord directions[] = { Coord(1, 0), Coord( -1, 0), Coord( 0, -1), Coord( 0, 1)}; // Used by the bfs
	Coord invertedDirections[] = { Coord( -1, 0), Coord( 1, 0), Coord( 0, 1), Coord( 0, -1)}; // used for the creation of the path

	const unsigned numDirections = 4;

	std::queue< Coord> bfsQueue;
		
	bool foundPath = false;
	
	//The algorithm starts here
	bfsQueue.push( start);

	while( ! foundPath && ! bfsQueue.empty())
	{
		Coord parent = bfsQueue.front();
		if( parent == end)
		{
			foundPath = true;
			break;
		}

		for( unsigned i = 0; i < numDirections; ++i)
		{
			Coord child = parent + directions[i];
			if( IsPassable( table, child) && pathTracer[ child] == UNREACHED)
			{
				pathTracer[ child] = static_cast< Parent>( i);
				bfsQueue.push( child);
			}
		}

		bfsQueue.pop();
	}
	
	std::vector< Coord> path;

	if( foundPath)
	{
		Parent p = pathTracer[ end];
		path.push_back( end);

		Coord current = end;

		while( p != START)
		{
			current = current + invertedDirections[ p];
			p = pathTracer[ current];
			path.push_back( current);
		}
	}

	return path;
}

int main(int argc, char* argv[])
{
	
	Matrix<char> mat;

	InputMatrix( mat);

	Coord start;
	Coord end;

	InputEndingCoordinates( mat, start, end);

	std::vector< Coord> pathToEnd = FindPath( mat, start, end);

	if( pathToEnd.empty())
	{
		std::cout << "No path exists between the starting and edning point" << std::endl;
	}
	else
	{
		for( unsigned i = 0; i < pathToEnd.size(); ++i)
		{
			mat[ pathToEnd[i]] = PATH;
		}
	}

	OutputMatrix( mat);

	system("pause");

	return 0;
}

