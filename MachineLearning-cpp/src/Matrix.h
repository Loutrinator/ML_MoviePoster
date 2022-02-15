#pragma once

#include <vector>
#include <cassert>
#include <stdexcept>
#include <span>

template<typename T>
class Matrix
{
public:
	Matrix(size_t width, size_t height, const T& defaultValue = {}):
		_data(width * height, defaultValue), _width(width), _height(height)
	{
	
	}
	
	size_t width() const
	{
		return _width;
	}
	
	size_t height() const
	{
		return _height;
	}
	
	T& operator()(size_t x, size_t y)
	{
#if !NDEBUG
		if (!isInBounds(x, y))
		{
			throw std::out_of_range("Position outside of range");
		}
#endif
		return getUnsafe(x, y);
	}
	
	const T& operator()(size_t x, size_t y) const
	{
#if !NDEBUG
		if (!isInBounds(x, y))
		{
			throw std::out_of_range("Position outside of range");
		}
#endif
		return getUnsafe(x, y);
	}
	
	void multiply(const std::span<T> input, std::span<T> output) const
	{
#if !NDEBUG
		if (input.size() != width())
		{
			throw std::invalid_argument("input has wrong size");
		}
		if (output.size() != height())
		{
			throw std::invalid_argument("output has wrong size");
		}
#endif
		
		for (size_t y = 0; y < height(); y++)
		{
			for (size_t x = 0; x < width(); x++)
			{
				output[y] += getUnsafe(x, y) * input[x];
			}
		}
	}
	
	const std::vector<T>& getData() const
	{
		return _data;
	}
	
	std::vector<T>& getData()
	{
		return _data;
	}

    void getCofactor(Matrix<float> mat, Matrix<float> temp, int p, int q, int n)
    {
        int i = 0, j = 0;

        // Looping for each element of the matrix
        for (int row = 0; row < n; row++)
        {
            for (int col = 0; col < n; col++)
            {
                //  Copying into temporary matrix only those element
                //  which are not in given row and column
                if (row != p && col != q)
                {
                    temp(i,j++) = mat(row,col);

                    // Row is filled, so increase row index and
                    // reset col index
                    if (j == n - 1)
                    {
                        j = 0;
                        i++;
                    }
                }
            }
        }
    }

/* Recursive function for finding determinant of matrix.
   n is current dimension of A[][]. */
    int determinant(Matrix<float> mat, int n)
    {
        int D = 0; // Initialize result

        //  Base case : if matrix contains single element
        if (n == 1)
            return mat(0,0);

        Matrix<float> temp(mat.getData().size(),mat.getData().size()); // To store cofactors

        int sign = 1;  // To store sign multiplier

        // Iterate for each element of first row
        for (int f = 0; f < n; f++)
        {
            // Getting Cofactor of A[0][f]
            getCofactor(mat, temp, 0, f, n);
            D += sign * mat(0,f) * determinant(temp, n - 1);

            // terms are to be added with alternate sign
            sign = -sign;
        }

        return D;
    }

// Function to get adjoint of A[N][N] in adj[N][N].
    void adjoint(Matrix<float> mat, Matrix<float> adj)
    {
        int N = mat.getData().size();
        if (N == 1)
        {
            adj(0,0) = 1;
            return;
        }

        // temp is used to store cofactors of A[][]
        int sign = 1, temp[N][N];

        for (int i=0; i<N; i++)
        {
            for (int j=0; j<N; j++)
            {
                // Get cofactor of A[i][j]
                getCofactor(mat, temp, i, j, N);

                // sign of adj[j][i] positive if sum of row
                // and column indexes is even.
                sign = ((i+j)%2==0)? 1: -1;

                // Interchanging rows and columns to get the
                // transpose of the cofactor matrix
                adj(j,i) = (sign)*(determinant(temp, N-1));
            }
        }
    }

// Function to calculate and store inverse, returns false if
// matrix is singular
    bool inverse(Matrix<float> mat, Matrix<float> inverse)
    {
        int N = mat.getData().size();
        int det = determinant(mat, det);
        if (det == 0)
        {
            std::cout << "Singular matrix, can't find its inverse";
            return false;
        }

        // Find adjoint
        Matrix<float> adj(N,N);
        adjoint(mat, adj);

        // Find Inverse using formula "inverse(A) = adj(A)/det(A)"
        for (int i=0; i<N; i++)
            for (int j=0; j<N; j++)
                inverse(i,j) = adj(i,j)/float(det);

        return true;
    }


private:
	std::vector<T> _data;
	size_t _width;
	size_t _height;
	
	bool isInBounds(size_t x, size_t y) const
	{
		if (x < 0 || y < 0)
		{
			return false;
		}
		
		if (x >= _width || y >= _height)
		{
			return false;
		}
		
		return true;
	}
	
	T& getUnsafe(size_t x, size_t y)
	{
		return _data[pos2Index(x, y)];
	}
	
	const T& getUnsafe(size_t x, size_t y) const
	{
		return _data[pos2Index(x, y)];
	}
	
	size_t pos2Index(size_t x, size_t y) const
	{
		return y * _width + x;
	}
};
