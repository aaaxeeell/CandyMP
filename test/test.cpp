#include <filesystem>
#include "board.h"
#include "candy.h"
#include "controller.h"
#include "game.h"
#include "util.h"

#include <iostream>

bool test()
{
    // Test board 2D container
    Candy c(CandyType::TYPE_ORANGE);
    Board b(10, 10);
    b.setCell(&c, 0, 0);
    if (b.getCell(0, 0) != &c)
    {
        return false;
    }

    // Dump and load board
    {
        Board b2(10, 10);
        if (!b.dump(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (!b2.load(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (b2.getCell(0, 0)->getType() != c.getType())
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_board.txt");
    }

    // Dump and load game
    {
        Game g;
        Controller cont;
        g.update(cont);
        if (!g.dump(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        Game g2;
        if (!g2.load(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        if (g != g2)
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_game.txt");
    }


    return true;
}


bool testGetCell()
{
    Board b(5,5);
    bool TOT_OK = true;

    // getCell retorna nullptr.

    if (b.getCell(0,0) != nullptr)
        TOT_OK = false;
    if (b.getCell(4,4) != nullptr)
        TOT_OK = false;
    if (b.getCell(6,10) != nullptr)
        TOT_OK = false; 


    // Coordenades fora de rang retorna nullptr.
    if (b.getCell(-1,  0) != nullptr) 
        TOT_OK = false;
    if (b.getCell( 0, -1) != nullptr) 
        TOT_OK = false;
    if (b.getCell( 5,  0) != nullptr) 
        TOT_OK = false;
    if (b.getCell( 0,  5) != nullptr) 
        TOT_OK = false;
 
    // Coordenades recuperables.
    
    Board b(5, 5);
    bool ok = true;
    Candy* c = new Candy(CandyType::TYPE_RED);
    b.setCell(c, 2, 3);
 
    if (b.getCell(2, 3) != c)                             
        TOT_OK = false;
    if (b.getCell(2, 3)->getType() != CandyType::TYPE_RED)      
        TOT_OK = false;
        
    if (b.getCell(2, 2) != nullptr)                        
        TOT_OK = false;
 
        delete c;
    

    // comptarEnDireccio en HORITZONTAL
    Board b(5, 5);
    bool ok = true;
    Candy* c0 = new Candy(CandyType::TYPE_RED);
    Candy* c1 = new Candy(CandyType::TYPE_RED);
    Candy* c2 = new Candy(CandyType::TYPE_RED);
    b.setCell(c0, 0, 2);
    b.setCell(c1, 1, 2);
    b.setCell(c2, 2, 2);
 
    if (b.comptarEnDireccio(0, 2,  1, 0, CandyType::TYPE_RED) != 2) 
        ok = false;
    if (b.comptarEnDireccio(2, 2, -1, 0, CandyType::TYPE_RED) != 2) 
        TOT_OK = false;
    if (b.comptarEnDireccio(1, 2,  1, 0, CandyType::TYPE_RED) != 1) 
        TOT_OK = false;
    if (b.comptarEnDireccio(1, 2,  0,-1, CandyType::TYPE_RED) != 0) 
        TOT_OK = false;
 
        delete c0; delete c1; delete c2;


    Board b(5, 5);
    bool ok = true;
 
    Candy* c0 = new Candy(CandyType::TYPE_RED);
    Candy* c1 = new Candy(CandyType::TYPE_RED);
    Candy* c2 = new Candy(CandyType::TYPE_BLUE);
    b.setCell(c0, 0, 0);
    b.setCell(c1, 1, 0);
    b.setCell(c2, 2, 0);
 
    if (b.comptarEnDireccio(0, 0, 1, 0, CandyType::TYPE_RED) != 1) 
        TOT_OK = false;
 
    delete c0; delete c1; delete c2;


    // shouldExplode en linia horitzontal de 3
    // shouldExplode: nomes dos son del mateix tipus i NO EXPLOTA

    return TOT_OK;

    
}   

bool