#include <QApplication>
#include <QMessageBox>
#include <QString>

bool startServer();
void startClient();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // تعریف استایل‌شیت کلی (Global QSS) برای تم صورتی و کرم
    QString mainStyleSheet = R"(
        /* پس‌زمینه اصلی تمام پنجره‌ها و دیالوگ‌ها */
        QMainWindow, QDialog {
            background-color: #FDF8F5;
            color: #2C2C2C;
            font-family: "Segoe UI", "Tahoma", sans-serif;
            font-size: 13px;
        }

        /* پس‌زمینه ویجت‌ها و پنل‌های داخلی */
        QWidget#centralWidget, QFrame, QGroupBox {
            background-color: #FDF8F5;
            border: none;
            color: #2C2C2C;
        }

        /* برچسب‌های متنی */
        QLabel {
            color: #2C2C2C;
            background-color: transparent;
        }

        /* ورودی‌های متن، کادرهای اعداد و منوهای کشویی */
        QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            background-color: #FFFFFF;
            border: 1.5px solid #F48FB1;
            border-radius: 8px;
            padding: 6px 10px;
            color: #2C2C2C;
            selection-background-color: #F48FB1;
            selection-color: #FFFFFF;
        }

        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, QComboBox:focus {
            border: 2px solid #D81B60;
            background-color: #FFFFFF;
        }

        /* دکمه‌های اصلی برنامه با رنگ صورتی پررنگ */
        QPushButton {
            background-color: #D81B60;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            padding: 8px 16px;
            font-weight: bold;
            min-height: 20px;
        }

        QPushButton:hover {
            background-color: #C2185B;
        }

        QPushButton:pressed {
            background-color: #880E4F;
        }

        QPushButton:disabled {
            background-color: #E0E0E0;
            color: #9E9E9E;
        }

        /* تب‌ها و سربرگ‌های پنجره‌ها */
        QTabWidget::pane {
            border: 1px solid #F48FB1;
            background: #FFFFFF;
            border-radius: 8px;
        }

        QTabBar::tab {
            background: #FCE4EC;
            color: #880E4F;
            padding: 8px 16px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            margin-right: 2px;
            font-weight: 500;
        }

        QTabBar::tab:selected {
            background: #D81B60;
            color: #FFFFFF;
            font-weight: bold;
        }

        /* جداول و لیست‌های اطلاعات */
        QTableWidget, QListWidget, QTreeView, QTableView {
            background-color: #FFFFFF;
            border: 1px solid #F8BBD0;
            gridline-color: #FCE4EC;
            border-radius: 8px;
            color: #2C2C2C;
            alternate-background-color: #FFF5F8;
        }

        QHeaderView::section {
            background-color: #FCE4EC;
            color: #880E4F;
            padding: 8px;
            border: none;
            font-weight: bold;
        }

        /* نوار اسکرول */
        QScrollBar:vertical {
            border: none;
            background: #FDF8F5;
            width: 8px;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical {
            background: #F48FB1;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical:hover {
            background: #D81B60;
        }

        QScrollBar:horizontal {
            border: none;
            background: #FDF8F5;
            height: 8px;
            border-radius: 4px;
        }

        QScrollBar::handle:horizontal {
            background: #F48FB1;
            border-radius: 4px;
        }

        QScrollBar::handle:horizontal:hover {
            background: #D81B60;
        }
    )";

    // اعمال تم به کل برنامه (حتی QMessageBox خطا هم از تم جدید پیروی می‌کنه)
    app.setStyleSheet(mainStyleSheet);

    if (!startServer()) {
        QMessageBox::critical(nullptr, "خطا", "سرور راه‌اندازی نشد؛ برنامه بسته می‌شود.");
        return 1;
    }

    startClient();

    return app.exec();
}