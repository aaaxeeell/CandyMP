#include "game.h"
#include <random>
#include "graphics.h"
#include "candy.h"
#include <fstream>

Game::Game() : m_board(), m_fallingBlock(nullptr), m_frameCount(0), m_gameOver(false), m_score(0)
{
    int iniX = m_board.getWidth() / 2;
    m_fallingBlock = new Block(iniX, -3);
}

Game::~Game()
{
    if (m_fallingBlock != nullptr)
    {
        delete m_fallingBlock;
        m_fallingBlock = nullptr;
    }
}

void Game::update(const Controller& controller)
{
    if (!m_gameOver && m_fallingBlock != nullptr)
    {
        if (controller.isLeftPressed())
        {
            m_fallingBlock->moveLeft(m_board);
        }
        else if (controller.isRightPressed())
        {
            m_fallingBlock->moveRight(m_board);
        }

        if (controller.isKey1Pressed())
        {
            m_fallingBlock->rotate();
        }

        if (controller.isKey2Pressed())
        {
            dump("data/save.txt");
        }

        m_frameCount++;
        bool baixar = controller.isDownPressed();

        if (m_frameCount >= 60 || baixar)
        {
            m_frameCount = 0;

            if (m_fallingBlock->canFall(m_board))
            {
                m_fallingBlock->fall();
            }
            else
            {
                for (int i = 0; i < 3; i++)
                {
                    Candy* caramel = m_fallingBlock->extractCandy(i);
                    if (caramel != nullptr)
                    {
                        int posY = m_fallingBlock->getY() + i;
                        if (posY >= 0)
                        {
                            m_board.setCell(caramel, m_fallingBlock->getX(), posY);
                        }
                        else
                        {
                            delete caramel;
                        }
                    }
                }

                delete m_fallingBlock;
                m_fallingBlock = nullptr;

                std::vector<Candy*> explotats = m_board.explodeAndDrop();
                m_score += (int)explotats.size() * 10;
                for (size_t i = 0; i < explotats.size(); i++)
                {
                    delete explotats[i];
                }

                int iniX = m_board.getWidth() / 2;
                m_fallingBlock = new Block(iniX, -3);

                if (!m_fallingBlock->canFall(m_board) && m_board.getCell(iniX, 0) != nullptr)
                {
                    m_gameOver = true;
                }
            }
        }
    }
}

void Game::render(GraphicManager& graphics)
{
    const int board_padding = 3;

    // Marc del tauler
    graphics.drawRectangle(
        CANDY_IMAGE_WIDTH * board_padding, CANDY_IMAGE_HEIGHT * board_padding,
        CANDY_IMAGE_WIDTH * m_board.getWidth(),
        CANDY_IMAGE_HEIGHT * m_board.getHeight(),
        5, 150, 150, 150);

    // Caramels del tauler
    for (int y = 0; y < m_board.getHeight(); y++)
    {
        for (int x = 0; x < m_board.getWidth(); x++)
        {
            Candy* c = m_board.getCell(x, y);
            if (c != nullptr)
            {
                graphics.drawImage(c->getResourceName(),
                    CANDY_IMAGE_WIDTH * (board_padding + x),
                    CANDY_IMAGE_HEIGHT * (board_padding + y));
            }
        }
    }

    // Bloc que cau
    if (!m_gameOver && m_fallingBlock != nullptr)
    {
        m_fallingBlock->draw(graphics);
    }

    // Logo
    graphics.drawImage("img/logo_small.png", 10, 10);

    // Peu
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
                      "Buttons: [Q] [W] [E]  --  Exit [ESC]",
                      25, 700, 20, 100, 100, 100);

    // Puntuació gran a la dreta (estil enunciat)
    graphics.drawText(std::to_string(m_score), 600, 30, 70, 125, 200, 125);

    // Panell Game Over amb puntuació final
    if (m_gameOver)
    {
        graphics.drawRectangle(100, 280, 550, 200, 6, 80, 80, 80);
        graphics.drawText("Game Over", 175, 300, 80, 200, 50, 50);
        graphics.drawText("Final score: " + std::to_string(m_score),
                          220, 410, 40, 80, 80, 80);
    }
}

