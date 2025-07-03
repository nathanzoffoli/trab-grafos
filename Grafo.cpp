#include "Grafo.h"
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <climits>
#include <algorithm>

Grafo::Grafo(std::vector<char> charVector) {
    if (charVector.size() < 4) {
        ordem = 0;
        return;
    }

    in_direcionado = (charVector[0] == '1');
    in_ponderado_aresta = (charVector[1] == '1');
    in_ponderado_vertice = (charVector[2] == '1');
    ordem = charVector[3] - '0';
    
    int index = 4;
    
    // Criar vértices
    for (int i = 0; i < ordem; i++) {
        char id = charVector[index++];
        No* novoNo = new No(id);
        
        if (in_ponderado_vertice && index < charVector.size()) {
            novoNo->peso = charVector[index++] - '0';
        }
        
        lista_adj.push_back(novoNo);
    }
    
    // Criar arestas
    numero_arestas = 0;
    while (index < charVector.size()) {
        char origem = charVector[index++];
        char destino = charVector[index++];
        int peso = 1;
        
        if (in_ponderado_aresta && index < charVector.size()) {
            peso = charVector[index++] - '0';
        }
        
        No* noOrigem = encontrarNo(origem);
        if (noOrigem) {
            noOrigem->adicionarAresta(destino, peso);
            numero_arestas++;
            
            if (!in_direcionado) {
                No* noDestino = encontrarNo(destino);
                if (noDestino) {
                    noDestino->adicionarAresta(origem, peso);
                }
            }
        }
    }
}

Grafo::~Grafo() {
    for (No* n : lista_adj) {
        delete n;
    }
}

No* Grafo::encontrarNo(char id) {
    for (No* n : lista_adj) {
        if (n->id == id) return n;
    }
    return nullptr;
}

std::vector<char> Grafo::fecho_transitivo_direto(char id_no) {
    std::vector<char> fecho;
    std::unordered_set<char> visitados;
    std::queue<char> fila;
    
    visitados.insert(id_no);
    fila.push(id_no);
    
    while (!fila.empty()) {
        char atual = fila.front();
        fila.pop();
        
        No* no = encontrarNo(atual);
        if (!no) continue;
        
        for (Aresta* a : no->arestas) {
            if (visitados.find(a->id_no_alvo) == visitados.end()) {
                visitados.insert(a->id_no_alvo);
                fecho.push_back(a->id_no_alvo);
                fila.push(a->id_no_alvo);
            }
        }
    }
    
    return fecho;
}

std::vector<char> Grafo::fecho_transitivo_indireto(char id_no) {
    std::vector<char> fecho;
    std::unordered_set<char> visitados;
    std::queue<char> fila;
    
    visitados.insert(id_no);
    fila.push(id_no);
    
    while (!fila.empty()) {
        char atual = fila.front();
        fila.pop();
        
        for (No* no : lista_adj) {
            for (Aresta* a : no->arestas) {
                if (a->id_no_alvo == atual && visitados.find(no->id) == visitados.end()) {
                    visitados.insert(no->id);
                    fecho.push_back(no->id);
                    fila.push(no->id);
                }
            }
        }
    }
    
    return fecho;
}

std::vector<char> Grafo::caminho_minimo_dijkstra(char origem, char destino) {
    std::unordered_map<char, int> distancias;
    std::unordered_map<char, char> predecessores;
    std::priority_queue<std::pair<int, char>, 
                   std::vector<std::pair<int, char>>, 
                   std::greater<std::pair<int, char>>> fila;
    
    for (No* no : lista_adj) {
        distancias[no->id] = INT_MAX;
    }
    
    distancias[origem] = 0;
    fila.push({0, origem});
    
    while (!fila.empty()) {
        char u = fila.top().second;
        fila.pop();
        
        No* noU = encontrarNo(u);
        if (!noU) continue;
        
        for (Aresta* a : noU->arestas) {
            char v = a->id_no_alvo;
            int peso = a->peso;
            
            if (distancias[u] != INT_MAX && distancias[v] > distancias[u] + peso) {
                distancias[v] = distancias[u] + peso;
                predecessores[v] = u;
                fila.push({distancias[v], v});
            }
        }
    }
    
    std::vector<char> caminho;
    if (distancias[destino] == INT_MAX) return caminho;
    
    for (char at = destino; at != origem; at = predecessores[at]) {
        caminho.push_back(at);
    }
    caminho.push_back(origem);
    
    std::reverse(caminho.begin(), caminho.end());
    return caminho;
}

