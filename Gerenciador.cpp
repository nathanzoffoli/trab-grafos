#include "Gerenciador.h"
#include <fstream>
#include <sstream>
#include <algorithm>

char Gerenciador::get_id_entrada() {
    char id;
    std::cout << "Digite o ID do nó: ";
    std::cin >> id;
    return id;
}

bool Gerenciador::pergunta_imprimir_arquivo(std::string nome_arquivo) {
    int resposta;
    std::cout << "Deseja salvar em " << nome_arquivo << "? (1-Sim/2-Nao): ";
    std::cin >> resposta;
    return resposta == 1;
}

void imprimirListaAdjacencias(std::stringstream& resultado, Grafo* grafo) {
    // Ordena os nós por ID para impressão consistente
    std::vector<No*> nos_ordenados = grafo->lista_adj;
    std::sort(nos_ordenados.begin(), nos_ordenados.end(), 
        [](No* a, No* b) { return a->id < b->id; });

    for (No* no : nos_ordenados) {
        resultado << no->id << ": ";
        
        // Ordena as arestas por ID de destino para impressão consistente
        std::vector<Aresta*> arestas_ordenadas = no->arestas;
        std::sort(arestas_ordenadas.begin(), arestas_ordenadas.end(),
            [](Aresta* a, Aresta* b) { return a->id_no_alvo < b->id_no_alvo; });

        bool primeiro = true;
        for (Aresta* a : arestas_ordenadas) {
            if (!primeiro) resultado << " -> ";
            resultado << a->id_no_alvo;
            primeiro = false;
        }
        resultado << "\n";
    }
}

void imprimirVectorChar(std::stringstream& resultado, const std::vector<char>& vec) {
    if (vec.empty()) return;
    
    resultado << vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        resultado << "," << vec[i];
    }
}

