#include <Windows.h>
#include "onut.h"
#include "Game.h"
#include "Main.h"
#include "Login.h"
#include "eg.h"
#include "Globals.h"

onut::UIContext *g_pUIContext = nullptr;
View *g_pCurrentView = nullptr;

void hookButtonSounds(onut::UIControl *pCtrl)
{
    if (dynamic_cast<onut::UIButton *>(pCtrl))
    {
        pCtrl->onMouseEnter = [](onut::UIControl* pCtrl, const onut::UIMouseEvent& evt)
        {
            OPlaySound("buttonHover.wav");
        };
        pCtrl->onClick = [](onut::UIControl* pCtrl, const onut::UIMouseEvent& evt)
        {
            OPlaySound("buttonClick.wav");
        };
    }
    if (dynamic_cast<onut::UITextBox *>(pCtrl))
    {
        pCtrl->onMouseEnter = [](onut::UIControl* pCtrl, const onut::UIMouseEvent& evt)
        {
            OPlaySound("buttonHover.wav");
        };
        pCtrl->onClick = [](onut::UIControl* pCtrl, const onut::UIMouseEvent& evt)
        {
            OPlaySound("txtClick.wav");
        };
    }

    for (auto pChild : pCtrl->getChildren())
    {
        hookButtonSounds(pChild);
    }
}

