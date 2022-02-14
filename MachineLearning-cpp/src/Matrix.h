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
