#include "PdfReaderWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPdfPageNavigator>

PdfReaderWidget::PdfReaderWidget(QWidget *parent) : QWidget(parent) {
    document = new QPdfDocument(this);
    buildUi();
}

void PdfReaderWidget::buildUi() {
    auto* mainLayout = new QVBoxLayout(this);

    pdfView = new QPdfView(this);
    pdfView->setDocument(document);
    pdfView->setPageMode(QPdfView::PageMode::SinglePage);
    pdfView->setZoomMode(QPdfView::ZoomMode::FitToWidth);

    auto* toolbar = new QHBoxLayout();
    btnPrev = new QPushButton("◀ صفحه‌ی قبل");
    btnNext = new QPushButton("صفحه‌ی بعد ▶");
    lblPageInfo = new QLabel();
    spinGotoPage = new QSpinBox();
    spinGotoPage->setMinimum(1);
    btnZoomIn = new QPushButton("بزرگ‌نمایی +");
    btnZoomOut = new QPushButton("کوچک‌نمایی -");

    toolbar->addWidget(btnPrev);
    toolbar->addWidget(lblPageInfo);
    toolbar->addWidget(spinGotoPage);
    toolbar->addWidget(btnNext);
    toolbar->addStretch();
    toolbar->addWidget(btnZoomOut);
    toolbar->addWidget(btnZoomIn);

    mainLayout->addLayout(toolbar);
    mainLayout->addWidget(pdfView, /*stretch=*/1);

    connect(btnPrev, &QPushButton::clicked, this, &PdfReaderWidget::triggerPrevPage);
    connect(btnNext, &QPushButton::clicked, this, &PdfReaderWidget::triggerNextPage);
    connect(spinGotoPage, QOverload<int>::of(&QSpinBox::valueChanged), this, &PdfReaderWidget::triggerGotoPage);
    connect(btnZoomIn, &QPushButton::clicked, this, &PdfReaderWidget::zoomIn);
    connect(btnZoomOut, &QPushButton::clicked, this, &PdfReaderWidget::zoomOut);
}

void PdfReaderWidget::openFile(const QString &filePath, int bookId, int startPage) {
    currentLoadedFilePath = filePath;
    currentBookId = bookId;

    QPdfDocument::Error err = document->load(filePath);
    if (err != QPdfDocument::Error::None) {
        lblPageInfo->setText("خطا در بازکردنِ فایلِ PDF");
        return;
    }

    spinGotoPage->setMaximum(document->pageCount());

    connect(pdfView->pageNavigator(), &QPdfPageNavigator::currentPageChanged,
            this, &PdfReaderWidget::onCurrentPageChanged, Qt::UniqueConnection);

    int zeroBasedStart = qBound(0, startPage - 1, document->pageCount() - 1);
    pdfView->pageNavigator()->jump(zeroBasedStart, {});

    lblPageInfo->setText(QString("صفحه %1 از %2").arg(startPage).arg(document->pageCount()));
    spinGotoPage->blockSignals(true);
    spinGotoPage->setValue(startPage);
    spinGotoPage->blockSignals(false);
}

void PdfReaderWidget::triggerNextPage() {
    int current = pdfView->pageNavigator()->currentPage();
    if (current + 1 < document->pageCount()) {
        pdfView->pageNavigator()->jump(current + 1, {});
    }
}

void PdfReaderWidget::triggerPrevPage() {
    int current = pdfView->pageNavigator()->currentPage();
    if (current - 1 >= 0) {
        pdfView->pageNavigator()->jump(current - 1, {});
    }
}

void PdfReaderWidget::triggerGotoPage(int pageNumberOneBased) {
    int zeroBased = qBound(0, pageNumberOneBased - 1, document->pageCount() - 1);
    pdfView->pageNavigator()->jump(zeroBased, {});
}

void PdfReaderWidget::zoomIn() {
    pdfView->setZoomFactor(pdfView->zoomFactor() * 1.2);
}

void PdfReaderWidget::zoomOut() {
    pdfView->setZoomFactor(pdfView->zoomFactor() / 1.2);
}

void PdfReaderWidget::onCurrentPageChanged(int newPageZeroBased)
{
    int oneBased = newPageZeroBased + 1;

    lblPageInfo->setText(
        QString("صفحه %1 از %2")
            .arg(oneBased)
            .arg(document->pageCount()));

    spinGotoPage->blockSignals(true);
    spinGotoPage->setValue(oneBased);
    spinGotoPage->blockSignals(false);

    if (currentBookId != -1) {
        emit pageChanged(currentBookId, oneBased);
    }
}



