#pragma once

template <typename T>
class MeanFilter {
public:
    MeanFilter(const size_t windowSize)
        : _windowSize(windowSize), _count(0), _index(0), _sum(0)
    {
        _items = new T[windowSize]();
    }

    ~MeanFilter() { delete[] _items; }

    T AddValue(const T value)
    {
        if (_count >= _windowSize)
            _sum -= _items[_index];
        else
            ++_count;

        _items[_index] = value;
        _sum += value;

        if (++_index >= _windowSize)
            _index = 0;

        return static_cast<T>(_sum / _count);
    }

    T GetFiltered() const { return static_cast<T>(_sum / _count); }

private:
    T *_items;
    size_t _windowSize;
    size_t _count;
    size_t _index;
    T _sum;
};
