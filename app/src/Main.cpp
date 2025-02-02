#include <iostream>
#include <memory>
#include "MyApp.h"

int main(int argc, char** argv) {
    auto app = std::make_unique<MyApp>();

    return app->run(argc, argv);
}
