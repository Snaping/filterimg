#include "filtercontrolspanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>

FilterControlsPanel::FilterControlsPanel(QWidget* parent)
    : QWidget(parent)
    , m_selectedRow(-1)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    QGroupBox* gridGroup = new QGroupBox(QStringLiteral("网格划分"), this);
    QFormLayout* gridForm = new QFormLayout(gridGroup);
    m_rowsSpin = new QSpinBox(gridGroup);
    m_rowsSpin->setRange(1, 16);
    m_rowsSpin->setValue(2);
    m_colsSpin = new QSpinBox(gridGroup);
    m_colsSpin->setRange(1, 16);
    m_colsSpin->setValue(2);
    gridForm->addRow(QStringLiteral("行数:"), m_rowsSpin);
    gridForm->addRow(QStringLiteral("列数:"), m_colsSpin);
    connect(m_rowsSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &FilterControlsPanel::filterChainChanged);
    connect(m_colsSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &FilterControlsPanel::filterChainChanged);
    mainLayout->addWidget(gridGroup);

    QGroupBox* filterGroup = new QGroupBox(QStringLiteral("滤镜链"), this);
    QVBoxLayout* filterLayout = new QVBoxLayout(filterGroup);

    m_filterList = new QListWidget(filterGroup);
    connect(m_filterList, &QListWidget::currentRowChanged, this, &FilterControlsPanel::onFilterSelected);
    filterLayout->addWidget(m_filterList);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_addBtn = new QPushButton(QStringLiteral("+ 添加"), filterGroup);
    m_removeBtn = new QPushButton(QStringLiteral("- 删除"), filterGroup);
    m_upBtn = new QPushButton(QStringLiteral("↑"), filterGroup);
    m_downBtn = new QPushButton(QStringLiteral("↓"), filterGroup);
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_removeBtn);
    btnLayout->addWidget(m_upBtn);
    btnLayout->addWidget(m_downBtn);
    filterLayout->addLayout(btnLayout);

    connect(m_addBtn, &QPushButton::clicked, this, &FilterControlsPanel::onAddFilter);
    connect(m_removeBtn, &QPushButton::clicked, this, &FilterControlsPanel::onRemoveFilter);
    connect(m_upBtn, &QPushButton::clicked, this, &FilterControlsPanel::onMoveUp);
    connect(m_downBtn, &QPushButton::clicked, this, &FilterControlsPanel::onMoveDown);

    mainLayout->addWidget(filterGroup);

    QGroupBox* paramGroup = new QGroupBox(QStringLiteral("参数调节"), this);
    QVBoxLayout* paramLayout = new QVBoxLayout(paramGroup);

    setupParamWidgets();
    paramLayout->addWidget(m_paramStack);

    mainLayout->addWidget(paramGroup);
    mainLayout->addStretch();

    refreshParamWidget();
}

