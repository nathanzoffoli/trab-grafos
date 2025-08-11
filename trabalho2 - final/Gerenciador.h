#ifndef GERENCIADOR_H
#define GERENCIADOR_H

#include <string>
#include <vector>

class Grafo;

class Gerenciador {
public:
    static void comandos(Grafo* grafo);

private:
    static char get_id_entrada();
    static bool pergunta_imprimir_arquivo(const std::string& nome_arquivo);
    static std::vector<char> get_conjunto_ids(Grafo* grafo, int tam);
};

#endif