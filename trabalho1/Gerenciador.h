#ifndef GERENCIADOR_H
#define GERENCIADOR_H

#include "Grafo.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class Gerenciador {
public:
    static void comandos(Grafo* grafo);
    static char get_id_entrada();
    static std::vector<char> get_conjunto_ids(Grafo* grafo, int tam);
    static bool pergunta_imprimir_arquivo(std::string nome_arquivo);

    template<typename T>
    static void imprimir_vetor(std::ostream& os, const std::vector<T>& vec, const std::string& label = "") {
        if (!label.empty()) os << label << ": ";
        os << "[ ";
        for (size_t i = 0; i < vec.size(); ++i) {
            os << vec[i];
            if (i != vec.size() - 1) os << ", ";
        }
        os << " ]\n";
    }

    static void imprimir_grafo(std::ostream& os, Grafo* grafo, const std::string& label = "") {
        if (!label.empty()) os << label << "\n";
        os << "Ordem: " << grafo->ordem << " | Arestas: " << grafo->numero_arestas << "\n";
        os << "Direcionado: " << (grafo->in_direcionado ? "Sim" : "Não") << "\n";
        os << "Ponderado: " << (grafo->in_ponderado_aresta ? "Sim" : "Não") << "\n\n";
        
        for (No* no : grafo->lista_adj) {
            os << no->id;
            if (grafo->in_ponderado_vertice) os << "(" << no->peso << ")";
            os << " -> ";
            
            for (Aresta* aresta : no->arestas) {
                os << aresta->id_no_alvo;
                if (grafo->in_ponderado_aresta) os << "[" << aresta->peso << "]";
                os << " ";
            }
            os << "\n";
        }
    }
};

#endif
