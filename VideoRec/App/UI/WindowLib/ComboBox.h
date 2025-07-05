#ifndef WINDOWLIB_COMBO_BOX_H_
#define WINDOWLIB_COMBO_BOX_H_

#include "WndBase.h"
#include "CallbackManager.h"

class ComboBox : public WndBase, public CallbackManager
{
private:
    using WndBase::GetWndText;
    using WndBase::SetWndText;

public:
    ComboBox(const std::pair<int, int> &pos = { 0, 0 }, const std::pair<int, int> &size = { 20, 20 });

    virtual void SetWndParent(WndBase *wnd) override;

    void AddItem(const char *text);

    void SelectItem(const int &id);

    int GetItemId();

    /* Number of symbols (wide char) without null ending */
    int GetItemLength();

    /* Need free after use (delete[] ...) */
    char *GetItem(int &bytes_were_written);

    /* Need free after use (delete[] ...) */
    char *GetItem();

    /* Need free after use (delete[] ...) */
    char *GetOldItem(const int &max_symbols_count);
};

#endif /* WINDOWLIB_COMBO_BOX_H_ */
