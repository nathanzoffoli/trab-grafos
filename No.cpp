#include "No.h"

No::No() : id('\0'), peso(0) {}

No::No(char id) : id(id), peso(0) {}

No::No(char id, int peso) : id(id), peso(peso) {}

void No::adicionarAresta(char destino, int peso) {
    Aresta* nova = new Aresta(destino, peso);
    arestas.push_back(nova);
}

bool No::removerAresta(char destino) {
    for (auto it = arestas.begin(); it != arestas.end(); ++it) {
        if ((*it)->id_no_alvo == destino) {
            delete *it;
            arestas.erase(it);
            return true;
        }
    }
    return false;
}

No::~No() {
    for (Aresta* a : arestas) {
        delete a;
    }
}