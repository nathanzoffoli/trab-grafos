#include "Grafo.h"
#include "No.h"
#include "Aresta.h"
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <algorithm>
#include <sstream>


Grafo::Grafo(bool direcionado, bool ponderado_aresta, bool ponderado_vertice,
             const std::vector<char>& ids_vertices,
             const std::vector<std::pair<char, char>>& arestas_pares) {
    
    this->in_direcionado = direcionado;
    this->in_ponderado_aresta = ponderado_aresta;
    this->in_ponderado_vertice = ponderado_vertice;
    this->ordem = ids_vertices.size();
    this->numero_arestas = 0;

    
    for (char id : ids_vertices) {
        No* novoNo = new No(id);
        lista_nos.push_back(novoNo);
        mapa_nos[id] = novoNo;
    }

    
    for (const auto& par_aresta : arestas_pares) {
        int peso = 1; // Peso padrão
        No* noOrigem = encontrarNo(par_aresta.first);
        No* noDestino = encontrarNo(par_aresta.second);

        if (noOrigem && noDestino) {
            noOrigem->adicionarAresta(par_aresta.second, peso);
            this->numero_arestas++;
            if (!in_direcionado) {
                noDestino->adicionarAresta(par_aresta.first, peso);
            }
        }
    }
}


Grafo::Grafo(bool direcionado, bool ponderado_aresta, bool ponderado_vertice, int ordem_inicial) {
    this->in_direcionado = direcionado;
    this->in_ponderado_aresta = ponderado_aresta;
    this->in_ponderado_vertice = ponderado_vertice;
    this->ordem = ordem_inicial;
    this->numero_arestas = 0;
}


Grafo::~Grafo() {
    for (No* n : lista_nos) {
        delete n;
    }
}

