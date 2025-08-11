#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <iomanip>
#include <limits>
#include <chrono> 

#include "Gerenciador.h"
#include "Grafo.h"
#include "AlgoritmosGulosos.h"


char Gerenciador::get_id_entrada() {
    char id;
    std::cout << "Digite o ID do no: ";
    std::cin >> id;
    return id;
}

bool Gerenciador::pergunta_imprimir_arquivo(const std::string& nome_arquivo) {
    char resposta;
    std::cout << "Deseja salvar em " << nome_arquivo << "? (s/n): ";
    std::cin >> resposta;
    return tolower(resposta) == 's';
}

void imprimirListaAdjacencias(std::stringstream& resultado, Grafo* grafo) {
    std::vector<No*> nos_ordenados = grafo->lista_nos;

    std::sort(nos_ordenados.begin(), nos_ordenados.end(), 
        [](No* a, No* b) { return a->id < b->id; });

    for (No* no : nos_ordenados) {
        resultado << no->id << " (" << no->arestas.size() << " vizinhos): ";
        
        std::vector<Aresta*> arestas_ordenadas = no->arestas;
        std::sort(arestas_ordenadas.begin(), arestas_ordenadas.end(),
            [](Aresta* a, Aresta* b) { return a->id_no_alvo < b->id_no_alvo; });

        bool primeiro = true;
        for (Aresta* a : arestas_ordenadas) {
            if (!primeiro) resultado << ", ";
            resultado << a->id_no_alvo;
            if (grafo->in_ponderado_aresta) {
                resultado << "(" << a->peso << ")";
            }
            primeiro = false;
        }
        resultado << "\n";
    }
}

void imprimirVectorChar(std::stringstream& resultado, const std::vector<char>& vec) {
    if (vec.empty()) {
        resultado << "Nenhum resultado encontrado";
        return;
    }
    
    resultado << vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        resultado << "," << vec[i];
    }
}

