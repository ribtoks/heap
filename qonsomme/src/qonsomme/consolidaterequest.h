#ifndef CONSOLIDATEREQUEST_H
#define CONSOLIDATEREQUEST_H

#include <QString>

struct ConsolidateRequest {
    ConsolidateRequest(const QString &filepath):
        m_Filepath(filepath)
    {}

    QString m_Filepath;
};

#endif // CONSOLIDATEREQUEST_H
