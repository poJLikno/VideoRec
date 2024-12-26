#ifndef SMART_OBJECT_H_
#define SMART_OBJECT_H_

#include <string>

template <class T>
class SmtObj/* Use only with 'new' memory allocation */
{
private:
    T *_ptr = nullptr;

public:
    SmtObj() {}
    SmtObj(T *ptr) : _ptr(ptr) {}
    SmtObj(const SmtObj<T> &smt_obj) = delete;
    SmtObj(SmtObj<T> &&smt_obj)
    {
        std::swap(_ptr, smt_obj._ptr);
    }

    SmtObj<T> &operator= (SmtObj<T> &&smt_obj)
    {
        std::swap(_ptr, smt_obj._ptr);

        return *this;
    }

    ~SmtObj()
    {
        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }
    }

    T *Ptr()
    {
        return _ptr;
    }

    T &Ref()
    {
        if (!_ptr)
        {
            throw std::string("SmtObj isn't initialazed!");
        }
        return *_ptr;
    }
};

#endif
