#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <utility> 
#include "Gerenciador.h"
#include "Grafo.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_grafo.txt>\n";
        return 1;
    }
    
    try {
        std::ifstream arquivo(argv[1]);
        if (!arquivo) {
            throw std::runtime_error("Erro ao abrir arquivo!");
        }
        
        std::string linha;

        // Ler as flags
        std::getline(arquivo, linha);
        std::stringstream ss_flags(linha);
        int flag1, flag2, flag3;
        ss_flags >> flag1 >> flag2 >> flag3;
        bool direcionado = (flag1 == 1);
        bool ponderado_aresta = (flag2 == 1);
        bool ponderado_vertice = (flag3 == 1);

        // Ler a ordem (número de vértices)
        int ordem = 0;
        arquivo >> ordem; // Lê o número diretamente do stream do arquivo

        // Ler os IDs dos vértices individualmente --- LÓGICA CORRIGIDA ---
        std::vector<char> ids_vertices;
        char id_vertice;
        for (int i = 0; i < ordem; ++i) {
            if (arquivo >> id_vertice) { // Lê o próximo caractere não-espaço
                ids_vertices.push_back(id_vertice);
            }
        }
        
        // Ler as arestas
        std::vector<std::pair<char, char>> arestas_pares;
        char origem, destino;
        while (arquivo >> origem >> destino) { // Esta forma de leitura é robusta
            arestas_pares.push_back({origem, destino});
        }
        
        // Criar o Grafo com os dados processados
        std::unique_ptr<Grafo> grafo(new Grafo(direcionado, ponderado_aresta, ponderado_vertice, ids_vertices, arestas_pares));
        
        Gerenciador::comandos(grafo.get());
        
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}