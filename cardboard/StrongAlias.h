#ifndef BUILD_CARDBOARD_STRONGALIAS_H
#define BUILD_CARDBOARD_STRONGALIAS_H

#include <utility>

template <typename T, typename Tag>
class StrongAlias
{
public:
    explicit StrongAlias(const T& value): m_value{value} {};
    explicit StrongAlias(T&& value): m_value{std::move(value)} {};
    explicit StrongAlias(): m_value{} {}

    T& get() { return m_value; }
    const T& get() const { return m_value; }

    explicit operator T() { return m_value; }
    explicit operator T() const { return m_value; }

    auto& operator=(const T& t)
    {
        m_value = t;

        return *this;
    }

    auto& operator=(T&& t)
    {
        m_value = std::move(t);

        return *this;
    }

private:
    T m_value;
};



#endif //BUILD_CARDBOARD_STRONGALIAS_H
