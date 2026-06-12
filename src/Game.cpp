#include "Core/Application.h"
#include "Core/Assets.h"

int main() {

    Lgt::Application app;

    app.Init();    
    app.Run();
    app.Shutdown();

    return 0;
}
