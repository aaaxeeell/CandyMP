#ifndef BLOCK_H
#define BLOCK_H

#include "candy.h"
#include "graphics.h"
#include "board.h"

class Block
{
public:
    Block(int startX, int startY);
    ~Block();

    bool moveLeft(const Board& board);
    bool moveRight(const Board& board);

    bool canFall(const Board& board) const;
    void fall();
    void rotate();
    void draw(GraphicManager& graphics) const;
    bool operator==(const Block& other) const;
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    Candy* getCandy(int index) const { return m_candies[index]; }
    Candy* extractCandy(int index);

    void setCandy(Candy* candy, int index) 
    { 
        if(index >= 0 && index < 3) 
        { 
            m_candies[index] = candy; 
        } 
    }

private:
    int m_x;
    int m_y;
    Candy* m_candies[3]; // B   loc que conté exactament 3 caramels.


};

#endif