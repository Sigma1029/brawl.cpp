#ifndef OWNHOMEDATA_H
#define OWNHOMEDATA_H

#include "../Stream/PiranhaMessage.h"

class OwnHomeData : public PiranhaMessage {
public:
    OwnHomeData();
    void encode() override;
};

#endif