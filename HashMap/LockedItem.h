#pragma once

#include <memory>

typedef enum : int {
    PERM_WRITE,
    PERM_READ
} Permissions;

template<class Type>
class LockedItem
{
    static_assert(std::is_default_constructible_v<Type>, "LockedItem must be default constructable");
private:
    std::unique_ptr<Type> m_ptr{};
    int m_perms : sizeof(Permissions) = {};
    bool m_was_error{ false };

public:
    using const_this = const LockedItem<Type>;

    LockedItem() = delete;
    LockedItem(Type&&);

    void assign_permission(Permissions const&);
    void remove_permission(Permissions const&);

    bool has_permission(Permissions const&) const;
    Type const& read() const;
    bool write(Type const&) const;

    bool was_error() const noexcept;

    __forceinline const_this& const_reference() const {
        return *this;
    }
};

template<class Type>
inline LockedItem<Type>::LockedItem(Type&& val)
{
    m_ptr = std::make_unique<Type>(val);
}

template<class Type>
inline void LockedItem<Type>::assign_permission(Permissions const& perm)
{
    if (m_perms & perm) {
        return;
    }

    m_perms |= static_cast<int>(perm);
}

template<class Type>
inline void LockedItem<Type>::remove_permission(Permissions const& perm)
{
    if (m_perms & perm) {
        m_perms = (m_perms & ~perm);
    }
}

template<class Type>
inline bool LockedItem<Type>::has_permission(Permissions const& perm) const
{
    return (m_perms & perm);
}

template<class Type>
inline Type const& LockedItem<Type>::read() const
{
    if (!(m_perms & PERM_READ)) {
        return Type();
    }

    return *m_ptr;
}

template<class Type>
inline bool LockedItem<Type>::write(Type const& value) const
{
    if (!(m_perms & PERM_WRITE)) {
        return false;
    }

    *m_ptr = value;
    return true;
}

template<class Type>
inline bool LockedItem<Type>::was_error() const noexcept
{
    if (m_was_error) {
        m_was_error = false;
        return true;
    }
    return false;
}
