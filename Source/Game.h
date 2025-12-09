// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include "Renderer/Renderer.h"
#include "AudioSystem.h"
#include "Actors/Robot.h"
#include "Actors/RepairLevel.h"

struct TiledTileInfo {
    uint32_t id;       // real tile ID (-1 = empty)
    bool flipH;        // horizontal flip
    bool flipV;        // vertical flip
    bool flipD;        // diagonal flip (used for rotation)
};

enum class GameScene
{
    MainMenu,
    Level1,
    PauseMenu,
    GameOver,
    Win
};

class Game
{
public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Audio
    AudioSystem* GetAudio() { return mAudio; }
    SoundHandle GetMusicHandle() const { return mMusicHandle; }

    // Tiles Processing
    TiledTileInfo DecodeTiledID(uint32_t raw);

    // Scene Handling
    void SetScene(GameScene scene);
    void UnloadScene();
    void SetIsPaused(const bool isPaused) { mIsPaused = isPaused; }
    void SetSceneTransitioning(const bool isSceneTransitioning) { mIsSceneTransitioning = isSceneTransitioning; }

    // Renderer
    class Renderer* GetRenderer() { return mRenderer; }

    // Getters
    class Robot* GetPlayer() { return mRobot; }
    class HUD* GetHUD() { return mHUD; }
    bool GetIsPaused() { return mIsPaused; }
    int GetLevelWidth() const { return mLevelWidth; }
    int GetLevelHeight() const { return mLevelHeight; }
    float GetWinConditionY() const { return mWinConditionY; }
    float GetWinConditionX() const { return mWinConditionX; }

    static const int WINDOW_WIDTH   = 960;
    static const int WINDOW_HEIGHT  = 768;
    static const int TILE_SIZE      = 32;
    static const int SPAWN_DISTANCE = 400;
    static const int FPS = 60;

    static const int EPS = 2;

    // Draw functions
    void AddDrawable(class DrawComponent* drawable);
    void RemoveDrawable(class DrawComponent* drawable);
    std::vector<class DrawComponent*>& GetDrawables() { return mDrawables; }

    // Collider functions
    void AddCollider(class AABBColliderComponent* collider);
    void RemoveCollider(class AABBColliderComponent* collider);
    std::vector<class AABBColliderComponent*>& GetColliders() { return mColliders; }

    // Camera functions
    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) {
        mCameraPos = position;
        };

    void LoadSounds();

    void PlayDeadChunk();
    void PlayJumpChunk() const;
    void PlayShootChunk() const;
    void PlayStageClearChunk();
    void SpawnRandomGoldRings(int quantidade, int width, int height, int** levelData);

    void PlayGlitchChunk() const;
    void PlayFailedShotChunk() const;

    int LoadProgressData();
    void SaveProgressData(int levelNumber);
    bool CheckLevelExists(int levelNumber);

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void UpdateCamera();
    void GenerateOutput();

    // Level loading
    void LoadLevelDimensions(int levelNumber);
    int **LoadLevel(const std::string& fileName, int width, int height);
    void BuildLevel(int** levelData, int width, int height);

    // All the actors in the game
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;

    // Camera
    Vector2 mCameraPos;

    // All the draw components
    std::vector<class DrawComponent*> mDrawables;

    // All the collision components
    std::vector<class AABBColliderComponent*> mColliders;

    // UI screens
    std::vector<class UIScreen*> mUIStack;
    bool mIsSceneTransitioning;

    // SDL stuff
    SDL_Window* mWindow;
    class Renderer* mRenderer;

    // Audio system
    AudioSystem* mAudio;
    SoundHandle mMusicHandle;

    //Mix_Chunk *mBumpChunk;
    //Mix_Chunk *mCoinChunk;
    Mix_Chunk *mDeadChunk;
    Mix_Chunk *mJumpChunk;
    Mix_Chunk *mShootChunk;
    //Mix_Chunk *mPowerUpChunk;
    Mix_Chunk *mStageClearChunk;
    Mix_Chunk* mGlitchChunk;
    Mix_Chunk* mFailedShotChunk;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;
    bool mIsPaused;

    // Game-specific
    class Robot *mRobot;
    class HUD *mHUD;
    int **mLevelData;
    int mLevelWidth;
    int mLevelHeight;
    RepairLevel mSavedRepairLevel;
    float mWinConditionY;
    float mWinConditionX;
};
