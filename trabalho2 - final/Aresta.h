#ifndef ARESTA_H
#define ARESTA_H

class Aresta {
public:
    char id_no_alvo;
    int peso;

    Aresta();
    explicit Aresta(char destino, int p = 1);
};

#endif