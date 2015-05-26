#pragma once
#include "View.h"
#include <unordered_map>

class ChatFader : public onut::Object
{
public:
    ChatFader(onut::UILabel *pLabel);
    void update();

private:
    OAnimf m_alphaAnim;
    onut::UILabel *m_pLabel;
};

class Game : public View
{
public:
    Game();
    virtual ~Game();

    void update() override;
    void render() override;
    void enter() override;
    void rts_update();

    void onChatMessage(const std::string &msg);
    void showChat();
    void updateChats();

    onut::UITextBox *pChat;
    onut::UIControl *pChatContainer;
    onut::UILabel *pChatMsgTemplate;
};