void FilterControlsPanel::setupParamWidgets()
{
    m_paramStack = new QStackedWidget(this);

    QWidget* noParam = new QWidget(m_paramStack);
    QVBoxLayout* noParamLayout = new QVBoxLayout(noParam);
    noParamLayout->addWidget(new QLabel(QStringLiteral("此滤镜无参数")));
    noParamLayout->addStretch();
    m_paramStack->addWidget(noParam);

    QWidget* blurPage = new QWidget(m_paramStack);
    QVBoxLayout* blurLayout = new QVBoxLayout(blurPage);
    m_blurRadiusSlider = new QSlider(Qt::Horizontal, blurPage);
    m_blurRadiusSlider->setRange(1, 15);
    m_blurRadiusSlider->setValue(3);
    blurLayout->addWidget(createSliderLabel(QStringLiteral("模糊半径: %1"), m_blurRadiusSlider));
    blurLayout->addWidget(m_blurRadiusSlider);

    m_blurSigmaSpin = new QDoubleSpinBox(blurPage);
    m_blurSigmaSpin->setRange(0.1, 10.0);
    m_blurSigmaSpin->setSingleStep(0.1);
    m_blurSigmaSpin->setValue(1.5);
    QHBoxLayout* sigmaLayout = new QHBoxLayout();
    sigmaLayout->addWidget(new QLabel(QStringLiteral("Sigma (σ):")));
    sigmaLayout->addWidget(m_blurSigmaSpin);
    blurLayout->addLayout(sigmaLayout);
    blurLayout->addStretch();
    m_paramStack->addWidget(blurPage);

    QWidget* sobelPage = new QWidget(m_paramStack);
    QVBoxLayout* sobelLayout = new QVBoxLayout(sobelPage);
    m_sobelThresholdSlider = new QSlider(Qt::Horizontal, sobelPage);
    m_sobelThresholdSlider->setRange(0, 255);
    m_sobelThresholdSlider->setValue(50);
    sobelLayout->addWidget(createSliderLabel(QStringLiteral("阈值: %1"), m_sobelThresholdSlider));
    sobelLayout->addWidget(m_sobelThresholdSlider);
    sobelLayout->addStretch();
    m_paramStack->addWidget(sobelPage);

    QWidget* oilPage = new QWidget(m_paramStack);
    QVBoxLayout* oilLayout = new QVBoxLayout(oilPage);
    m_oilRadiusSlider = new QSlider(Qt::Horizontal, oilPage);
    m_oilRadiusSlider->setRange(1, 10);
    m_oilRadiusSlider->setValue(3);
    oilLayout->addWidget(createSliderLabel(QStringLiteral("笔触半径: %1"), m_oilRadiusSlider));
    oilLayout->addWidget(m_oilRadiusSlider);

    m_oilLevelsSlider = new QSlider(Qt::Horizontal, oilPage);
    m_oilLevelsSlider->setRange(2, 256);
    m_oilLevelsSlider->setValue(20);
    oilLayout->addWidget(createSliderLabel(QStringLiteral("色阶数: %1"), m_oilLevelsSlider));
    oilLayout->addWidget(m_oilLevelsSlider);
    oilLayout->addStretch();
    m_paramStack->addWidget(oilPage);

    QWidget* sharpenPage = new QWidget(m_paramStack);
    QVBoxLayout* sharpenLayout = new QVBoxLayout(sharpenPage);
    m_sharpenStrengthSpin = new QDoubleSpinBox(sharpenPage);
    m_sharpenStrengthSpin->setRange(0.1, 5.0);
    m_sharpenStrengthSpin->setSingleStep(0.1);
    m_sharpenStrengthSpin->setValue(1.0);
    QHBoxLayout* sLayout = new QHBoxLayout();
    sLayout->addWidget(new QLabel(QStringLiteral("锐化强度:")));
    sLayout->addWidget(m_sharpenStrengthSpin);
    sharpenLayout->addLayout(sLayout);
    sharpenLayout->addStretch();
    m_paramStack->addWidget(sharpenPage);

    connect(m_blurRadiusSlider, &QSlider::valueChanged, this, &FilterControlsPanel::onParamChanged);
    connect(m_blurSigmaSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FilterControlsPanel::onParamChanged);
    connect(m_sobelThresholdSlider, &QSlider::valueChanged, this, &FilterControlsPanel::onParamChanged);
    connect(m_oilRadiusSlider, &QSlider::valueChanged, this, &FilterControlsPanel::onParamChanged);
    connect(m_oilLevelsSlider, &QSlider::valueChanged, this, &FilterControlsPanel::onParamChanged);
    connect(m_sharpenStrengthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FilterControlsPanel::onParamChanged);
}

QLabel* FilterControlsPanel::createSliderLabel(const QString& text, QSlider* slider)
{
    QLabel* label = new QLabel(text.arg(slider->value()), this);
    connect(slider, &QSlider::valueChanged, [label, text](int val) {
        label->setText(text.arg(val));
    });
    return label;
}

