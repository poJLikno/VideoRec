#ifndef WINDOWLIB_EDIT_H
#define WINDOWLIB_EDIT_H

#include "WndBase.h"
#include "CallbackManager.h"

class Edit : public WndBase, public CallbackManager {
public:
    Edit(const WndPairValue &pos = { 0, 0 }, const WndPairValue &size = { 60, 20 });

    virtual void SetWndParent(WndBase *wnd) override;
};

#endif
