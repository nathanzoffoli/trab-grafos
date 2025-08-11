#include "No.h"
#include "Aresta.h"
#include <algorithm>

No::No() : id('\0'), peso(0), domina(false), dominado(false) {}

No::No(char id) : id(id), peso(0), domina(false), dominado(false) {}

No::No(char id, int peso) : id(id), peso(peso), domina(false), dominado(false) {}

void No::adicionarAresta(char destino, int peso) {
    arestas.push_back(new Aresta(destino, peso));
}

bool No::removerAresta(char destino) {
    auto it = std::find_if(arestas.begin(), arestas.end(), 
        [destino](Aresta* a) { return a->id_no_alvo == destino; });
    
    if (it != arestas.end()) {
        delete *it;
        arestas.erase(it);
        return true;
    }
    return false;
}

No::~No() {
    for (Aresta* a : arestas) {
        delete a;
    }
}