#ifndef COMPAREVIEWER_H
#define COMPAREVIEWER_H

#include <QWidget>
#include <QImage>
#include <QPushButton>
#include <QStackedWidget>
#include "imageviewer.h"
#include "splitcomparewidget.h"

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

protected:
    void wheelEvent(QWheelEvent* event) override;

private:
    void setupUi();

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
    SplitCompareWidget* m_splitCompareWidget;

    bool m_showGrid;
    int m_gridRows;
    int m_gridCols;
};

#endif
