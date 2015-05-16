#pragma once
#include "View.h"
#include "Globals.h"
#include <chrono>

class Joining : public View
{
public:
    enum class eState
    {
        JOINING,
        STATE_ERROR,
        WAITING
    } state = eState::JOINING;

    Joining();
    virtual ~Joining();

    void update() override;
    void render() override;
    void requestUpdate();
    void updateGameContent(const std::string& json);
    void showError(const std::string &json);
    void natPunchThrough();
    void sendIPPort();
    void updateRTSPeers();
    void forceStartGame();

    void addToRow(onut::UIControl *pRow, onut::UIControl *pControl);
    onut::UIControl *buildCardFromUser(const Globals::SUser &user);

    onut::UIControl *pUserCardTemplate;

    Globals::SUser topUsers;
    Globals::SUser bottomUsers;
    std::chrono::steady_clock::time_point lastUpdateRequestTime;
    bool lastRequestCompleted = true;
};
