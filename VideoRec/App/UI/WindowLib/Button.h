#ifndef WINDOWLIB_BUTTON_H
#define WINDOWLIB_BUTTON_H

#include "WndBase.h"
#include "CallbackManager.h"

class Button : public WndBase, public CallbackManager {
protected:
    const char *_text;

public:
    Button(const char *text,
        const WndPairValue &pos = { 0, 0 },
        const WndPairValue &size = { 20, 20 });

    virtual void SetWndParent(WndBase *wnd) override;
};

#endif
