#ifndef WINDOWLIB_EDIT_H_
#define WINDOWLIB_EDIT_H_

#include "WndBase.h"
#include "CallbackManager.h"

class Edit : public WndBase, public CallbackManager
{
public:
    Edit(const std::pair<int, int> &pos = { 0, 0 }, const std::pair<int, int> &size = { 60, 20 });

    virtual void SetWndParent(WndBase *wnd) override;
};

#endif /* WINDOWLIB_EDIT_H_ */
