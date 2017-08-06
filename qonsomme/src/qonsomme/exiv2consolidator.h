#ifndef EXIV2CONSOLIDATOR_H
#define EXIV2CONSOLIDATOR_H

#include <memory>
#include "consolidaterequest.h"
#include "consolidationresult.h"

class Exiv2Consolidator
{
public:
    Exiv2Consolidator(const std::shared_ptr<ConsolidateRequest> &request);

public:
    std::shared_ptr<ConsolidationResult> consolidate();

private:
    std::shared_ptr<ConsolidateRequest> m_Request;
};

#endif // EXIV2CONSOLIDATOR_H
