#ifndef APP_H_
#define APP_H_

#include "../SmtObj.h"

#include "Model/Model.h"
#include "UI/UI.h"

class App
{
private:
    SmtObj<UI> _ui;
    SmtObj<Model> _model;

public:
    App(const char *app_name, const char *app_version);
    App(const App &) = delete;
    ~App() = default;

    int Run();
};

#endif /* APP_H_ */
