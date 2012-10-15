#include "patterntool.h"

PatternTool::PatternTool(QString pattern, QObject *parent) :
    QObject(parent),
    pattern(pattern)
{
    QStringList list = pattern.split('%', QString::SkipEmptyParts);
    QStringList regExpList;

    for(int i = 0 ; i < list.count() ; ++i) {

        QString elt = list.at(i);

        QString classicRexpPattern = "(.+)";

        if(elt == "ID" || elt == "id") {
            regExpList << "([0-9]+)";
            replacementMap[i+1] = "bpid";
        } else if(elt == "ARTISTS" || elt == "artists") {
            regExpList << classicRexpPattern;
            replacementMap[i+1] = "artists";
        } else if(elt == "TITLE" || elt == "title") {
            regExpList << classicRexpPattern;
            replacementMap[i+1] = "title";
        } else if(elt == "EXT" || elt == "ext") {
            regExpList << "([a-zA-Z0-9]{2,4})";
            replacementMap[i+1] = "ext";
        } else if(elt == "OTHER" || elt == "other") {
            regExpList << "(.+)";
        } else {
            regExpList << QString("(%1)").arg(elt.replace(".", "\\."));
        }
    }
    regExpList.prepend("^");
    regExpList.append("$");

    rexp = QRegExp(regExpList.join(""), Qt::CaseInsensitive, QRegExp::RegExp2);
}

QMap<QString, QString> PatternTool::parseValues(QString &source) const
{
    QMap<QString, QString> result;
    if(rexp.indexIn(source) != -1) {
        foreach(int i, replacementMap.keys()) {
            result[replacementMap[i]] = rexp.cap(i);
        }
    }
    return result;
}
