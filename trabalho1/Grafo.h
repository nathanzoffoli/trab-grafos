#ifndef GRAFO_H
#define GRAFO_H

#include "No.h"
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>

class Grafo {
public:
    Grafo(std::vector<char> charVector);
    ~Grafo();
    
    // Métodos básicos
    std::vector<char> fecho_transitivo_direto(char id_no);
    std::vector<char> fecho_transitivo_indireto(char id_no);
    std::vector<char> caminho_minimo_dijkstra(char id_no_a, char id_no_b);
    std::vector<char> caminho_minimo_floyd(char id_no_a, char id_no_b);
    Grafo* arvore_geradora_minima_prim();
    Grafo* arvore_geradora_minima_kruskal();
    Grafo* arvore_caminhamento_profundidade(char id_no);
    
    // Métricas do grafo
    int raio();
    int diametro();
    std::vector<char> centro();
    std::vector<char> periferia();
    std::vector<char> vertices_de_articulacao();
    
    // Utilitários
    No* encontrarNo(char id);
    bool existeAresta(char origem, char destino);
    
    // Atributos
    int ordem;
    int numero_arestas;
    bool in_direcionado;
    bool in_ponderado_aresta;
    bool in_ponderado_vertice;
    std::vector<No*> lista_adj;

private:
    void dfs_articulacao(char u, std::unordered_map<char, int>& discovery,
                        std::unordered_map<char, int>& low, std::unordered_map<char, char>& parent,
                        std::unordered_map<char, bool>& visited, int& time,
                        std::vector<char>& articulationPoints);
    
    void dfs_caminhamento(char no_atual, std::unordered_map<char, bool>& visitados, Grafo* arvore);
};

#endif
