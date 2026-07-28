#include <QApplication>
#include <QMessageBox>
#include <QString>

bool startServer();
void startClient();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // استایل‌شیت اصلاح‌شده: پس‌زمینه کرم مشخص + دکمه‌های قهوه‌ای ملایم (کاراملی/شکلاتی)
    QString mainStyleSheet = R"(
        /* پس‌زمینه اصلی کلیه پنجره‌ها و ویجت‌های داخل پنجره (کرم warm & soft) */
        QMainWindow, QDialog, QWidget {
            background-color: #F3E9DC;
            color: #4A3B32;
            font-family: "Segoe UI", "Tahoma", sans-serif;
            font-size: 13px;
        }

        /* فریم‌ها و پنل‌های داخلی */
        QFrame, QGroupBox {
            background-color: #F3E9DC;
            border: none;
            color: #4A3B32;
        }

        /* برچسب‌های متنی */
        QLabel {
            color: #4A3B32;
            background-color: transparent;
        }

        /* کادرهای ورودی متن (QLineEdit) و کادرهای اعداد */
        QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            background-color: #FFFFFF;
            border: 1.5px solid #CDBA9F;
            border-radius: 8px;
            padding: 6px 10px;
            color: #4A3B32;
            selection-background-color: #8C6239;
            selection-color: #FFFFFF;
        }

        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, QComboBox:focus {
            border: 2px solid #8C6239;
            background-color: #FFFFFF;
        }

        /* دکمه‌های اصلی: قهوه‌ای کاراملی ملایم با متن کرم روشم */
        QPushButton {
            background-color: #8C6239;
            color: #FAF4ED;
            border: none;
            border-radius: 8px;
            padding: 8px 16px;
            font-weight: bold;
            min-height: 22px;
        }

        QPushButton:hover {
            background-color: #A07244;
        }

        QPushButton:pressed {
            background-color: #74502C;
        }

        QPushButton:disabled {
            background-color: #DCD0C0;
            color: #9C8F80;
        }

        /* تب‌ها و دکمه‌های سربرگ (مثل تب ورود / ثبت‌نام بالای فرم) */
        QTabWidget::pane {
            border: 1px solid #CDBA9F;
            background: #FFFFFF;
            border-radius: 8px;
        }

        QTabBar::tab {
            background: #E5D5C0;
            color: #4A3B32;
            padding: 8px 18px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            margin-right: 2px;
            font-weight: 500;
        }

        QTabBar::tab:selected {
            background: #8C6239;
            color: #FAF4ED;
            font-weight: bold;
        }

        /* جداول و لیست‌ها */
        QTableWidget, QListWidget, QTreeView, QTableView {
            background-color: #FFFFFF;
            border: 1px solid #CDBA9F;
            gridline-color: #E5D5C0;
            border-radius: 8px;
            color: #4A3B32;
            alternate-background-color: #FBF7F2;
        }

        QHeaderView::section {
            background-color: #E5D5C0;
            color: #4A3B32;
            padding: 8px;
            border: none;
            font-weight: bold;
        }

        /* نوار اسکرول */
        QScrollBar:vertical {
            border: none;
            background: #F3E9DC;
            width: 8px;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical {
            background: #CDBA9F;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical:hover {
            background: #8C6239;
        }

        QScrollBar:horizontal {
            border: none;
            background: #F3E9DC;
            height: 8px;
            border-radius: 4px;
        }

        QScrollBar::handle:horizontal {
            background: #CDBA9F;
            border-radius: 4px;
        }

        QScrollBar::handle:horizontal:hover {
            background: #8C6239;
        }
    )";

    // اعمال تم
    app.setStyleSheet(mainStyleSheet);

    if (!startServer()) {
        QMessageBox::critical(nullptr, "خطا", "سرور راه‌اندازی نشد؛ برنامه بسته می‌شود.");
        return 1;
    }

    startClient();

    return app.exec();
}