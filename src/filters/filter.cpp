#include "filter.h"
#include <QPainter>
#include <cmath>
#include <algorithm>
#include <vector>

namespace FilterTypes {

QString typeToString(Type type)
{
    switch (type) {
    case Grayscale: return QStringLiteral("灰度");
    case Invert: return QStringLiteral("反色");
    case GaussianBlur: return QStringLiteral("高斯模糊");
    case SobelEdge: return QStringLiteral("Sobel边缘");
    case Emboss: return QStringLiteral("浮雕");
    case OilPaint: return QStringLiteral("油画");
    case Sharpen: return QStringLiteral("锐化");
    case Sepia: return QStringLiteral("复古棕");
    default: return QStringLiteral("未知");
    }
}

Type stringToType(const QString& name)
{
    if (name == QStringLiteral("灰度")) return Grayscale;
    if (name == QStringLiteral("反色")) return Invert;
    if (name == QStringLiteral("高斯模糊")) return GaussianBlur;
    if (name == QStringLiteral("Sobel边缘")) return SobelEdge;
    if (name == QStringLiteral("浮雕")) return Emboss;
    if (name == QStringLiteral("油画")) return OilPaint;
    if (name == QStringLiteral("锐化")) return Sharpen;
    if (name == QStringLiteral("复古棕")) return Sepia;
    return Grayscale;
}

}

QRect Filter::expandRectForKernel(const QRect& rect, int kernelRadius, const QSize& imageSize)
{
    if (kernelRadius <= 0) return rect;
    int x1 = qMax(0, rect.left() - kernelRadius);
    int y1 = qMax(0, rect.top() - kernelRadius);
    int x2 = qMin(imageSize.width() - 1, rect.right() + kernelRadius);
    int y2 = qMin(imageSize.height() - 1, rect.bottom() + kernelRadius);
    return QRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

static inline int clamp(int v, int lo, int hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline QRgb clampRgb(int r, int g, int b)
{
    return qRgb(clamp(r, 0, 255), clamp(g, 0, 255), clamp(b, 0, 255));
}

QImage Filter::apply(const QImage& source, const FilterParams& params)
{
    switch (params.type) {
    case FilterTypes::Grayscale:
        return toGrayscale(source);
    case FilterTypes::Invert:
        return toInvert(source);
    case FilterTypes::GaussianBlur: {
        int radius = params.params.value("radius", 3).toInt();
        double sigma = params.params.value("sigma", 1.5).toDouble();
        return toGaussianBlur(source, radius, sigma);
    }
    case FilterTypes::SobelEdge: {
        int threshold = params.params.value("threshold", 50).toInt();
        return toSobelEdge(source, threshold);
    }
    case FilterTypes::Emboss:
        return toEmboss(source);
    case FilterTypes::OilPaint: {
        int radius = params.params.value("radius", 3).toInt();
        int levels = params.params.value("levels", 20).toInt();
        return toOilPaint(source, radius, levels);
    }
    case FilterTypes::Sharpen: {
        double strength = params.params.value("strength", 1.0).toDouble();
        return toSharpen(source, strength);
    }
    case FilterTypes::Sepia:
        return toSepia(source);
    default:
        return source;
    }
}

QImage Filter::toGrayscale(const QImage& source)
{
    QImage result = source.convertToFormat(QImage::Format_RGB32);
    for (int y = 0; y < result.height(); ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            int r = qRed(line[x]);
            int g = qGreen(line[x]);
            int b = qBlue(line[x]);
            int gray = qRound(0.299 * r + 0.587 * g + 0.114 * b);
            line[x] = qRgb(gray, gray, gray);
        }
    }
    return result;
}

QImage Filter::toInvert(const QImage& source)
{
    QImage result = source.convertToFormat(QImage::Format_RGB32);
    for (int y = 0; y < result.height(); ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            line[x] = qRgb(255 - qRed(line[x]), 255 - qGreen(line[x]), 255 - qBlue(line[x]));
        }
    }
    return result;
}

