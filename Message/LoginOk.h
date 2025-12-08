#ifndef LOGINOK_H
#define LOGINOK_H

#include "../Stream/PiranhaMessage.h"

class LoginOk : public PiranhaMessage {
public:
    LoginOk();
    void encode() override;
};

#endif