#include "compareviewer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QSlider>

CompareViewer::CompareViewer(QWidget* parent)
    : QWidget(parent)
    , m_mode(SplitView)
    , m_splitPos(50)
    , m_showGrid(false)
    , m_gridRows(2)
    , m_gridCols(2)
    , m_draggingSlider(false)
    , m_toggleShowOriginal(false)
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
    connect(bg, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &CompareViewer::setViewMode);

    tbLayout->addWidget(new QLabel(QStringLiteral("显示模式:"), m_toolBar));
    tbLayout->addWidget(m_splitBtn);
    tbLayout->addWidget(m_originalBtn);
    tbLayout->addWidget(m_resultBtn);
    tbLayout->addWidget(m_toggleBtn);
    tbLayout->addStretch();

    mainLayout->addWidget(m_toolBar);

    m_stackedWidget = new QStackedWidget(this);

    m_splitWidget = new QWidget(m_stackedWidget);
    m_splitWidget->setMinimumHeight(200);
    m_splitWidget->setMouseTracking(true);
    m_splitWidget->setAttribute(Qt::WA_OpaquePaintEvent, false);

    m_originalViewer = new ImageViewer(m_stackedWidget);
    m_resultViewer = new ImageViewer(m_stackedWidget);

    m_stackedWidget->addWidget(m_splitWidget);
    m_stackedWidget->addWidget(m_originalViewer);
    m_stackedWidget->addWidget(m_resultViewer);
    m_stackedWidget->setCurrentIndex(0);

    mainLayout->addWidget(m_stackedWidget, 1);
}

void CompareViewer::setOriginalImage(const QImage& image)
{
    m_original = image;
    m_originalViewer->setImage(image);
    m_splitWidget->update();
}

void CompareViewer::setResultImage(const QImage& image)
{
    m_result = image;
    m_resultViewer->setImage(image);
    m_splitWidget->update();
}

void CompareViewer::clear()
{
    m_original = QImage();
    m_result = QImage();
    m_originalViewer->clear();
    m_resultViewer->clear();
    m_splitWidget->update();
}

void CompareViewer::setViewMode(ViewMode mode)
{
    if (m_mode == mode) return;
    m_mode = mode;
    switch (mode) {
    case SplitView:
        m_stackedWidget->setCurrentWidget(m_splitWidget);
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
        m_stackedWidget->setCurrentWidget(m_splitWidget);
        m_toggleBtn->setChecked(true);
        break;
    }
    emit modeChanged(mode);
    update();
}

void CompareViewer::setSplitPosition(int percent)
{
    m_splitPos = qBound(0, percent, 100);
    m_splitWidget->update();
}

void CompareViewer::onModeChanged()
{
    m_splitWidget->update();
}

void CompareViewer::setGridOverlay(int rows, int cols, bool show)
{
    m_gridRows = rows;
    m_gridCols = cols;
    m_showGrid = show;
    m_originalViewer->showGridOverlay(rows, cols, show);
    m_resultViewer->showGridOverlay(rows, cols, show);
    m_splitWidget->update();
}

static void drawGridOnImage(QPainter& p, const QImage& img, int x, int y, int w, int h,
                             int gridRows, int gridCols, bool showGrid, bool isOriginal,
                             const QString& labelText)
{
    if (img.isNull()) return;
    QRect target(x, y, w, h);
    QSize scaled = img.size().scaled(w, h, Qt::KeepAspectRatio);
    int dx = x + (w - scaled.width()) / 2;
    int dy = y + (h - scaled.height()) / 2;

    p.drawImage(QRect(dx, dy, scaled.width(), scaled.height()), img);

    if (showGrid) {
        QPen oldPen = p.pen();
        p.setPen(QPen(QColor(isOriginal ? 0 : 255, isOriginal ? 120 : 50, 0, 180), 2, Qt::DashLine));
        int cellW = scaled.width() / gridCols;
        int cellH = scaled.height() / gridRows;
        for (int c = 1; c < gridCols; ++c) {
            int gx = dx + c * cellW;
            p.drawLine(gx, dy, gx, dy + scaled.height());
        }
        for (int r = 1; r < gridRows; ++r) {
            int gy = dy + r * cellH;
            p.drawLine(dx, gy, dx + scaled.width(), gy);
        }
        for (int r = 0; r < gridRows; ++r) {
            for (int c = 0; c < gridCols; ++c) {
                int idx = r * gridCols + c;
                int tx = dx + c * cellW + 6;
                int ty = dy + r * cellH + 20;
                QFont f = p.font();
                f.setBold(true);
                f.setPointSize(qMax(9, scaled.width() / 120));
                p.setFont(f);
                p.drawText(tx, ty, QString::number(idx + 1));
            }
        }
        p.setPen(oldPen);
    }

    if (!labelText.isEmpty()) {
        QFont f = p.font();
        f.setBold(true);
        f.setPointSize(qMax(10, w / 80));
        p.setFont(f);
        p.setPen(Qt::white);
        int ly = dy + 8;
        p.drawText(dx + 10, ly + QFontMetrics(f).ascent(), labelText);
    }

    Q_UNUSED(target);
}