void Gerenciador::comandos(Grafo* grafo) {
    while (true) {
        std::cout << "\nMENU PRINCIPAL:\n"
                  << "a - Fecho transitivo direto\n"
                  << "b - Fecho transitivo indireto\n"
                  << "c - Caminho minimo (Dijkstra)\n"
                  << "d - Caminho minimo (Floyd)\n"
                  << "e - AGM (Prim)\n"
                  << "f - AGM (Kruskal)\n"
                  << "g - Arvore DFS\n"
                  << "h - Metricas do grafo\n"
                  << "i - Vertices de articulacao\n"
                  << "p - Imprimir lista de adjacencias (DEBUG)\n"
                  << "\n--- ALGORITMOS PARA CONJUNTO 2-DOMINANTE ---\n"
                  << "j - Guloso \n"
                  << "k - Guloso Randomizado\n"
                  << "l - Guloso Randomizado Reativo\n"
                  << "\n0 - Sair\n"
                  << "Opcao: ";
        
        char opcao;
        std::cin >> opcao;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        std::stringstream resultado;
        std::string nome_arquivo;
        
        try {
            switch (opcao) {
                
                case 'a': {
                    char id = get_id_entrada();
                    std::vector<char> fecho = grafo->fecho_transitivo_direto(id);
                    resultado << "Fecho transitivo direto de " << id << ": ";
                    imprimirVectorChar(resultado, fecho);
                    nome_arquivo = "fecho_direto.txt";
                    break;
                }
                case 'b': {
                    char id = get_id_entrada();
                    std::vector<char> fecho = grafo->fecho_transitivo_indireto(id);
                    resultado << "Fecho transitivo indireto de " << id << ": ";
                    imprimirVectorChar(resultado, fecho);
                    nome_arquivo = "fecho_indireto.txt";
                    break;
                }
                case 'c': {
                     char origem = get_id_entrada();
                    char destino = get_id_entrada();
                    std::vector<char> caminho = grafo->caminho_minimo_dijkstra(origem, destino);
                    resultado << "Caminho minimo (Dijkstra): ";
                    imprimirVectorChar(resultado, caminho);
                    nome_arquivo = "dijkstra.txt";
                    break;
                }
                case 'd': {
                    char origem = get_id_entrada();
                    char destino = get_id_entrada();
                    std::vector<char> caminho = grafo->caminho_minimo_floyd(origem, destino);
                    resultado << "Caminho minimo (Floyd): ";
                    imprimirVectorChar(resultado, caminho);
                    nome_arquivo = "floyd.txt";
                    break;
                }
                case 'e': {
                    Grafo* agm = grafo->arvore_geradora_minima_prim();
                    resultado << "AGM (Prim):\n";
                    imprimirListaAdjacencias(resultado, agm);
                    delete agm;
                    nome_arquivo = "agm_prim.txt";
                    break;
                }
                case 'f': {
                    Grafo* agm = grafo->arvore_geradora_minima_kruskal();
                    resultado << "AGM (Kruskal):\n";
                    imprimirListaAdjacencias(resultado, agm);
                    delete agm;
                    nome_arquivo = "agm_kruskal.txt";
                    break;
                }
                case 'g': {
                    char id = get_id_entrada();
                    Grafo* dfs = grafo->arvore_caminhamento_profundidade(id);
                    resultado << "Arvore DFS a partir de " << id << ":\n";
                    imprimirListaAdjacencias(resultado, dfs);
                    delete dfs;
                    nome_arquivo = "dfs.txt";
                    break;
                }
                case 'h': {
                    resultado << "Métricas do grafo:\n";
                    resultado << "Raio: " << grafo->raio() << "\n";
                    resultado << "Diâmetro: " << grafo->diametro() << "\n";
                    std::vector<char> centro = grafo->centro();
                    resultado << "Centro: ";
                    imprimirVectorChar(resultado, centro);
                    std::vector<char> periferia = grafo->periferia();
                    resultado << "\nPeriferia: ";
                    imprimirVectorChar(resultado, periferia);
                    nome_arquivo = "metricas.txt";
                    break;
                }
                case 'i': {
                    std::vector<char> articulacoes = grafo->vertices_de_articulacao();
                    resultado << "Vértices de articulacao: ";
                    imprimirVectorChar(resultado, articulacoes);
                    nome_arquivo = "articulacoes.txt";
                    break;
                }
                 case 'p': {
                    resultado << "Estrutura do Grafo em Memoria:\n";
                    imprimirListaAdjacencias(resultado, grafo);
                    nome_arquivo = "debug_grafo.txt";
                    break;
                }


                case 'j': {
                    auto inicio = std::chrono::high_resolution_clock::now();
                    std::vector<char> D = AlgoritmosGulosos::algoritmoGuloso(grafo);
                    auto fim = std::chrono::high_resolution_clock::now();
                    auto duracao_ms = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count();

                    resultado << "Conjunto 2-dominante (Guloso):\n";
                    imprimirVectorChar(resultado, D);
                    resultado << "\nTamanho: " << D.size();
                    resultado << "\nTempo de execucano: " << duracao_ms << " ms";
                    nome_arquivo = "guloso.txt";
                    break;
                }
                case 'k': {
                    float alfa;
                    int iteracoes;
                    std::cout << "Digite o valor de alfa (0.0=guloso, 1.0=aleatorio): ";
                    std::cin >> alfa;
                    std::cout << "Digite o numero de iteracoes: ";
                    std::cin >> iteracoes;

                    auto inicio = std::chrono::high_resolution_clock::now();
                    std::vector<char> D = AlgoritmosGulosos::algoritmoGulosoRandomizado(grafo, alfa, iteracoes);
                    auto fim = std::chrono::high_resolution_clock::now();
                    auto duracao_ms = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count();

                    resultado << "Conjunto 2-dominante (Randomizado com alfa=" << alfa << "):\n";
                    imprimirVectorChar(resultado, D);
                    resultado << "\nTamanho: " << D.size();
                    resultado << "\nTempo de execucao: " << duracao_ms << " ms";
                    nome_arquivo = "guloso_randomizado.txt";
                    break;
                }
                case 'l': {
                    int iteracoes;
                    std::cout << "Digite o numero de iteracoes para o algoritmo reativo: ";
                    std::cin >> iteracoes;

                    auto inicio = std::chrono::high_resolution_clock::now();
                    std::vector<char> D = AlgoritmosGulosos::algoritmoGulosoRandomizadoReativo(grafo, iteracoes);
                    auto fim = std::chrono::high_resolution_clock::now();
                    auto duracao_ms = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count();

                    resultado << "Conjunto 2-dominante (Reativo):\n";
                    imprimirVectorChar(resultado, D);
                    resultado << "\nTamanho: " << D.size();
                    resultado << "\nTempo de execucao: " << duracao_ms << " ms";
                    nome_arquivo = "guloso_reativo.txt";
                    break;
                }
                case '0':
                    return;
                default:
                    std::cout << "Opcao invalida!\n";
                    continue;
            }
        } catch (const std::exception& e) {
            resultado << "Erro: " << e.what();
        }
        
        std::cout << "\nRESULTADO:\n" << resultado.str() << "\n";
        
        if (!nome_arquivo.empty() && pergunta_imprimir_arquivo(nome_arquivo)) {
            std::ofstream arquivo(nome_arquivo);
            if (arquivo) {
                arquivo << resultado.str();
                std::cout << "Resultado salvo em " << nome_arquivo << "\n";
            } else {
                std::cerr << "Erro ao salvar arquivo!\n";
            }
        }
    }
}