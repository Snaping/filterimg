#include "compareviewer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QLabel>

CompareViewer::CompareViewer(QWidget* parent)
    : QWidget(parent)
    , m_mode(SplitView)
    , m_splitPos(50)
    , m_showGrid(false)
    , m_gridRows(2)
    , m_gridCols(2)
{
    setupUi();
}

void CompareViewer::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(2);

    m_toolBar = new QWidget(this);
    m_toolBar->setStyleSheet(QStringLiteral("QWidget { background-color: #f0f0f0; border-bottom: 1px solid #ccc; }"));
    QHBoxLayout* tbLayout = new QHBoxLayout(m_toolBar);
    tbLayout->setContentsMargins(8, 4, 8, 4);
    tbLayout->setSpacing(6);

    m_splitBtn = new QPushButton(QStringLiteral("左右分屏"), m_toolBar);
    m_splitBtn->setCheckable(true);
    m_splitBtn->setChecked(true);
    m_originalBtn = new QPushButton(QStringLiteral("仅原图"), m_toolBar);
    m_originalBtn->setCheckable(true);
    m_resultBtn = new QPushButton(QStringLiteral("仅结果"), m_toolBar);
    m_resultBtn->setCheckable(true);
    m_toggleBtn = new QPushButton(QStringLiteral("切换对比"), m_toolBar);
    m_toggleBtn->setCheckable(true);

    QButtonGroup* bg = new QButtonGroup(this);
    bg->addButton(m_splitBtn, SplitView);
    bg->addButton(m_originalBtn, OriginalOnly);
    bg->addButton(m_resultBtn, ResultOnly);
    bg->addButton(m_toggleBtn, ToggleView);
    connect(bg, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [this](int id) {
        setViewMode(static_cast<ViewMode>(id));
    });

    tbLayout->addWidget(new QLabel(QStringLiteral("显示模式:"), m_toolBar));
    tbLayout->addWidget(m_splitBtn);
    tbLayout->addWidget(m_originalBtn);
    tbLayout->addWidget(m_resultBtn);
    tbLayout->addWidget(m_toggleBtn);
    tbLayout->addStretch();

    mainLayout->addWidget(m_toolBar);

    m_stackedWidget = new QStackedWidget(this);

    m_splitCompareWidget = new SplitCompareWidget(m_stackedWidget);
    connect(m_splitCompareWidget, &SplitCompareWidget::splitPositionChanged, this, &CompareViewer::setSplitPosition);

    m_originalViewer = new ImageViewer(m_stackedWidget);
    m_resultViewer = new ImageViewer(m_stackedWidget);

    m_stackedWidget->addWidget(m_splitCompareWidget);
    m_stackedWidget->addWidget(m_originalViewer);
    m_stackedWidget->addWidget(m_resultViewer);
    m_stackedWidget->setCurrentIndex(0);

    mainLayout->addWidget(m_stackedWidget, 1);
}

void CompareViewer::setOriginalImage(const QImage& image)
{
    m_original = image;
    m_originalViewer->setImage(image);
    m_splitCompareWidget->setOriginalImage(image);
}

void CompareViewer::setResultImage(const QImage& image)
{
    m_result = image;
    m_resultViewer->setImage(image);
    m_splitCompareWidget->setResultImage(image);
}

void CompareViewer::clear()
{
    m_original = QImage();
    m_result = QImage();
    m_originalViewer->clear();
    m_resultViewer->clear();
    m_splitCompareWidget->setImages(QImage(), QImage());
}

void CompareViewer::setViewMode(ViewMode mode)
{
    if (m_mode == mode) return;
    m_mode = mode;
    switch (mode) {
    case SplitView:
        m_splitCompareWidget->setMode(SplitCompareWidget::SplitView);
        m_stackedWidget->setCurrentWidget(m_splitCompareWidget);
        m_splitBtn->setChecked(true);
        break;
    case OriginalOnly:
        m_stackedWidget->setCurrentWidget(m_originalViewer);
        m_originalBtn->setChecked(true);
        break;
    case ResultOnly:
        m_stackedWidget->setCurrentWidget(m_resultViewer);
        m_resultBtn->setChecked(true);
        break;
    case ToggleView:
        m_splitCompareWidget->setMode(SplitCompareWidget::ToggleView);
        m_stackedWidget->setCurrentWidget(m_splitCompareWidget);
        m_toggleBtn->setChecked(true);
        break;
    }
    emit modeChanged(m_mode);
}

void CompareViewer::setSplitPosition(int percent)
{
    m_splitPos = qBound(0, percent, 100);
    m_splitCompareWidget->setSplitPosition(m_splitPos);
}

void CompareViewer::setGridOverlay(int rows, int cols, bool show)
{
    m_gridRows = rows;
    m_gridCols = cols;
    m_showGrid = show;
    m_originalViewer->showGridOverlay(rows, cols, show);
    m_resultViewer->showGridOverlay(rows, cols, show);
    m_splitCompareWidget->setGridOverlay(rows, cols, show);
}

void CompareViewer::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (m_stackedWidget->currentWidget() == m_originalViewer) {
            m_originalViewer->handleWheelEvent(event);
        } else if (m_stackedWidget->currentWidget() == m_resultViewer) {
            m_resultViewer->handleWheelEvent(event);
        }
        event->accept();
        return;
    }
    QWidget::wheelEvent(event);
}

void CompareViewer::zoomIn()
{
    if (m_mode == OriginalOnly || m_stackedWidget->currentWidget() == m_originalViewer) {
        m_originalViewer->zoomIn();
    } else if (m_mode == ResultOnly || m_stackedWidget->currentWidget() == m_resultViewer) {
        m_resultViewer->zoomIn();
    } else {
        m_originalViewer->zoomIn();
        m_resultViewer->zoomIn();
    }
}

void CompareViewer::zoomOut()
{
    if (m_mode == OriginalOnly || m_stackedWidget->currentWidget() == m_originalViewer) {
        m_originalViewer->zoomOut();
    } else if (m_mode == ResultOnly || m_stackedWidget->currentWidget() == m_resultViewer) {
        m_resultViewer->zoomOut();
    } else {
        m_originalViewer->zoomOut();
        m_resultViewer->zoomOut();
    }
}

void CompareViewer::fitToWindow()
{
    m_originalViewer->fitToWindow(true);
    m_resultViewer->fitToWindow(true);
}

void CompareViewer::resetZoom()
{
    m_originalViewer->resetZoom();
    m_resultViewer->resetZoom();
}
