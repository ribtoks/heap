#ifndef CONSOLIDATIONRESULT_H
#define CONSOLIDATIONRESULT_H

#include <QString>
#include <QStringList>

class ConsolidationResult {
public:
    QString m_Filepath;
    QString m_Title;
    QString m_Description;
    QStringList m_Keywords;
};

#endif // CONSOLIDATIONRESULT_H
