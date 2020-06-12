#pragma once

#include <iostream>
#include <utility>
#include <array>

template<typename T>
class ManagedArray {
    static constexpr size_t SmallBufferSize = (2 * sizeof(T*)) / sizeof(T);
    union {
        T* _begin;
        T _bytes[SmallBufferSize];
    };
    size_t _size;
    size_t _capacity;//0 if small buffer is used


    // constexpr ManagedArray(T* first, size_t sz)
    //         : first(first), last(first + sz) {}
    // constexpr ManagedArray(T* first, T* last)
    //         : first(first), last(last) {}
public:
    ManagedArray() : _bytes{}, _size(0), _capacity(0) {}

    //for small buffer
    constexpr ManagedArray(std::array<T, SmallBufferSize> buf, size_t size)
        : _size(size), _capacity(0) {
        for (size_t i = 0; i < size; ++i)
            _bytes[i] = buf[i];
    }
    explicit ManagedArray(size_t sz) {
        if (sz < SmallBufferSize) {
            new (this) ManagedArray({}, sz);
        }
        else {
            _begin = new T[sz];
            _capacity = _size = sz;
        }
    }

    ~ManagedArray() {
        if ()
        delete[] this->first;
    }
    constexpr static ManagedArray CreateSmallBuffer(size_t sz) {
        
    }


    ManagedArray(std::nullptr_t) : ManagedArray() {}
    ManagedArray(std::initializer_list<T> l) : ManagedArray(l.size()) {
        T* it = this->first;
        for (auto& v: l) *it++ = v;
    }
    ManagedArray(const ManagedArray& rhs) : ManagedArray(rhs.size()) {
        T* it = this->first;
        for (auto& v: rhs) *it++ = v;
    }
    ManagedArray(ManagedArray&& rhs)
            : ManagedArray(std::exchange(rhs.first, nullptr),
                           std::exchange(rhs.last, nullptr)) {}

    ManagedArray& operator=(const ManagedArray& rhs) {
        if (rhs.size() <= this->size()) {
            this->last = this->first + rhs.size();
        } else {
            T* tmp = new T[rhs.size()];
            this->~ManagedArray();
            this->first = tmp;
            this->last = tmp+rhs.size();
        }

        for (size_t i = 0; i < this->size(); ++i) {
            this->first[i] = rhs[i];
        }
        return *this;
    }
    ManagedArray& operator=(ManagedArray&& rhs) {
        this->~ManagedArray();

        this->first = std::exchange(rhs.first, nullptr);
        this->last = std::exchange(rhs.last, nullptr);

        return *this;
    }


    void resize(size_t sz) {
        if (sz <= this->size()) {
            this->last = this->first + sz;
            return;
        }
        T* tmp = new T[sz];
        for (size_t i = 0; i < this->size(); ++i) {
            tmp[i] = (*this)[i];
        }
        this->~ManagedArray();
        this->first = tmp;
        this->last = tmp+sz;
    }

    constexpr T* begin() { return first; }
    constexpr T* end() { return last; }

    constexpr const T* begin() const { return first; }
    constexpr const T* end() const { return last; }

    constexpr auto rbegin() { return std::reverse_iterator<T*>(last); }
    constexpr auto rend() { return std::reverse_iterator<T*>(first); }

    constexpr auto rbegin() const { return std::reverse_iterator<const T*>(last); }
    constexpr auto rend() const { return std::reverse_iterator<const T*>(first); }

    constexpr T& front() { return *first; }
    constexpr T& back() { return *(last-1); }
    constexpr const T& front() const { return *first; }
    constexpr const T& back() const { return *(last-1); }

    constexpr operator bool() {
        return first != nullptr;
    }

    friend std::ostream& operator<<(std::ostream& s, ManagedArray v) {
        s << "(";
        if (v.first != nullptr) {
            T* it = v.first;
            for (T* end = v.last - 1; it < end; ++it)
                s << *it << ", ";

            if (it < v.last) s << *it;
        }
        return s << ")";
    }
    constexpr const T& operator[](size_t i) const { return first[i]; }
    constexpr T& operator[](size_t i) { return first[i]; }

    constexpr size_t size() const { return last - first; }

    constexpr T* find(const T& val) {
        for (auto it = first; it != last; ++it) {
            if (*it == val) return it;
        }
        return nullptr;
    }

    // P is a predicate on T
    template<typename P>
    constexpr T* find_if(P p) {
        for (auto it = first; it != last; ++it) {
            if (p(it->val)) return it;
        }
        return nullptr;
    }

    constexpr bool contains(const T& val) const { return find(val) != nullptr; }

    template<typename P>
    constexpr T* contains_if(P p) const { return find(p) != nullptr; }
};

// enum class BaseVectorType {
//     Resizeable, NonResizeable
// };

// template<typename T, BaseVectorType Type>
// struct BaseVector : public ManagedPtrRange<T> {
//     T* allocEnd;
//     constexpr size_t capacity() const { return allocEnd - this->first; }
//     BaseVector(size_t capacity, size_t sz)
//             : ManagedPtrRange<T>(capacity) {
//         assert(capacity > 0, "can't have zero capacity");
//         allocEnd = this->last;
//         this->last = this->first+sz;
//     }
//     BaseVector(const BaseVector& rhs) : BaseVector(rhs.capacity(), rhs.size()) {
//         T* it = this->first;
//         for (auto& v: rhs) *it++ = v;
//     }

//     explicit BaseVector(size_t capacity) : BaseVector(capacity, 0) {}
//     BaseVector() : ManagedPtrRange<T>(nullptr) {}
//     BaseVector(std::nullptr_t)
//             : ManagedPtrRange<T>(nullptr), allocEnd(nullptr) {}
//     BaseVector(std::initializer_list<T> l)
//             : ManagedPtrRange<T>(l), allocEnd(this->last) {}

//     void push_back(const T& val) {
//         if constexpr (Type == BaseVectorType::Resizeable) {
//             if (this->first == nullptr) {
//                 new (this) BaseVector<T, Type>(2);
//             }
//             else if (this->last >= allocEnd) {
//                 size_t newCapacity = capacity() *2;
//                 T* newData = new T[newCapacity];
//                 size_t sz = this->size();
//                 for (size_t i = 0; i < sz; ++i) {
//                     newData[i] = std::move(this->first[i]);
//                 }
//                 delete this->first;
//                 this->first = newData;
//                 this->last = newData + sz;
//                 this->allocEnd = newData + newCapacity;
//             }
//         } else {
//             assert(this->last != allocEnd, "Overflow");
//         }
//         *(this->last++) = val;
//     }
//     constexpr bool empty() const {return this->size() == 0; }
//     void insert(T* pos, const T& val) {
//         assert(this->last != allocEnd, "Overflow");
//         for (auto it = this->last-1; it != pos; --it)
//             *it = *(it-1);
//         ++this->last;
//         *pos = val;
//     }
//     void remove(T* pos) {
//         assert(this->last != this->first, "Empty vector");
//         for (auto it = pos; it != this->last; ++it)
//             *it = *(it+1);
//         --this->last;
//     }

//     void pop_front() { remove(this->first); }
//     void pop_back() {
//         assert(this->last != this->first, "Empty vector");
//         --this->last;
//     }
//     T& front() { return *(this->first); }
//     const T& front() const { return *(this->first); }
//     T& back() { return *(this->last-1); }
//     const T& back() const { return *(this->last-1); }
// };

// template<typename T>
// using StaticVector = BaseVector<T, BaseVectorType::NonResizeable>;

// template<typename T>
// using Vector = BaseVector<T, BaseVectorType::Resizeable>;
