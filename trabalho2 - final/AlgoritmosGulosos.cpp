#include "AlgoritmosGulosos.h"
#include "Grafo.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <numeric>
#include <map>

// --- 1. ALGORITMO GULOSO (Inalterado) ---
// Esta função permanece exatamente como na resposta anterior.
// --- 1. ALGORITMO GULOSO CORRETO E DETERMINÍSTICO ---
std::vector<char> AlgoritmosGulosos::algoritmoGuloso(Grafo* grafo) {
    std::unordered_set<char> solucao;
    std::unordered_set<char> dominados;
    std::unordered_map<char, int> contagemDominacao;
    
    std::vector<No*> nos_ordenados = grafo->lista_nos;

    for(No* no : nos_ordenados){
        contagemDominacao[no->id] = 0;
    }

    // --- MUDANÇA PRINCIPAL AQUI: REGRA DE DESEMPATE ---
    // Ordena por grau e, em caso de empate, por ID (ordem alfabética).
    std::sort(nos_ordenados.begin(), nos_ordenados.end(), [](No* a, No* b) {
        if (a->arestas.size() != b->arestas.size()) {
            return a->arestas.size() < b->arestas.size();
        }
        return a->id < b->id; // Regra de desempate
    });

    for (No* no_candidato : nos_ordenados) {
        if (solucao.size() + dominados.size() == grafo->ordem) {
            break;
        }

        char id_candidato = no_candidato->id;

        if (solucao.count(id_candidato)) {
            continue;
        }

        if (contagemDominacao[id_candidato] < 2) {
            solucao.insert(id_candidato);

            if(dominados.count(id_candidato)){
                dominados.erase(id_candidato);
            }

            for (Aresta* aresta : no_candidato->arestas) {
                char id_vizinho = aresta->id_no_alvo;
                contagemDominacao[id_vizinho]++;
                if (contagemDominacao[id_vizinho] == 2 && !solucao.count(id_vizinho)) {
                    dominados.insert(id_vizinho);
                }
            }
        }
    }

    std::vector<char> resultado(solucao.begin(), solucao.end());
    std::sort(resultado.begin(), resultado.end());
    return resultado;
}


// --- LÓGICA AUXILIAR COM A NOVA ESTRUTURA RANDOMIZADA ---
std::vector<char> construirSolucaoAlfa(Grafo* grafo, float alfa) {
    std::unordered_set<char> solucao;
    std::unordered_set<char> dominados;
    std::unordered_map<char, int> contagemDominacao;

    
    std::vector<No*> candidatos = grafo->lista_nos;
    for(No* no : candidatos) {
        contagemDominacao[no->id] = 0;
    }
    
    // Ordenar os candidatos por grau,
    std::sort(candidatos.begin(), candidatos.end(), [](No* a, No* b){
        return a->arestas.size() < b->arestas.size();
    });

    std::mt19937 g(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    while(solucao.size() + dominados.size() < grafo->ordem && !candidatos.empty()){
        // Criar a Lista de Candidatos Restrita (RCL)
        int grau_min = candidatos.front()->arestas.size();
        int grau_max = candidatos.back()->arestas.size();
        float limite_grau = grau_min + alfa * (grau_max - grau_min);
        
        std::vector<No*> rcl;
        for(No* no_candidato : candidatos){
            if(no_candidato->arestas.size() <= limite_grau){
                rcl.push_back(no_candidato);
            } else {
                break; // Como a lista está ordenada, podemos parar aqui
            }
        }

        if(rcl.empty()) break;

        // Sortear um candidato da RCL
        std::uniform_int_distribution<> dist(0, rcl.size() - 1);
        No* no_escolhido = rcl[dist(g)];
        char id_escolhido = no_escolhido->id;
        
        // Remover o escolhido da lista de candidatos para não ser pego de novo
        candidatos.erase(std::remove_if(candidatos.begin(), candidatos.end(), [id_escolhido](No* n){
            return n->id == id_escolhido;
        }), candidatos.end());

        // Processar o nó escolhido 
        if (!solucao.count(id_escolhido) && contagemDominacao[id_escolhido] < 2) {
             solucao.insert(id_escolhido);
            if(dominados.count(id_escolhido)) dominados.erase(id_escolhido);

            for (Aresta* aresta : no_escolhido->arestas) {
                char id_vizinho = aresta->id_no_alvo;
                contagemDominacao[id_vizinho]++;
                if (contagemDominacao[id_vizinho] == 2 && !solucao.count(id_vizinho)) {
                    dominados.insert(id_vizinho);
                }
            }
        }
    }

    std::vector<char> resultado(solucao.begin(), solucao.end());
    std::sort(resultado.begin(), resultado.end());
    return resultado;
}


// --- 2. ALGORITMO GULOSO RANDOMIZADO ---
std::vector<char> AlgoritmosGulosos::algoritmoGulosoRandomizado(Grafo* grafo, float alfa, int maxIteracoes) {
    std::vector<char> melhorSolucao;

    for (int i = 0; i < maxIteracoes; ++i) {
        std::vector<char> solucaoAtual = construirSolucaoAlfa(grafo, alfa);
        if (melhorSolucao.empty() || solucaoAtual.size() < melhorSolucao.size()) {
            melhorSolucao = solucaoAtual;
        }
    }
    return melhorSolucao;
}


// --- 3. ALGORITMO GULOSO RANDOMIZADO REATIVO (com alfa) ---
std::vector<char> AlgoritmosGulosos::algoritmoGulosoRandomizadoReativo(Grafo* grafo, int maxIteracoes) {
    std::vector<char> melhorSolucaoGeral;
    size_t menorTamanhoGeral = -1;

    std::vector<float> alfas = { 0.2, 0.5, 0.8}; 
    std::vector<double> probabilidades(alfas.size(), 1.0 / alfas.size());
    std::vector<double> qualidadeMedia(alfas.size(), 0.0);
    std::vector<int> contadorUso(alfas.size(), 0);

    std::mt19937 g(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    
    for (int i = 0; i < maxIteracoes; ++i) {
        std::discrete_distribution<> dist(probabilidades.begin(), probabilidades.end());
        int indice_alfa = dist(g);
        float alfa_escolhido = alfas[indice_alfa];

        std::vector<char> solucaoAtual = construirSolucaoAlfa(grafo, alfa_escolhido);

        if (melhorSolucaoGeral.empty() || solucaoAtual.size() < menorTamanhoGeral) {
            melhorSolucaoGeral = solucaoAtual;
            menorTamanhoGeral = solucaoAtual.size();
        }

        if(solucaoAtual.size() > 0){
            double qualidade = (menorTamanhoGeral > 0) ? static_cast<double>(menorTamanhoGeral) / solucaoAtual.size() : 1.0;
            qualidadeMedia[indice_alfa] = (qualidadeMedia[indice_alfa] * contadorUso[indice_alfa] + qualidade) / (contadorUso[indice_alfa] + 1);
            contadorUso[indice_alfa]++;
        }

        if (i > 0 && i % 30 == 0) {
            double somaQualidade = 0;
            for(double q : qualidadeMedia) somaQualidade += q;

            if(somaQualidade > 0){
                for(size_t j = 0; j < alfas.size(); ++j){
                    probabilidades[j] = qualidadeMedia[j] / somaQualidade;
                }
            }
        }
    }
    
    return melhorSolucaoGeral;
}