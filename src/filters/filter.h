#ifndef FILTER_H
#define FILTER_H

#include <QImage>
#include <QString>
#include <QMap>
#include <QVariant>

namespace FilterTypes {
    enum Type {
        Grayscale = 0,
        Invert,
        GaussianBlur,
        SobelEdge,
        Emboss,
        OilPaint,
        Sharpen,
        Sepia
    };

    QString typeToString(Type type);
    Type stringToType(const QString& name);
}

struct FilterParams {
    FilterTypes::Type type;
    QMap<QString, QVariant> params;

    FilterParams() : type(FilterTypes::Grayscale) {}
    FilterParams(FilterTypes::Type t) : type(t) {}
};

class Filter
{
public:
    static QImage apply(const QImage& source, const FilterParams& params);

    static QImage toGrayscale(const QImage& source);
    static QImage toInvert(const QImage& source);
    static QImage toGaussianBlur(const QImage& source, int radius = 3, double sigma = 1.5);
    static QImage toSobelEdge(const QImage& source, int threshold = 50);
    static QImage toEmboss(const QImage& source);
    static QImage toOilPaint(const QImage& source, int radius = 3, int intensityLevels = 20);
    static QImage toSharpen(const QImage& source, double strength = 1.0);
    static QImage toSepia(const QImage& source);

    static QRect expandRectForKernel(const QRect& rect, int kernelRadius, const QSize& imageSize);
};

#endif
