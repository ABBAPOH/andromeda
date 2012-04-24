#include "kdesettings.h"

#include <QtCore/QStringList>
#include <QtCore/QRect>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>

static const char hexDigits[] = "0123456789ABCDEF";

static bool iniUnescapedKey(const QByteArray &key, int from, int to, QString &result)
{
    bool lowercaseOnly = true;
    int i = from;
    result.reserve(result.length() + (to - from));
    while (i < to) {
        int ch = (uchar)key.at(i);

        if (ch == '\\') {
            result += QLatin1Char('/');
            ++i;
            continue;
        }

        if (ch != '%' || i == to - 1) {
            if (uint(ch - 'A') <= 'Z' - 'A') // only for ASCII
                lowercaseOnly = false;
            result += QLatin1Char(ch);
            ++i;
            continue;
        }

        int numDigits = 2;
        int firstDigitPos = i + 1;

        ch = key.at(i + 1);
        if (ch == 'U') {
            ++firstDigitPos;
            numDigits = 4;
        }

        if (firstDigitPos + numDigits > to) {
            result += QLatin1Char('%');
            // ### missing U
            ++i;
            continue;
        }

        bool ok;
        ch = key.mid(firstDigitPos, numDigits).toInt(&ok, 16);
        if (!ok) {
            result += QLatin1Char('%');
            // ### missing U
            ++i;
            continue;
        }

        QChar qch(ch);
        if (qch.isUpper())
            lowercaseOnly = false;
        result += qch;
        i = firstDigitPos + numDigits;
    }
    return lowercaseOnly;
}

inline static void iniChopTrailingSpaces(QString &str)
{
    int n = str.size() - 1;
    QChar ch;
    while (n >= 0 && ((ch = str.at(n)) == QLatin1Char(' ') || ch == QLatin1Char('\t')))
        str.truncate(n--);
}

static bool iniUnescapedStringList(const QByteArray &str, int from, int to,
                                   QString &stringResult, QStringList &stringListResult)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    static const char escapeCodes[][2] =
    {
        { 'a', '\a' },
        { 'b', '\b' },
        { 'f', '\f' },
        { 'n', '\n' },
        { 'r', '\r' },
        { 't', '\t' },
        { 'v', '\v' },
        { '"', '"' },
        { '?', '?' },
        { '\'', '\'' },
        { '\\', '\\' }
    };
    static const int numEscapeCodes = sizeof(escapeCodes) / sizeof(escapeCodes[0]);

    bool isStringList = false;
    bool inQuotedString = false;
    bool currentValueIsQuoted = false;
    int escapeVal = 0;
    int i = from;
    char ch;

StSkipSpaces:
    while (i < to && ((ch = str.at(i)) == ' ' || ch == '\t'))
        ++i;
    // fallthrough

StNormal:
    while (i < to) {
        switch (str.at(i)) {
        case '\\':
            ++i;
            if (i >= to)
                goto end;

            ch = str.at(i++);
            for (int j = 0; j < numEscapeCodes; ++j) {
                if (ch == escapeCodes[j][0]) {
                    stringResult += QLatin1Char(escapeCodes[j][1]);
                    goto StNormal;
                }
            }

            if (ch == 'x') {
                escapeVal = 0;

                if (i >= to)
                    goto end;

                ch = str.at(i);
                if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
                    goto StHexEscape;
            } else if (ch >= '0' && ch <= '7') {
                escapeVal = ch - '0';
                goto StOctEscape;
            } else if (ch == '\n' || ch == '\r') {
                if (i < to) {
                    char ch2 = str.at(i);
                    // \n, \r, \r\n, and \n\r are legitimate line terminators in INI files
                    if ((ch2 == '\n' || ch2 == '\r') && ch2 != ch)
                        ++i;
                }
            } else {
                // the character is skipped
            }
            break;
        case '"':
            ++i;
            currentValueIsQuoted = true;
            inQuotedString = !inQuotedString;
            if (!inQuotedString)
                goto StSkipSpaces;
            break;
        case ';':
            if (!inQuotedString) {
                if (!currentValueIsQuoted)
                    iniChopTrailingSpaces(stringResult);
                if (!isStringList) {
                    isStringList = true;
                    stringListResult.clear();
                    stringResult.squeeze();
                }
                stringListResult.append(stringResult);
                stringResult.clear();
                currentValueIsQuoted = false;
                ++i;
                goto StSkipSpaces;
            }
            // fallthrough
        default: {
            int j = i + 1;
            while (j < to) {
                ch = str.at(j);
                if (ch == '\\' || ch == '"' || ch == ';')
                    break;
                ++j;
            }

#ifndef QT_NO_TEXTCODEC
            if (codec) {
                stringResult += codec->toUnicode(str.constData() + i, j - i);
            } else
#endif
            {
                int n = stringResult.size();
                stringResult.resize(n + (j - i));
                QChar *resultData = stringResult.data() + n;
                for (int k = i; k < j; ++k)
                    *resultData++ = QLatin1Char(str.at(k));
            }
            i = j;
        }
        }
    }
    goto end;