void Gerenciador::comandos(Grafo* grafo) {
    while (true) {
        std::cout << "\nMENU:\n"
                  << "a - Fecho transitivo direto\n"
                  << "b - Fecho transitivo indireto\n"
                  << "c - Caminho minimo (Dijkstra)\n"
                  << "d - Caminho minimo (Floyd)\n"
                  << "e - AGM (Prim)\n"
                  << "f - AGM (Kruskal)\n"
                  << "g - Arvore DFS\n"
                  << "h - Metricas do grafo\n"
                  << "i - Vertices de articulacao\n"
                  << "0 - Sair\n"
                  << "Opcao: ";
        
        char opcao;
        std::cin >> opcao;
        
        std::stringstream resultado;
        std::string nome_arquivo;
        
        switch (opcao) {
            case 'a': {
                char id = get_id_entrada();
                std::vector<char> fecho = grafo->fecho_transitivo_direto(id);
                imprimirVectorChar(resultado, fecho);
                nome_arquivo = "fecho_direto.txt";
                break;
            }
            case 'b': {
                char id = get_id_entrada();
                std::vector<char> fecho = grafo->fecho_transitivo_indireto(id);
                imprimirVectorChar(resultado, fecho);
                nome_arquivo = "fecho_indireto.txt";
                break;
            }
            case 'c': {
                char origem = get_id_entrada();
                char destino = get_id_entrada();
                std::vector<char> caminho = grafo->caminho_minimo_dijkstra(origem, destino);
                
                if (caminho.empty()) {
                    resultado << "Nao existe caminho entre " << origem << " e " << destino;
                } else {
                    imprimirVectorChar(resultado, caminho);
                    if (grafo->in_ponderado_aresta) {
                        int peso_total = 0;
                        for (size_t i = 0; i < caminho.size() - 1; i++) {
                            No* no = grafo->encontrarNo(caminho[i]);
                            for (Aresta* a : no->arestas) {
                                if (a->id_no_alvo == caminho[i+1]) {
                                    peso_total += a->peso;
                                    break;
                                }
                            }
                        }
                        resultado << "\n" << peso_total;
                    }
                }
                nome_arquivo = "dijkstra.txt";
                break;
            }
            case 'd': {
                char origem = get_id_entrada();
                char destino = get_id_entrada();
                std::vector<char> caminho = grafo->caminho_minimo_floyd(origem, destino);
                
                if (caminho.empty()) {
                    resultado << "Nao existe caminho entre " << origem << " e " << destino;
                } else {
                    imprimirVectorChar(resultado, caminho);
                    if (grafo->in_ponderado_aresta) {
                        int peso_total = 0;
                        for (size_t i = 0; i < caminho.size() - 1; i++) {
                            No* no = grafo->encontrarNo(caminho[i]);
                            for (Aresta* a : no->arestas) {
                                if (a->id_no_alvo == caminho[i+1]) {
                                    peso_total += a->peso;
                                    break;
                                }
                            }
                        }
                        resultado << "\n" << peso_total;
                    }
                }
                nome_arquivo = "floyd.txt";
                break;
            }
            case 'e': {
                if (grafo->lista_adj.empty()) {
                    resultado << "Grafo vazio!";
                } else {
                    Grafo* agm = grafo->arvore_geradora_minima_prim();
                    if (agm->numero_arestas == 0) {
                        resultado << "Grafo nao conexo! Nao existe AGM.";
                    } else {
                        imprimirListaAdjacencias(resultado, agm);
                    }
                    delete agm;
                }
                nome_arquivo = "agm_prim.txt";
                break;
            }
            case 'f': {
                if (grafo->lista_adj.empty()) {
                    resultado << "Grafo vazio!";
                } else {
                    Grafo* agm = grafo->arvore_geradora_minima_kruskal();
                    if (agm->numero_arestas == 0) {
                        resultado << "Grafo nao conexo! Nao existe AGM.";
                    } else {
                        imprimirListaAdjacencias(resultado, agm);
                    }
                    delete agm;
                }
                nome_arquivo = "agm_kruskal.txt";
                break;
            }
            case 'g': {
    char id = get_id_entrada();
    Grafo* dfs = grafo->arvore_caminhamento_profundidade(id);
    
    // Verifica se o nó inicial existe
    if (grafo->encontrarNo(id) == nullptr) {
        resultado << "Nó inicial não encontrado!";
    } else {
        // Filtra apenas nós alcançáveis
        std::vector<char> alcancaveis = grafo->fecho_transitivo_direto(id);
        alcancaveis.push_back(id); // Inclui o nó inicial
        
        // Imprime apenas nós alcançáveis
        std::stringstream temp;
        imprimirListaAdjacencias(temp, dfs);
        
        // Filtra as linhas para mostrar apenas nós alcançáveis
        std::string linha;
        while (std::getline(temp, linha)) {
            if (linha.empty()) continue;
            char no_id = linha[0];
            if (std::find(alcancaveis.begin(), alcancaveis.end(), no_id) != alcancaveis.end()) {
                resultado << linha << "\n";
            }
        }
    }
    
    delete dfs;
    nome_arquivo = "dfs.txt";
    break;
}
            case 'h': {
                resultado << grafo->raio() << "\n";
                resultado << grafo->diametro() << "\n";
                
                std::vector<char> centro = grafo->centro();
                imprimirVectorChar(resultado, centro);
                resultado << "\n";
                
                std::vector<char> periferia = grafo->periferia();
                imprimirVectorChar(resultado, periferia);
                
                nome_arquivo = "metricas.txt";
                break;
            }
            case 'i': {
                std::vector<char> articulacoes = grafo->vertices_de_articulacao();
                imprimirVectorChar(resultado, articulacoes);
                nome_arquivo = "articulacoes.txt";
                break;
            }
            case '0':
                return;
            default:
                std::cout << "Opçao invalida!\n";
                continue;
        }
        
        std::cout << "\nRESULTADO:\n" << resultado.str() << "\n";
        if (pergunta_imprimir_arquivo(nome_arquivo)) {
            std::ofstream arquivo(nome_arquivo);
            arquivo << resultado.str();
            std::cout << "Salvo em " << nome_arquivo << "\n";
        }
    }
}