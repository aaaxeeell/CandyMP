#include "board.h"
#include "candy.h"
#include "game.h"
#include "util.h"
#include <iostream>
#include <vector>

bool test()
{
    { //Test 1, límits
        Board b(5, 5);
        Candy c(CandyType::TYPE_RED);
        b.setCell(&c, 0, 0);
        
        if (b.getCell(0, 0) != &c) 
        {
            std::cout << "ERROR: setCell o getCell bàsic." << std::endl;
            return false;
        }
        if (b.getCell(10, 10) != nullptr) 
        {
            std::cout << "ERROR: No retorna nullptr fora de rango" << std::endl;
            return false;
        }

        if (b.getCell(-1, 0) != nullptr || b.getCell(5, 0) != nullptr) 
        {
            std::cout << "ERROR: getCell no gestiona correctament els límits del tauler." << std::endl;
            return false;
        }
    }

    {
        Candy c(CandyType::TYPE_ORANGE);
        Board b(10, 10);
        b.setCell(&c, 0, 0);
        if (b.getCell(0, 0) != &c)
        {
            std::cout << "ERROR: Registre bàsic setCell/getCell amb TYPE_ORANGE." << std::endl;
            return false;
        }
    }

    {
        Board b(10, 10);
        Candy c(CandyType::TYPE_ORANGE);
        b.setCell(&c, 0, 0);
        Board b2(10, 10);
        
        std::string directory = getDataDirPath();
        std::string path = directory + "dump_board.txt";
        
        if (!b.dump(path))
        {
            std::cout << "ERROR:'dump' ha retornat false." << std::endl;
            return false;
        }

        if (!b2.load(path))
        {
            std::cout << "ERROR: Arxiu creat però 'load' no ha pogut llegir-ho.." << std::endl;
            return false;
        }
    }

    // comptarEnDireccio i explosió.
    {
        Board b(5, 5);
        Candy c1(CandyType::TYPE_BLUE), c2(CandyType::TYPE_BLUE), c3(CandyType::TYPE_BLUE);
        
        b.setCell(&c1, 0, 0);
        b.setCell(&c2, 1, 0);
        b.setCell(&c3, 2, 0);

        if (b.comptarEnDireccio(0, 0, 1, 0, CandyType::TYPE_BLUE) < 2) 
        {
            std::cout << "ERROR: comptarEnDireccio no compta els veïns." << std::endl;
            return false;
        }

        if (!b.shouldExplode(1, 0)) 
        {
            std::cout << "ERROR: shouldExplode no detecta 3 en linia." << std::endl;
            return false;
        }
    }

    {
        Board b(5, 5);
        Candy* c0 = new Candy(CandyType::TYPE_RED);
        Candy* c1 = new Candy(CandyType::TYPE_RED);
        Candy* c2 = new Candy(CandyType::TYPE_BLUE);

        b.setCell(c0, 0, 0);
        b.setCell(c1, 1, 0);
        b.setCell(c2, 2, 0);

        if (b.comptarEnDireccio(0, 0, 1, 0, CandyType::TYPE_RED) != 1) 
        { 
            std::cout << "ERROR: comptarEnDireccio no compta correctament." << std::endl;
            delete c0; 
            delete c1; 
            delete c2; 
            return false; 
        }

        delete c0; 
        delete c1; 
        delete c2;
    }

    // explode and drop
    {
        Board b(3, 3);
        Candy rojo(CandyType::TYPE_RED), azul(CandyType::TYPE_BLUE);
        
        // Tres rojos abajo, un azul encima del primero
        b.setCell(&rojo, 0, 2); 
        b.setCell(&rojo, 1, 2);
        b.setCell(&rojo, 2, 2);
        b.setCell(&azul, 0, 1);

        b.explodeAndDrop();

        // El azul debería haber caído a la posición (0, 2)
        if (b.getCell(0, 2) != &azul) 
        {
            std::cout << "ERROR: El Candy superior no ha caigut tras la explosió." << std::endl;
            return false;
        }
    }

    {
        Board b(5, 5);
        Candy* r0 = new Candy(CandyType::TYPE_RED);
        Candy* r1 = new Candy(CandyType::TYPE_RED);
        Candy* r2 = new Candy(CandyType::TYPE_RED);

        b.setCell(r0, 0, 0); 
        b.setCell(r1, 1, 0); 
        b.setCell(r2, 2, 0);

        if (!b.shouldExplode(1, 0)) 
        { 
            std::cout << "shouldExplode no detecta linia horitzontal de 3." << std::endl;
            delete r0; 
            delete r1; 
            delete r2; 
            return false; 
        }
        delete r0; 
        delete r1; 
        delete r2;
    }

    // --- 4. TEST DE PERSISTENCIA (DUMP/LOAD) ---
    {
        Board b(5, 5);
        Candy c(CandyType::TYPE_GREEN);
        b.setCell(&c, 1, 1);
        
        std::string path = "test_dump.txt"; 
        if (b.dump(path)) 
        {
            Board b2(5, 5);
            if (b2.load(path)) 
            {
                if (b2.getCell(1, 1) == nullptr || b2.getCell(1, 1)->getType() != CandyType::TYPE_GREEN) 
                {
                    std::cout << "[Fallo] Los datos cargados no coinciden" << std::endl;
                    return false;
                }
            } 
            else 
            {
                std::cout << "[Fallo] No se pudo cargar el archivo generado" << std::endl;
                return false;
            }
        } 
        else 
        {
            std::cout << "[Aviso] Fallo al escribir archivo (revisa permisos), saltando..." << std::endl;
        }
    }

    { //Test 5- ShouldExplode
        Board b(5, 5); //creem tres caramels vermells seguits
        Candy* r0 = new Candy(CandyType::TYPE_RED);
        Candy* r1 = new Candy(CandyType::TYPE_RED);
        Candy* r2 = new Candy(CandyType::TYPE_RED);

        b.setCell(r0, 0, 0); 
        b.setCell(r1, 1, 0); 
        b.setCell(r2, 2, 0);

        if (!b.shouldExplode(1, 0))  //comproben si el centre explota
        { 
            std::cout << "[FALLO] shouldExplode no detecta línea horizontal de 3" << std::endl;
            delete r0; 
            delete r1; 
            delete r2; 
            return false; 
        }
        delete r0; 
        delete r1; 
        delete r2;
    }

    { //Test6 - Diagonals
        Board b(5, 5);
        Candy* y0 = new Candy(CandyType::TYPE_YELLOW);
        Candy* y1 = new Candy(CandyType::TYPE_YELLOW);
        Candy* y2 = new Candy(CandyType::TYPE_YELLOW);

        b.setCell(y0, 0, 0); //posem caramels en escalera
        b.setCell(y1, 1, 1); 
        b.setCell(y2, 2, 2);

        if (!b.shouldExplode(1, 1)) 
        { 
            std::cout << "[FALLO] shouldExplode no detecta línea diagonal" << std::endl;
            delete y0;
            delete y1;
            delete y2;
            return false;
        }
        delete y0;
        delete y1;
        delete y2;
    }

    { //test 7 - explode and drop (gravetat)
        Board b(5, 5);
        Candy* r0 = new Candy(CandyType::TYPE_RED);
        Candy* r1 = new Candy(CandyType::TYPE_RED);
        Candy* r2 = new Candy(CandyType::TYPE_RED);

        b.setCell(r0, 0, 4); //posem fitxes vermelles a la fila 4 (abaix)
        b.setCell(r1, 1, 4); 
        b.setCell(r2, 2, 4);

        std::vector<Candy*> exploded = b.explodeAndDrop(); //funció deixa caure

        if (exploded.size() != 3) 
        { 
            std::cout << "[FALLO] explodeAndDrop no devuelve 3 caramelos" << std::endl;
            
            //Borramos los caramelos, pero no se hace con delete porque no estan en el tablero
            //Estan en exploded, por eso lo hacemos asi, tmb se hace con for(auto c : exploded)
            for (int i = 0; i < (int)exploded.size(); i++) 
            {
                delete exploded[i];
            }
            return false; 
        }
    
        // Borramos los caramelos explotados de forma manual y clara
        for (int i = 0; i < (int)exploded.size(); i++) 
        {
            delete exploded[i]; 
        }


    }
    return true;
}