std::vector<char> Grafo::caminho_minimo_floyd(char origem, char destino) {
    std::unordered_map<char, std::unordered_map<char, int>> dist;
    std::unordered_map<char, std::unordered_map<char, char>> next;
    
    // Inicialização
    for (No* u : lista_adj) {
        for (No* v : lista_adj) {
            dist[u->id][v->id] = (u->id == v->id) ? 0 : INT_MAX;
            next[u->id][v->id] = '\0';
        }
    }
    
    // Preenche distâncias diretas
    for (No* u : lista_adj) {
        for (Aresta* a : u->arestas) {
            dist[u->id][a->id_no_alvo] = a->peso;
            next[u->id][a->id_no_alvo] = a->id_no_alvo;
        }
    }
    
    // Algoritmo de Floyd-Warshall
    for (No* k : lista_adj) {
        for (No* i : lista_adj) {
            for (No* j : lista_adj) {
                if (dist[i->id][k->id] != INT_MAX && dist[k->id][j->id] != INT_MAX &&
                    dist[i->id][j->id] > dist[i->id][k->id] + dist[k->id][j->id]) {
                    dist[i->id][j->id] = dist[i->id][k->id] + dist[k->id][j->id];
                    next[i->id][j->id] = next[i->id][k->id];
                }
            }
        }
    }
    
    // Reconstruir caminho
    std::vector<char> caminho;
    if (next[origem][destino] == '\0') return caminho;
    
    caminho.push_back(origem);
    char atual = origem;
    while (atual != destino) {
        atual = next[atual][destino];
        caminho.push_back(atual);
    }
    
    return caminho;
}

Grafo* Grafo::arvore_geradora_minima_prim() {
    // Cria um novo grafo para a AGM com os mesmos parâmetros do grafo original
    Grafo* agm = new Grafo({in_direcionado ? '1' : '0', '1', '0', static_cast<char>(ordem + '0')});
    
    if (lista_adj.empty()) return agm;

    // Prepara estruturas para o algoritmo
    std::priority_queue<std::pair<int, std::pair<char, char>>,
                       std::vector<std::pair<int, std::pair<char, char>>>,
                       std::greater<std::pair<int, std::pair<char, char>>>> fila;
    
    std::unordered_set<char> incluidos;

    // Adiciona todos os nós ao AGM (sem arestas ainda)
    for (No* no : lista_adj) {
        No* novoNo = new No(no->id);
        agm->lista_adj.push_back(novoNo);
    }

    // Começa pelo primeiro nó do grafo
    char primeiro_no = lista_adj[0]->id;
    incluidos.insert(primeiro_no);

    // Adiciona arestas do primeiro nó à fila de prioridade
    No* noInicial = encontrarNo(primeiro_no);
    if (noInicial) {
        for (Aresta* a : noInicial->arestas) {
            fila.push({a->peso, {noInicial->id, a->id_no_alvo}});
        }
    }

    while (!fila.empty()) {
        auto elemento = fila.top();
        fila.pop();
        
        int peso = elemento.first;
        char u = elemento.second.first;
        char v = elemento.second.second;

        if (incluidos.find(v) == incluidos.end()) {
            incluidos.insert(v);

            // Adiciona aresta na AGM
            No* noU = agm->encontrarNo(u);
            if (noU) {
                noU->adicionarAresta(v, peso);
                if (!in_direcionado) {
                    No* noV = agm->encontrarNo(v);
                    if (noV) {
                        noV->adicionarAresta(u, peso);
                    }
                }
                agm->numero_arestas++;
            }

            // Adiciona novas arestas à fila
            No* noV = encontrarNo(v);
            if (noV) {
                for (Aresta* a : noV->arestas) {
                    if (incluidos.find(a->id_no_alvo) == incluidos.end()) {
                        fila.push({a->peso, {v, a->id_no_alvo}});
                    }
                }
            }
        }
    }

    return agm;
}


