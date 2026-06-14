#include "splitcomparewidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFontMetrics>
#include <algorithm>

SplitCompareWidget::SplitCompareWidget(QWidget* parent)
    : QWidget(parent)
    , m_mode(SplitView)
    , m_splitPos(50)
    , m_showGrid(false)
    , m_gridRows(2)
    , m_gridCols(2)
    , m_draggingSlider(false)
    , m_toggleShowOriginal(false)
{
    setMinimumHeight(200);
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void SplitCompareWidget::setImages(const QImage& original, const QImage& result)
{
    m_original = original;
    m_result = result;
    update();
}

void SplitCompareWidget::setOriginalImage(const QImage& img)
{
    m_original = img;
    update();
}

void SplitCompareWidget::setResultImage(const QImage& img)
{
    m_result = img;
    update();
}

void SplitCompareWidget::setMode(Mode mode)
{
    if (m_mode == mode) return;
    m_mode = mode;
    update();
}

void SplitCompareWidget::setSplitPosition(int percent)
{
    m_splitPos = qBound(0, percent, 100);
    update();
}

void SplitCompareWidget::setGridOverlay(int rows, int cols, bool show)
{
    m_gridRows = qMax(1, rows);
    m_gridCols = qMax(1, cols);
    m_showGrid = show;
    update();
}

void SplitCompareWidget::drawImageWithGrid(QPainter& p, const QImage& img, int x, int y, int w, int h,
                                            bool isOriginal, const QString& label)
{
    if (img.isNull() || w <= 0 || h <= 0) return;
    QSize scaled = img.size().scaled(w, h, Qt::KeepAspectRatio);
    int dx = x + (w - scaled.width()) / 2;
    int dy = y + (h - scaled.height()) / 2;

    p.drawImage(QRect(dx, dy, scaled.width(), scaled.height()), img);

    if (m_showGrid) {
        QPen oldPen = p.pen();
        p.setPen(QPen(QColor(isOriginal ? 0 : 255, isOriginal ? 120 : 50, 0, 200), 2, Qt::DashLine));
        int cellW = scaled.width() / m_gridCols;
        int cellH = scaled.height() / m_gridRows;
        for (int c = 1; c < m_gridCols; ++c) {
            int gx = dx + c * cellW;
            p.drawLine(gx, dy, gx, dy + scaled.height());
        }
        for (int r = 1; r < m_gridRows; ++r) {
            int gy = dy + r * cellH;
            p.drawLine(dx, gy, dx + scaled.width(), gy);
        }
        if (cellW > 25 && cellH > 25) {
            for (int r = 0; r < m_gridRows; ++r) {
                for (int c = 0; c < m_gridCols; ++c) {
                    int idx = r * m_gridCols + c;
                    int tx = dx + c * cellW + 6;
                    int ty = dy + r * cellH + 20;
                    QFont f = p.font();
                    f.setBold(true);
                    f.setPointSize(qMax(9, scaled.width() / 120));
                    p.setFont(f);
                    p.setPen(QColor(isOriginal ? 0 : 255, isOriginal ? 120 : 50, 0, 220));
                    p.drawText(tx, ty, QString::number(idx + 1));
                }
            }
        }
        p.setPen(oldPen);
    }

    if (!label.isEmpty()) {
        QFont f = p.font();
        f.setBold(true);
        f.setPointSize(qMax(10, w / 80));
        p.setFont(f);
        QFontMetrics fm(f);
        int pad = 6;
        int tw = fm.horizontalAdvance(label) + pad * 2;
        int th = fm.height() + pad;
        p.fillRect(dx + 8, dy + 8, tw, th, QColor(0, 0, 0, 160));
        p.setPen(Qt::white);
        p.drawText(dx + 8 + pad, dy + 8 + fm.ascent() + pad / 2, label);
    }
}

void SplitCompareWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), QColor(50, 50, 50));

    int w = width();
    int h = height();

    if (m_mode == ToggleView) {
        if (m_toggleShowOriginal) {
            drawImageWithGrid(p, m_original, 0, 0, w, h, true, QStringLiteral("原图 (点击切换)"));
        } else {
            if (m_result.isNull()) {
                drawImageWithGrid(p, m_original, 0, 0, w, h, true, QStringLiteral("暂无结果 (点击切换)"));
            } else {
                drawImageWithGrid(p, m_result, 0, 0, w, h, false, QStringLiteral("结果图 (点击切换)"));
            }
        }
        return;
    }

    int halfW = (w * m_splitPos) / 100;
    if (m_result.isNull()) {
        drawImageWithGrid(p, m_original, 0, 0, w, h, true, QStringLiteral("原图 (尚未生成结果)"));
    } else {
        QRect leftRect(0, 0, halfW, h);
        QRect rightRect(halfW, 0, w - halfW, h);
        p.save();
        p.setClipRect(leftRect);
        drawImageWithGrid(p, m_original, 0, 0, w, h, true, QStringLiteral("原图"));
        p.restore();

        p.save();
        p.setClipRect(rightRect);
        drawImageWithGrid(p, m_result, 0, 0, w, h, false, QStringLiteral("结果图"));
        p.restore();
    }

    QPen linePen(QColor(255, 255, 255), 3);
    p.setPen(linePen);
    p.drawLine(halfW, 0, halfW, h);

    int handleW = 44;
    int handleH = 64;
    int hx = halfW - handleW / 2;
    int hy = h / 2 - handleH / 2;
    QRect handleRect(hx, hy, handleW, handleH);
    p.setBrush(QColor(255, 255, 255, 230));
    p.setPen(QColor(100, 100, 100));
    p.drawRoundedRect(handleRect, 8, 8);
    p.setPen(QColor(50, 50, 50));
    QFont f = p.font();
    f.setBold(true);
    f.setPointSize(16);
    p.setFont(f);
    p.drawText(handleRect, Qt::AlignCenter, QStringLiteral("◀ ▶"));
}

void SplitCompareWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    if (m_mode == ToggleView) {
        m_toggleShowOriginal = !m_toggleShowOriginal;
        update();
        return;
    }
    int halfW = (width() * m_splitPos) / 100;
    int handleW = 44;
    int handleH = 64;
    int hx = halfW - handleW / 2;
    int hy = height() / 2 - handleH / 2;
    QRect handleRect(hx, hy, handleW, handleH);
    if (handleRect.contains(event->pos())) {
        m_draggingSlider = true;
        setCursor(Qt::SizeHorCursor);
    }
}

void SplitCompareWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_draggingSlider) {
        int p = (event->pos().x() * 100) / qMax(1, width());
        m_splitPos = qBound(5, p, 95);
        emit splitPositionChanged(m_splitPos);
        update();
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void SplitCompareWidget::leaveEvent(QEvent* event)
{
    if (m_draggingSlider) {
        m_draggingSlider = false;
        unsetCursor();
    }
    QWidget::leaveEvent(event);
}
