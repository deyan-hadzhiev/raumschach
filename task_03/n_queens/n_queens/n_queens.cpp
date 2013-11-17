#include <iostream>
#include <random>
#include <ctime>
#include <vector>

template< class T>
inline T Max( T a, T b) { return a > b ? a : b;}

template< class T>
inline T Min( T a, T b) { return a < b ? a : b;}

int GetRand( int max)
{
	return rand() % max;
}

class CheckerBoard
{
	friend void PrintCheckerBoard( const CheckerBoard& board, FILE* output = NULL);
public:
	CheckerBoard( int n)
		: queens( nullptr), numQueens(n)
	{
		if( n > 0)
			queens = new int[numQueens];
		for( int i = 0; i < numQueens; ++i) queens[i] = numQueens * numQueens;
	}

	~CheckerBoard()
	{
		delete[] queens;
		queens = nullptr;
		numQueens = 0;
	}

	unsigned GetQueensNumber() const { return numQueens;}
	// deletes the buffer, so the queens must be assignt again
	void SetQueensNumber( int newNumber)
	{
		if( newNumber > 0)
		{
			numQueens = newNumber;
			delete[] queens;
			queens = new int[numQueens];
			for( int i = 0; i < numQueens; ++i) queens[i] = numQueens * numQueens;
		}
	}

	void SetQueen( int col, int row)
	{
		queens[col] = row;
	}

	void Initialize()
	{
		for( int col = 0; col < numQueens; ++col)
		{
			bool posFound = false;
			for( int row = 0; row < numQueens && !posFound; ++row)
			{
				if( Hits( col, row) == 0)
				{
					SetQueen( col, row);
					posFound = true;
				}
			}
			if( !posFound)
			{
				std::vector<int> minRows;
				FindMinRowConflicts( minRows, col);
				int numMinRows = minRows.size();
				int row = numMinRows > 1 ? minRows[GetRand( numMinRows)] : minRows[0];

				SetQueen( col, row);
			}
		}
	}

	bool MinConflicts( unsigned maxIterations)
	{
		bool goalAchieved = false;
		int lastCol = 0;

		for(int i = 0; i < maxIterations && !goalAchieved; ++i)
		{
			int numConflicts = FindConflicts();
			if( goalAchieved = (numConflicts == 0)) break;
			//else printf("%d %d\n", i, numConflicts);
			int candidateColumn = MaxColConflicts();

			if( candidateColumn == lastCol) Initialize();
			else lastCol = candidateColumn;

			int candidateRow = MinRowConflicts( candidateColumn);
			SetQueen( candidateColumn, candidateRow);
		}
		return goalAchieved;
	}

	int FindConflicts() const
	{
		int conflicts = 0;
		for( int i = 0; i < numQueens; ++i)
		{
			conflicts += Hits( i, queens[i]);
		}
		return conflicts / 2;
	}

	int MaxColConflicts() const
	{
		int conflicts = 0;
		int col = 0;
		for( int i = 0; i < numQueens; ++i)
		{
			int hits = Hits( i, queens[i]);
			if( conflicts < hits)
			{
				conflicts = hits;
				col = i;
			}
		}
		return col;
	}

	void FindMinRowConflicts( std::vector<int>& container, int col)
	{
		int conflicts = numQueens;
		for( int i = 0; i < numQueens; ++i)
		{
			int hits = Hits( col, i);
			if( hits < conflicts)
			{
				conflicts = hits;
				container.erase( container.begin(), container.end());
				container.push_back( i);
			}
			else if( hits == conflicts)
			{
				container.push_back( i);
			}
		}

	}

	int MinRowConflicts( int col) const
	{
		int conflicts = numQueens;
		int row = 0;
		for( int i = 0; i < numQueens; ++i)
		{
			int hits = Hits( col, i);
			if( hits < conflicts)
			{
				conflicts = hits;
				row = i;
			}
		}
		return row;
	}

	int Hits( int col, int row) const
	{
		int total = 0;
		for( int i = 0; i < numQueens; ++i)
		{
			if( col == i) continue;
			else
			{
				total += (queens[i] == row || abs(i - col) == abs(queens[i] - row)) ? 1 : 0;
			}
		}
		return total;
	}
private:
	// disable copy and assignment
	CheckerBoard( const CheckerBoard& copy);
	const CheckerBoard& operator=( const CheckerBoard& assigned);

	int * queens;
	int numQueens;
};

void PrintCheckerBoard( const CheckerBoard& board, FILE* output)
{
	int size = board.numQueens;
	char * row = new char[size + 1];
	row[size] = 0;

	if( output == NULL) output = stdout;

	const char EMPTY = '_';
	const char QUEEN = '*';

	auto resetRow = [] ( char* arr, int sz, char ch) { for(int i = 0; i < sz; ++i) arr[i] = ch;};

	resetRow( row, size, EMPTY);
	for( int rown = 0; rown < size; ++rown)
	{
		for( int col = 0; col < size; ++col)
		{
			if( board.queens[col] == size - 1 - rown) row[col] = QUEEN;
		}
		fprintf( output,"%s\n", row);
		resetRow( row, size, EMPTY);
	}
	delete[] row;
}

int main( int argc, char* argv[])
{
	srand( time(NULL));
	int n = 8;
	std::cout << "Enter the number of queens" << std::endl;

	const int MAX_ITERATIONS = 1000000;

	std::cin >> n;

	long long start = clock();

	std::cout << "Initializing the checker board" << std::endl;

	CheckerBoard check( n);
	check.Initialize();
	
	std::cout << "Starting calculation ( max iterations: " << MAX_ITERATIONS << ")" << std::endl;

	bool found = check.MinConflicts( MAX_ITERATIONS);

	std::cout << "Calculation finished - result " << (found ? "" : "NOT ") << "found" << std::endl;

	long long end = clock();

	float calculation = (float)(end - start)/ CLOCKS_PER_SEC;

	int ms = (calculation - floor( calculation)) * 1000;
	int second = floor( calculation);
	int sec = second % 60;
	int min = (second / 60) % 60;
	int hrs = second / 3600;

	printf("Calculation time : %dh %dm %d.%ds\n", hrs, min, sec, ms);

	const char filename[] = "output.txt";

	if( found)
	{
		printf("Outputing into : %s\n", filename);
		FILE* out = fopen( filename, "wt");
		if( out != NULL)
		{
			PrintCheckerBoard( check, out);
			fclose( out);
		}
	}
	system( "pause");

	return 0;
}