Grafo* Grafo::arvore_geradora_minima_kruskal() {
    struct ArestaKruskal {
        char u, v;
        int peso;
        bool operator<(const ArestaKruskal& other) const {
            return peso < other.peso;
        }
    };

    // Cria um novo grafo para a AGM
    Grafo* agm = new Grafo({in_direcionado ? '1' : '0', '1', '0', static_cast<char>(ordem + '0')});
    
    std::vector<ArestaKruskal> arestas;
    std::unordered_map<char, char> parent;
    std::unordered_map<char, int> rank;

    // Coleta todas as arestas do grafo
    for (No* no : lista_adj) {
        for (Aresta* a : no->arestas) {
            // Para grafos não direcionados, adiciona cada aresta apenas uma vez
            if (!in_direcionado && no->id > a->id_no_alvo) continue;
            arestas.push_back({no->id, a->id_no_alvo, a->peso});
        }
    }

    // Ordena arestas por peso
    std::sort(arestas.begin(), arestas.end());

    // Função para encontrar o conjunto de um elemento (Union-Find)
    auto find = [&](char u) {
        while (parent[u] != u) {
            parent[u] = parent[parent[u]]; // Path compression
            u = parent[u];
        }
        return u;
    };

    // Inicializa estruturas para Union-Find com todos os nós
    for (No* no : lista_adj) {
        parent[no->id] = no->id;
        rank[no->id] = 0;
        
        // Adiciona nós ao AGM (sem arestas ainda)
        No* novoNo = new No(no->id);
        agm->lista_adj.push_back(novoNo);
    }

    // Algoritmo de Kruskal
    for (const auto& a : arestas) {
        char rootU = find(a.u);
        char rootV = find(a.v);

        if (rootU != rootV) {
            // Adiciona aresta ao AGM
            No* noU = agm->encontrarNo(a.u);
            if (noU) {
                noU->adicionarAresta(a.v, a.peso);
                if (!in_direcionado) {
                    No* noV = agm->encontrarNo(a.v);
                    if (noV) {
                        noV->adicionarAresta(a.u, a.peso);
                    }
                }
                agm->numero_arestas++;
            }

            // Union
            if (rank[rootU] > rank[rootV]) {
                parent[rootV] = rootU;
            } else {
                parent[rootU] = rootV;
                if (rank[rootU] == rank[rootV]) {
                    rank[rootV]++;
                }
            }
        }
    }

    return agm;
}

Grafo* Grafo::arvore_caminhamento_profundidade(char id_no) {
    Grafo* dfsTree = new Grafo({in_direcionado ? '1' : '0', '0', '0', static_cast<char>(ordem + '0')});
    std::unordered_map<char, bool> visitados;
    std::stack<char> pilha;
    
    // Adiciona todos os nós (sem arestas)
    for (No* no : lista_adj) {
        No* novoNo = new No(no->id);
        dfsTree->lista_adj.push_back(novoNo);
    }
    
    pilha.push(id_no);
    visitados[id_no] = true;
    
    while (!pilha.empty()) {
        char atual = pilha.top();
        pilha.pop();
        
        No* noAtual = encontrarNo(atual);
        if (!noAtual) continue;
        
        for (Aresta* a : noAtual->arestas) {
            if (!visitados[a->id_no_alvo]) {
                visitados[a->id_no_alvo] = true;
                pilha.push(a->id_no_alvo);
                
                // Adiciona aresta na árvore DFS
                No* noTree = dfsTree->encontrarNo(atual);
                if (noTree) {
                    noTree->adicionarAresta(a->id_no_alvo);
                }
            }
        }
    }
    
    return dfsTree;
}

int Grafo::raio() {
    int raio = INT_MAX;
    for (No* no : lista_adj) {
        std::unordered_map<char, int> distancias;
        std::queue<char> fila;
        
        for (No* n : lista_adj) {
            distancias[n->id] = -1;
        }
        
        distancias[no->id] = 0;
        fila.push(no->id);
        int ecc = 0;
        
        while (!fila.empty()) {
            char atual = fila.front();
            fila.pop();
            
            No* noAtual = encontrarNo(atual);
            if (!noAtual) continue;
            
            for (Aresta* a : noAtual->arestas) {
                if (distancias[a->id_no_alvo] == -1) {
                    distancias[a->id_no_alvo] = distancias[atual] + 1;
                    ecc = std::max(ecc, distancias[a->id_no_alvo]);
                    fila.push(a->id_no_alvo);
                }
            }
        }
        
        raio = std::min(raio, ecc);
    }
    return raio;
}

int Grafo::diametro() {
    int diametro = 0;
    for (No* no : lista_adj) {
        std::unordered_map<char, int> distancias;
        std::queue<char> fila;
        
        for (No* n : lista_adj) {
            distancias[n->id] = -1;
        }
        
        distancias[no->id] = 0;
        fila.push(no->id);
        int ecc = 0;
        
        while (!fila.empty()) {
            char atual = fila.front();
            fila.pop();
            
            No* noAtual = encontrarNo(atual);
            if (!noAtual) continue;
            
            for (Aresta* a : noAtual->arestas) {
                if (distancias[a->id_no_alvo] == -1) {
                    distancias[a->id_no_alvo] = distancias[atual] + 1;
                    ecc = std::max(ecc, distancias[a->id_no_alvo]);
                    fila.push(a->id_no_alvo);
                }
            }
        }
        
        diametro = std::max(diametro, ecc);
    }
    return diametro;
}

