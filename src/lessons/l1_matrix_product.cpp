#include <iostream>
#include <thread>
#include <vector>

using std::cout;
using std::endl;

struct Matrix
{
public:
	size_t width;
	size_t height;
	std::vector<float> data;

	Matrix(size_t w, size_t h)
	{
		width = w;
		height = h;
		data.resize(width * height, 0.0f);
	}

	inline float at(size_t x, size_t y) const
	{
		return data[x + y * width];
	}

	inline void setAt(size_t x, size_t y, float f)
	{
		auto i = x + y * width;
		data[i] = f;
	}

	void print()
	{
		for (size_t i = 0; i < data.size(); ++i)
		{
			cout << data[i] << " \t";
			if(i % width == width - 1){
				cout << endl;
			}
		}
	}
};

void mDot(const Matrix &a, const Matrix &b, Matrix &c, size_t x, size_t y)
{
	float sum = 0.0f;
	for (size_t i = 0; i < a.width; ++i)
	{
		sum += a.at(i, y) * b.at(x, i);
	}
	c.setAt(x, y, sum);
}

int main()
{

	// Matrix a{4, 4};
	// a.data = {
	// 	1.0f, 2.0f, 1.0f, 1.0f,
	// 	1.0f, 3.0f, 1.0f, 1.0f,
	// 	1.0f, 4.0f, 1.0f, 1.0f,
	// 	1.0f, 5.0f, 1.0f, 1.0f};

	// Matrix b{4, 4};
	// b.data = {
	// 	1.0f, 2.0f, 1.0f, 1.0f,
	// 	1.0f, 3.0f, 1.0f, 1.0f,
	// 	1.0f, 4.0f, 1.0f, 0.0f,
	// 	1.0f, 5.0f, 1.0f, 1.0f};

	// Matrix c{4, 4};

	Matrix matrixA{3, 2};
	matrixA.data = {
		1.0f, 1.0f, 12.0f,
		2.0f, 1.0f, 1.0f};

	Matrix matrixB{2, 3};
	matrixB.data = {
		1.0f, 1.0f,
		0.5f, 4.0f,
		1.0f, 1.0f};

	Matrix matrixC{matrixA.height, matrixB.width};

	// (Compatible Matrix dimensions are assumed)

	// Thread Pool
	std::vector<std::thread> threads{};

	for (size_t y = 0; y < matrixC.height; ++y)
	{
		for (size_t x = 0; x < matrixC.width; ++x)
		{
			threads.emplace_back(
				[=, &matrixA, &matrixB, &matrixC]()
				{
					mDot(matrixA, matrixB, matrixC, x, y);
				});
		}
	}

	cout << "Waiting for " << threads.size() << " threads to finish." << endl;


	for (auto it = threads.begin(); it != threads.end(); ++it)
	{
		it->join();
	}

	cout << "\nA:" << endl;
	matrixA.print();

	cout << "\nB:" << endl;
	matrixB.print();

	cout << "\nC:" << endl;
	matrixC.print();

	return 0;
}