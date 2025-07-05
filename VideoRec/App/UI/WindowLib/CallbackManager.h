#ifndef CALLBACK_MANAGER_H_
#define CALLBACK_MANAGER_H_

#include <string.h>
#include <memory>
#include <vector>

/* For main callbacks */
#ifndef GetControl
#define GetControl(type, ptr) ((type *)ptr)
#endif

/* For parent resize callback only */
#ifndef GetControlForParentResize
#define GetControlForParentResize(type, ptr) ((type *)((ParentResizeCallbackParams *)ptr)->wnd)
#endif

/* For parent resize callback only */
#ifndef GetMiscForParentResize
#define GetMiscForParentResize(ptr) \
    Window *parent_wnd = (Window *)GetControlForParentResize(WndBase, ptr)->GetWndParent();\
    const std::pair<int, int> &old_parent_wnd_size = (const std::pair<int, int> &)((ParentResizeCallbackParams *)ptr)->old_size;\
    const std::pair<int, int> &new_parent_wnd_size = parent_wnd->GetWndSize()
#endif

// Do callback functions
class ICall
{
public:
    virtual void Call(void *) = 0;
    virtual ~ICall() = default;
};

template<class T>
class CCall : public ICall
{
private:
    T _callback;

public:
    CCall(T callback) : _callback(callback) {}

    virtual void Call(void *params) override
    {
        _callback(params);
    }
};

class Callback
{
private:
    std::unique_ptr<ICall> _callback;

public:
    Callback() = default;
    ~Callback() = default;

    // Delete copy semantic
    Callback(const Callback &) = delete;
    Callback &operator=(const Callback &) = delete;

    // Init move semantic
    Callback(Callback &&obj) noexcept
    {
        _callback.swap(obj._callback);
    }
    Callback &operator=(Callback &&obj) noexcept
    {
        _callback.swap(obj._callback);

        return *this;
    }

    // Init callback constructors
    template<class T>
    Callback(T callback)
    {
        if (_callback.get())
        {
            _callback.reset();
        }

        _callback = std::unique_ptr<ICall>(new CCall(callback));
    }

    template<class T>
    Callback &operator=(T callback)
    {
        if (_callback.get())
        {
            _callback.reset();
        }

        _callback = std::unique_ptr<ICall>(new CCall(callback));

        return *this;
    }

    // Call callback
    void operator()(void *params)
    {
        _callback->Call(params);
    }

    // Check callback
    bool HasCallback()
    {
        return (_callback.get() ? true : false);
    }
};

class CallbackManager
{
protected:
    std::vector<char *> _callbacks_names = std::vector<char *>();
    std::vector<Callback> _callbacks = std::vector<Callback>();

    CallbackManager() = default;

public:
    virtual ~CallbackManager() = default;

    template<class T>
    void AddCallback(const char *name, T callback)
    {
        _callbacks_names.push_back((char *)name);
        _callbacks.push_back(callback);
    }

    virtual void operator()(const char* name, void *params) final
    {
        for (int i = 0; i < _callbacks_names.size(); ++i)
        {
            if (memcmp(name, _callbacks_names[i], strlen(name)) == 0)
            {
                _callbacks[i](params);
                break;
            }
        }
    }

    virtual bool HasCallback(const char *name) final
    {
        for (int i = 0; i < _callbacks_names.size(); ++i)
        {
            if (memcmp(name, _callbacks_names[i], strlen(name)) == 0)
            {
                return _callbacks[i].HasCallback();
            }
        }

        return false;
    }
};

#endif /* CALLBACK_MANAGER_H_ */
