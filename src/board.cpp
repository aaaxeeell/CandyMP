#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>

Board::Board(int width, int height)
    : m_width(width), m_height(height)
{
    m_tauler = new Candy*[m_width * m_height];

    // Inicialitzem tota la matriu a nullptr.
    for (int i = 0; i < m_width * m_height; i++)
    {
        m_tauler[i] = nullptr;
    }
}


Board::~Board()
{
    // Canviem simplement a eliminar el tauler.
    delete[] m_tauler;            
}


Candy* Board::getCell(int x, int y) const
{
    // Idea extreta de internet. Si nosaltres tenim una matriu posada com a concatenació de files
    // podem utilitzar una formula on y * m_width ens diu a la fila on estem. Al multiplicarla per 
    // m_width estem dient quantes cel.les ens hem de saltar per arribar a la fila. Un cop estem a 
    // la fila correcte, ens movem mitjançant + x.

    Candy* c = nullptr;
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        c = m_tauler[y * m_width + x];
    }
    return c;

}

void Board::setCell(Candy* candy, int x, int y)
{
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        m_tauler[y * m_width + x] = candy;
    }
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
                    Candy* CaramelAExplotar = getCell(i,j);
                    if (CaramelAExplotar != nullptr)
                    {
                        posicionsAExplotar.push_back(CaramelAExplotar);
                    }
                    // eliminem la posició.
                    setCell(nullptr, i, j);
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
                    Candy* CaramelActual = getCell(x,y);
                    if (CaramelActual != nullptr)
                    {
                        setCell(CaramelActual, x, writeY);
                        if (writeY != y)
                        {
                            setCell(nullptr, x, y);
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
                    fitxer << "6 "; // posició buida (si no es cap tipus).
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
                Candy* caramelVell = getCell(j,i);
                if (caramelVell != nullptr)
                    delete caramelVell;
                
                setCell(nullptr, j, i);
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
                    Candy* nouCaramel = new Candy(static_cast<CandyType>(tipusCandy));
                    setCell(nouCaramel, j, i);
                }
                else
                {
                    setCell(nullptr, j, i);
                }
                
            }
        }
        fitxer.close();
        fitxerCarregat = true;
    }
    return fitxerCarregat;
}
