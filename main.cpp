#include <QApplication>
#include <QMessageBox>

bool startServer();
void startClient();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!startServer()) {
        QMessageBox::critical(nullptr, "خطا", "سرور راه‌اندازی نشد؛ برنامه بسته می‌شود.");
        return 1;
    }

    startClient();

    return app.exec();
}
