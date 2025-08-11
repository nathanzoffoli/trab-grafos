#ifndef GRAFO_H
#define GRAFO_H

#include <vector>
#include <string>
#include <utility> // Para std::pair
#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <algorithm>
#include <queue>
#include <stack>
#include "No.h"
#include "Aresta.h"

class No;
class Aresta;

class Grafo {
public:
    std::unordered_map<char, No*> mapa_nos;
    std::vector<No*> lista_nos;
    int ordem;
    int numero_arestas;
    bool in_direcionado;
    bool in_ponderado_aresta;
    bool in_ponderado_vertice;

    // Construtor principal para criar o grafo a partir do arquivo
    explicit Grafo(bool direcionado, bool ponderado_aresta, bool ponderado_vertice,
                   const std::vector<char>& ids_vertices,
                   const std::vector<std::pair<char, char>>& arestas_pares);
    
    // Construtor auxiliar para criar grafos vazios (usado em AGM, etc.)
    explicit Grafo(bool direcionado, bool ponderado_aresta, bool ponderado_vertice, int ordem_inicial);


    ~Grafo();

    No* encontrarNo(char id);
    std::vector<char> fecho_transitivo_direto(char id_no);
    std::vector<char> fecho_transitivo_indireto(char id_no);
    std::vector<char> caminho_minimo_dijkstra(char origem, char destino);
    std::vector<char> caminho_minimo_floyd(char origem, char destino);
    Grafo* arvore_geradora_minima_prim();
    Grafo* arvore_geradora_minima_kruskal();
    Grafo* arvore_caminhamento_profundidade(char id_no);
    int raio();
    int diametro();
    std::vector<char> centro();
    std::vector<char> periferia();
    std::vector<char> vertices_de_articulacao();

private:
    void dfs_articulacao(char u, std::unordered_map<char, int>& discovery,
                        std::unordered_map<char, int>& low, std::unordered_map<char, char>& parent,
                        std::unordered_map<char, bool>& visitado, int& time,
                        std::vector<char>& articulationPoints);
};

#endif