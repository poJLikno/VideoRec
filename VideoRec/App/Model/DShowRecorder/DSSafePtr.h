#ifndef DS_SAFE_PTR_H
#define DS_SAFE_PTR_H

#include <dshow.h>

#pragma comment(lib, "strmiids.lib")

template<class T>
class DSSafePtr
{
public:
    T *object = nullptr;

    ~DSSafePtr()
    {
        DSSafePtr::SafeRelease();
    }

    void SafeRelease()
    {
        if (object)
        {
            object->Release();
            object = nullptr;
        }
    }
};

#endif
