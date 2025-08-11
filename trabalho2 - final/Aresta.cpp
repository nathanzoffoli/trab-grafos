#include "Aresta.h"

Aresta::Aresta() : id_no_alvo('\0'), peso(0) {}

Aresta::Aresta(char destino, int p) : id_no_alvo(destino), peso(p) {}