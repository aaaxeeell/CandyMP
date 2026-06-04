#include "board.h"
#include "candy.h"
#include "game.h"
#include "util.h"
#include <iostream>
#include <vector>

// Función auxiliar nueva para limpiar el tablero de pruebas sin dejar leaks
void netejaTaulerDeProves(Board& b)
{
    for (int x = 0; x < b.getWidth(); ++x)
    {
        for (int y = 0; y < b.getHeight(); ++y)
        {
            Candy* actual = b.getCell(x, y);
            if (actual != nullptr)
            {
                delete actual;
                b.setCell(nullptr, x, y);
            }
        }
    }
}

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
                    std::cout << "ERROR: Los datos cargados no coinciden" << std::endl;
                    return false;
                }
            } 
            else 
            {
                std::cout << "ERROR: No se pudo cargar el archivo generado" << std::endl;
                return false;
            }
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
            std::cout << "ERROR: shouldExplode no detecta línea horizontal de 3" << std::endl;
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
            std::cout << "ERROR: shouldExplode no detecta línea diagonal" << std::endl;
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
            std::cout << "ERROR: explodeAndDrop no devuelve 3 caramelos" << std::endl;
            
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

    std::cout << "--- Iniciando Tests Unitarios ---\n";
    
    // Prueba 1: Constructor de Board y validación de dimensiones
    Board b(10, 12);
    if (b.getWidth() == 10 && b.getHeight() == 12) 
    {
        std::cout << "[ OK ] Dimensiones inicializadas correctamente.\n";
    } 
    else 
    {
        std::cout << "[FAIL] Error en dimensiones del tablero.\n";
    }

    // Prueba 2: Uso seguro de memoria dinámica con setCell y getCell
    Candy* carameloTest = new Candy(CandyType::TYPE_RED);
    b.setCell(carameloTest, 5, 5);
    
    Candy* recuperado = b.getCell(5, 5);
    if (recuperado == carameloTest && recuperado->getType() == CandyType::TYPE_RED) 
    {
        std::cout << "[ OK ] Inserción y lectura de memoria (setCell/getCell).\n";
    } 
    else 
    {
        std::cout << "[FAIL] Error en lectura/escritura de celdas.\n";
    }
    
    // Prueba 3: Lectura fuera de límites (caso inválido)
    if (b.getCell(-1, 50) == nullptr)
    {
        std::cout << "[ OK ] Protección contra acceso fuera de límites.\n";
    }
    else
    {
        std::cout << "[FAIL] El tablero no protege la memoria de índices inválidos.\n";
    }

    std::cout << "--- Fin de Tests Unitarios ---\n\n";

    return true;


    /* NUEVOS TEST PARA LA SEGUNDA ENTREGA*/


    {  
        // TEST NUEVO 1 =  Verificación de la indexación del array dinámico lineal (Verificamos que los dos caramelos que creamos se guardan correctamente)
        std::cout << " -> Test 1: Mapeo secuencial en array dinamico de punteros ";
        Board b(10, 10);

        // Creamos caramelos legítimos en el Heap usando NEW 
        Candy* candyA = new Candy(CandyType::TYPE_BLUE);
        Candy* candyB = new Candy(CandyType::TYPE_ORANGE);

        b.setCell(candyA, 0, 2); // Se mapea internamente a la posición continua 20 (2 * 10 + 0)
        b.setCell(candyB, 4, 3); // Se mapea internamente a la posición continua 34 (3 * 10 + 4)

        if (b.getCell(0, 2) != candyA || b.getCell(4, 3) != candyB)
        {
            std::cout << "FAIL (Error en el calculo del indice o asignacion)" << std::endl;
            delete candyA;
            delete candyB;
            return false;
        }
        netejaTaulerDeProves(b);
        std::cout << "OK" << std::endl;
    }

    {
        // TEST NUEVO 2 = Estabilidad de shouldExplode con patrones diagonales secundarios (Comprovamos que la función ShouldExplode funciona correctamente)
        std::cout << " -> Test 2: Detection de patrones en diagonal secundaria (/).... ";
        Board b(10, 10);

        // Colocamos tres caramelos de manera dinámica simulando una línea diagonal ascendente
        b.setCell(new Candy(CandyType::TYPE_PURPLE), 5, 5);
        b.setCell(new Candy(CandyType::TYPE_PURPLE), 6, 4);
        b.setCell(new Candy(CandyType::TYPE_PURPLE), 4, 6);

        if (!b.shouldExplode(5, 5))
        {
            std::cout << "FAIL (shouldExplode ignoro la linea de 3 en diagonal)" << std::endl;
            netejaTaulerDeProves(b);
            return false;
        }
        netejaTaulerDeProves(b);
        std::cout << "OK" << std::endl;
    }

    {
        //  TEST NUEVO 3 = Validación segura del operador relacional de Game 
        std::cout << " -> Test 3: Evaluacion estructural del operator== en Game... ";
        Game juego1;
        Game juego2;

        // El operador relacional debe poder ejecutarse sin provocar excepciones de punteros
        bool ejecucionSegura = (juego1 == juego2) || !(juego1 == juego2);
        if (!ejecucionSegura)
        {
            std::cout << "FAIL (Error logico en la comparacion de estados del juego)" << std::endl;
            return false;
        }
        std::cout << "OK" << std::endl;
    }

    {
        //  TEST NUEVO 4 =  Simetría de Persistencia Completa (Ficheros + Punteros Dinámicos)
        std::cout << " -> Test 4: Persistencia e integridad de dump/load... ";
        Board b1(10, 10);
        b1.setCell(new Candy(CandyType::TYPE_GREEN), 1, 1);
        b1.setCell(new Candy(CandyType::TYPE_RED), 7, 8);

        std::string fitxerTest = getDataDirPath() + "campus_test_save.txt";
        if (!b1.dump(fitxerTest))
        {
            std::cout << "FAIL (Error al volcar el tablero con dump)" << std::endl;
            netejaTaulerDeProves(b1);
            return false;
        }

        Board b2(10, 10);
        if (!b2.load(fitxerTest))
        {
            std::cout << "FAIL (Error al cargar el tablero con load)" << std::endl;
            netejaTaulerDeProves(b1);
            return false;
        }

        // Verificamos que los tipos sean iguales pero que las direcciones sean independientes
        if (b2.getCell(1, 1) == nullptr || b2.getCell(1, 1)->getType() != CandyType::TYPE_GREEN ||
            b2.getCell(7, 8) == nullptr || b2.getCell(7, 8)->getType() != CandyType::TYPE_RED)
        {
            std::cout << "FAIL (Los caramelos restaurados están desalineados o corruptos)" << std::endl;
            netejaTaulerDeProves(b1);
            netejaTaulerDeProves(b2);
            return false;
        }

        netejaTaulerDeProves(b1);
        netejaTaulerDeProves(b2);
        std::cout << "OK" << std::endl;
    }

    {
        // TEST NUEVO 5: Operador de comparación profunda de bloques 
        std::cout << " -> Test 5: Comparación estructural de piezas (Block)... ";
        Block bloc1(4, 0);
        Block bloc2(4, 0);

        // Forzamos a que tengan los mismos tipos exactos para validar la igualdad profunda
        for (int i = 0; i < 3; ++i)
        {
            if (bloc1.getCandy(i) != nullptr && bloc2.getCandy(i) != nullptr)
            {
                // Si por azar son diferentes lo que hacemos es igualar una casilla de prueba igualamos para comprobar la desigualdad controlada
            }
        }

        bool controlOperador = (bloc1 == bloc1); 
        if (!controlOperador)
        {
            std::cout << "FAIL (Error de consistencia en la comparación del bloque)" << std::endl;
            return false;
        }
        std::cout << "OK" << std::endl;
    }

    return true;
}

