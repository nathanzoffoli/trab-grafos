#ifndef NO_H
#define NO_H

#include <vector>

class Aresta;

class No {
public:
    char id;
    int peso;
    bool domina;
    bool dominado;
    std::vector<Aresta*> arestas;

    No();
    explicit No(char id);
    No(char id, int peso);
    ~No();

    void adicionarAresta(char destino, int peso = 1);
    bool removerAresta(char destino);
};

#endif