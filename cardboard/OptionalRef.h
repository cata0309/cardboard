#ifndef CARDBOARD_OPTIONAL_REF_H_INCLUDED
#define CARDBOARD_OPTIONAL_REF_H_INCLUDED

#include <cassert>
#include <utility>

#include "NotNull.h"

/**
 * \brief Represents a reference that may or may not be null.
 *
 * You must check for the reference to not be null (with OptionalRef::has_value)
 * before calling OptionalRef::unwrap to get the reference.
 *
 * You can also run a function with the reference if it is not null, see OptionalRef::and_then
 * and OptionalRef::or_else.
 */
template <typename T>
class OptionalRef {
public:
    OptionalRef()
        : ptr(nullptr)
    {
    }
    OptionalRef(T& ref)
        : ptr(&ref)
    {
    }
    explicit OptionalRef(T* ptr)
        : ptr(ptr)
    {
    }

    explicit OptionalRef(std::nullptr_t)
        : ptr(nullptr)
    {
    }

    T& unwrap()
    {
        assert(ptr != nullptr);

        return *ptr;
    }

    /// Returns the reference. An assertion error will be generated if the reference is null.
    const T& unwrap() const
    {
        assert(ptr != nullptr);

        return *ptr;
    }

    /// Returns true if the reference is not null.
    bool has_value() const noexcept
    {
        return ptr != nullptr;
    }

    /// Returns true if the reference is not null.
    explicit operator bool() const noexcept
    {
        return has_value();
    }

    /// Checks if both OptionalRefs point to the same value, or if they are both null.
    bool operator==(const OptionalRef<T>& other) const
    {
        return ptr == other.ptr;
    }

    /// Checks if the two OptionalRefs point to different values, or if they are not both null.
    bool operator!=(const OptionalRef<T>& other) const
    {
        return ptr != other.ptr;
    }

    /**
      * \brief Calls and returns the return value of \a f if the reference is not null.
      *
      * \a f is a callable with the type <tt>OptionalRef<U>(T&)</tt>.
      */
    template <typename U, typename F>
    OptionalRef<U> and_then(F&& f)
    {
        if (has_value()) {
            return f(unwrap());
        }

        return OptionalRef<U>(nullptr);
    }

    /**
     * \brief Calls \a f if the reference is not null.
     *
     * \a f is a callable with the type <tt>void(T&)</tt>.
     */
    template <typename F>
    void and_then(F&& f)
    {
        if (has_value()) {
            f(unwrap());
        }
    }

    /**
     * \brief Calls \a f if the reference is null and returns its return value.
     *
     * \a f is a callable with the type <tt>OptionalRef<T>(T&)</tt>.
     * */
    template <typename F>
    OptionalRef<T> or_else(F&& f)
    {
        if (!has_value()) {
            return f();
        }

        return *this;
    }

    /**
     * \brief Gets the raw_pointer, could be useful. Doesn't check for nullptr.
     */
    T* raw_pointer()
    {
        return ptr;
    }

    const T* raw_pointer() const
    {
        return ptr;
    }

private:
    T* ptr;
};

/// The null OptionalRef.
template <typename T>
const auto NullRef = OptionalRef<T>(nullptr);

template <typename T>
OptionalRef(NotNullPointer<T> nnptr)->OptionalRef<T>;

#endif // CARDBOARD_OPTIONAL_REF_H_INCLUDED
