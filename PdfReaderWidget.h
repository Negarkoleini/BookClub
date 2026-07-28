#ifndef PDFREADERWIDGET_H
#define PDFREADERWIDGET_H
#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QPdfDocument>
#include <QPdfView>

class PdfReaderWidget : public QWidget {
    Q_OBJECT
private:
    QString currentLoadedFilePath;
    int currentBookId = -1;
    qreal currentZoomFactor = 1.0;

    QPdfDocument* document;
    QPdfView* pdfView;

    QLabel* lblPageInfo;
    QSpinBox* spinGotoPage;
    QPushButton* btnNext;
    QPushButton* btnPrev;
    QPushButton* btnZoomIn;
    QPushButton* btnZoomOut;

    void buildUi();

public:
    explicit PdfReaderWidget(QWidget *parent = nullptr);
    ~PdfReaderWidget() override = default;

    void openFile(const QString &filePath, int bookId, int startPage = 1);
    void jumpToPage(int pageNumber);

private slots:
    void triggerNextPage();
    void triggerPrevPage();
    void triggerGotoPage(int pageNumberOneBased);
    void zoomIn();
    void zoomOut();
    void onCurrentPageChanged(int newPageZeroBased);

signals:
    void pageChanged(int bookId, int newPageNum);
};

#endif // PDFREADERWIDGET_H
