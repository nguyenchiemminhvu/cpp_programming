#include <iostream>

class player;
class enemy;

class game_object
{
public:
    virtual ~game_object() = default;
    virtual void collide_with(game_object& other) = 0;
    virtual void collide_with_player(player& player) = 0;
    virtual void collide_with_enemy(enemy& enemy) = 0;
};

class player : public game_object
{
public:
    void collide_with(game_object& other) override
    {
        other.collide_with_player(*this);
    }

    void collide_with_player(player& other) override
    {
        std::cout << "Player collides with Player" << std::endl;
    }

    void collide_with_enemy(enemy& other) override
    {
        std::cout << "Player collides with Enemy" << std::endl;
    }
};

class enemy : public game_object
{
public:
    void collide_with(game_object& other) override
    {
        other.collide_with_enemy(*this);
    }

    void collide_with_player(player& other) override
    {
        std::cout << "Enemy collides with Player" << std::endl;
    }

    void collide_with_enemy(enemy& other) override
    {
        std::cout << "Enemy collides with Enemy" << std::endl;
    }
};

int main()
{
    player p;
    enemy e;

    p.collide_with(e); // Player collides with Enemy
    e.collide_with(p); // Enemy collides with Player

    return 0;
}