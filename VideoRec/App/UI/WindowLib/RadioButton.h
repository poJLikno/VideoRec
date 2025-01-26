#ifndef WINDOWLIB_RADIO_BUTTON_H_
#define WINDOWLIB_RADIO_BUTTON_H_

#include "Button.h"

class RadioButton : public Button
{
private:
    bool _new_group = false;

public:
    RadioButton(const char *text, const bool &create_new_group,
        const WndPairValue &pos = { 0, 0 },
        const WndPairValue &size = { 20, 20 });

    virtual void SetWndParent(WndBase *wnd) override;

    virtual bool GetState() const;
    virtual void SetState(const bool &flag);
};

#endif /* WINDOWLIB_RADIO_BUTTON_H_ */
