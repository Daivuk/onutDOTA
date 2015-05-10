#pragma once
#include "View.h"
#include "Globals.h"

class Joining : public View
{
public:
    Joining();
    virtual ~Joining();

    void update() override;
    void render() override;

    void addToRow(onut::UIControl *pRow, onut::UIControl *pControl);
    void removeFromRow(onut::UIControl *pRow, onut::UIControl *pControl);
    onut::UIControl *buildCardFromUser(const Globals::SUser &user);

    onut::UIControl *pUserCardTemplate;

    Globals::SUser topUsers;
    Globals::SUser bottomUsers;
};
