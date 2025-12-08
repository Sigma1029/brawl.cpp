#include "LoginOk.h"

LoginOk::LoginOk() {
    id = 20104;
}

void LoginOk::encode() {
    writeInt(1);
    writeInt(1);
    writeInt(1);
    writeInt(1);
    writeString("penisdev");
    writeString();
    writeString();
    writeInt(29);
    writeInt(1);
    writeInt(1);
    writeString("prod");
    writeInt(0);
    writeInt(0);
    writeInt(0);
    writeString();
    writeString();
    writeString();
    writeInt(0);
    writeString();
    writeString("RU");
    writeString();
    writeInt(1);
    writeString();
    writeInt(2);
    writeString();
    writeString();
    writeInt(1);
    writeString();
}