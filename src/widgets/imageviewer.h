#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QScrollArea>
#include <QLabel>
#include <QImage>

class ImageViewer : public QScrollArea
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget* parent = nullptr);

    void setImage(const QImage& image);
    QImage image() const { return m_image; }
    void clear();

    void fitToWindow(bool enable);
    void zoomIn();
    void zoomOut();
    void resetZoom();
    double zoomFactor() const { return m_zoomFactor; }

    void showGridOverlay(int rows, int cols, bool show);

signals:
    void zoomChanged(double factor);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void updatePixmap();
    void adjustScrollBar(QScrollBar* bar, double factor);

    QLabel* m_imageLabel;
    QImage m_image;
    double m_zoomFactor;
    bool m_fitToWindow;
    int m_gridRows;
    int m_gridCols;
    bool m_showGrid;
};

#endif
