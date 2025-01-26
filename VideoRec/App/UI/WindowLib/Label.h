#ifndef WINDOWLIB_LABEL_H
#define WINDOWLIB_LABEL_H

#include "WndBase.h"
#include "CallbackManager.h"

class Label : public WndBase, public CallbackManager {
private:
    const char *_text;

public:
    Label(const char *text, const WndPairValue &pos = { 0, 0 }, const WndPairValue &size = { 20, 20 });

    virtual void SetWndParent(WndBase *wnd) override;
};

#endif
