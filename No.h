#ifndef NO_H
#define NO_H

#include "Aresta.h"
#include <vector>

class No {
public:
    char id;
    int peso;
    std::vector<Aresta*> arestas;
    
    No();
    explicit No(char id);
    No(char id, int peso);
    ~No();
    
    void adicionarAresta(char destino, int peso = 1);
    bool removerAresta(char destino);
};

#endif