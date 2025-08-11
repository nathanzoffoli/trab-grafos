#ifndef ALGORITMOSGULOSOS_H
#define ALGORITMOSGULOSOS_H

#include <vector>

class Grafo;

class AlgoritmosGulosos {
public:
    // O algoritmo guloso determinístico e eficiente (inalterado)
    static std::vector<char> algoritmoGuloso(Grafo* grafo);

    // O algoritmo randomizado com a nova lógica e o parâmetro alfa
    static std::vector<char> algoritmoGulosoRandomizado(Grafo* grafo, float alfa, int maxIteracoes);

    // O algoritmo Reativo que volta a fazer sentido com o alfa
    static std::vector<char> algoritmoGulosoRandomizadoReativo(Grafo* grafo, int maxIteracoes);
};

#endif