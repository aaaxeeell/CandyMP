#include "block.h"
#include <cstdlib>

Block::Block(int startX, int startY)
    : m_x(startX), m_y(startY)
{
    // Emplemenem blocs de 3 amb tipus aleatoris.
    for (int i = 0; i < 3; ++i)
    {
        int randomType = std::rand() % static_cast<int>(CandyType::COUNT);
        m_candies[i] = new Candy(static_cast<CandyType>(randomType));
    }
}

Block::~Block()
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_candies[i] != nullptr)
        {
            delete m_candies[i];
            m_candies[i] = nullptr;
        }
    }
}

bool Block::moveLeft(const Board& board)
{
    bool potMoure = true;
    int targetX = m_x - 1;
    
    if (targetX < 0)
    {
        potMoure = false;
    }
    else
    {
        // El bucle se detiene si canMove pasa a ser false
        for (int i = 0; i < 3 && potMoure; ++i)
        {
            int targetY = m_y + i;
            if (targetY >= 0 && targetY < board.getHeight())
            {
                if (board.getCell(targetX, targetY) != nullptr)
                {
                    potMoure = false;
                }
            }
        }
    }

    if (potMoure)
    {
        m_x = targetX;
    }
    
    return potMoure;
}

bool Block::moveRight(const Board& board)
{
    bool potMoure = true;
    int targetX = m_x + 1;
    
    if (targetX >= board.getWidth())
    {
        potMoure = false;
    }
    else
    {
        for (int i = 0; i < 3 && potMoure; ++i)
        {
            int targetY = m_y + i;
            if (targetY >= 0 && targetY < board.getHeight())
            {
                if (board.getCell(targetX, targetY) != nullptr)
                {
                    potMoure = false;
                }
            }
        }
    }

    if (potMoure)
    {
        m_x = targetX;
    }
    
    return potMoure;
}

bool Block::canFall(const Board& board) const
{
    bool potCaure = true;
    int lowestY = m_y + 2;
    int targetY = lowestY + 1;

    if (targetY >= board.getHeight())
    {
        potCaure = false;
    }
    else if (targetY >= 0)
    {
        if (board.getCell(m_x, targetY) != nullptr)
        {
            potCaure = false;
        }
    }

    return potCaure;
}

void Block::fall()
{
    m_y++;
}

void Block::rotate()
{
    // intercanvi ciclic.
    Candy* temp = m_candies[2];
    m_candies[2] = m_candies[1];
    m_candies[1] = m_candies[0];
    m_candies[0] = temp;
}

void Block::draw(GraphicManager& graphics) const
{
    // Constante de desplazamiento del tablero extraída de game.cpp
    const int BOARD_PADDING = 3;

    for (int i = 0; i < 3; ++i)
    {
        if (m_candies[i] != nullptr)
        {
            int currentY = m_y + i;
            // Evitamos dibujar componentes si se encuentran fuera del área superior visible
            if (currentY >= 0)
            {
                int screenX = (BOARD_PADDING + m_x) * CANDY_IMAGE_WIDTH;
                int screenY = (BOARD_PADDING + currentY) * CANDY_IMAGE_HEIGHT;
                
                graphics.drawImage(m_candies[i]->getResourceName(), screenX, screenY);
            }
        }
    }
}

bool Block::operator==(const Block& other) const
{
    // Empezamos asumiendo que son iguales si sus posiciones coinciden
    bool igual = (m_x == other.m_x && m_y == other.m_y);

    // Iteramos solo si siguen siendo iguales
    for (int i = 0; i < 3 && igual; ++i)
    {
        if (m_candies[i] == nullptr || other.m_candies[i] == nullptr)
        {
            if (m_candies[i] != other.m_candies[i])
            {
                igual = false;
            }
        }
        else if (m_candies[i]->getType() != other.m_candies[i]->getType())
        {
            igual = false;
        }
    }

    return igual;
}

Candy* Block::extractCandy(int index)
{
    Candy* extracted = nullptr;
    
    if (index >= 0 && index < 3)
    {
        extracted = m_candies[index];
        m_candies[index] = nullptr; 
    }
    
    return extracted;
}