StHexEscape:
    if (i >= to) {
        stringResult += QChar(escapeVal);
        goto end;
    }

    ch = str.at(i);
    if (ch >= 'a')
        ch -= 'a' - 'A';
    if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F')) {
        escapeVal <<= 4;
        escapeVal += strchr(hexDigits, ch) - hexDigits;
        ++i;
        goto StHexEscape;
    } else {
        stringResult += QChar(escapeVal);
        goto StNormal;
    }

StOctEscape:
    if (i >= to) {
        stringResult += QChar(escapeVal);
        goto end;
    }

    ch = str.at(i);
    if (ch >= '0' && ch <= '7') {
        escapeVal <<= 3;
        escapeVal += ch - '0';
        ++i;
        goto StOctEscape;
    } else {
        stringResult += QChar(escapeVal);
        goto StNormal;
    }

end:
    if (!currentValueIsQuoted)
        iniChopTrailingSpaces(stringResult);
    if (isStringList)
        stringListResult.append(stringResult);
    return isStringList;
}

static QStringList splitArgs(const QString &s, int idx)
{
    int l = s.length();
    Q_ASSERT(l > 0);
    Q_ASSERT(s.at(idx) == QLatin1Char('('));
    Q_ASSERT(s.at(l - 1) == QLatin1Char(')'));

    QStringList result;
    QString item;

    for (++idx; idx < l; ++idx) {
        QChar c = s.at(idx);
        if (c == QLatin1Char(')')) {
            Q_ASSERT(idx == l - 1);
            result.append(item);
        } else if (c == QLatin1Char(' ')) {
            result.append(item);
            item.clear();
        } else {
            item.append(c);
        }
    }

    return result;
}

