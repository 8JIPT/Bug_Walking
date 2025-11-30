#include "Texture.h"

Texture::Texture()
: mTextureID(0)
, mWidth(0)
, mHeight(0)
{
}

Texture::~Texture()
{
}

GLuint Texture::SDLFormatToGL(SDL_PixelFormat* format){
    if (!format) return GL_RGBA;
    switch (format->BitsPerPixel){
        case 32: return GL_RGBA;
        case 24: return GL_RGB;
        default: return GL_RGBA;
    }
}

bool Texture::Load(const std::string &filePath)
{
    if (mTextureID != 0){
        Unload();
        mTextureID = 0;
    }

    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface){
        SDL_Log("Load texture error: %s - %s", filePath.c_str(), IMG_GetError());
        return false;
    }

    mWidth = surface->w;
    mHeight = surface->h;

    // Convert surface to RGBA32 format for consistent handling
    SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    
    if (!convertedSurface) {
        SDL_Log("Failed to convert surface format: %s", SDL_GetError());
        return false;
    }

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Upload texture data in RGBA format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, convertedSurface->pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(convertedSurface);

    return true;
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
    mWidth = surface->w;
    mHeight = surface->h;

    // Convert surface to BGRA32 format for consistent handling
    SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
    if (!convertedSurface)
    {
        SDL_Log("Failed to convert surface format for text: %s", SDL_GetError());
        return;
    }

    // Generate a GL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    // Use linear filtering for text rendering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload the surface pixels to the texture (RGBA format)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, convertedSurface->pixels);

    SDL_FreeSurface(convertedSurface);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive(int index) const
{
    if (mTextureID == 0){
        return;
    }
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

}
