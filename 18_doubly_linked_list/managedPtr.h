#pragma once

#include <utility>
#include <iostream>

template<typename Base, typename T>
concept BaseOf = std::is_base_of_v<Base, T>;

template<typename V>
class ManagedPtr {
    V* ptr = nullptr;
public:
    ManagedPtr() {}

    template<typename T> requires BaseOf<V, T>
    ManagedPtr(T&& arg) : ptr(new T(std::forward<T>(arg))) {}

    template<typename... Args>
    ManagedPtr(Args&& ... args) : ptr(new V(std::forward<Args>(args)...)) {}

    ManagedPtr(std::nullptr_t) : ptr(nullptr) {}
    ManagedPtr(ManagedPtr&& rhs) : ptr(std::exchange(rhs.ptr, nullptr)) {}

    ManagedPtr& operator=(ManagedPtr&& rhs) {
        delete ptr;
        ptr = std::exchange(rhs.ptr, nullptr);
        return *this;
    }

    // ManagedPtr(const ManagedPtr& rhs) : ptr(new V(*rhs)) {}
    // ManagedPtr& operator=(const ManagedPtr& rhs) {
    //     if (!ptr) ptr = new ManagedPtr(*rhs);
    //     else {
    //         ptr->~V();
    //         *ptr = *rhs;
    //     }
    //     return *this;
    // }

    template <typename T>
    T* as() {
        return dynamic_cast<T*>(ptr);
    }

    template <typename T>
    bool is() const {
        return dynamic_cast<const T*>(ptr);
    }

    ~ManagedPtr() noexcept {
        // if (ptr)
        //     std::cout << "Del\n";
        delete ptr;
    }

    friend bool operator !=(std::nullptr_t, const ManagedPtr& v) { return v.ptr != nullptr; }
    friend bool operator !=(const ManagedPtr& v, std::nullptr_t) { return v.ptr != nullptr; }
    friend bool operator ==(std::nullptr_t, const ManagedPtr& v) { return v.ptr == nullptr; }
    friend bool operator ==(const ManagedPtr& v, std::nullptr_t) { return v.ptr == nullptr; }

    V& operator*()  { return *ptr; }
    V* operator->() { return ptr; }
    V* operator&()  { return ptr; }
    const V& operator*() const  { return *ptr; }
    const V* operator&() const  { return ptr; }
    const V* operator->() const { return ptr; }
    operator bool() const { return ptr; }

    friend std::ostream& operator<<(std::ostream& s, const ManagedPtr& ptr) {
        if (ptr) return s << *ptr;
        return s << "null";
    }

};
