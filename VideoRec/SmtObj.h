#ifndef SMART_OBJECT_H_
#define SMART_OBJECT_H_

#include <string>

/* Use only with 'new' memory allocation */
template <class T>
class SmtObj
{
private:
    T *_ptr;
    void (*_custom_delete)(T *p);

public:
    SmtObj() noexcept : _ptr(nullptr), _custom_delete(nullptr) {}

    SmtObj(T *ptr, void (*custom_delete)(T *p) = [](T *p)->void { delete p; }) noexcept : _ptr(ptr), _custom_delete(custom_delete) {}

    SmtObj(const SmtObj<T> &smt_obj) = delete;

    SmtObj(SmtObj<T> &&smt_obj) noexcept
    {
        std::swap(_ptr, smt_obj._ptr);
        std::swap(_custom_delete, smt_obj._custom_delete);
    }

    SmtObj<T> &operator= (SmtObj<T> &&smt_obj) noexcept
    {
        std::swap(_ptr, smt_obj._ptr);
        std::swap(_custom_delete, smt_obj._custom_delete);

        return *this;
    }

    SmtObj<T> &operator= (const SmtObj<T> &smt_obj) = delete;

    ~SmtObj() noexcept
    {
        reset();
    }

    void reset() noexcept
    {
        if (_ptr)
        {
            _custom_delete(_ptr);
            _ptr = nullptr;
            _custom_delete = nullptr;
        }
    }

    T *get() noexcept
    {
        return _ptr;
    }

    const T *get() const noexcept
    {
        return (const T *)_ptr;
    }

    T *operator-> () noexcept
    {
        return get();
    }
    const T *operator-> () const noexcept
    {
        return get();
    }

    T &operator* () noexcept
    {
        return *get();
    }
    const T &operator* () const noexcept
    {
        return (const T &)*get();
    }

    T &operator[] (size_t index) noexcept
    {
        return _ptr[index];
    }
    const T &operator[] (size_t index) const noexcept
    {
        return _ptr[index];
    }

    operator const T *() const noexcept
    {
        return _ptr;
    }
};

/* Use only with 'new' memory allocation */
template <class T>
class SmtObj<T[]>
{
private:
    T *_ptr;

public:
    SmtObj() noexcept : _ptr(nullptr) {}

    SmtObj(T *ptr) noexcept : _ptr(ptr) {}

    SmtObj(const SmtObj<T[]> &smt_obj) = delete;

    SmtObj(SmtObj<T[]> &&smt_obj) noexcept
    {
        std::swap(_ptr, smt_obj._ptr);
    }

    SmtObj<T[]> &operator= (SmtObj<T[]> &&smt_obj) noexcept
    {
        std::swap(_ptr, smt_obj._ptr);

        return *this;
    }

    SmtObj<T[]> &operator= (const SmtObj<T[]> &smt_obj) = delete;

    ~SmtObj() noexcept
    {
        reset();
    }

    void reset() noexcept
    {
        if (_ptr)
        {
            delete[] _ptr;
            _ptr = nullptr;
        }
    }

    T *get() noexcept
    {
        return _ptr;
    }

    const T *get() const noexcept
    {
        return _ptr;
    }

    T *operator-> () noexcept
    {
        return get();
    }
    const T *operator-> () const noexcept
    {
        return get();
    }

    T &operator* () noexcept
    {
        return *get();
    }
    const T &operator* () const noexcept
    {
        return *get();
    }

    T &operator[] (size_t index) noexcept
    {
        return _ptr[index];
    }
    const T &operator[] (size_t index) const noexcept
    {
        return _ptr[index];
    }

    operator const T *() const noexcept
    {
        return _ptr;
    }
};

#endif
