#ifndef __RECYCLER_BUFFER_HPP__
#define __RECYCLER_BUFFER_HPP__

// ─────────────────────────────────────────────────────────────
//                  INCLUDE
// ─────────────────────────────────────────────────────────────

// C++ Headers
#include <cstddef>
#include <cstdint>
#include <memory>
#include <cstring>

// ─────────────────────────────────────────────────────────────
//                  DECLARATION
// ─────────────────────────────────────────────────────────────

namespace recycler {

// ─────────────────────────────────────────────────────────────
//                  CLASS
// ─────────────────────────────────────────────────────────────

template<typename T>
class Buffer
{
    // ──────── ATTRIBUTES ────────────
private:
    std::unique_ptr<T[]> _buffer;
    std::size_t _length = 0;
    std::size_t _maxSize = 0;

    // ──────── CONSTRUCTOR ────────────
public:
    Buffer(std::size_t length = 0, bool clearBuffer = true)
    {
        reset(length, clearBuffer);
    }

    Buffer(std::initializer_list<T> l) { reset(l); }

    bool reset(std::size_t length, bool clearBuffer = true)
    {
        resize(length);
        if(clearBuffer)
        {
            for(std::size_t i = 0; i < _length; ++i) { _buffer[i] = {}; }
        }
        return true;
    }

    bool reset(std::initializer_list<T> l)
    {
        // Allocate required buffer
        if(!reset(l.size()))
            return false;

        // Then copy initializer_list to out buffer
        auto it = l.begin();
        for(std::size_t i = 0; i < _length; ++i) { _buffer[i] = *it++; }
        return true;
    }

    // ──────── API ────────────
public:
    T* buffer() { return _buffer.get(); }

    const T* buffer() const { return _buffer.get(); }

    std::size_t length() const { return _length; }

    std::size_t size() const { return length(); }

    bool empty() const { return length() == 0; }

    std::size_t maxSize() const { return _maxSize; }

    void release()
    {
        if(_length != _maxSize)
        {
            if(_length)
            {
                _buffer = std::make_unique<T[]>(_length);
                _maxSize = _length;
            }
            else
                reset(0);
        }
    }

    bool resize(std::size_t length)
    {
        if(length == 0)
        {
            _length = 0;
            _maxSize = 0;
            _buffer = nullptr;
            return true;
        }

        if(!_buffer || _maxSize < length)
        {
            _buffer = std::make_unique<T[]>(length);
            _length = length;
            _maxSize = length;
        }
        else if(length != _length)
        {
            _length = length;
        }

        return true;
    }

    void clear() { reset(0); }

    // ──────── ACCESSOR ────────────
public:
    T& operator[](const std::size_t offset) { return _buffer[offset]; }

    const T& operator[](const std::size_t offset) const
    {
        return _buffer[offset];
    }

    operator T*() { return _buffer.get(); }
    operator const T*() const { return _buffer.get(); }

    // ──────── ITERATOR ────────────
public:
    class iterator
    {
        T* _ptr = nullptr;

    public:
        iterator(T* ptr) : _ptr(ptr) {}

        iterator(const iterator& other) : _ptr(other._ptr) {}

        iterator(iterator&& other) noexcept : _ptr(other._ptr) {}

        iterator& operator=(const iterator& other)
        {
            if(this == &other)
                return *this;
            _ptr = other._ptr;
            return *this;
        }

        iterator& operator=(iterator&& other) noexcept
        {
            if(this == &other)
                return *this;
            _ptr = other._ptr;
            return *this;
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return lhs._ptr == rhs._ptr;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return !(lhs == rhs);
        }

        iterator& operator+=(const std::ptrdiff_t& diff)
        {
            _ptr += diff;
            return (*this);
        }
        iterator& operator-=(const std::ptrdiff_t& diff)
        {
            _ptr -= diff;
            return (*this);
        }
        iterator& operator++()
        {
            ++_ptr;
            return (*this);
        }
        iterator& operator--()
        {
            --_ptr;
            return (*this);
        }
        iterator operator++(int)
        {
            auto temp(*this);
            ++_ptr;
            return temp;
        }
        iterator operator--(int)
        {
            auto temp(*this);
            --_ptr;
            return temp;
        }
        iterator operator+(const std::ptrdiff_t& diff)
        {
            auto oldPtr = _ptr;
            _ptr += diff;
            auto temp(*this);
            _ptr = oldPtr;
            return temp;
        }
        iterator operator-(const std::ptrdiff_t& diff)
        {
            auto oldPtr = _ptr;
            _ptr -= diff;
            auto temp(*this);
            _ptr = oldPtr;
            return temp;
        }
        std::ptrdiff_t operator-(const iterator& it)
        {
            return std::distance(it._ptr, this->_ptr);
        }

        T& operator*() { return *_ptr; }
        const T& operator*() const { return *_ptr; }
        T* operator->() { return _ptr; }
    };

    class const_iterator
    {
        const T* _ptr = nullptr;

    public:
        const_iterator(T* ptr) : _ptr(ptr) {}

        const_iterator(const const_iterator& other) : _ptr(other._ptr) {}

        const_iterator(const_iterator&& other) noexcept : _ptr(other._ptr) {}

        const_iterator& operator=(const const_iterator& other)
        {
            if(this == &other)
                return *this;
            _ptr = other._ptr;
            return *this;
        }

        const_iterator& operator=(const_iterator&& other) noexcept
        {
            if(this == &other)
                return *this;
            _ptr = other._ptr;
            return *this;
        }

        friend bool operator==(
            const const_iterator& lhs, const const_iterator& rhs)
        {
            return lhs._ptr == rhs._ptr;
        }

        friend bool operator!=(
            const const_iterator& lhs, const const_iterator& rhs)
        {
            return !(lhs == rhs);
        }

        const_iterator& operator+=(const std::ptrdiff_t& diff)
        {
            _ptr += diff;
            return (*this);
        }
        const_iterator& operator-=(const std::ptrdiff_t& diff)
        {
            _ptr -= diff;
            return (*this);
        }
        const_iterator& operator++()
        {
            ++_ptr;
            return (*this);
        }
        const_iterator& operator--()
        {
            --_ptr;
            return (*this);
        }
        const_iterator operator++(int)
        {
            auto temp(*this);
            ++_ptr;
            return temp;
        }
        const_iterator operator--(int)
        {
            auto temp(*this);
            --_ptr;
            return temp;
        }
        const_iterator operator+(const std::ptrdiff_t& diff)
        {
            auto oldPtr = _ptr;
            _ptr += diff;
            auto temp(*this);
            _ptr = oldPtr;
            return temp;
        }
        const_iterator operator-(const std::ptrdiff_t& diff)
        {
            auto oldPtr = _ptr;
            _ptr -= diff;
            auto temp(*this);
            _ptr = oldPtr;
            return temp;
        }
        std::ptrdiff_t operator-(const const_iterator& it)
        {
            return std::distance(it._ptr, this->_ptr);
        }

        const T& operator*() { return *_ptr; }
        const T* operator->() { return _ptr; }
    };

    iterator begin() { return iterator(&_buffer.get()[0]); }
    iterator end() { return iterator(&_buffer.get()[_length]); }
    const_iterator cbegin() const { return const_iterator(&_buffer.get()[0]); }
    const_iterator cend() const
    {
        return const_iterator(&_buffer.get()[_length]);
    }
};

}

#endif
