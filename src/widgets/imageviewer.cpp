#include "imageviewer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QApplication>

ImageViewer::ImageViewer(QWidget* parent)
    : QScrollArea(parent)
    , m_zoomFactor(1.0)
    , m_fitToWindow(false)
    , m_gridRows(2)
    , m_gridCols(2)
    , m_showGrid(false)
{
    m_imageLabel = new QLabel(this);
    m_imageLabel->setBackgroundRole(QPalette::Base);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(false);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet(QStringLiteral("QLabel { background-color: #333; }"));

    setWidget(m_imageLabel);
    setBackgroundRole(QPalette::Dark);
    setWidgetResizable(false);
}

void ImageViewer::setImage(const QImage& image)
{
    m_image = image;
    updatePixmap();
}

void ImageViewer::clear()
{
    m_image = QImage();
    m_imageLabel->clear();
    m_imageLabel->resize(0, 0);
    m_zoomFactor = 1.0;
}

void ImageViewer::fitToWindow(bool enable)
{
    m_fitToWindow = enable;
    updatePixmap();
}

void ImageViewer::zoomIn()
{
    if (m_image.isNull()) return;
    m_fitToWindow = false;
    m_zoomFactor *= 1.25;
    updatePixmap();
}

void ImageViewer::zoomOut()
{
    if (m_image.isNull()) return;
    m_fitToWindow = false;
    m_zoomFactor /= 1.25;
    updatePixmap();
}

void ImageViewer::resetZoom()
{
    if (m_image.isNull()) return;
    m_fitToWindow = false;
    m_zoomFactor = 1.0;
    updatePixmap();
}

void ImageViewer::showGridOverlay(int rows, int cols, bool show)
{
    m_gridRows = rows;
    m_gridCols = cols;
    m_showGrid = show;
    updatePixmap();
}

void ImageViewer::updatePixmap()
{
    if (m_image.isNull()) {
        m_imageLabel->clear();
        return;
    }

    QImage img = m_image.copy();
    QSize displaySize;

    if (m_fitToWindow) {
        int viewW = viewport()->width();
        int viewH = viewport()->height();
        QSize scaled = m_image.size().scaled(viewW, viewH, Qt::KeepAspectRatio);
        displaySize = scaled;
        m_zoomFactor = static_cast<double>(scaled.width()) / m_image.width();
    } else {
        displaySize = QSize(qRound(m_image.width() * m_zoomFactor),
                           qRound(m_image.height() * m_zoomFactor));
    }

    if (m_showGrid && displaySize.isValid()) {
        QPainter p(&img);
        p.setPen(QPen(QColor(255, 0, 0, 180), 1, Qt::DashLine));
        int cellW = img.width() / m_gridCols;
        int cellH = img.height() / m_gridRows;
        for (int c = 1; c < m_gridCols; ++c) {
            int x = c * cellW;
            p.drawLine(x, 0, x, img.height());
        }
        for (int r = 1; r < m_gridRows; ++r) {
            int y = r * cellH;
            p.drawLine(0, y, img.width(), y);
        }
        for (int r = 0; r < m_gridRows; ++r) {
            for (int c = 0; c < m_gridCols; ++c) {
                int idx = r * m_gridCols + c;
                int x = c * cellW + 4;
                int y = r * cellH + 16;
                p.setPen(Qt::red);
                p.setFont(QFont(QStringLiteral("Arial"), 10, QFont::Bold));
                p.drawText(x, y, QString::number(idx + 1));
            }
        }
    }

    QPixmap pix = QPixmap::fromImage(img).scaled(displaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_imageLabel->setPixmap(pix);
    m_imageLabel->resize(pix.size());

    emit zoomChanged(m_zoomFactor);
}

void ImageViewer::adjustScrollBar(QScrollBar* bar, double factor)
{
    bar->setValue(static_cast<int>(factor * bar->value() + ((factor - 1) * bar->pageStep() / 2)));
}

void ImageViewer::wheelEvent(QWheelEvent* event)
{
    if (m_image.isNull()) return;
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
        return;
    }
    QScrollArea::wheelEvent(event);
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        fitToWindow(!m_fitToWindow);
        event->accept();
        return;
    }
    QScrollArea::mouseDoubleClickEvent(event);
}

void ImageViewer::paintEvent(QPaintEvent* event)
{
    QScrollArea::paintEvent(event);
}
