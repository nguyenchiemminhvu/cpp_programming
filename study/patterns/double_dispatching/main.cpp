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

// ------------------------------------

class pdf_document;
class word_document;

class printer
{
public:
    virtual ~printer() = default;
    virtual void print_pdf(pdf_document& doc) = 0;
    virtual void print_word(word_document& doc) = 0;
};

class inkjet_printer : public printer
{
public:
    void print_pdf(pdf_document& doc) override
    {
        std::cout << "Printing PDF document on inkjet printer" << std::endl;
    }

    void print_word(word_document& doc) override
    {
        std::cout << "Printing Word document on inkjet printer" << std::endl;
    }
};

class laser_printer : public printer
{
public:
    void print_pdf(pdf_document& doc) override
    {
        std::cout << "Printing PDF document on laser printer" << std::endl;
    }

    void print_word(word_document& doc) override
    {
        std::cout << "Printing Word document on laser printer" << std::endl;
    }
};

class document
{
public:
    virtual ~document() = default;
    virtual void print_on(printer& p) = 0;
};

class pdf_document : public document
{
public:
    void print_on(printer& p) override
    {
        p.print_pdf(*this);
    }
};

class word_document : public document
{
public:
    void print_on(printer& p) override
    {
        p.print_word(*this);
    }
};

int main()
{
    player p;
    enemy e;

    p.collide_with(e); // Player collides with Enemy
    e.collide_with(p); // Enemy collides with Player

    pdf_document pdf;
    word_document word;
    inkjet_printer inkjet;
    laser_printer laser;

    pdf.print_on(inkjet);
    pdf.print_on(laser);
    word.print_on(inkjet);
    word.print_on(laser);

    return 0;
}