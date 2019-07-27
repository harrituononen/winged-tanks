#ifndef LOGIC_LEVEL_DATA_HPP
#define LOGIC_LEVEL_DATA_HPP

static int const    TEAM_INFO_FONT_SIZE = 32;
static int const    BUTTON_FONT_SIZE    = 48;

static int const    NUM_LEVELS          = 5;

static float const  RED_TEAM_OFFSET     = 360.0f;

static std::string const LEVEL_NAMES[] =
{
    "Stalactica",
    "Urban Apex",
    "Primeval Woods",
    "Desolate Dunes",
    "Snow-clad Adobe"
};

static int const NUM_DEFAULT_PLAYER_NAMES = 10;

static std::string const DEFAULT_PLAYER_NAMES[] =
{
    "CoolPlayer",
    "SickShots",
    "IWinGames",
    "PleaseNo",
    "FlyingTank",
    "Supreme",
    "RealOwner",
    "Sry4Ownage",
    "UpHigh",
    "KovaKamu"
};

static graphics::Textures const LEVEL_IDS[] =
{
    graphics::Textures::CAVE,
    graphics::Textures::CITY,
    graphics::Textures::DESERT,
    graphics::Textures::JUNGLE,
    graphics::Textures::SNOW
};

static std::string const LEVEL_PATHS[] =
{
    "res/textures/levels/cave.tga",
    "res/textures/levels/city.tga",
    "res/textures/levels/jungle.tga",
    "res/textures/levels/desert.tga",
    "res/textures/levels/snow.tga"
};

static math::Vector2i const THUMBNAIL_SOURCE_DIMENSIONS({ 400, 400 });
static math::Vector2i const THUMBNAIL_DIMENSIONS({ 400, 400 });
    
#endif // LOGIC_LEVEL_DATA_HPP