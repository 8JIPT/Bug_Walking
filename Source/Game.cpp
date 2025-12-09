// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "CSV.h"
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Robot.h"
#include "Actors/Chaser.h"
#include "Actors/ChaserSpawner.h"
#include "Actors/Walker.h"
#include "Actors/WalkerSpawner.h"
#include "Actors/GoldRing.h"
#include "AudioSystem.h"
#include "UI/Screens/HUD.h"
#include "UI/Screens/MainMenu.h"
#include "UI/Screens/PauseMenu.h"
#include "UI/Screens/GameOver.h"
#include "UI/Screens/Win.h"
#include "UI/Screens/CrossFadeScreen.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(true)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
        ,mRobot(nullptr)
        ,mAudio(nullptr)
        ,mHUD(nullptr)
        ,mLevelData(nullptr)
        ,mIsPaused(false)
        ,mIsSceneTransitioning(false)
        ,mMusicHandle(SoundHandle::Invalid)
        ,mEmergencyAlertHandle(SoundHandle::Invalid)
        ,mLevelWidth(30)
        ,mLevelHeight(90)
        ,mSavedRepairLevel(RepairLevel::Critical)
        ,mWinConditionY(-1.0f)
        ,mWinConditionX(-1.0f)
{

}

bool Game::Initialize()
{
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Init SDL Image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer: %s", Mix_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Bug Walking",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH, WINDOW_HEIGHT,SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mAudio = new AudioSystem();

    LoadSounds();

    // Start with main menu scene
    SetScene(GameScene::MainMenu);

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::InitializeActors()
{
    // Load and build level for playing scene
    mLevelData = LoadLevel("../Assets/Levels/level2/level2.csv", mLevelWidth, mLevelHeight);
    if (mLevelData) BuildLevel(mLevelData, mLevelWidth, mLevelHeight);
}

void Game::UnloadScene()
{
    // Stop emergency alert if playing
    if (mEmergencyAlertHandle != SoundHandle::Invalid) {
        mAudio->StopSound(mEmergencyAlertHandle);
        mEmergencyAlertHandle = SoundHandle::Invalid;
    }

    // Save robot's repair level before destroying
    if (mRobot) {
        mSavedRepairLevel = mRobot->GetRepairLevel();
        SDL_Log("Saving Robot repair level: %d", static_cast<int>(mSavedRepairLevel));
    }

    // Set all actors to destroy so they will be deleted in UpdateActors
    for (auto* actor : mActors) {
        actor->SetState(ActorState::Destroy);
    }

    // Delete UI screens (preserve crossfade if present)
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if (dynamic_cast<CrossFadeScreen*>(*iter) != nullptr) {
            ++iter;
        } else {
            // Check if this is the HUD before deleting
            if (*iter == mHUD) {
                mHUD = nullptr;
            }
            delete *iter;
            iter = mUIStack.erase(iter);
        }
    }

    // Delete level data (if any)
    if (mLevelData) {
        for (int i = 0; i < mLevelHeight; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    // Reset pointers
    mRobot = nullptr;
    // HUD already deleted in the UI stack loop above
    mHUD = nullptr;
}

void Game::SetScene(GameScene nextScene)
{
    mIsSceneTransitioning = true;

    // If we are not paused, unload current scene so the new one starts clean.
    if (!mIsPaused) UnloadScene();

    switch (nextScene) {
        case GameScene::MainMenu: {
            auto* menu = new MainMenu(this, "../Assets/Fonts/Silver.ttf");
            // Reset repair level when returning to main menu
            mSavedRepairLevel = RepairLevel::Critical;
            SDL_Log("Reset repair level to Critical");
            // Start music if not already playing
            if (mMusicHandle == SoundHandle::Invalid || mAudio->GetSoundState(mMusicHandle) != SoundState::Playing) {
                mMusicHandle = mAudio->PlaySound("S31-The Gears of Progress.ogg", true);
                mAudio->SetSoundVolume(mMusicHandle, 51); // Set to 40% volume (51/128)
            }
            break;
        }
        case GameScene::Level: {
            // Reset camera to start position
            mCameraPos = Vector2::Zero;
            
            // Load current level from progress data
            int currentLevel = LoadProgressData();
            LoadLevelDimensions(currentLevel);
            
            std::string level = "level" + std::to_string(currentLevel);
            std::string levelPath = "../Assets/Levels/" + level + "/" + level + ".csv";
            
            // Initialize level and HUD
            mLevelData = LoadLevel(levelPath, mLevelWidth, mLevelHeight);
            if (mLevelData) BuildLevel(mLevelData, mLevelWidth, mLevelHeight);

            mHUD = new HUD(this, "../Assets/Fonts/Silver.ttf");
            if (mRobot && mHUD)
            {
                mHUD->SetHealth(mRobot->GetHitPoints());
            }
            break;
        }
        case GameScene::PauseMenu: {
            auto* pauseMenu = new PauseMenu(this, "../Assets/Fonts/Silver.ttf");
            if (mAudio) mAudio->PauseSound(mMusicHandle);
            if (mAudio) mAudio->PauseSound(mEmergencyAlertHandle);
            break;
        }
        case GameScene::GameOver: {
            auto* gameOver = new GameOver(this, "../Assets/Fonts/Silver.ttf");
            if (mAudio) mAudio->StopSound(mMusicHandle);
            break;
        }
        case GameScene::Win: {
            auto* winScreen = new Win(this, "../Assets/Fonts/Silver.ttf");
            //if (mAudio) mAudio->StopSound(mMusicHandle);
            break;
        }
        default:
            SDL_Log("Unknown scene.");
            break;
    }

    mIsSceneTransitioning = false;
}

void Game::LoadLevelDimensions(int levelNumber)
{
    std::string level = "level" + std::to_string(levelNumber);
    std::string dimensionsPath = "../Assets/Levels/" + level + "/dimensions.txt";
    
    std::ifstream file(dimensionsPath);
    if (!file.is_open()) {
        SDL_Log("Dimensions file not found for level %d. Using defaults (30x90).", levelNumber);
        mLevelWidth = 30;
        mLevelHeight = 90;
        mWinConditionY = -1.0f;
        mWinConditionX = -1.0f;
        return;
    }
    
    file >> mLevelWidth >> mLevelHeight >> mWinConditionY >> mWinConditionX;
    file.close();
    
    SDL_Log("Loaded level %d dimensions: %dx%d, Win: Y=%.0f X=%.0f", levelNumber, mLevelWidth, mLevelHeight, mWinConditionY, mWinConditionX);
}

int Game::LoadProgressData()
{
    std::ifstream file("../Assets/Levels/progressData.txt");
    if (!file.is_open()) {
        std::ofstream outFile("../Assets/Levels/progressData.txt");
        if (outFile.is_open()) {
            outFile << "1";
            outFile.close();
        }
        return 1;
    }
    
    int levelNumber = 1;
    file >> levelNumber;
    file.close();
    
    SDL_Log("Loaded progress data: Level %d", levelNumber);
    return levelNumber;
}

void Game::SaveProgressData(int levelNumber)
{
    std::ofstream file("../Assets/Levels/progressData.txt");
    if (!file.is_open()) {
        SDL_Log("Failed to save progress data!");
        return;
    }
    
    file << levelNumber;
    file.close();
    SDL_Log("Saved progress data: Level %d", levelNumber);
}

bool Game::CheckLevelExists(int levelNumber)
{
    std::string level = "level" + std::to_string(levelNumber);
    std::string levelPath = "../Assets/Levels/" + level + "/" + level + ".csv";
    
    std::ifstream file(levelPath);
    bool exists = file.is_open();
    if (file.is_open()) {
        file.close();
    }
    
    SDL_Log("Checking level %d: %s", levelNumber, exists ? "exists" : "not found");
    return exists;
}

int **Game::LoadLevel(const std::string& fileName, int width, int height)
{
    int** level = new int*[height];
    for (int i = 0; i < height; ++i) {
        level[i] = new int[width];
        for (int j = 0; j < width; ++j) {
            level[i][j] = -1;
        }
    }

    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Failed to open level file: %s", fileName.c_str());
        return nullptr;
    }
    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < height){
        std::vector<int> tiles = CSVHelper::Split(line);
        for (int col = 0; col < width && col < tiles.size(); ++col){
            level[row][col] = tiles[col];
        }
        ++row;
    }

    return level;
}

