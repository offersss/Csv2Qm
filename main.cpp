#include "Csv.h"

#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QTextCodec>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
static const QStringList Languages = {
    "zh_CN", "en"
};
#endif

static QString convertContext(QList<QVariantMap> source, QString contextTitle, QString language)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QStringList Languages;
    Languages<<"zh_CN"<<"en";
#endif

    QString context = QString("<context>\n");
    context += QString("\t<name>%1</name>\n").arg(contextTitle);
    for (int i = 0; i < source.count(); i++) {
        QVariantMap item;
        item["source"] = source.at(i).value("key").toString();
        if (Languages.contains(language)) {
            item["translation"] = source.at(i).value(language).toString();
        }
        else {
            qDebug()<<"Failed!!!";
            exit(-1);
        }

        context += QString("\t<message>\n\t\t<source>%1</source>\n\t\t<translation>%2</translation>\n\t</message>\n")
                    .arg(item["source"].toString())
                    .arg(item["translation"].toString());
    }
    context += QString("</context>\n");
    return context;
}

static bool convert(QList<QVariantMap> source, QString language, QString outputFile)
{
    QString head = QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n") +
        QString("<!DOCTYPE TS>\n") +
        QString("<TS version=\"2.0\" language=\"%1\">\n").arg(language);

    QString content;
    content += convertContext(source, "QObject", language);

    QString tail = QString("</TS>");

    QString result = head + content + tail;
    QFile file(outputFile);
    file.open(QFile::WriteOnly);
    file.write(result.toUtf8());
    file.close();
}

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
    const char *ts2qmCmdLine = "lrelease-qt4 *.ts";
#else
    const char *ts2qmCmdLine = "lrelease *.ts";
#endif

    QString file = "translation.csv";
    if (argc == 2) {
        file = argv[1];
    }

    Csv csvor;
    csvor.open(file);

    QList<QVariantMap> csvContexts = csvor.readAll();

    if (csvContexts.isEmpty()) {
        qDebug()<<"csvContexts Failed!!!";
        return -1;
    }

    convert(csvContexts, "zh_CN", "zh_CN.ts");
    convert(csvContexts, "en", "en.ts");

    if (system(ts2qmCmdLine) == 0)
        qDebug()<<"Success!!!";
    else
        qDebug()<<"Failed!!!";

    return 0;
}
