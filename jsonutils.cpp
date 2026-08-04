#include "jsonutils.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

QString readTitleFromJson(const QString &dir)
{
    QFile f(QDir(dir).filePath(VIDEO_INFO_FILENAME));
    if (!f.open(QIODevice::ReadOnly))
        return QString();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (doc.isNull() || !doc.isObject())
        return QString();

    return doc.object().value("title").toString();
}

QString sanitizeTitle(const QString &raw)
{
    QString t = raw;
    // 替换 Windows 文件名非法字符
    t.replace(QRegularExpression(R"([\\/:*?"<>|])"), "_");
    t = t.trimmed();
    while (t.endsWith('.')) // 去掉结尾的点
        t.chop(1);
    return t.isEmpty() ? "untitled" : t;
}
