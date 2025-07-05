#ifndef WINDOWLIB_MENU_POINT_H_
#define WINDOWLIB_MENU_POINT_H_

#include <stdint.h>
#include <windows.h>
#include "TextUtils.h"
#include "CallbackManager.h"

enum MenuPointType
{
    Enabled = MF_ENABLED,
    Checked = MF_CHECKED
};

class MenuPoint : public CallbackManager
{
private:
    static uint32_t ID_COUNTER;

    HMENU _parent_hmenu = { 0 };

    const char *_text;
    bool _state = true;
    unsigned int _id = 0;
    unsigned int  _type = 0;

public:
    MenuPoint(const char *text, const MenuPointType &menu_point_type = MenuPointType::Enabled);

    void SetMenuParent(HMENU hmenu);

    unsigned int GetId() const;
    bool GetState() const;
    void SetState(const bool &flag);
};

#endif /* WINDOWLIB_MENU_POINT_H_ */
