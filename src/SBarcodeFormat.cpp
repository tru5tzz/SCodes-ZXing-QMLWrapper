#include "SBarcodeFormat.h"

/*!
 *  Provide access to ZXing::BarcodeFormat's with SCodes::SBarcodeFormat keys
 */
namespace {
const QMap<SCodes::SBarcodeFormat, ZXing::BarcodeFormat> k_formatsTranslations
{
    { SCodes::SBarcodeFormat::None, ZXing::BarcodeFormat::None },
    { SCodes::SBarcodeFormat::Aztec, ZXing::BarcodeFormat::Aztec },
    { SCodes::SBarcodeFormat::Codabar, ZXing::BarcodeFormat::Codabar },
    { SCodes::SBarcodeFormat::Code39, ZXing::BarcodeFormat::Code39 },
    { SCodes::SBarcodeFormat::Code93, ZXing::BarcodeFormat::Code93 },
    { SCodes::SBarcodeFormat::Code128, ZXing::BarcodeFormat::Code128 },
    { SCodes::SBarcodeFormat::DataBar, ZXing::BarcodeFormat::DataBar },
    { SCodes::SBarcodeFormat::DataBarExpanded, ZXing::BarcodeFormat::DataBarExpanded },
    { SCodes::SBarcodeFormat::DataMatrix, ZXing::BarcodeFormat::DataMatrix },
    { SCodes::SBarcodeFormat::EAN8, ZXing::BarcodeFormat::EAN8 },
    { SCodes::SBarcodeFormat::EAN13, ZXing::BarcodeFormat::EAN13 },
    { SCodes::SBarcodeFormat::ITF, ZXing::BarcodeFormat::ITF },
    { SCodes::SBarcodeFormat::MaxiCode, ZXing::BarcodeFormat::MaxiCode },
    { SCodes::SBarcodeFormat::PDF417, ZXing::BarcodeFormat::PDF417 },
    { SCodes::SBarcodeFormat::QRCode, ZXing::BarcodeFormat::QRCode },
    { SCodes::SBarcodeFormat::UPCA, ZXing::BarcodeFormat::UPCA },
    { SCodes::SBarcodeFormat::UPCE, ZXing::BarcodeFormat::UPCE },
    { SCodes::SBarcodeFormat::Any, ZXing::BarcodeFormat::Any },
};
}

ZXing::BarcodeFormat SCodes::toZXingFormat(SBarcodeFormat format)
{
    return k_formatsTranslations[format];
}

QString SCodes::toString(SBarcodeFormat format)
{
    QString *converted;
    /* TODO: fix qDebug
    */
    qDebug() << "SBarcodeFormat::SCodes::toString return: " << QString(stdStringtoQt(converted, ZXing::ToString(toZXingFormat(format))));
    return QString(stdStringtoQt(converted, ZXing::ToString(toZXingFormat(format))));
}

SCodes::SBarcodeFormat SCodes::fromString(const QString &formatName)
{
    const auto zxingFormat = ZXing::BarcodeFormatFromString(formatName.toStdString());

    QMapIterator<SCodes::SBarcodeFormat, ZXing::BarcodeFormat> it(k_formatsTranslations);

    while (it.hasNext()) {
        it.next();

        if (it.value() == zxingFormat) {
            return it.key();
        }
    }

    return SCodes::SBarcodeFormat::None;
}

ZXing::BarcodeFormats SCodes::toZXingFormat(SBarcodeFormats formats)
{
    ZXing::BarcodeFormats zXingformats;

    int formatEnumIndex = SCodes::staticMetaObject.indexOfEnumerator("SBarcodeFormat");

    QMetaEnum sBarcodeFormatEnumMeta = SCodes::staticMetaObject.enumerator(formatEnumIndex);

    for (int i = 0; i < sBarcodeFormatEnumMeta.keyCount(); ++i) {
        const auto key = sBarcodeFormatEnumMeta.key(i);

        const auto value = sBarcodeFormatEnumMeta.keyToValue(key);

        const auto enumValue = static_cast<SCodes::SBarcodeFormat>(value);

        if (formats.testFlag(enumValue)) {
            zXingformats |= toZXingFormat(enumValue);
        }
    }

    return zXingformats;
}

QString SCodes::stdStringtoQt(QString *buffer, std::string str) {
    int length = str.length();
    buffer = new QString();
    QString result;

    for (int i = 0; i < length; i++) {
        buffer->append(str[i]);
        result.append(str[i]);
    }

    /* TODO: fix qDebug
    */
    qDebug() << "SBarcodeFormat::SCodes::stdStringtoQt returns: " << result;
    return result;
}
