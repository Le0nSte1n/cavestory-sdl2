#include "first_cave_bat.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "player.h"
#include "timer.h"

const units::FPS kFps{60};
const auto kMaxFrameTime = std::chrono::milliseconds{5 * 1000 / 60};
units::Tile Game::kScreenWidth{20};
units::Tile Game::kScreenHeight{15};

Game::Game() :
    sdlEngine_(),
    graphics_(),
    player_{new Player(graphics_,
            Vector<units::Game>{
            units::tileToGame(Game::kScreenWidth/2),
            units::tileToGame(Game::kScreenHeight/2)}
            )
    },
    bat_{new FirstCaveBat(graphics_,
            Vector<units::Game>{
            units::tileToGame(7),
            units::tileToGame(Game::kScreenHeight/2 + 1)}
            )
    },
    map_{Map::createTestMap(graphics_)}
{
    runEventLoop();
}

Game::~Game()
{
}

void Game::runEventLoop() {
    Input input;
    SDL_Event event;

    bool running{true};
    auto last_updated_time = std::chrono::high_resolution_clock::now();
    while (running) {
        using std::chrono::high_resolution_clock;
        using std::chrono::milliseconds;
        using std::chrono::duration_cast;

        const auto start_time = high_resolution_clock::now();

        input.beginNewFrame();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                input.keyDownEvent(event);
                break;
            case SDL_KEYUP:
                input.keyUpEvent(event);
                break;
            default:
                break;
            }
        }
        if (input.wasKeyPressed(SDL_SCANCODE_Q)) {
            running = false;
        }

        // Player Horizontal Movement
        if (input.isKeyHeld(SDL_SCANCODE_LEFT)
                && input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
            // if both left and right are being pressed we need to stop moving
            player_->stopMoving();
        } else if (input.isKeyHeld(SDL_SCANCODE_LEFT)) {
            player_->startMovingLeft();
        } else if (input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
            player_->startMovingRight();
        } else {
            player_->stopMoving();
        }

        if (input.isKeyHeld(SDL_SCANCODE_UP) &&
                input.isKeyHeld(SDL_SCANCODE_DOWN)) {
            player_->lookHorizontal();
        } else if (input.isKeyHeld(SDL_SCANCODE_UP)) {
            player_->lookUp();
        } else if (input.isKeyHeld(SDL_SCANCODE_DOWN)) {
            player_->lookDown();
        } else {
            player_->lookHorizontal();
        }

        // Player Jump
        if (input.wasKeyPressed(SDL_SCANCODE_Z)) {
            player_->startJump();
        } else if (input.wasKeyReleased(SDL_SCANCODE_Z)) {
            player_->stopJump();
        }

        // update scene and last_updated_time
        const auto current_time = high_resolution_clock::now();
        const auto upd_elapsed_time = current_time - last_updated_time;

        update(std::min(
                    duration_cast<milliseconds>(upd_elapsed_time),
                    kMaxFrameTime)
                );
        last_updated_time = current_time;

        draw(graphics_);

        // calculate delay for constant fps
        const auto end_time = high_resolution_clock::now();
        const auto elapsed_time = duration_cast<milliseconds>(
                end_time - start_time);

        const auto delay_duration = milliseconds(1000) / kFps - elapsed_time;
        if (delay_duration.count() >= 0)
            SDL_Delay(delay_duration.count());
    }
}

void Game::update(const std::chrono::milliseconds elapsed_time)
{
    Timer::updateAll(elapsed_time);
    //TODO: update map when it is changed
    player_->update(elapsed_time, *map_);
    bat_->update(elapsed_time, player_->getCenterX());

    const auto batRect = bat_->getDamageRectangle();
    if (batRect.collidesWith(player_->getDamageRectangle())) {
        player_->takeDamage();
    }
}

void Game::draw(Graphics& graphics) const
{
    graphics.clear();

    map_->drawBackground(graphics);
    bat_->draw(graphics);
    player_->draw(graphics);
    map_->draw(graphics);
    player_->drawHUD(graphics);

    graphics.flip();
}
