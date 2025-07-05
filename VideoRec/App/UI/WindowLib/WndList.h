#ifndef WINDOWLIB_WND_LIST_H_
#define WINDOWLIB_WND_LIST_H_

#include <iostream>
#include <vector>
#include <stdlib.h>

template<class T>
class WndList
{
private:
    int _wnd_count_deleted = 0;
    std::vector<T *> _wnd_list = std::vector<T *>();

public:
    WndList() = default;
    ~WndList();

    T *&operator[](int i);

    int GetCount();
    int GetListLength();

    int Append(T *wnd);/* Return new index of window */

    void Delete(const int &index);
};

template<class T>
inline WndList<T>::~WndList()
{
    for (int i = 0; i < _wnd_list.size(); ++i)
    {
        _wnd_list[i] = nullptr;
    }
}

template<class T>
inline T *&WndList<T>::operator[](int i)
{
    return _wnd_list[i];
}

template<class T>
inline int WndList<T>::GetCount()
{
    return (int)_wnd_list.size() - _wnd_count_deleted;
}

template<class T>
inline int WndList<T>::GetListLength()
{
    return (int)_wnd_list.size();
}

template<class T>
inline int WndList<T>::Append(T *wnd)
{
    _wnd_list.push_back(wnd);

    return (int)_wnd_list.size() - 1;
}

template<class T>
inline void WndList<T>::Delete(const int &index)
{
    if (_wnd_list[index])
    {
        _wnd_list[index] = nullptr;
        ++_wnd_count_deleted;
    }
}

#endif /* WINDOWLIB_WND_LIST_H_ */
