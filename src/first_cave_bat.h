#ifndef FIRST_CAVE_BAT_H_
#define FIRST_CAVE_BAT_H_

#include <map>
#include <memory>
#include <chrono>
#include "vector.h"
#include "units.h"
#include "rectangle.h"

struct Sprite;
struct Graphics;

struct FirstCaveBat {
   FirstCaveBat(Graphics& graphics, Vector<units::Game> pos);
   ~FirstCaveBat();

   void draw(Graphics& graphics) const;
   void update(const std::chrono::milliseconds elapsed_time,
           const units::Game player_x);

   const Rectangle getDamageRectangle() const;

private:
   enum class Facing {
       FIRST_FACING,
       LEFT = FIRST_FACING,
       RIGHT,
       LAST_FACING
   };
   struct SpriteState {
       SpriteState(Facing facing) :
           facing(facing)
       {}
       Facing facing;
   };
   friend bool operator<(const SpriteState& a, const SpriteState& b) {
       return a.facing < b.facing;
   }

   void initializeSprites(Graphics& graphics);
   void initializeSprite(Graphics& graphics, const SpriteState& sprite_state);
   const SpriteState getSpriteState() const;

   Vector<units::Game> pos_;
   const units::Game center_y_;
   Facing facing_;
   units::Degrees flight_angle_;
   std::map<SpriteState, std::shared_ptr<Sprite> > sprites_;
};

#endif /* FIRST_CAVE_BAT_H_ */