void Game::run()
{
    const int screen_width = 750;
    const int screen_height = 750;
    const int bg_red = 255;
    const int bg_green = 255;
    const int bg_blue = 255;
    runGraphicGame(*this, screen_width, screen_height, bg_red, bg_green, bg_blue);
}

bool Game::dump(const std::string& output_path) const
{
    bool guardat = false;
    std::ofstream fitxer;
    fitxer.open(output_path);

    if (fitxer.is_open())
    {
        for (int y = 0; y < m_board.getHeight(); y++)
        {
            for (int x = 0; x < m_board.getWidth(); x++)
            {
                Candy* c = m_board.getCell(x, y);
                if (c != nullptr)
                {
                    fitxer << (int)c->getType() << " ";
                }
                else
                {
                    fitxer << "_ ";
                }
            }
            fitxer << "\n";
        }

        if (m_fallingBlock != nullptr)
        {
            fitxer << m_fallingBlock->getX() << " " << m_fallingBlock->getY() << "\n";
            for (int i = 0; i < 3; i++)
            {
                Candy* c = m_fallingBlock->getCandy(i);
                if (c != nullptr)
                {
                    fitxer << (int)c->getType() << " ";
                }
                else
                {
                    fitxer << "_ ";
                }
            }
            fitxer << "\n";
        }
        fitxer.close();
        guardat = true;
    }
    return guardat;
}

bool Game::load(const std::string& input_path)
{
    bool carregat = false;
    std::ifstream fitxer;
    fitxer.open(input_path);

    if (fitxer.is_open())
    {
        for (int x = 0; x < m_board.getWidth(); x++)
        {
            for (int y = 0; y < m_board.getHeight(); y++)
            {
                Candy* c = m_board.getCell(x, y);
                if (c != nullptr)
                {
                    delete c;
                }
                m_board.setCell(nullptr, x, y);
            }
        }

        std::string valor;
        for (int y = 0; y < m_board.getHeight(); y++)
        {
            for (int x = 0; x < m_board.getWidth(); x++)
            {
                if (fitxer >> valor)
                {
                    if (valor != "_")
                    {
                        int tipus = std::stoi(valor);
                        Candy* nou = new Candy(static_cast<CandyType>(tipus));
                        m_board.setCell(nou, x, y);
                    }
                }
            }
        }

        if (m_fallingBlock != nullptr)
        {
            delete m_fallingBlock;
            m_fallingBlock = nullptr;
        }

        int bX, bY;
        if (fitxer >> bX >> bY)
        {
            m_fallingBlock = new Block(bX, bY);
            for (int i = 0; i < 3; i++)
            {
                Candy* antic = m_fallingBlock->extractCandy(i);
                if (antic != nullptr)
                {
                    delete antic;
                }

                if (fitxer >> valor)
                {
                    if (valor != "_")
                    {
                        int tipus = std::stoi(valor);
                        Candy* nou = new Candy(static_cast<CandyType>(tipus));
                        m_fallingBlock->setCandy(nou, i);
                    }
                }
            }
        }

        fitxer.close();
        carregat = true;
    }
    return carregat;
}

bool Game::operator==(const Game& other) const
{
    bool iguals = true;

    if (m_board.getWidth() != other.m_board.getWidth() || m_board.getHeight() != other.m_board.getHeight())
    {
        iguals = false;
    }

    for (int x = 0; x < m_board.getWidth() && iguals; x++)
    {
        for (int y = 0; y < m_board.getHeight() && iguals; y++)
        {
            Candy* c1 = m_board.getCell(x, y);
            Candy* c2 = other.m_board.getCell(x, y);

            if (c1 == nullptr || c2 == nullptr)
            {
                if (c1 != c2)
                {
                    iguals = false;
                }
            }
            else if (c1->getType() != c2->getType())
            {
                iguals = false;
            }
        }
    }

    if (iguals)
    {
        if (m_fallingBlock == nullptr || other.m_fallingBlock == nullptr)
        {
            if (m_fallingBlock != other.m_fallingBlock)
            {
                iguals = false;
            }
        }
        else if (!(*m_fallingBlock == *other.m_fallingBlock))
        {
            iguals = false;
        }
    }

    return iguals;
}