void FilterControlsPanel::onAddFilter()
{
    QStringList items;
    items << FilterTypes::typeToString(FilterTypes::Grayscale);
    items << FilterTypes::typeToString(FilterTypes::Invert);
    items << FilterTypes::typeToString(FilterTypes::GaussianBlur);
    items << FilterTypes::typeToString(FilterTypes::SobelEdge);
    items << FilterTypes::typeToString(FilterTypes::Emboss);
    items << FilterTypes::typeToString(FilterTypes::OilPaint);
    items << FilterTypes::typeToString(FilterTypes::Sharpen);
    items << FilterTypes::typeToString(FilterTypes::Sepia);

    bool ok = false;
    QString choice = QInputDialog::getItem(this, QStringLiteral("添加滤镜"),
        QStringLiteral("选择滤镜类型:"), items, 0, false, &ok);
    if (!ok) return;

    FilterTypes::Type type = FilterTypes::stringToType(choice);
    FilterParams params(type);

    switch (type) {
    case FilterTypes::GaussianBlur:
        params.params["radius"] = m_blurRadiusSlider->value();
        params.params["sigma"] = m_blurSigmaSpin->value();
        break;
    case FilterTypes::SobelEdge:
        params.params["threshold"] = m_sobelThresholdSlider->value();
        break;
    case FilterTypes::OilPaint:
        params.params["radius"] = m_oilRadiusSlider->value();
        params.params["levels"] = m_oilLevelsSlider->value();
        break;
    case FilterTypes::Sharpen:
        params.params["strength"] = m_sharpenStrengthSpin->value();
        break;
    default: break;
    }

    m_filterChain.append(params);
    int idx = m_filterList->count();
    m_filterList->addItem(QString("%1. %2").arg(idx + 1).arg(choice));
    m_filterList->setCurrentRow(idx);
    emit filterChainChanged();
}

void FilterControlsPanel::onRemoveFilter()
{
    int row = m_filterList->currentRow();
    if (row < 0 || row >= m_filterChain.size()) return;
    m_filterChain.removeAt(row);
    delete m_filterList->takeItem(row);
    for (int i = row; i < m_filterList->count(); ++i) {
        QString text = m_filterList->item(i)->text();
        int dotPos = text.indexOf('.');
        if (dotPos > 0) {
            QString rest = text.mid(dotPos);
            m_filterList->item(i)->setText(QString("%1%2").arg(i + 1).arg(rest));
        }
    }
    if (m_filterList->count() > 0) {
        m_filterList->setCurrentRow(qMin(row, m_filterList->count() - 1));
    } else {
        m_selectedRow = -1;
        refreshParamWidget();
    }
    emit filterChainChanged();
}

void FilterControlsPanel::onMoveUp()
{
    int row = m_filterList->currentRow();
    if (row <= 0) return;
    std::swap(m_filterChain[row], m_filterChain[row - 1]);

    QString textRow = m_filterList->item(row)->text();
    QString textPrev = m_filterList->item(row - 1)->text();
    m_filterList->item(row)->setText(QString("%1. %2").arg(row + 1).arg(textRow.mid(textRow.indexOf('.') + 2)));
    m_filterList->item(row - 1)->setText(QString("%1. %2").arg(row).arg(textPrev.mid(textPrev.indexOf('.') + 2)));

    m_filterList->setCurrentRow(row - 1);
    emit filterChainChanged();
}

void FilterControlsPanel::onMoveDown()
{
    int row = m_filterList->currentRow();
    if (row < 0 || row >= m_filterChain.size() - 1) return;
    std::swap(m_filterChain[row], m_filterChain[row + 1]);

    QString textRow = m_filterList->item(row)->text();
    QString textNext = m_filterList->item(row + 1)->text();
    m_filterList->item(row)->setText(QString("%1. %2").arg(row + 1).arg(textRow.mid(textRow.indexOf('.') + 2)));
    m_filterList->item(row + 1)->setText(QString("%1. %2").arg(row + 2).arg(textNext.mid(textNext.indexOf('.') + 2)));

    m_filterList->setCurrentRow(row + 1);
    emit filterChainChanged();
}

