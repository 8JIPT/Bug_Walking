#include "IntroScene.h"
#include "../../Game.h"
#include "../UIText.h"
#include "../UIRect.h"

IntroScene::IntroScene(Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
    , mState(IntroState::Flashing)
    , mFlashTimer(0.0f)
    , mFlashCount(0)
    , mCurrentLineIndex(0)
    , mTypeTimer(0.0f)
    , mCharIndex(0)
    , mAlarmHandle(SoundHandle::Invalid)
    , mTextDisplay(nullptr)
{
    mBackground = AddRect(Vector2(Game::WINDOW_WIDTH/2, Game::WINDOW_HEIGHT/2),
                          Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT));
    mBackground->SetColor(Vector4(0.3f, 0.0f, 0.0f, 1.0f)); // vermelho alerta

    mFlashRect = AddRect(Vector2(Game::WINDOW_WIDTH/2, Game::WINDOW_HEIGHT/2),
                         Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT));
    mFlashRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.7f)); // flash
    mFlashRect->SetIsVisible(false);

    int wrapLimit = static_cast<int>(Game::WINDOW_WIDTH * 0.7f);

    mTextDisplay = new UIText(game, "", mFont,
                              Vector2(Game::WINDOW_WIDTH/2, Game::WINDOW_HEIGHT/2 - 20),
                              1.0f, 0.0f, 40, wrapLimit);

    mTextDisplay->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    mTextDisplay->SetBackgroundColor(Vector4(0.3f, 0.0f, 0.0f, 1.0f));

    // roteiro
    mDialogueLines.push_back("\nALERT!\nEARTH IS UNDER PLANETARY SIEGE\nUNKNOWN HOSTILES DETECTED");

    mDialogueLines.push_back("\nCHECKING SENTINEL STATUS:\nSENTINEL PROJECT V4 - UNRESPONSIVE\nSENTINEL PROJECT V3 - UNRESPONSIVE\nSENTINEL PROJECT V2 - UNRESPONSIVE\nSENTINEL PROJECT V1 - OFFLINE");

    mDialogueLines.push_back("\nEMERGENCY SYSTEM OVERRIDE:\nACTIVATE: V1 .......... SUCCESS");

    mDialogueLines.push_back("\nSYSTEM STATUS: CRITICAL\nMOVEMENT: CRITICAL\nWEAPONS: CRITICAL");

    mDialogueLines.push_back("\nMAINTENANCE: OVERDUE");

    mDialogueLines.push_back("\nPRIORITY OVERRIDE\nNEW OBJECTIVE: SAVE HUMANITY");

    // loop alarme
    AudioSystem* audio = GetGame()->GetAudio();
    if (audio) {
        mAlarmHandle = audio->PlaySound("Emergency Alert.wav", true);
        audio->SetSoundVolume(mAlarmHandle, 20);
    }
}

IntroScene::~IntroScene()
{
    AudioSystem* audio = GetGame()->GetAudio();
    if (audio && mAlarmHandle != SoundHandle::Invalid) {
        audio->StopSound(mAlarmHandle);
    }

    if (mTextDisplay) {
        delete mTextDisplay;
        mTextDisplay = nullptr;
    }
}

void IntroScene::Update(float deltaTime)
{
    UIScreen::Update(deltaTime);

    if (mState == IntroState::Flashing)
    {
        mFlashTimer += deltaTime;
        if (mFlashTimer >= 0.2f) {
            mFlashTimer = 0.0f;
            mFlashCount++;

            bool isDark = (mFlashCount % 2 != 0);
            mFlashRect->SetIsVisible(isDark);

            if (mTextDisplay) mTextDisplay->SetIsVisible(!isDark);

            if (mFlashCount >= 6) {
                mState = IntroState::Typing;
                mFlashRect->SetIsVisible(false);
                if (mTextDisplay) mTextDisplay->SetIsVisible(true);
            }
        }
    }
    else if (mState == IntroState::Typing)
    {
        if (mCurrentLineIndex >= mDialogueLines.size()) {
            mState = IntroState::Finished;
            return;
        }

        std::string targetText = mDialogueLines[mCurrentLineIndex];

        if (mCharIndex < targetText.length()) {
            mTypeTimer += deltaTime;
            if (mTypeTimer >= 0.04f) {
                mTypeTimer = 0.0f;
                mCurrentTextShown += targetText[mCharIndex];
                mCharIndex++;
                if (mTextDisplay) mTextDisplay->SetText(mCurrentTextShown);
            }
        }
    }
    else if (mState == IntroState::Finished)
    {
        GetGame()->SetScene(GameScene::Level);
    }
}

// para passar a escrita
void IntroScene::HandleKeyPress(int key)
{
    if (key == SDLK_RETURN || key == SDLK_SPACE || key == SDLK_KP_ENTER)
    {
        if (mState == IntroState::Flashing) {
            mState = IntroState::Typing;
            mFlashRect->SetIsVisible(false);
            if (mTextDisplay) mTextDisplay->SetIsVisible(true);
        }
        else if (mState == IntroState::Typing)
        {
            std::string targetText = mDialogueLines[mCurrentLineIndex];

            if (mCharIndex < targetText.length()) {
                mCurrentTextShown = targetText;
                mCharIndex = targetText.length();
                if (mTextDisplay) mTextDisplay->SetText(mCurrentTextShown);
            }
            else {
                mCurrentLineIndex++;
                if (mCurrentLineIndex >= mDialogueLines.size()) {
                    mState = IntroState::Finished;
                    GetGame()->SetScene(GameScene::Level);
                } else {
                    mCurrentTextShown = "";
                    mCharIndex = 0;
                    if (mTextDisplay) mTextDisplay->SetText("");
                }
            }
        }
    }
}