No* Grafo::encontrarNo(char id) {
    auto it = mapa_nos.find(id);
    if (it != mapa_nos.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<char> Grafo::fecho_transitivo_direto(char id_no) {
    std::vector<char> fecho;
    std::unordered_set<char> visitados;
    std::queue<char> fila;
    
    if (encontrarNo(id_no) == nullptr) return fecho;
    
    visitados.insert(id_no);
    fila.push(id_no);
    
    while (!fila.empty()) {
        char atual = fila.front();
        fila.pop();
        
        No* no = encontrarNo(atual);
        if (!no) continue;
        
        for (Aresta* a : no->arestas) {
            if (visitados.insert(a->id_no_alvo).second) {
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

    if (encontrarNo(id_no) == nullptr) return fecho;
    
    visitados.insert(id_no);
    fila.push(id_no);
    
    while (!fila.empty()) {
        char atual = fila.front();
        fila.pop();
        
        for (No* no : lista_nos) {
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
    
    for (No* no : lista_nos) {
        distancias[no->id] = INT_MAX;
    }

    if (encontrarNo(origem) == nullptr || encontrarNo(destino) == nullptr) return {};
    
    distancias[origem] = 0;
    fila.push({0, origem});
    
    while (!fila.empty()) {
        char u = fila.top().second;
        fila.pop();
        
        if (u == destino) break;

        No* noU = encontrarNo(u);
        if (!noU) continue;
        
        for (Aresta* a : noU->arestas) {
            char v = a->id_no_alvo;
            int peso = a->peso;
            
            if (distancias.count(v) && distancias[u] != INT_MAX && distancias[v] > distancias[u] + peso) {
                distancias[v] = distancias[u] + peso;
                predecessores[v] = u;
                fila.push({distancias[v], v});
            }
        }
    }
    
    std::vector<char> caminho;
    if (distancias.find(destino) == distancias.end() || distancias[destino] == INT_MAX) return caminho;
    
    for (char at = destino; ; at = predecessores[at]) {
        caminho.push_back(at);
        if (at == origem) break;
    }
    
    std::reverse(caminho.begin(), caminho.end());
    return caminho;
}

std::vector<char> Grafo::caminho_minimo_floyd(char origem, char destino) {
    std::unordered_map<char, std::unordered_map<char, int>> dist;
    std::unordered_map<char, std::unordered_map<char, char>> next;
    
    for (No* u : lista_nos) {
        for (No* v : lista_nos) {
            dist[u->id][v->id] = (u->id == v->id) ? 0 : INT_MAX;
            next[u->id][v->id] = '\0';
        }
    }
    
    for (No* u : lista_nos) {
        for (Aresta* a : u->arestas) {
            dist[u->id][a->id_no_alvo] = a->peso;
            next[u->id][a->id_no_alvo] = a->id_no_alvo;
        }
    }
    
    for (No* k : lista_nos) {
        for (No* i : lista_nos) {
            for (No* j : lista_nos) {
                if (dist[i->id][k->id] != INT_MAX && dist[k->id][j->id] != INT_MAX &&
                    dist[i->id][j->id] > dist[i->id][k->id] + dist[k->id][j->id]) {
                    dist[i->id][j->id] = dist[i->id][k->id] + dist[k->id][j->id];
                    next[i->id][j->id] = next[i->id][k->id];
                }
            }
        }
    }
    
    std::vector<char> caminho;
    if (next.find(origem) == next.end() || next.at(origem).find(destino) == next.at(origem).end() || next.at(origem).at(destino) == '\0') return caminho;
    
    caminho.push_back(origem);
    char atual = origem;
    while (atual != destino) {
        atual = next[atual][destino];
        caminho.push_back(atual);
    }
    
    return caminho;
}

Grafo* Grafo::arvore_geradora_minima_prim() {
    // --- CHAMADA CORRIGIDA ---
    Grafo* agm = new Grafo(in_direcionado, true, false, ordem);
    
    if (lista_nos.empty()) return agm;

    std::priority_queue<std::pair<int, std::pair<char, char>>,
                       std::vector<std::pair<int, std::pair<char, char>>>,
                       std::greater<std::pair<int, std::pair<char, char>>>> fila;
    
    std::unordered_set<char> incluidos;

    for (No* no : lista_nos) {
        No* novoNo = new No(no->id);
        agm->lista_nos.push_back(novoNo);
        agm->mapa_nos[no->id] = novoNo;
    }

    if (lista_nos.empty()) return agm;

    char primeiro_no = lista_nos[0]->id;
    incluidos.insert(primeiro_no);

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

    // --- CHAMADA CORRIGIDA ---
    Grafo* agm = new Grafo(in_direcionado, true, false, ordem);
    if (lista_nos.empty()) return agm;

    std::vector<ArestaKruskal> arestas;
    std::unordered_map<char, char> parent;
    std::unordered_map<char, int> rank;

    for (No* no : lista_nos) {
        parent[no->id] = no->id;
        rank[no->id] = 0;
        
        No* novoNo = new No(no->id);
        agm->lista_nos.push_back(novoNo);
        agm->mapa_nos[no->id] = novoNo;

        for (Aresta* a : no->arestas) {
            if (!in_direcionado && no->id > a->id_no_alvo) continue;
            arestas.push_back({no->id, a->id_no_alvo, a->peso});
        }
    }

    std::sort(arestas.begin(), arestas.end());

    auto find = [&](char u) {
        while (parent[u] != u) {
            parent[u] = parent[parent[u]];
            u = parent[u];
        }
        return u;
    };

    for (const auto& a : arestas) {
        char rootU = find(a.u);
        char rootV = find(a.v);

        if (rootU != rootV) {
            No* noU = agm->encontrarNo(a.u);
            if (noU) {
                noU->adicionarAresta(a.v, a.peso);
                if (!in_direcionado) {
                    No* noV = agm->encontrarNo(a.v);
                    if (noV) noV->adicionarAresta(a.u, a.peso);
                }
                agm->numero_arestas++;
            }

            if (rank[rootU] > rank[rootV]) {
                parent[rootV] = rootU;
            } else {
                parent[rootU] = rootV;
                if (rank[rootU] == rank[rootV]) rank[rootV]++;
            }
        }

        if (agm->numero_arestas == ordem - 1) break;
    }

    if (agm->numero_arestas != ordem - 1) {
        delete agm;
        
        return new Grafo(in_direcionado, true, false, ordem);
    }

    return agm;
}

Grafo* Grafo::arvore_caminhamento_profundidade(char id_no) {
   
    Grafo* dfsTree = new Grafo(in_direcionado, false, false, ordem);
    std::unordered_map<char, bool> visitados;
    std::stack<char> pilha;
    
    for (No* no : lista_nos) {
        No* novoNo = new No(no->id);
        dfsTree->lista_nos.push_back(novoNo);
        dfsTree->mapa_nos[no->id] = novoNo;
    }
    
    if (encontrarNo(id_no) == nullptr) return dfsTree;

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
    int raio_val = INT_MAX;
    for (No* no : lista_nos) {
        std::unordered_map<char, int> distancias;
        std::queue<char> fila;
        
        for (No* n : lista_nos) {
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
                if(encontrarNo(a->id_no_alvo) && distancias[a->id_no_alvo] == -1) {
                    distancias[a->id_no_alvo] = distancias[atual] + 1;
                    ecc = std::max(ecc, distancias[a->id_no_alvo]);
                    fila.push(a->id_no_alvo);
                }
            }
        }
        raio_val = std::min(raio_val, ecc);
    }
    return raio_val;
}

int Grafo::diametro() {
    int diametro_val = 0;
    for (No* no : lista_nos) {
        std::unordered_map<char, int> distancias;
        std::queue<char> fila;
        
        for (No* n : lista_nos) {
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
                 if(encontrarNo(a->id_no_alvo) && distancias[a->id_no_alvo] == -1) {
                    distancias[a->id_no_alvo] = distancias[atual] + 1;
                    ecc = std::max(ecc, distancias[a->id_no_alvo]);
                    fila.push(a->id_no_alvo);
                }
            }
        }
        diametro_val = std::max(diametro_val, ecc);
    }
    return diametro_val;
}

std::vector<char> Grafo::centro() {
    std::vector<char> centros;
    int minEccentricity = INT_MAX;
    std::unordered_map<char, int> eccentricities;
    
    for (No* no : lista_nos) {
        std::unordered_map<char, int> distancias;
        std::queue<char> fila;
        
        for (No* n : lista_nos) {
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
                if (encontrarNo(a->id_no_alvo) && distancias[a->id_no_alvo] == -1) {
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
    
    for (No* no : lista_nos) {
        std::unordered_map<char, int> distancias;
        std::queue<char> fila;
        
        for (No* n : lista_nos) {
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
                if (encontrarNo(a->id_no_alvo) && distancias[a->id_no_alvo] == -1) {
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
    std::unordered_map<char, bool> visitado;
    int time = 0;
    
    for (No* no : lista_nos) {
        discovery[no->id] = 0;
        low[no->id] = 0;
        parent[no->id] = '\0';
        visitado[no->id] = false;
    }
    
    for (No* no : lista_nos) {
        if (!visitado[no->id]) {
            dfs_articulacao(no->id, discovery, low, parent, visitado, time, articulacoes);
        }
    }
    return articulacoes;
}

void Grafo::dfs_articulacao(char u, std::unordered_map<char, int>& discovery,
                           std::unordered_map<char, int>& low, std::unordered_map<char, char>& parent,
                           std::unordered_map<char, bool>& visitado, int& time,
                           std::vector<char>& articulationPoints) {
    visitado[u] = true;
    discovery[u] = low[u] = ++time;
    int children = 0;
    
    No* noU = encontrarNo(u);
    if (!noU) return;
    
    for (Aresta* a : noU->arestas) {
        char v = a->id_no_alvo;
        if (encontrarNo(v) == nullptr) continue;
        
        if (!visitado[v]) {
            children++;
            parent[v] = u;
            dfs_articulacao(v, discovery, low, parent, visitado, time, articulationPoints);
            
            low[u] = std::min(low[u], low[v]);
            
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