TiledTileInfo Game::DecodeTiledID(uint32_t raw)
{
    const uint32_t FLIP_H = 0x80000000;
    const uint32_t FLIP_V = 0x40000000;
    const uint32_t FLIP_D = 0x20000000;
    const uint32_t TILE_ID_MASK = 0x1FFFFFFF;

    TiledTileInfo tile;

    tile.flipH = (raw & FLIP_H) != 0;
    tile.flipV = (raw & FLIP_V) != 0;
    tile.flipD = (raw & FLIP_D) != 0;

    // Obtain the real ID
    tile.id = raw & TILE_ID_MASK;

    return tile;
}

void Game::BuildLevel(int** levelData, int width, int height){
    for (int row = 0; row < height; ++row){
        for (int col = 0; col < width; ++col){
            int rawID = levelData[row][col];
            if (rawID == -1) continue;  // Empty tile
            TiledTileInfo info;
            info = DecodeTiledID(static_cast<uint32_t>(rawID));
            Block* block = nullptr;
            Vector2 position(col * TILE_SIZE + TILE_SIZE * 0.5f, row * TILE_SIZE + TILE_SIZE * 0.5f);
            std::string baseAddr = "../Assets/Sprites/Blocks";
            switch (info.id){
                case 222:{//Player
                    mRobot = new Robot(this);
                    mRobot->SetPosition(position);
                    // Restore saved repair level
                    mRobot->SetRepairLevel(mSavedRepairLevel);
                    SDL_Log("Restored Robot repair level: %d", static_cast<int>(mSavedRepairLevel));
                    break;
                }
                case 237: {//Chaser
                    ChaserSpawner* spawnerC = new ChaserSpawner(this, SPAWN_DISTANCE);
                    spawnerC->SetPosition(position);
                    break;
                }
                case 238: {//Walker
                    WalkerSpawner* spawnerW = new WalkerSpawner(this, position);
                    break;
                }
                default:
                    // For regular tiles, use the CSV value directly for the filename
                    std::string blockAddr = "/Free Industrial Zone Tileset/1 Tiles/IndustrialTile_";
                    if (info.id < 9) blockAddr += "0" + std::to_string(info.id + 1) + ".png";
                    else blockAddr += std::to_string(info.id + 1) + ".png";
                    block = new Block(this, baseAddr + blockAddr);
                    block->SetPosition(position);
                    break;
            }
            if (block)
            {
                block->SetFlip(info.flipH, info.flipV, info.flipD);
            }
        }
    }
    // Adiciona 10 GoldRings aleatórios após construir o mapa
    SpawnRandomGoldRings(10, width, height, levelData);
}

