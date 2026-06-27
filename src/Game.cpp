#include "Core/Application.h"

int main() {

    Lgt::Application app;

    app.Init();
    app.Run();
    app.Shutdown();

    return 0;
}
