#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype> 
using namespace std;

// Classe Lista
template <typename T>
class Lista {
protected:
    vector<T> elementos;

public:
    void inserir(const T& elemento) {
        elementos.push_back(elemento);
    }

    size_t tamanho() const {
        return elementos.size();
    }

    T& operator[](size_t index) {
        return elementos[index];
    }

    void imprimir() const {
        for (const auto& elemento : elementos) {
            cout << elemento << endl;
        }
    }
};

// Classe ListaOrdenada
template <typename T>
class ListaOrdenada : public Lista<T> {
public:
    void ordenar() {
        sort(this->elementos.begin(), this->elementos.end());
    }

    void ordenarPorFuncao(bool (*funcaoComparacao)(const T&, const T&)) {
        sort(this->elementos.begin(), this->elementos.end(), funcaoComparacao);
    }
};

// Classe Termo
class Termo {
private:
    string termo;
    long peso;

public:
    Termo() : termo(""), peso(0) {}
    Termo(string termo, long peso) : termo(termo), peso(peso) {}

    string getTermo() const { return termo; }
    long getPeso() const { return peso; }

    static bool compararPorPeso(const Termo& t1, const Termo& t2) {
        return t1.peso > t2.peso;
    }

    bool operator<(const Termo& outro) const {
        return termo < outro.termo;
    }

    friend ostream& operator<<(ostream& out, const Termo& t) {
        out << t.peso << "\t" << t.termo;
        return out;
    }
};

// Classe Autocompletar
class Autocompletar {
private:
    ListaOrdenada<Termo> termos;

    int buscaBinaria(const string& prefixo) {
        int inicio = 0, fim = termos.tamanho() - 1;
        while (inicio <= fim) {
            int meio = (inicio + fim) / 2;
            string termoAtual = termos[meio].getTermo().substr(0, prefixo.size());

            transform(termoAtual.begin(), termoAtual.end(), termoAtual.begin(), ::tolower);
            string prefixoLower = prefixo;
            transform(prefixoLower.begin(), prefixoLower.end(), prefixoLower.begin(), ::tolower);

            if (termoAtual < prefixoLower)
                inicio = meio + 1;
            else if (termoAtual > prefixoLower)
                fim = meio - 1;
            else
                return meio;
        }
        return -1;
    }

public:
    void carregarTermos(const string& arquivo) {
        ifstream inFile(arquivo);
        string linha;

        while (getline(inFile, linha)) {
            linha.erase(0, linha.find_first_not_of(" \t")); // Remover espaços no início
            linha.erase(linha.find_last_not_of(" \t") + 1);

            if (linha.empty()) continue;

            size_t tabPos = linha.find('\t');
            if (tabPos == string::npos) continue;

            try {
                long peso = stol(linha.substr(0, tabPos));
                string termo = linha.substr(tabPos + 1);

                if (!termo.empty()) {
                    termos.inserir(Termo(termo, peso));
                }
            } catch (const exception& e) {
                continue; // Ignorar linhas inválidas
            }
        }

        termos.ordenar();
    }

    vector<Termo> procurar(const string& prefixo, int k) {
        int indice = buscaBinaria(prefixo);
        if (indice == -1) return {};

        vector<Termo> resultados;
        int r = prefixo.size();

        for (int i = indice; i < termos.tamanho(); ++i) {
            string termoAtual = termos[i].getTermo().substr(0, r);
            transform(termoAtual.begin(), termoAtual.end(), termoAtual.begin(), ::tolower);
            string prefixoLower = prefixo;
            transform(prefixoLower.begin(), prefixoLower.end(), prefixoLower.begin(), ::tolower);

            if (termoAtual == prefixoLower)
                resultados.push_back(termos[i]);
            else
                break;
        }

        sort(resultados.begin(), resultados.end(), Termo::compararPorPeso);

        if ((int)resultados.size() > k) resultados.resize(k);
        return resultados;
    }
};

// Função principal
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <arquivo> <k>\n";
        return 1;
    }

    string arquivo = argv[1];
    int k = stoi(argv[2]);

    Autocompletar autocompletar;
    autocompletar.carregarTermos(arquivo);

    while (true) {
        string prefixo;
        cout << "Entre com o termo a ser auto-completado (digite \"sair\" para encerrar): ";
        getline(cin, prefixo);

        if (prefixo == "sair") break;

        vector<Termo> resultados = autocompletar.procurar(prefixo, k);
        for (const auto& termo : resultados) {
            cout << termo << endl;
        }
    }

    return 0;
}