QImage Filter::toGaussianBlur(const QImage& source, int radius, double sigma)
{
    if (radius <= 0) radius = 1;
    int size = 2 * radius + 1;
    std::vector<double> kernel(size * size);
    double sum = 0.0;
    double twoSigmaSq = 2.0 * sigma * sigma;

    for (int ky = -radius; ky <= radius; ++ky) {
        for (int kx = -radius; kx <= radius; ++kx) {
            double val = std::exp(-(kx * kx + ky * ky) / twoSigmaSq) / (M_PI * twoSigmaSq);
            kernel[(ky + radius) * size + (kx + radius)] = val;
            sum += val;
        }
    }
    for (auto& v : kernel) v /= sum;

    QImage src = source.convertToFormat(QImage::Format_RGB32);
    QImage result(src.size(), QImage::Format_RGB32);
    int w = src.width(), h = src.height();

    for (int y = 0; y < h; ++y) {
        QRgb* dstLine = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < w; ++x) {
            double r = 0, g = 0, b = 0;
            for (int ky = -radius; ky <= radius; ++ky) {
                int sy = clamp(y + ky, 0, h - 1);
                const QRgb* srcLine = reinterpret_cast<const QRgb*>(src.scanLine(sy));
                for (int kx = -radius; kx <= radius; ++kx) {
                    int sx = clamp(x + kx, 0, w - 1);
                    double wv = kernel[(ky + radius) * size + (kx + radius)];
                    r += qRed(srcLine[sx]) * wv;
                    g += qGreen(srcLine[sx]) * wv;
                    b += qBlue(srcLine[sx]) * wv;
                }
            }
            dstLine[x] = clampRgb(qRound(r), qRound(g), qRound(b));
        }
    }
    return result;
}

QImage Filter::toSobelEdge(const QImage& source, int threshold)
{
    QImage src = toGrayscale(source);
    int w = src.width(), h = src.height();
    QImage result(src.size(), QImage::Format_RGB32);

    static const int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    static const int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int y = 0; y < h; ++y) {
        QRgb* dstLine = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < w; ++x) {
            int sx = 0, sy = 0;
            for (int ky = -1; ky <= 1; ++ky) {
                int yy = clamp(y + ky, 0, h - 1);
                const QRgb* srcLine = reinterpret_cast<const QRgb*>(src.scanLine(yy));
                for (int kx = -1; kx <= 1; ++kx) {
                    int xx = clamp(x + kx, 0, w - 1);
                    int gray = qRed(srcLine[xx]);
                    sx += gray * gx[ky + 1][kx + 1];
                    sy += gray * gy[ky + 1][kx + 1];
                }
            }
            int mag = static_cast<int>(std::sqrt(sx * sx + sy * sy));
            int val = mag > threshold ? 255 : 0;
            dstLine[x] = qRgb(val, val, val);
        }
    }
    return result;
}

QImage Filter::toEmboss(const QImage& source)
{
    QImage src = source.convertToFormat(QImage::Format_RGB32);
    int w = src.width(), h = src.height();
    QImage result(src.size(), QImage::Format_RGB32);

    static const int kernel[3][3] = {{-2, -1, 0}, {-1, 1, 1}, {0, 1, 2}};

    for (int y = 0; y < h; ++y) {
        QRgb* dstLine = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < w; ++x) {
            int r = 0, g = 0, b = 0;
            for (int ky = -1; ky <= 1; ++ky) {
                int yy = clamp(y + ky, 0, h - 1);
                const QRgb* srcLine = reinterpret_cast<const QRgb*>(src.scanLine(yy));
                for (int kx = -1; kx <= 1; ++kx) {
                    int xx = clamp(x + kx, 0, w - 1);
                    int kv = kernel[ky + 1][kx + 1];
                    r += qRed(srcLine[xx]) * kv;
                    g += qGreen(srcLine[xx]) * kv;
                    b += qBlue(srcLine[xx]) * kv;
                }
            }
            dstLine[x] = clampRgb(r + 128, g + 128, b + 128);
        }
    }
    return result;
}

