#include "applicationcontext.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication eventLoop(argc, argv);
    ApplicationContext::GetContext()->SetApplicationMainEventLoop(&eventLoop);
    return eventLoop.exec();
}
