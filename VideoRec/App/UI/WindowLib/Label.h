#ifndef WINDOWLIB_LABEL_H_
#define WINDOWLIB_LABEL_H_

#include "WndBase.h"
#include "CallbackManager.h"

class Label : public WndBase, public CallbackManager
{
private:
    const char *_text;

public:
    Label(const char *text, const std::pair<int, int> &pos = { 0, 0 }, const std::pair<int, int> &size = { 20, 20 });

    virtual void SetWndParent(WndBase *wnd) override;
};

#endif /* WINDOWLIB_LABEL_H_ */