void Game::LoadSounds() {

    mDeadChunk       = Mix_LoadWAV("../Assets/Sounds/Dead.wav");
    mJumpChunk       = Mix_LoadWAV("../Assets/Sounds/Jump.wav");
    mShootChunk   = Mix_LoadWAV("../Assets/Sounds/Laser_Shoot.wav");
    mStageClearChunk = Mix_LoadWAV("../Assets/Sounds/StageClear.wav");
    mGlitchChunk     = Mix_LoadWAV("../Assets/Sounds/Glitch.flac");
    mFailedShotChunk = Mix_LoadWAV("../Assets/Sounds/FailedShot.mp3");

    if (!mGlitchChunk) {
        SDL_Log("Falha ao carregar Glitch.flac: %s", Mix_GetError());
    }
}

void Game::PlayDeadChunk() {


}

void Game::PlayJumpChunk() const {

    Mix_PlayChannel(-1, mJumpChunk, 0);
}

void Game::PlayShootChunk() const {

    Mix_PlayChannel(-1, mShootChunk, 0);
}

void Game::PlayStageClearChunk() {


}

void Game::PlayGlitchChunk() const {
    if (mGlitchChunk) {
        Mix_PlayChannel(-1, mGlitchChunk, 0);
    }
}

void Game::PlayFailedShotChunk() const {
    if (mGlitchChunk) {
        Mix_PlayChannel(-1, mFailedShotChunk, 0);
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        // Calculate delta time in seconds
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        // Sleep to maintain frame rate
        int sleepTime = (1000 / FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                if (!event.key.repeat){
                    // Check for pause menu toggle first (only if in-game with only HUD)
                    if (event.key.keysym.sym == SDLK_RETURN && mRobot != nullptr){
                        bool onlyHUD = true;
                        for (auto* ui : mUIStack) {
                            if (dynamic_cast<HUD*>(ui) == nullptr) {
                                onlyHUD = false;
                                break;
                            }
                        }
                        if (onlyHUD) {
                            mIsPaused = true;
                            SetScene(GameScene::PauseMenu);
                            break;
                        }
                    }
                    // Let topmost UI handle key press
                    if (!mUIStack.empty()){
                        mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                    }
                }
                break;
        }
    }

    // Only process actor input when not paused
    if (!mIsPaused)
    {
        const Uint8* state = SDL_GetKeyboardState(nullptr);

        for (auto actor : mActors)
        {
            actor->ProcessInput(state);
        }
    }
}