void FilterControlsPanel::onFilterSelected(int row)
{
    m_selectedRow = row;
    if (row >= 0 && row < m_filterChain.size()) {
        const auto& f = m_filterChain[row];
        switch (f.type) {
        case FilterTypes::GaussianBlur:
            m_blurRadiusSlider->blockSignals(true);
            m_blurSigmaSpin->blockSignals(true);
            m_blurRadiusSlider->setValue(f.params.value("radius", 3).toInt());
            m_blurSigmaSpin->setValue(f.params.value("sigma", 1.5).toDouble());
            m_blurRadiusSlider->blockSignals(false);
            m_blurSigmaSpin->blockSignals(false);
            break;
        case FilterTypes::SobelEdge:
            m_sobelThresholdSlider->blockSignals(true);
            m_sobelThresholdSlider->setValue(f.params.value("threshold", 50).toInt());
            m_sobelThresholdSlider->blockSignals(false);
            break;
        case FilterTypes::OilPaint:
            m_oilRadiusSlider->blockSignals(true);
            m_oilLevelsSlider->blockSignals(true);
            m_oilRadiusSlider->setValue(f.params.value("radius", 3).toInt());
            m_oilLevelsSlider->setValue(f.params.value("levels", 20).toInt());
            m_oilRadiusSlider->blockSignals(false);
            m_oilLevelsSlider->blockSignals(false);
            break;
        case FilterTypes::Sharpen:
            m_sharpenStrengthSpin->blockSignals(true);
            m_sharpenStrengthSpin->setValue(f.params.value("strength", 1.0).toDouble());
            m_sharpenStrengthSpin->blockSignals(false);
            break;
        default: break;
        }
    }
    refreshParamWidget();
}

void FilterControlsPanel::refreshParamWidget()
{
    if (m_selectedRow < 0 || m_selectedRow >= m_filterChain.size()) {
        m_paramStack->setCurrentIndex(0);
        return;
    }
    FilterTypes::Type t = m_filterChain[m_selectedRow].type;
    switch (t) {
    case FilterTypes::GaussianBlur: m_paramStack->setCurrentIndex(1); break;
    case FilterTypes::SobelEdge:    m_paramStack->setCurrentIndex(2); break;
    case FilterTypes::OilPaint:     m_paramStack->setCurrentIndex(3); break;
    case FilterTypes::Sharpen:      m_paramStack->setCurrentIndex(4); break;
    default:                        m_paramStack->setCurrentIndex(0); break;
    }
}

void FilterControlsPanel::onParamChanged()
{
    if (m_selectedRow < 0 || m_selectedRow >= m_filterChain.size()) return;
    auto& f = m_filterChain[m_selectedRow];
    switch (f.type) {
    case FilterTypes::GaussianBlur:
        f.params["radius"] = m_blurRadiusSlider->value();
        f.params["sigma"] = m_blurSigmaSpin->value();
        break;
    case FilterTypes::SobelEdge:
        f.params["threshold"] = m_sobelThresholdSlider->value();
        break;
    case FilterTypes::OilPaint:
        f.params["radius"] = m_oilRadiusSlider->value();
        f.params["levels"] = m_oilLevelsSlider->value();
        break;
    case FilterTypes::Sharpen:
        f.params["strength"] = m_sharpenStrengthSpin->value();
        break;
    default: break;
    }
    emit parametersChanged();
}

QList<FilterParams> FilterControlsPanel::currentFilterChain() const
{
    return m_filterChain;
}

void FilterControlsPanel::setGridRows(int rows)
{
    m_rowsSpin->setValue(rows);
}

void FilterControlsPanel::setGridCols(int cols)
{
    m_colsSpin->setValue(cols);
}

int FilterControlsPanel::gridRows() const
{
    return m_rowsSpin->value();
}

int FilterControlsPanel::gridCols() const
{
    return m_colsSpin->value();
}
