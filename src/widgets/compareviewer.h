#ifndef COMPAREVIEWER_H
#define COMPAREVIEWER_H

#include <QWidget>
#include <QImage>
#include <QPushButton>
#include <QStackedWidget>
#include "imageviewer.h"

class CompareViewer : public QWidget
{
    Q_OBJECT

public:
    enum ViewMode {
        SplitView = 0,
        OriginalOnly,
        ResultOnly,
        ToggleView
    };

    explicit CompareViewer(QWidget* parent = nullptr);

    void setOriginalImage(const QImage& image);
    void setResultImage(const QImage& image);
    void clear();

    ViewMode currentMode() const { return m_mode; }

    void setGridOverlay(int rows, int cols, bool show);

signals:
    void modeChanged(ViewMode mode);

public slots:
    void setViewMode(ViewMode mode);
    void setSplitPosition(int percent);
    void zoomIn();
    void zoomOut();
    void fitToWindow();
    void resetZoom();

private slots:
    void onModeChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void setupUi();
    void renderSplitView();

    QImage m_original;
    QImage m_result;
    ViewMode m_mode;
    int m_splitPos;

    QWidget* m_toolBar;
    QPushButton* m_splitBtn;
    QPushButton* m_originalBtn;
    QPushButton* m_resultBtn;
    QPushButton* m_toggleBtn;

    QStackedWidget* m_stackedWidget;
    ImageViewer* m_originalViewer;
    ImageViewer* m_resultViewer;
    QWidget* m_splitWidget;

    bool m_showGrid;
    int m_gridRows;
    int m_gridCols;
    bool m_draggingSlider;
    bool m_toggleShowOriginal;
};

#endif
