#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>

Board::Board(int width, int height)
    : m_width(width), m_height(height),
    m_tauler(width, std::vector<Candy*>(height, nullptr))
{
    // Inicializamos el almacén de punteros a null
    for (int i = 0; i < DEFAULT_BOARD_WIDTH; i++)
        for (int j = 0; j < DEFAULT_BOARD_HEIGHT; j++)
            m_storage[i][j] = nullptr;
}


Board::~Board()
{
    for (int i = 0; i < DEFAULT_BOARD_WIDTH; i++)
    {
        for (int j = 0; j < DEFAULT_BOARD_HEIGHT; j++)
        {
            if (m_storage[i][j] != nullptr)
            {
                delete m_storage[i][j]; // Borramos la memoria reservada
            }
        }
    }                
}


Candy* Board::getCell(int x, int y) const
{
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        return m_tauler[x][y];
    }
    return nullptr;
}

void Board::setCell(Candy* candy, int x, int y)
{
    m_tauler[x][y] = candy;
}

int Board::comptarEnDireccio(int x, int y, int dx, int dy, CandyType tipusCaramel) const
{
    int comptador = 0;
    // començem a la casella DEL COSTAT.
    int actX = x + dx;
    int actY = y + dy;

    // condicions: mentre la casella sigui vàlida:
    while (getCell(actX, actY) != nullptr)
    {
        // condicions: si el caramel és del mateix tipus
        if (getCell(actX, actY)->getType() == tipusCaramel)
        {
            comptador++;
            actX += dx; // avançem en x
            actY += dy; // avançem en y
        }
        else
        {
            // si es d'un altre color, parem el bucle.
            break;
        }
    }

    return comptador;
}

bool Board::shouldExplode(int x, int y) const
{
    // x i y son les coordenades de la peça.
    // dx i dy els vectors de direcció.
    Candy* caramelCentral = getCell(x, y);
    
    bool boom = false;

    // seguretat
    if (caramelCentral == nullptr) 
    {
        boom = false;
    }
    else
    {
        CandyType tipusCaramelCentral = caramelCentral->getType();

        // HORITZONTAL: dx = 1  i dx = -1, dy = 0
        if ((comptarEnDireccio(x, y, 1, 0, tipusCaramelCentral) +
            comptarEnDireccio(x, y, -1, 0, tipusCaramelCentral) + 1) >= SHORTEST_EXPLOSION_LINE)
        {
            boom = true;
        }

        // VERTICAL: dy = 1 i dy = -1 , dx = 0
        if (!boom && (comptarEnDireccio(x, y, 0, 1, tipusCaramelCentral) +
                    comptarEnDireccio(x, y, 0, -1, tipusCaramelCentral) + 1) >= SHORTEST_EXPLOSION_LINE)
        {
            boom = true;
        }

        // DIAGONAL PRINCIPAL: dx = 1, dy = 1 i dx = -1, dy = -1
        if (!boom && (comptarEnDireccio(x, y, 1, 1, tipusCaramelCentral) +
                    comptarEnDireccio(x, y, -1, -1, tipusCaramelCentral) + 1) >= SHORTEST_EXPLOSION_LINE)
        {
            boom = true;
        }

        // DIAGONAL SECUNDÀRIA: dx = -1, dy = 1 i dx = 1, dy = -1
        if (!boom && (comptarEnDireccio(x, y, -1, 1, tipusCaramelCentral) +
                    comptarEnDireccio(x, y, 1, -1, tipusCaramelCentral) + 1) >= SHORTEST_EXPLOSION_LINE)
        {
            boom = true;
        }
    }
    return boom;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    std::vector<Candy*> posicionsAExplotar;
    bool explosions;

    do 
    {
        explosions = false;

        // A: MARCAR
        // preguntem a shouldExplode si totes les posicions del tauler han d'explotar:
        bool marcatXExplotar[DEFAULT_BOARD_WIDTH][DEFAULT_BOARD_HEIGHT] = {false};

        for (int x = 0; x < m_width; x++)
        {
            for (int y = 0; y < m_height; y++)
            {
                if (getCell(x, y) != nullptr && shouldExplode(x, y))
                {
                    marcatXExplotar[x][y] = true;
                    explosions = true;
                }
            }
        }

        // B: GUARDAR I BORRAR
        // mirem el vector a explotar, afegim al vector que retornarem per paràmetre i 
        // eliminem la seva posició del tauler.
        for (int i = 0; i < m_width; i++)
        {
            for (int j = 0; j < m_height; j++)
            {
                if (marcatXExplotar[i][j] == true)
                {
                    // afegim a posicionsAExplotar la posició del tauler que conté 
                    // el caramel a explotar.
                    if (m_tauler[i][j] != nullptr)
                    {
                        posicionsAExplotar.push_back(m_tauler[i][j]);
                    }
                    // eliminem la posició.
                    m_tauler[i][j] = nullptr;
                }
            }
        }

        // C: DROP (COMPACTAR) compactar cap a abaix
        if (explosions)
        {
            for (int x = 0; x < m_width; x++)
            {
                int writeY = m_height - 1; // posició més baixa on podem escriure.
                for (int y = m_height - 1; y >= 0; y--)
                {
                    if (m_tauler[x][y] != nullptr)
                    {
                        m_tauler[x][writeY] = m_tauler[x][y];

                        if (writeY != y)
                        {
                            m_tauler[x][y] = nullptr; // buidem on estava abans.
                        }
                        writeY--;
                    }
                }
            }
        }

    } while(explosions);

    return posicionsAExplotar;
}


bool Board::dump(const std::string& output_path) const
{
    bool fitxerVolcat = false;  
    
    // OBERTURA
    std::ofstream fitxer;
    fitxer.open(output_path);

    if (fitxer.is_open())
    {
        for (int i = 0; i < m_height; i++)
        {
            for (int j = 0; j < m_width; j++)
            {
                Candy* caramelActual = getCell(j, i);

                if (caramelActual != nullptr)
                {
                    fitxer << (int)caramelActual->getType() << " ";
                }
                else
                {
                    fitxer << "6 ";
                }
            }
            fitxer << "\n";
        }
        fitxer.close();
        fitxerVolcat = true;
    }

    return fitxerVolcat;
}

bool Board::load(const std::string& input_path)
{
    bool fitxerCarregat = false;
    int tipusCandy;
    // OBERTURA
    std::ifstream fitxer;
    fitxer.open(input_path);

    if (fitxer.is_open())
    {
        // NETEJA DEL TAULER
        for (int i = 0; i < m_height; i++)
        {
            for (int j = 0; j < m_width; j++)
            {
                m_tauler[i][j] = nullptr;
            }
        }

        for (int i = 0; i < m_height; i++)
        {
            for (int j = 0; j < m_width; j++)
            {
                // llegim del fitxer.
                fitxer >> tipusCandy;
                if (tipusCandy >= 0 && tipusCandy < 6)
                {
                    // Creamos el objeto REAL en memoria. No es local, así que no desaparece.
                    m_storage[j][i] = new Candy(static_cast<CandyType>(tipusCandy));
                    // El tablero apunta a esa dirección de memoria
                    m_tauler[j][i] = m_storage[j][i];
                }
                else
                {
                    m_tauler[j][i] = nullptr;
                }
                
            }
        }
        fitxer.close();
    }
    return true;
}