void CompareViewer::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    if (m_mode != SplitView && m_mode != ToggleView) return;

    QPainter p(m_splitWidget);
    p.fillRect(m_splitWidget->rect(), QColor(50, 50, 50));

    int w = m_splitWidget->width();
    int h = m_splitWidget->height();

    if (m_mode == ToggleView) {
        if (m_toggleShowOriginal) {
            drawGridOnImage(p, m_original, 0, 0, w, h, m_gridRows, m_gridCols, m_showGrid, true, QStringLiteral("原图 (点击切换)"));
        } else {
            drawGridOnImage(p, m_result, 0, 0, w, h, m_gridRows, m_gridCols, m_showGrid, false, QStringLiteral("结果图 (点击切换)"));
        }
        return;
    }

    int halfW = (w * m_splitPos) / 100;

    drawGridOnImage(p, m_original, 0, 0, halfW, h, m_gridRows, m_gridCols, m_showGrid, true, QStringLiteral("原图"));
    drawGridOnImage(p, m_result, halfW, 0, w - halfW, h, m_gridRows, m_gridCols, m_showGrid, false, QStringLiteral("结果图"));

    QPen linePen(QColor(255, 255, 255), 3);
    p.setPen(linePen);
    p.drawLine(halfW, 0, halfW, h);

    int handleW = 40;
    int handleH = 60;
    int hx = halfW - handleW / 2;
    int hy = h / 2 - handleH / 2;
    QRect handleRect(hx, hy, handleW, handleH);
    p.setBrush(QColor(255, 255, 255, 220));
    p.setPen(QColor(100, 100, 100));
    p.drawRoundedRect(handleRect, 8, 8);
    p.setPen(QColor(50, 50, 50));
    QFont f = p.font();
    f.setBold(true);
    f.setPointSize(16);
    p.setFont(f);
    p.drawText(handleRect, Qt::AlignCenter, QStringLiteral("◀ ▶"));

    Q_UNUSED(event);
}

void CompareViewer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_splitWidget->update();
}

void CompareViewer::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    if (m_mode != SplitView) return;
    if (m_draggingSlider) {
        int localX = event->pos().x() - m_splitWidget->x();
        int p = (localX * 100) / qMax(1, m_splitWidget->width());
        m_splitPos = qBound(5, p, 95);
        m_splitWidget->update();
    }
}

void CompareViewer::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    if (m_mode == ToggleView && event->button() == Qt::LeftButton) {
        m_toggleShowOriginal = !m_toggleShowOriginal;
        m_splitWidget->update();
        return;
    }
    if (m_mode == SplitView && event->button() == Qt::LeftButton) {
        QPoint pos = event->pos();
        int handleW = 40;
        int handleH = 60;
        int halfW = (m_splitWidget->width() * m_splitPos) / 100;
        int hx = halfW - handleW / 2;
        int hy = m_splitWidget->height() / 2 - handleH / 2;
        QRect handleRect(hx + m_splitWidget->x(), hy + m_splitWidget->y(), handleW, handleH);
        if (handleRect.contains(pos)) {
            m_draggingSlider = true;
            setCursor(Qt::SizeHorCursor);
        }
    }
}

void CompareViewer::leaveEvent(QEvent* event)
{
    if (m_draggingSlider) {
        m_draggingSlider = false;
        unsetCursor();
    }
    QWidget::leaveEvent(event);
}

void CompareViewer::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (m_stackedWidget->currentWidget() == m_originalViewer) {
            m_originalViewer->wheelEvent(event);
        } else if (m_stackedWidget->currentWidget() == m_resultViewer) {
            m_resultViewer->wheelEvent(event);
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
