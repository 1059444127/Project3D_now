#include "Application.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    Application app(argc, argv);
    return app.exec();
}
