#include <QApplication>

void startServer();
void startClient();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    startServer();

    startClient();

    return app.exec();
}
