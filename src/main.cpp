#include "app/Application.h"

#include <windows.h>

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, PWSTR, int) {
    app::Application app(inst);
    return app.Run();
}
