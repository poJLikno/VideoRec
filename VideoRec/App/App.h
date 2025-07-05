#ifndef APP_H_
#define APP_H_

#include "Model/Model.h"
#include "UI/UI.h"

class App
{
private:
    std::unique_ptr<UI> _ui;
    std::unique_ptr<Model> _model;

public:
    App(const char *app_name, const char *app_version);
    App(const App &) = delete;
    ~App();

    int Run();
};

#endif /* APP_H_ */
