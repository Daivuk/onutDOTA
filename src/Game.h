#pragma once
#include "View.h"

class Game : public View
{
public:
    Game();
    virtual ~Game();

    void update() override;
    void render() override;
    void enter() override;

    void onChatMessage(const std::string &msg);
    void showChat();

    onut::UITextBox *pChat;
    onut::UIControl *pChatContainer;
    onut::UILabel *pChatMsgTemplate;
};
