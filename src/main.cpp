#include "./application.hpp"

int main() {
    Application app;

    if (!app.start()) {
        return -1;
    } else {
        return 0;
    }
}
