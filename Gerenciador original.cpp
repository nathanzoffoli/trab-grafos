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

std::vector<char> Gerenciador::get_conjunto_ids(Grafo* grafo, int tam) {
    std::vector<char> ids;
    while (ids.size() < static_cast<size_t>(tam)) {
        char id = get_id_entrada();
        bool existe = false;
        
        for (No* no : grafo->lista_adj) {
            if (no->id == id) {
                existe = true;
                break;
            }
        }
        
        if (!existe) {
            std::cout << "ID invalido! Tente novamente.\n";
            continue;
        }
        
        if (std::find(ids.begin(), ids.end(), id) == ids.end()) {
            ids.push_back(id);
        } else {
            std::cout << "ID ja selecionado! Tente outro.\n";
        }
    }
    return ids;
}

bool Gerenciador::pergunta_imprimir_arquivo(std::string nome_arquivo) {
    int resposta;
    std::cout << "Deseja salvar em " << nome_arquivo << "? (1-Sim/2-Nao): ";
    std::cin >> resposta;
    return resposta == 1;
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
                imprimir_vetor(resultado, fecho, "Fecho transitivo direto de " + std::string(1, id));
                nome_arquivo = "fecho_direto.txt";
                break;
            }
            case 'b': {
                char id = get_id_entrada();
                std::vector<char> fecho = grafo->fecho_transitivo_indireto(id);
                imprimir_vetor(resultado, fecho, "Fecho transitivo indireto de " + std::string(1, id));
                nome_arquivo = "fecho_indireto.txt";
                break;
            }
            case 'c': {
                char origem = get_id_entrada();
                char destino = get_id_entrada();
                std::vector<char> caminho = grafo->caminho_minimo_dijkstra(origem, destino);
                
                if (caminho.empty()) {
                    resultado << "Nao existe caminho entre " << origem << " e " << destino << "\n";
                } else {
                    imprimir_vetor(resultado, caminho, "Caminho mínimo (Dijkstra)");
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
                        resultado << "Peso total: " << peso_total << "\n";
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
                    resultado << "Nao existe caminho entre " << origem << " e " << destino << "\n";
                } else {
                    imprimir_vetor(resultado, caminho, "Caminho mínimo (Floyd)");
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
                        resultado << "Peso total: " << peso_total << "\n";
                    }
                }
                nome_arquivo = "floyd.txt";
                break;
            }
            case 'e': {
                int tam;
                std::cout << "Tamanho do subconjunto: ";
                std::cin >> tam;
                
                if (tam <= 0 || tam > grafo->ordem) {
                    std::cout << "Tamanho invalido!\n";
                    continue;
                }
                
                std::vector<char> vertices = get_conjunto_ids(grafo, tam);
                Grafo* agm = grafo->arvore_geradora_minima_prim(vertices);
                
                resultado << "AGM (Prim) para os vertices:\n";
                imprimir_vetor(resultado, vertices, "Vertices selecionados");
                imprimir_grafo(resultado, agm, "Arvore Geradora Mínima");
                
                delete agm;
                nome_arquivo = "agm_prim.txt";
                break;
            }
            case 'f': {
                int tam;
                std::cout << "Tamanho do subconjunto: ";
                std::cin >> tam;
                
                if (tam <= 0 || tam > grafo->ordem) {
                    std::cout << "Tamanho invalido!\n";
                    continue;
                }
                
                std::vector<char> vertices = get_conjunto_ids(grafo, tam);
                Grafo* agm = grafo->arvore_geradora_minima_kruskal(vertices);
                
                resultado << "AGM (Kruskal) para os vertices:\n";
                imprimir_vetor(resultado, vertices, "Vertices selecionados");
                imprimir_grafo(resultado, agm, "Arvore Geradora Mínima");
                
                delete agm;
                nome_arquivo = "agm_kruskal.txt";
                break;
            }
            case 'g': {
                char id = get_id_entrada();
                Grafo* dfs = grafo->arvore_caminhamento_profundidade(id);
                
                resultado << "Arvore DFS a partir de " << id << ":\n";
                imprimir_grafo(resultado, dfs);
                
                delete dfs;
                nome_arquivo = "dfs.txt";
                break;
            }
            case 'h': {
                resultado << "Metricas do grafo:\n";
                resultado << "Raio: " << grafo->raio() << "\n";
                resultado << "Diametro: " << grafo->diametro() << "\n";
                
                std::vector<char> centro = grafo->centro();
                imprimir_vetor(resultado, centro, "Centro");
                
                std::vector<char> periferia = grafo->periferia();
                imprimir_vetor(resultado, periferia, "Periferia");
                
                nome_arquivo = "metricas.txt";
                break;
            }
            case 'i': {
                std::vector<char> articulacoes = grafo->vertices_de_articulacao();
                imprimir_vetor(resultado, articulacoes, "Vertices de articulacao");
                nome_arquivo = "articulacoes.txt";
                break;
            }
            case '0':
                return;
            default:
                std::cout << "Opçao invalida!\n";
                continue;
        }
        
        std::cout << "\nRESULTADO:\n" << resultado.str();
        if (pergunta_imprimir_arquivo(nome_arquivo)) {
            std::ofstream arquivo(nome_arquivo);
            arquivo << resultado.str();
            std::cout << "Salvo em " << nome_arquivo << "\n";
        }
    }
}