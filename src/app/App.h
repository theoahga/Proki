#pragma once

#include <GLFW/glfw3.h>

#include "ui/Ui.h"
#include "ui/process/ProcessUI.h"


class App {
public:
    App();
    ~App();
    void run();

private:
    GLFWwindow* window = nullptr;
    UI *ui = new UI();
    void init();
    void cleanup();
    void renderUI();
};
