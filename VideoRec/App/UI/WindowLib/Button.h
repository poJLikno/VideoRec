#ifndef WINDOWLIB_BUTTON_H_
#define WINDOWLIB_BUTTON_H_

#include "WndBase.h"
#include "CallbackManager.h"

class Button : public WndBase, public CallbackManager
{
protected:
    const char *_text;

public:
    Button(
        const char *text,
        const std::pair<int, int> &pos = { 0, 0 },
        const std::pair<int, int> &size = { 20, 20 });

    virtual void SetWndParent(WndBase *wnd) override;
};

#endif /* WINDOWLIB_BUTTON_H_ */
