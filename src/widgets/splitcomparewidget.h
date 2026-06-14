#ifndef SPLITCOMPAREWIDGET_H
#define SPLITCOMPAREWIDGET_H

#include <QWidget>
#include <QImage>

class SplitCompareWidget : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
        SplitView = 0,
        ToggleView
    };

    explicit SplitCompareWidget(QWidget* parent = nullptr);

    void setImages(const QImage& original, const QImage& result);
    void setOriginalImage(const QImage& img);
    void setResultImage(const QImage& img);
    void setMode(Mode mode);
    Mode mode() const { return m_mode; }
    void setSplitPosition(int percent);
    int splitPosition() const { return m_splitPos; }
    void setGridOverlay(int rows, int cols, bool show);

signals:
    void splitPositionChanged(int percent);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void drawImageWithGrid(QPainter& p, const QImage& img, int x, int y, int w, int h,
                           bool isOriginal, const QString& label);

    QImage m_original;
    QImage m_result;
    Mode m_mode;
    int m_splitPos;
    bool m_showGrid;
    int m_gridRows;
    int m_gridCols;
    bool m_draggingSlider;
    bool m_toggleShowOriginal;
};

#endif
