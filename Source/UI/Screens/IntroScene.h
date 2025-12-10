#pragma once
#include "UIScreen.h"
#include <vector>
#include <string>
#include "../../AudioSystem.h"

class IntroScene : public UIScreen
{
public:
    IntroScene(class Game* game, const std::string& fontName);
    ~IntroScene();

    void Update(float deltaTime) override;
    void HandleKeyPress(int key) override;

private:
    enum class IntroState {
        Flashing,
        Typing,
        Finished
    };

    IntroState mState;

    class UIRect* mBackground;
    class UIRect* mFlashRect;
    class UIText* mTextDisplay;

    // Lógica do Flash
    float mFlashTimer;
    int mFlashCount;
    const int MAX_FLASHES = 6;

    // Lógica do Typewriter
    std::vector<std::string> mDialogueLines;
    int mCurrentLineIndex;
    
    std::string mCurrentTextShown;
    float mTypeTimer;
    int mCharIndex;
    const float TYPE_SPEED = 0.05f;

    SoundHandle mAlarmHandle;
};