QImage Filter::toOilPaint(const QImage& source, int radius, int intensityLevels)
{
    if (radius < 1) radius = 1;
    if (intensityLevels < 2) intensityLevels = 2;

    QImage src = source.convertToFormat(QImage::Format_RGB32);
    int w = src.width(), h = src.height();
    QImage result(src.size(), QImage::Format_RGB32);

    for (int y = 0; y < h; ++y) {
        QRgb* dstLine = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < w; ++x) {
            std::vector<int> rCount(intensityLevels, 0);
            std::vector<int> gCount(intensityLevels, 0);
            std::vector<int> bCount(intensityLevels, 0);
            std::vector<int> counts(intensityLevels, 0);

            for (int ky = -radius; ky <= radius; ++ky) {
                int yy = clamp(y + ky, 0, h - 1);
                const QRgb* srcLine = reinterpret_cast<const QRgb*>(src.scanLine(yy));
                for (int kx = -radius; kx <= radius; ++kx) {
                    int xx = clamp(x + kx, 0, w - 1);
                    QRgb pix = srcLine[xx];
                    int r = qRed(pix);
                    int g = qGreen(pix);
                    int b = qBlue(pix);
                    int curIntensity = ((r + g + b) / 3) * intensityLevels / 256;
                    curIntensity = clamp(curIntensity, 0, intensityLevels - 1);
                    rCount[curIntensity] += r;
                    gCount[curIntensity] += g;
                    bCount[curIntensity] += b;
                    counts[curIntensity]++;
                }
            }

            int maxIdx = 0;
            for (int i = 1; i < intensityLevels; ++i) {
                if (counts[i] > counts[maxIdx]) maxIdx = i;
            }

            int c = counts[maxIdx] > 0 ? counts[maxIdx] : 1;
            dstLine[x] = clampRgb(rCount[maxIdx] / c, gCount[maxIdx] / c, bCount[maxIdx] / c);
        }
    }
    return result;
}

QImage Filter::toSharpen(const QImage& source, double strength)
{
    QImage src = source.convertToFormat(QImage::Format_RGB32);
    int w = src.width(), h = src.height();
    QImage result(src.size(), QImage::Format_RGB32);

    double center = 1.0 + 4.0 * strength;
    double side = -strength;

    for (int y = 0; y < h; ++y) {
        QRgb* dstLine = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < w; ++x) {
            double r = 0, g = 0, b = 0;
            int yy = clamp(y, 0, h - 1);
            int ym = clamp(y - 1, 0, h - 1);
            int yp = clamp(y + 1, 0, h - 1);
            int xx = clamp(x, 0, w - 1);
            int xm = clamp(x - 1, 0, w - 1);
            int xp = clamp(x + 1, 0, w - 1);

            const QRgb* l0 = reinterpret_cast<const QRgb*>(src.scanLine(ym));
            const QRgb* l1 = reinterpret_cast<const QRgb*>(src.scanLine(yy));
            const QRgb* l2 = reinterpret_cast<const QRgb*>(src.scanLine(yp));

            r = qRed(l1[xx]) * center + qRed(l0[xx]) * side + qRed(l2[xx]) * side + qRed(l1[xm]) * side + qRed(l1[xp]) * side;
            g = qGreen(l1[xx]) * center + qGreen(l0[xx]) * side + qGreen(l2[xx]) * side + qGreen(l1[xm]) * side + qGreen(l1[xp]) * side;
            b = qBlue(l1[xx]) * center + qBlue(l0[xx]) * side + qBlue(l2[xx]) * side + qBlue(l1[xm]) * side + qBlue(l1[xp]) * side;

            dstLine[x] = clampRgb(qRound(r), qRound(g), qRound(b));
        }
    }
    return result;
}

QImage Filter::toSepia(const QImage& source)
{
    QImage result = source.convertToFormat(QImage::Format_RGB32);
    for (int y = 0; y < result.height(); ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            int r = qRed(line[x]);
            int g = qGreen(line[x]);
            int b = qBlue(line[x]);
            int tr = qRound(0.393 * r + 0.769 * g + 0.189 * b);
            int tg = qRound(0.349 * r + 0.686 * g + 0.168 * b);
            int tb = qRound(0.272 * r + 0.534 * g + 0.131 * b);
            line[x] = clampRgb(tr, tg, tb);
        }
    }
    return result;
}