int CALLBACK WinMain(
    _In_  HINSTANCE hInstance,
    _In_  HINSTANCE hPrevInstance,
    _In_  LPSTR lpCmdLine,
    _In_  int nCmdShow
    )
{
    OSettings->setGameName("Oak Nut DOTA");
    OSettings->setResolution({1280, 720});

    onut::run([]
    {
        g_pUIContext = new onut::UIContext(onut::sUIVector2{OScreenWf, OScreenHf});

        g_pUIContext->onClipping = [](bool enabled, const onut::sUIRect& rect)
        {
            OSB->end();
            ORenderer->setScissor(enabled, onut::UI2Onut(rect));
            OSB->begin();
        };

        g_pUIContext->addStyle<onut::UIPanel>("blur", [](const onut::UIPanel* pControl, const onut::sUIRect& rect)
        {
            OSB->end();
            egEnable(EG_BLUR);
            egBlur(32.f);
            egPostProcess();
            ORenderer->resetState();
            OSB->begin();
            OSB->drawRect(nullptr, onut::UI2Onut(rect), Color(0, 0, 0, .35f));
        });

        auto getTextureForState = [](onut::UIControl *pControl, const std::string &filename)
        {
            static std::string stateFilename;
            stateFilename = filename;
            OTexture *pTexture;
            switch (pControl->getState(*g_pUIContext))
            {
                case onut::eUIState::NORMAL:
                    pTexture = OGetTexture(filename.c_str());
                    break;
                case onut::eUIState::DISABLED:
                    stateFilename.insert(filename.size() - 4, "_disabled");
                    pTexture = OGetTexture(stateFilename.c_str());
                    if (!pTexture) pTexture = OGetTexture(filename.c_str());
                    break;
                case onut::eUIState::HOVER:
                    stateFilename.insert(filename.size() - 4, "_hover");
                    pTexture = OGetTexture(stateFilename.c_str());
                    if (!pTexture) pTexture = OGetTexture(filename.c_str());
                    break;
                case onut::eUIState::DOWN:
                    stateFilename.insert(filename.size() - 4, "_down");
                    pTexture = OGetTexture(stateFilename.c_str());
                    if (!pTexture) pTexture = OGetTexture(filename.c_str());
                    break;
            }
            return pTexture;
        };

        g_pUIContext->addStyle<onut::UIImage>("spinner", [=](const onut::UIImage* pControl, const onut::sUIRect& rect)
        {
            static uint32_t frameAnim = 0;
            frameAnim++;
            auto frame = (frameAnim / 6) % 8;
            Vector4 uvs{
                static_cast<float>(frame) / 8.f,
                0,
                static_cast<float>(frame + 1) / 8.f,
                1
            };
            OSB->drawRectWithUVs(getTextureForState((onut::UIControl *)pControl, pControl->scale9Component.image.filename),
                                 onut::UI2Onut(rect), 
                                 uvs, 
                                 onut::UI2Onut(pControl->scale9Component.image.color));
        });

        g_pUIContext->drawRect = [=](onut::UIControl *pControl, const onut::sUIRect &rect, const onut::sUIColor &color)
        {
            OSB->drawRect(nullptr, onut::UI2Onut(rect), onut::UI2Onut(color));
        };

        g_pUIContext->drawTexturedRect = [=](onut::UIControl *pControl, const onut::sUIRect &rect, const onut::sUIImageComponent &image)
        {
            OSB->drawRect(getTextureForState(pControl, image.filename),
                          onut::UI2Onut(rect), 
                          onut::UI2Onut(image.color));
        };

        g_pUIContext->drawScale9Rect = [=](onut::UIControl* pControl, const onut::sUIRect& rect, const onut::sUIScale9Component& scale9)
        {
            const std::string &filename = scale9.image.filename;
            OTexture *pTexture;
            switch (pControl->getState(*g_pUIContext))
            {
                case onut::eUIState::NORMAL:
                    pTexture = OGetTexture(filename.c_str());
                    break;
                case onut::eUIState::DISABLED:
                    pTexture = OGetTexture((filename + "_disabled").c_str());
                    if (!pTexture) pTexture = OGetTexture(filename.c_str());
                    break;
                case onut::eUIState::HOVER:
                    pTexture = OGetTexture((filename + "_hover").c_str());
                    if (!pTexture) pTexture = OGetTexture(filename.c_str());
                    break;
                case onut::eUIState::DOWN:
                    pTexture = OGetTexture((filename + "_down").c_str());
                    if (!pTexture) pTexture = OGetTexture(filename.c_str());
                    break;
            }
            if (scale9.isRepeat)
            {
                OSB->drawRectScaled9RepeatCenters(getTextureForState(pControl, scale9.image.filename),
                                                  onut::UI2Onut(rect),
                                                  onut::UI2Onut(scale9.padding),
                                                  onut::UI2Onut(scale9.image.color));
            }
            else
            {
                OSB->drawRectScaled9(getTextureForState(pControl, scale9.image.filename),
                                     onut::UI2Onut(rect),
                                     onut::UI2Onut(scale9.padding),
                                     onut::UI2Onut(scale9.image.color));
            }
        };

        g_pUIContext->drawText = [=](onut::UIControl* pControl, const onut::sUIRect& rect, const onut::sUITextComponent& text)
        {
            if (text.text.empty()) return;
            auto align = onut::UI2Onut(text.font.align);
            auto oRect = onut::UI2Onut(rect);
            auto pFont = OGetBMFont(text.font.typeFace.c_str());
            auto oColor = onut::UI2Onut(text.font.color);
            if (pControl->getState(*g_pUIContext) == onut::eUIState::DISABLED)
            {
                oColor = {.4f, .4f, .4f, 1};
            }

            if (pFont)
            {
                if (pControl->getStyleName() == "password")
                {
                    static std::string pwd;
                    pwd.resize(text.text.size(), '*');
                    pFont->draw<>(pwd, ORectAlign<>(oRect, align), oColor, OSB, align);
                }
                else
                {
                    pFont->draw<>(text.text, ORectAlign<>(oRect, align), oColor, OSB, align);
                }
            }
        };

        Globals::init();
        g_pCurrentView = new Login();
        g_pCurrentView->enter();

        OWindow->onWrite = [](char c)
        {
            g_pUIContext->write(c);
        };
        OWindow->onKey = [](uintptr_t key)
        {
            g_pUIContext->keyDown(key);
        };
    },
        []
    {
        g_pUIContext->resize({OScreenWf, OScreenHf});
        g_pCurrentView->pUIScreen->update(*g_pUIContext, {OMousePos.x, OMousePos.y}, OInput->isStateDown(DIK_MOUSEB1));
        g_pCurrentView->update();
    },
        []
    {
        ORenderer->clear(Color::Black);
        g_pCurrentView->render();
        OSB->begin();
        g_pCurrentView->pUIScreen->render(*g_pUIContext);
        OSB->end();
    });
}