void Game::UpdateGame(float deltaTime)
{
    if (!mIsPaused && !mIsSceneTransitioning) {
        // Update all actors and pending actors
        UpdateActors(deltaTime);

        // Update audio system
        if (mAudio) mAudio->Update(deltaTime);

        // Check emergency alert based on Robot's health
        if (mRobot && !mRobot->IsDead()) {
            if (mRobot->GetHitPoints() <= 1) {
                // Start emergency alert if not already playing
                if (mEmergencyAlertHandle == SoundHandle::Invalid || 
                    mAudio->GetSoundState(mEmergencyAlertHandle) != SoundState::Playing) {
                    mEmergencyAlertHandle = mAudio->PlaySound("Emergency Alert.wav", true);
                    mAudio->SetSoundVolume(mEmergencyAlertHandle, 6); // 5% volume (6/128)
                }
            } else {
                // Stop emergency alert if health is above 1
                if (mEmergencyAlertHandle != SoundHandle::Invalid && 
                    mAudio->GetSoundState(mEmergencyAlertHandle) == SoundState::Playing) {
                    mAudio->StopSound(mEmergencyAlertHandle);
                    mEmergencyAlertHandle = SoundHandle::Invalid;
                }
            }
        }

        // Check if Robot is dead
        if (mRobot && mRobot->IsDead()) {
            // Stop emergency alert on death
            if (mEmergencyAlertHandle != SoundHandle::Invalid) {
                mAudio->StopSound(mEmergencyAlertHandle);
                mEmergencyAlertHandle = SoundHandle::Invalid;
            }
            SetScene(GameScene::GameOver);
            return;
        }

        // Update camera position
        UpdateCamera();
    }

    // Update UI screens (active ones)
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UI that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            if (std::find(deadActors.begin(), deadActors.end(), actor) == deadActors.end()) deadActors.emplace_back(actor);
        }
    }
    for (auto actor : deadActors) {
        const auto& comps = actor->GetComponents();
        SDL_Log("Deleting actor at %p, type=%s, numComponents=%zu", actor, actor->GetName(), comps.size());
    }
    for (auto actor : deadActors)
    {
        //SDL_Log("Deleting actor at %p, type=%s", actor, actor->GetName());
        RemoveActor(actor);
        delete actor;
    }
}

void Game::UpdateCamera()
{
    if (!mRobot) {
        return;
    }

    float robotX = mRobot->GetPosition().x;
    float robotY = mRobot->GetPosition().y;

    float targetX = robotX - WINDOW_WIDTH / 2.0f + TILE_SIZE / 2.0f;
    float targetY = robotY - WINDOW_HEIGHT / 2.0f + TILE_SIZE / 2.0f;

    float levelWidthInPixels = mLevelWidth * TILE_SIZE;
    float levelHeightInPixels = mLevelHeight * TILE_SIZE;

    if (targetX < 0) targetX = 0;
    if (targetX > levelWidthInPixels - WINDOW_WIDTH) targetX = levelWidthInPixels - WINDOW_WIDTH;

    if (targetY < 0) targetY = 0;
    if (targetY > levelHeightInPixels - WINDOW_HEIGHT) targetY = levelHeightInPixels - WINDOW_HEIGHT;

    mCameraPos.x = targetX;
    mCameraPos.y = targetY;
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    //background drawing (only when in gameplay)
    if (mRobot) {
        Texture* bgTexture = mRenderer->GetTexture("../Assets/Sprites/Background.png");
        if (bgTexture){
            Vector2 size(mLevelWidth * TILE_SIZE, mLevelHeight * TILE_SIZE);

            Vector2 topLeft = Vector2(mLevelWidth * TILE_SIZE / 2.0f, mLevelHeight * TILE_SIZE / 2.0f);
            mRenderer->DrawTexture(topLeft, size, 0.0f, Vector3::One, bgTexture, Vector4::UnitRect, mCameraPos, false);
        }
    }

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);

        if(mIsDebugging)
        {
           // Call draw for actor components
              for (auto comp : drawable->GetOwner()->GetComponents())
              {
                comp->DebugDraw(mRenderer);
              }
        }
    }

    // Draw UI elements
    mRenderer->Draw();

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    // Delete actors (destructors should call RemoveActor safely)
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete level data if still present
    if (mLevelData) {
        for (int i = 0; i < mLevelHeight; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    // Renderer cleanup
    if (mRenderer) {
        mRenderer->Shutdown();
        delete mRenderer;
        mRenderer = nullptr;
    }

    // Audio cleanup
    if (mAudio) {
        delete mAudio;
        mAudio = nullptr;
    }

    if (mWindow) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    Mix_FreeChunk(mDeadChunk);
    Mix_FreeChunk(mJumpChunk);
    Mix_FreeChunk(mShootChunk);
    Mix_FreeChunk(mStageClearChunk);
    Mix_FreeChunk(mGlitchChunk);
    Mix_FreeChunk(mFailedShotChunk);

    Mix_CloseAudio();

    SDL_Quit();
}

void Game::SpawnRandomGoldRings(int quantidade, int width, int height, int** levelData) {
    int tentativas = 0;
    int maxTentativas = quantidade * 10;
    int ringsCriados = 0;
    while (ringsCriados < quantidade && tentativas < maxTentativas) {
        int col = rand() % width;
        int row = rand() % height;
        if (levelData[row][col] == -1) { // Tile vazio
            Vector2 pos(col * TILE_SIZE + TILE_SIZE * 0.5f, row * TILE_SIZE + TILE_SIZE * 0.5f);
            GoldRing* ring = new GoldRing(this);
            ring->SetPosition(pos);
            ringsCriados++;
        }
        tentativas++;
    }
}
