#include <iostream>
#include <cstdio>
#include <vector>
#include <random>
#include <ctime>

class Point
{
public:
	float x,y;

	Point( float sx, float sy) : x( sx), y( sy) {}
	Point( const Point& copy) : x( copy.x), y( copy.y) {}

	Point& operator=( const Point& assign)
	{
		if( &assign != this)
		{
			this->x = assign.x;
			this->y = assign.y;
		}
		return *this;
	}

	static float GetDistance( const Point& a, const Point& b)
	{
		return sqrt( (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
	}
};

class CachedDistances
{
public:
	CachedDistances() : numDistances(0), distances( nullptr) {}
	CachedDistances( const std::vector<Point>& points)
	{
		Initialize( points);
	}

	void Initialize( const std::vector<Point>& points)
	{
		if( distances == nullptr)
		{
			delete [] distances;
			distances = nullptr;
		}

		numDistances = points.size();
		distances = new float[ numDistances * numDistances];

		for( int i = 0; i < numDistances; ++i)
		{
			for( int j = i; j < numDistances; ++j)
			{
				float dist = Point::GetDistance( points[i], points[j]);
				distances[i*numDistances + j] = dist;
				distances[j*numDistances + i] = dist;
			}
		}
	}

	float GetDistance( int from, int to) const
	{
		return distances[from * numDistances + to];
	}

	~CachedDistances()
	{
		delete [] distances;
		distances = nullptr;
	}
private:
	int numDistances;
	float* distances;
};

class GeneticString
{
public:
	GeneticString() : str(nullptr), size(0) {}
	GeneticString( const char* source) : str( nullptr)
	{
		int strLength = strlen( source);
		size = strLength;
		str = new char[size];
		strncpy( str, source, size);
	}

	GeneticString( const GeneticString& copy) : size( copy.size), str( nullptr)
	{
		str = new char[size];
		strncpy( str, copy.str, size);
	}

	~GeneticString()
	{
		delete[] str;
		str = nullptr;
		size = 0;
	}

	GeneticString& operator=( const GeneticString& assign)
	{
		if( str != nullptr)
		{
			delete[] str;
			str = nullptr;
		}
		size = assign.size;
		str = new char[size];
		strncpy( str, assign.str, size);
	}

	void SetString( const char* source, int srcSize = -1)
	{
		int sourceLength = srcSize > 0 ? srcSize : strlen( source);
		if( str == nullptr || size != sourceLength)
		{
			if( str == nullptr)
				delete[] str;
			size = sourceLength;
			str = new char[size];
		}
		strncpy( str, source , size);
	}

	typedef std::pair<GeneticString, GeneticString> GeneticPair;
	GeneticPair Crossover( const GeneticString& parentA, const GeneticString& parentB)
	{

	}

	std::string GetString() const
	{
		return std::string( str);
	}
private:
	int size;
	char* str;
};

class GeneticAlgorithm
{
public:
	GeneticAlgorithm( int sz, int maxGen) : size( sz), maxGenerations( maxGen)
	{
		populations.resize( sz);
	}

private:

	int size;
	std::vector <GeneticString> populations;
	int currentGeneration;
	const int maxGenerations;
};

class GeneticTravelingSalesman
{
public:
	GeneticTravelingSalesman( int n)
	{
		srand( static_cast <unsigned> ( std::time( NULL)));

		for( int i = 0; i < n; ++i)
		{
			towns.push_back( Point( generateRandomFloat(), generateRandomFloat()));
		}

		distances.Initialize( towns);
	}

	void PrintPoints() const
	{
		for( int i = 0; i < towns.size(); ++i)
		{
			printf("Point[%02d] = { %8.f, %8.f}\n", i, towns[i].x, towns[i].y);
		}
	}

	void PrintDistances() const
	{
		for( int i = 0; i < towns.size(); ++i)
		{
			for( int j = 0; j < towns.size(); ++j)
			{
				printf("%8.2f ", distances.GetDistance( i, j));
			}
			printf("\n");
		}
	}

	std::string GetShortestPath() const
	{

	}

private:
	float generateRandomFloat() const
	{
		return static_cast <float> ( RAND_MAX/2 - rand());
	}

	std::vector<Point> towns;
	CachedDistances distances;
};

int main( int argc, char* argv[])
{
	using std::cout;
	using std::cin;
	using std::endl;
	int numTowns = 0;

	while( numTowns <= 0 || numTowns > 10)
	{
		cout << "Please enter the desired number of towns" << endl;
		cin >> numTowns;
		if( numTowns <= 0 || numTowns > 10)
		{
			cout << "Invalid number of towns, try again! " << endl;
		}
	}

	GeneticTravelingSalesman gts( numTowns);

	gts.PrintPoints();
	gts.PrintDistances();

	system("pause");
	return 0;
}