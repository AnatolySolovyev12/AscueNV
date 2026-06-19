#pragma once

#include <string>

struct MyMessageObj 
{
    int chatId;
    std::string text;

    struct Chat {
        int id;
    };

    Chat* chat;  // указатель, чтобы было chat->id

    // ќператор -> дл€ доступа через message->text
    MyMessageObj* operator->() {
        return this;
    }

    MyMessageObj(int cid, const std::string& txt)
        : chatId(cid), text(txt), chat(new Chat{ cid }) {
    }

    ~MyMessageObj() {
        delete chat;
    }
};
