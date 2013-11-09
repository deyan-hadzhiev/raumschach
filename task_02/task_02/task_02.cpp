#include <iostream>
#include <algorithm>
#include <queue>
#include <cmath>
#include <set>
#include <string>

static const unsigned BOARD_SIDE = 3;

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

	Matrix( unsigned numRows , unsigned numColumns)
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

	Matrix( const ContentType arr[], unsigned numRows, unsigned numColumns)
		:
		table( nullptr),
		rows( numRows),
		columns( numColumns)
	{
		if( numRows > 0 && numColumns > 0)
		{
			if( Allocate( numRows, numColumns))
			{
				for( int i = 0; i < numRows*numColumns; ++i)
				{
					table[i] = arr[i];
				}
			}
		}
	}


	Matrix( const Matrix& copy)
		:
		table( nullptr),
		rows( copy.rows),
		columns( copy.columns)
	{
		if( Allocate( rows, columns))
		{
			for( int i = 0; i < rows*columns; ++i)
			{
				table[i] = copy.table[i];
			}
		}
	}

	Matrix& operator=(const Matrix& assign)
	{
		if( &assign != this)
		{
			clearTable();
			rows = assign.rows;
			columns = assign.columns;
			if( Allocate( rows, columns))
			{
				for( int i = 0; i < rows*columns; ++i)
				{
					table[i] = assign.table[i];
				}
			}
		}
		return *this;
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
		return table[ columns * c.row + c.column];
	}
	const ContentType& operator[]( Coord c) const 
	{ 
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

	void CopyToArray( ContentType* outArray) const
	{
		for( unsigned i = 0; i < rows*columns; ++i)
			outArray[i] = table[i];
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
};

static const Coord GoalPlace[] =
{
	Coord( 2, 2),
	Coord( 0, 0),
	Coord( 0, 1),
	Coord( 0, 2),
	Coord( 1, 0),
	Coord( 1, 1),
	Coord( 1, 2),
	Coord( 2, 0),
	Coord( 2, 1)
};

class Board
{
public:
	Board()
		:
		board( BOARD_SIDE, BOARD_SIDE)
	{}

	Board( const Board& copy)
		:
		board( copy.board)
	{}

	Board( const Matrix<char>& source)
		:
		board( source)
	{}

	Board( const Board& copy, Coord a, Coord b)
		:
		board( copy.board)
	{
		if( ValidSwap( a, b))
		{
			char tmp = board[a];
			board[a] = board[b];
			board[b] = tmp;
		}
	}

	bool Swap( Coord a, Coord b)
	{
		if( ValidSwap( a, b))
		{
			char tmp = board[a];
			board[a] = board[b];
			board[b] = tmp;
			return true;
		}
		else
		{
			return false;
		}
	}

	Coord ElementPosition( char element) const
	{
		for(unsigned i = 0; i < board.GetRows(); ++i)
		{
			for( unsigned j = 0; j < board.GetColumns(); ++j)
			{
				if( board[i][j] == element) return Coord( i, j);
			}
		}
		return Coord( -1, -1);
	}

	unsigned DistanceFromGoal() const
	{
		unsigned sum = 0;
		for( int i = 0; i < BOARD_SIDE; ++i)
		{
			for( int j = 0; j < BOARD_SIDE; ++j)
			{
				sum += manhatanDistance( Coord( i, j), GoalPlace[ numAt( Coord( i, j)) ]);
			}
		}
		return sum;
	}

	std::string GetAsString() const
	{
		unsigned size = board.GetRows() * board.GetColumns();
		char* input = new char[ size + 1];
		if( input != NULL)
		{
			board.CopyToArray( input);
			input[ size] = '\0';
		}
		return std::string( input);
	}

	bool ValidSwap( Coord a, Coord b) const
	{
		return board.ValidCoord(a) && board.ValidCoord(b) && manhatanDistance( a, b) == 1;
	}

private:
	unsigned manhatanDistance( Coord a, Coord b) const
	{
		return abs(a.row - b.row) + abs( a.column - b.column);
	}

	int numAt( Coord c) const { return board[c] - '0'; }

	Matrix<char> board;
};

struct BoardState
{
	BoardState( const Board& board, int turnsFromStart = 0)
		:
		currentBoard( board),
		fromStart( turnsFromStart),
		calculatedHeuristic( ~0)
	{}

	BoardState( const BoardState& copy)
		:
		currentBoard( copy.currentBoard),
		fromStart( copy.fromStart),
		calculatedHeuristic( copy.calculatedHeuristic)
	{}


	Board currentBoard;
	int fromStart;
	mutable unsigned calculatedHeuristic;

	bool Occured( const std::set< std::string>& visited)
	{
		return visited.find( currentBoard.GetAsString()) != visited.end();
	}

	unsigned Heuristic() const
	{
		if( calculatedHeuristic == ~0)
			calculatedHeuristic = currentBoard.DistanceFromGoal() + fromStart;
		return calculatedHeuristic;
	}

	bool operator<=(const BoardState& rhs) const
	{
		return (this->Heuristic() == rhs.Heuristic() ? this->fromStart >= rhs.fromStart : this->Heuristic() >= rhs.Heuristic());
	}

	bool operator<( const BoardState& rhs) const
	{
		return (this->Heuristic() == rhs.Heuristic() ? this->fromStart > rhs.fromStart : this->Heuristic() > rhs.Heuristic());
	}
};

int AStar( Board startingPosition)
{
	BoardState startingState( startingPosition, 0);
	
	const Coord dirs[] = { Coord( 1, 0), Coord( 0, -1), Coord( -1, 0), Coord( 0, 1)};
	const unsigned DIRS_COUNT = 4;

	std::priority_queue<BoardState> stateQueue;
	std::set< std::string> visitedStates;

	const Coord INVALID_COORD( -1, -1);

	stateQueue.push( startingState);

	visitedStates.insert( startingState.currentBoard.GetAsString());
	
	int i = 0;
	while( ! stateQueue.empty() && stateQueue.top().currentBoard.DistanceFromGoal() > 0)
	{
		BoardState currentState = stateQueue.top();
		stateQueue.pop();
		Coord zeroPos = currentState.currentBoard.ElementPosition( '0');
		if( zeroPos != INVALID_COORD)
		{
			for( unsigned i = 0; i < DIRS_COUNT; ++i)
			{
				Coord nextCoord = zeroPos + dirs[i];
				if( currentState.currentBoard.ValidSwap( zeroPos, nextCoord))
				{
					BoardState nextState( Board( currentState.currentBoard, zeroPos, nextCoord), currentState.fromStart + 1);
					if( ! nextState.Occured( visitedStates))
					{
						visitedStates.insert( nextState.currentBoard.GetAsString());
						stateQueue.push( nextState);
					}
				}
			}
		}
		//std::cout << i++ << " : " << stateQueue.size() << " :h: "<< currentState.Heuristic() << " :s " << currentState.fromStart << " - "<< currentState.Heuristic() - currentState.fromStart  << std::endl;
	}
	//std::cout << visitedStates.size() << std::endl;

	return stateQueue.empty() ? -1 : stateQueue.top().fromStart;
}

int main( int argc, char* argv[])
{
	//char table[] = "653248701";
	char table[] = "806547231";

	Matrix<char> tableMat( table, 3, 3);

	Board startBoard( tableMat);
	
	std::cout << AStar(startBoard) << std::endl;

	return 0;
}
