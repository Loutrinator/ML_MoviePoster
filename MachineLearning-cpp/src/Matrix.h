#pragma once

#include <vector>
#include <cassert>
#include <stdexcept>

template<typename T>
class Matrix
{
public:
	Matrix(int width, int height, const T& defaultValue = {}):
		_data(width * height, defaultValue), _width(width), _height(height)
	{
	
	}
	
	int width() const
	{
		return _width;
	}
	
	int height() const
	{
		return _height;
	}
	
	T& operator()(int x, int y)
	{
		if (!isInBounds(x, y))
		{
			throw std::out_of_range("Position outside of range");
		}
		
		return _data[pos2Index(x, y)];
	}
	
	const T& operator()(int x, int y) const
	{
		if (!isInBounds(x, y))
		{
			throw std::out_of_range("Position outside of range");
		}
		
		return _data[pos2Index(x, y)];
	}
	
	std::vector<T> operator*(const std::vector<T>& vector) const
	{
		assert(vector.size() == width());
		
		std::vector<T> res(height());
		
		for (int y = 0; y < height(); y++)
		{
			T value = 0;
			for (int x = 0; x < width(); x++)
			{
				value += this->operator()(x, y) * vector[x];
			}
			res[y] = value;
		}
		
		return res;
	}
	
	const std::vector<T>& getData() const
	{
		return _data;
	}
	
	std::vector<T>& getData()
	{
		return _data;
	}

private:
	std::vector<T> _data;
	int _width;
	int _height;
	
	bool isInBounds(int x, int y) const
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
	
	int pos2Index(int x, int y) const
	{
		return y * _width + x;
	}
	
	std::pair<int, int> index2Pos(int i) const
	{
		return std::make_pair(i % _width, i / _width);
	}
};
