#ifndef FILTERCONTROLSPANEL_H
#define FILTERCONTROLSPANEL_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include "../filters/filter.h"

class FilterControlsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FilterControlsPanel(QWidget* parent = nullptr);

    QList<FilterParams> currentFilterChain() const;

signals:
    void filterChainChanged();
    void parametersChanged();

public slots:
    void setGridRows(int rows);
    void setGridCols(int cols);
    int gridRows() const;
    int gridCols() const;

private slots:
    void onAddFilter();
    void onRemoveFilter();
    void onMoveUp();
    void onMoveDown();
    void onFilterSelected(int row);
    void onParamChanged();
    void updatePreviewParams();

private:
    void setupParamWidgets();
    FilterParams currentFilter(int row = -1) const;
    void refreshParamWidget();

    QListWidget* m_filterList;
    QPushButton* m_addBtn;
    QPushButton* m_removeBtn;
    QPushButton* m_upBtn;
    QPushButton* m_downBtn;

    QSpinBox* m_rowsSpin;
    QSpinBox* m_colsSpin;

    QStackedWidget* m_paramStack;
    int m_selectedRow;

    QList<FilterParams> m_filterChain;

    QSlider* m_blurRadiusSlider;
    QDoubleSpinBox* m_blurSigmaSpin;
    QSlider* m_sobelThresholdSlider;
    QSlider* m_oilRadiusSlider;
    QSlider* m_oilLevelsSlider;
    QDoubleSpinBox* m_sharpenStrengthSpin;

    QLabel* createSliderLabel(const QString& text, QSlider* slider);
};

#endif