std::vector<char> Grafo::centro() {
    std::vector<char> centros;
    int minEccentricity = INT_MAX;
    std::unordered_map<char, int> eccentricities;
    
    for (No* no : lista_adj) {
        std::unordered_map<char, int> distancias;
        std::queue<char> fila;
        
        for (No* n : lista_adj) {
            distancias[n->id] = -1;
        }
        
        distancias[no->id] = 0;
        fila.push(no->id);
        int ecc = 0;
        
        while (!fila.empty()) {
            char atual = fila.front();
            fila.pop();
            
            No* noAtual = encontrarNo(atual);
            if (!noAtual) continue;
            
            for (Aresta* a : noAtual->arestas) {
                if (distancias[a->id_no_alvo] == -1) {
                    distancias[a->id_no_alvo] = distancias[atual] + 1;
                    ecc = std::max(ecc, distancias[a->id_no_alvo]);
                    fila.push(a->id_no_alvo);
                }
            }
        }
        
        eccentricities[no->id] = ecc;
        minEccentricity = std::min(minEccentricity, ecc);
    }
    
    for (const auto& pair : eccentricities) {
        if (pair.second == minEccentricity) {
            centros.push_back(pair.first);
        }
    }
    
    return centros;
}

std::vector<char> Grafo::periferia() {
    std::vector<char> periferia;
    int maxEccentricity = 0;
    std::unordered_map<char, int> eccentricities;
    
    for (No* no : lista_adj) {
        std::unordered_map<char, int> distancias;
        std::queue<char> fila;
        
        for (No* n : lista_adj) {
            distancias[n->id] = -1;
        }
        
        distancias[no->id] = 0;
        fila.push(no->id);
        int ecc = 0;
        
        while (!fila.empty()) {
            char atual = fila.front();
            fila.pop();
            
            No* noAtual = encontrarNo(atual);
            if (!noAtual) continue;
            
            for (Aresta* a : noAtual->arestas) {
                if (distancias[a->id_no_alvo] == -1) {
                    distancias[a->id_no_alvo] = distancias[atual] + 1;
                    ecc = std::max(ecc, distancias[a->id_no_alvo]);
                    fila.push(a->id_no_alvo);
                }
            }
        }
        
        eccentricities[no->id] = ecc;
        maxEccentricity = std::max(maxEccentricity, ecc);
    }
    
    for (const auto& pair : eccentricities) {
        if (pair.second == maxEccentricity) {
            periferia.push_back(pair.first);
        }
    }
    
    return periferia;
}

std::vector<char> Grafo::vertices_de_articulacao() {
    std::vector<char> articulacoes;
    std::unordered_map<char, int> discovery;
    std::unordered_map<char, int> low;
    std::unordered_map<char, char> parent;
    std::unordered_map<char, bool> visited;
    int time = 0;
    
    for (No* no : lista_adj) {
        discovery[no->id] = 0;
        low[no->id] = 0;
        parent[no->id] = '\0';
        visited[no->id] = false;
    }
    
    for (No* no : lista_adj) {
        if (!visited[no->id]) {
            dfs_articulacao(no->id, discovery, low, parent, visited, time, articulacoes);
        }
    }
    
    return articulacoes;
}

void Grafo::dfs_articulacao(char u, std::unordered_map<char, int>& discovery,
                           std::unordered_map<char, int>& low, std::unordered_map<char, char>& parent,
                           std::unordered_map<char, bool>& visited, int& time,
                           std::vector<char>& articulationPoints) {
    visited[u] = true;
    discovery[u] = low[u] = ++time;
    int children = 0;
    
    No* noU = encontrarNo(u);
    if (!noU) return;
    
    for (Aresta* a : noU->arestas) {
        char v = a->id_no_alvo;
        
        if (!visited[v]) {
            children++;
            parent[v] = u;
            dfs_articulacao(v, discovery, low, parent, visited, time, articulationPoints);
            
            low[u] = std::min(low[u], low[v]);
            
            // Verifica se é ponto de articulação
            if (parent[u] == '\0' && children > 1) {
                if (std::find(articulationPoints.begin(), articulationPoints.end(), u) == articulationPoints.end()) {
                    articulationPoints.push_back(u);
                }
            }
            
            if (parent[u] != '\0' && low[v] >= discovery[u]) {
                if (std::find(articulationPoints.begin(), articulationPoints.end(), u) == articulationPoints.end()) {
                    articulationPoints.push_back(u);
                }
            }
        } else if (v != parent[u]) {
            low[u] = std::min(low[u], discovery[v]);
        }
    }
}