static QVariant stringToVariant(const QString &s)
{
    if (s.startsWith(QLatin1Char('@'))) {
        if (s.endsWith(QLatin1Char(')'))) {
            if (s.startsWith(QLatin1String("@ByteArray("))) {
                return QVariant(s.toLatin1().mid(11, s.size() - 12));
            } else if (s.startsWith(QLatin1String("@Variant("))) {
#ifndef QT_NO_DATASTREAM
                QByteArray a(s.toLatin1().mid(9));
                QDataStream stream(&a, QIODevice::ReadOnly);
                stream.setVersion(QDataStream::Qt_4_0);
                QVariant result;
                stream >> result;
                return result;
#else
                Q_ASSERT(!"QSettings: Cannot load custom types without QDataStream support");
#endif
#ifndef QT_NO_GEOM_VARIANT
            } else if (s.startsWith(QLatin1String("@Rect("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 4)
                    return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
            } else if (s.startsWith(QLatin1String("@Size("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 2)
                    return QVariant(QSize(args[0].toInt(), args[1].toInt()));
            } else if (s.startsWith(QLatin1String("@Point("))) {
                QStringList args = splitArgs(s, 6);
                if (args.size() == 2)
                    return QVariant(QPoint(args[0].toInt(), args[1].toInt()));
#endif
            } else if (s == QLatin1String("@Invalid()")) {
                return QVariant();
            }

        }
        if (s.startsWith(QLatin1String("@@")))
            return QVariant(s.mid(1));
    }

    return QVariant(s);
}

static QVariant stringListToVariantList(const QStringList &l)
{
    QStringList outStringList = l;
    for (int i = 0; i < outStringList.count(); ++i) {
        const QString &str = outStringList.at(i);

        if (str.startsWith(QLatin1Char('@'))) {
            if (str.length() >= 2 && str.at(1) == QLatin1Char('@')) {
                outStringList[i].remove(0, 1);
            } else {
                QVariantList variantList;
                for (int j = 0; j < l.count(); ++j)
                    variantList.append(stringToVariant(l.at(j)));
                return variantList;
            }
        }
    }
    return outStringList;
}

static bool readKDEIni(QIODevice &device, QSettings::SettingsMap &map)
{
    QString currentSection;

    while (!device.atEnd()) {
        QByteArray line = device.readLine().trimmed();

        if (line.isEmpty())
            continue;

        if (line.startsWith('#'))
            continue;

        if (line.startsWith('[')) {
            int index = line.indexOf(']');
            if (index == -1) {
                qWarning() << "missing ']'";
                return false;
            }
            if (index != line.length() - 1) {
                qWarning() << "Unexpected en of line";
                return false;
            }

            QByteArray section = line.mid(1, line.length() - 2);

            section = section.trimmed();

            if (qstricmp(section, "general") == 0) {
                currentSection.clear();
            } else {
                if (qstricmp(section, "%general") == 0) {
                    currentSection = QLatin1String(section.constData() + 1);
                } else {
                    currentSection.clear();
                    iniUnescapedKey(section, 0, section.size(), currentSection);
                }
                currentSection += QLatin1Char('/');
            }

            continue;
        }

        int keyLength = line.indexOf("=");
        QByteArray key = line.left(keyLength).trimmed();
        QByteArray data = line.mid(keyLength + 1).trimmed();

        QString string;
        QStringList stringList;
        bool isStringList = iniUnescapedStringList(data, 0, data.length(), string, stringList);

        QVariant variant;
        if (isStringList) {
            variant = stringListToVariantList(stringList);
        } else {
            variant = stringToVariant(string);
        }

        QString keyString;
        iniUnescapedKey(key, 0, key.length(), keyString);

        map.insert(currentSection + keyString, variant);

    }
    return true;
}

static void iniEscapedKey(const QString &key, QByteArray &result)
{
    result.reserve(result.length() + key.length() * 3 / 2);
    for (int i = 0; i < key.size(); ++i) {
        uint ch = key.at(i).unicode();

        /*if (ch == '/') {
            result += '\\';
        } else */if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')
                   || ch == '_' || ch == '-' || ch == '.' || ch == ' ' || ch == '/') {
            result += (char)ch;
        } else if (ch <= 0xFF) {
            result += '%';
            result += hexDigits[ch / 16];
            result += hexDigits[ch % 16];
        } else {
            result += "%U";
            QByteArray hexCode;
            for (int i = 0; i < 4; ++i) {
                hexCode.prepend(hexDigits[ch % 16]);
                ch >>= 4;
            }
            result += hexCode;
        }
    }
}

static QString variantToString(const QVariant &v)
{
    QString result;

    switch (v.type()) {
        case QVariant::Invalid:
            result = QLatin1String("@Invalid()");
            break;

        case QVariant::ByteArray: {
            QByteArray a = v.toByteArray();
            result = QLatin1String("@ByteArray(");
            result += QString::fromLatin1(a.constData(), a.size());
            result += QLatin1Char(')');
            break;
        }

        case QVariant::String:
        case QVariant::LongLong:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::Bool:
        case QVariant::Double:
        case QVariant::KeySequence: {
            result = v.toString();
            if (result.startsWith(QLatin1Char('@')))
                result.prepend(QLatin1Char('@'));
            break;
        }
#ifndef QT_NO_GEOM_VARIANT
        case QVariant::Rect: {
            QRect r = qvariant_cast<QRect>(v);
            result += QLatin1String("@Rect(");
            result += QString::number(r.x());
            result += QLatin1Char(' ');
            result += QString::number(r.y());
            result += QLatin1Char(' ');
            result += QString::number(r.width());
            result += QLatin1Char(' ');
            result += QString::number(r.height());
            result += QLatin1Char(')');
            break;
        }
        case QVariant::Size: {
            QSize s = qvariant_cast<QSize>(v);
            result += QLatin1String("@Size(");
            result += QString::number(s.width());
            result += QLatin1Char(' ');
            result += QString::number(s.height());
            result += QLatin1Char(')');
            break;
        }
        case QVariant::Point: {
            QPoint p = qvariant_cast<QPoint>(v);
            result += QLatin1String("@Point(");
            result += QString::number(p.x());
            result += QLatin1Char(' ');
            result += QString::number(p.y());
            result += QLatin1Char(')');
            break;
        }
#endif // !QT_NO_GEOM_VARIANT

        default: {
#ifndef QT_NO_DATASTREAM
            QByteArray a;
            {
                QDataStream s(&a, QIODevice::WriteOnly);
                s.setVersion(QDataStream::Qt_4_0);
                s << v;
            }

            result = QLatin1String("@Variant(");
            result += QString::fromLatin1(a.constData(), a.size());
            result += QLatin1Char(')');
#else
            Q_ASSERT(!"QSettings: Cannot save custom types without QDataStream support");
#endif
            break;
        }
    }

    return result;
}

static QStringList variantListToStringList(const QVariantList &l)
{
    QStringList result;
    QVariantList::const_iterator it = l.constBegin();
    for (; it != l.constEnd(); ++it)
        result.append(variantToString(*it));
    return result;
}

static void iniEscapedString(const QString &str, QByteArray &result, QTextCodec *codec)
{
    bool needsQuotes = false;
    bool escapeNextIfDigit = false;
    int i;
    int startPos = result.size();

    result.reserve(startPos + str.size() * 3 / 2);
    for (i = 0; i < str.size(); ++i) {
        uint ch = str.at(i).unicode();
        // we don't need to escape ; in this format
        if (ch == ';' /*|| ch == ','*/ || ch == '=')
            needsQuotes = true;

        if (escapeNextIfDigit
                && ((ch >= '0' && ch <= '9')
                    || (ch >= 'a' && ch <= 'f')
                    || (ch >= 'A' && ch <= 'F'))) {
            result += "\\x";
            result += QByteArray::number(ch, 16);
            continue;
        }

        escapeNextIfDigit = false;

        switch (ch) {
        case '\0':
            result += "\\0";
            escapeNextIfDigit = true;
            break;
        case '\a':
            result += "\\a";
            break;
        case '\b':
            result += "\\b";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        case '\v':
            result += "\\v";
            break;
        case '"':
        case '\\':
            result += '\\';
            result += (char)ch;
            break;
        default:
            if (ch <= 0x1F || (ch >= 0x7F && !codec)) {
                result += "\\x";
                result += QByteArray::number(ch, 16);
                escapeNextIfDigit = true;
#ifndef QT_NO_TEXTCODEC
            } else if (codec) {
                // slow
                result += codec->fromUnicode(str.at(i));
#endif
            } else {
                result += (char)ch;
            }
        }
    }

    if (needsQuotes
            || (startPos < result.size() && (result.at(startPos) == ' '
                                                || result.at(result.size() - 1) == ' '))) {
        result.insert(startPos, '"');
        result += '"';
    }
}

static void iniEscapedStringList(const QStringList &strs, QByteArray &result, QTextCodec *codec)
{
    if (strs.isEmpty()) {
        result += "@Invalid()";
    } else {
        for (int i = 0; i < strs.size(); ++i) {
            if (i != 0)
                result += "; ";
            iniEscapedString(strs.at(i), result, codec);
        }
    }
}

static bool writeKDEIni(QIODevice &device, const QSettings::SettingsMap &map)
{
    QSettings::SettingsMap::ConstIterator it = map.begin();
    QSettings::SettingsMap::ConstIterator eit = map.end();
    const char eol = '\n';

    QTextCodec *iniCodec = QTextCodec::codecForName("UTF-8");

    bool writeError = false;

    QByteArray currentGroup;
    for (; it != eit; ++it) {

        QByteArray realSection;

        int index = it.key().indexOf("/");
        QString section = (index != -1) ? it.key().mid(0, index) : QString();

        iniEscapedKey(section, realSection);

        if (realSection.isEmpty()) {
            realSection = "[General]";
        } else if (qstricmp(realSection, "general") == 0) {
            realSection = "[%General]";
        } else {
            realSection.prepend('[');
            realSection.append(']');
        }

        if (realSection != currentGroup) {
            currentGroup = realSection;
            writeError &= device.write(realSection + eol) == -1;

            if (writeError)
                break;
        }

        QString key = it.key().mid(index + 1);

        QByteArray block;
        iniEscapedKey(key, block);
        block += '=';

        const QVariant &value = it.value();

        /*
            The size() != 1 trick is necessary because
            QVariant(QString("foo")).toList() returns an empty
            list, not a list containing "foo".
        */
        if (value.type() == QVariant::StringList
                || (value.type() == QVariant::List && value.toList().size() != 1)) {
            iniEscapedStringList(variantListToStringList(value.toList()), block, iniCodec);
        } else {
            iniEscapedString(variantToString(value), block, iniCodec);
        }
        block += eol;

        writeError &= device.write(block) == -1;

        if (writeError)
            break;
    }

    return !writeError;
}

static QSettings::Format registerFormat()
{
    return QSettings::registerFormat(".conf", readKDEIni, writeKDEIni);
}

QSettings::Format KDESettings::KDEIniFormat = ::registerFormat();

KDESettings::KDESettings(QObject *parent) :
    QSettings(parent)
{
}

KDESettings::KDESettings(const QString &fileName, QObject *parent) :
    QSettings(fileName, KDEIniFormat, parent)
//    QSettings(fileName, QSettings::IniFormat, parent)
{
}
