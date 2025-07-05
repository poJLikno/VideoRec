#ifndef WINDOWLIB_CHECK_BOX_H_
#define WINDOWLIB_CHECK_BOX_H_

#include "Button.h"

class CheckBox : public Button
{
public:
    CheckBox(
        const char *text,
        const std::pair<int, int> &pos = { 0, 0 },
        const std::pair<int, int> &size = { 20, 20 });

    virtual void SetWndParent(WndBase *wnd) override;

    virtual bool GetState() const;
    virtual void SetState(const bool &flag);
};

#endif /* WINDOWLIB_CHECK_BOX_H_ */
