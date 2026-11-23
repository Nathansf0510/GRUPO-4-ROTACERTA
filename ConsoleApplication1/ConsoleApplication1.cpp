#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;
struct Encomenda {
    string codigo;
    string destino;
    double peso;
    double volume;
    bool prioritaria;
    bool precisaRefrigeracao;
    int ordemCadastro; // Usado para manter estabilidade e desempate
};

struct Veiculo {
    string destinoAtendido;
    double capPeso;
    double capVolume;
    bool temRefrigeracao;
};

struct EncomendaRejeitada {
    Encomenda encomenda;
    string motivo;
};

struct ResultadoPlano {
    string nomePlano;
    vector<Encomenda> selecionadas;
    vector<EncomendaRejeitada> pendentes;
    double pesoRestante;
    double volumeRestante;
    int qtdPrioritariasAtendidas;
};
// Funções auxiliares de leitura segura sem <limits>
void limparBuffer() {
    cin.clear();
    char c;
    while (cin.get(c)) {
        if (c == '\n') break;
    }
}

double lerDoublePositivo(const string& mensagem) {
    double valor;
    while (true) {
        cout << mensagem;
        if (cin >> valor && valor > 0) {
            limparBuffer();
            return valor;
        }
        cout << "[Erro] Digite um valor numerico maior que zero.\n";
        limparBuffer();
    }
}

bool lerSimNao(const string& mensagem) {
    char opcao;
    while (true) {
        cout << mensagem << " (S/N): ";
        if (cin >> opcao) {
            limparBuffer();
            opcao = static_cast<char>(toupper(static_cast<unsigned char>(opcao)));
            if (opcao == 'S') return true;
            if (opcao == 'N') return false;
        }
        cout << "[Erro] Responda apenas com S ou N.\n";
    }
}

bool codigoExiste(const vector<Encomenda>& lista, const string& codigo) {
    for (const auto& e : lista) {
        if (e.codigo == codigo) return true;
    }
    return false;
}

// Requisito 1: Cadastro de encomenda
void cadastrarEncomenda(vector<Encomenda>& encomendas, int& contadorOrdem) {
    Encomenda nova;

    while (true) {
        cout << "Codigo unico da encomenda: ";
        getline(cin, nova.codigo);
        if (nova.codigo.empty()) {
            cout << "[Erro] O codigo nao pode ser vazio.\n";
        } else if (codigoExiste(encomendas, nova.codigo)) {
            cout << "[Erro] Ja existe uma encomenda com este codigo.\n";
        } else {
            break;
        }
    }
    do {
        cout << "Destino: ";
        getline(cin, nova.destino);
        if (nova.destino.empty()) cout << "[Erro] O destino nao pode ser vazio.\n";
    } while (nova.destino.empty());

    nova.peso = lerDoublePositivo("Peso (kg): ");
    nova.volume = lerDoublePositivo("Volume (m3): ");
    nova.prioritaria = lerSimNao("Encomenda prioritaria?");
    nova.precisaRefrigeracao = lerSimNao("Precisa de refrigeracao?");
    nova.ordemCadastro = ++contadorOrdem;

    encomendas.push_back(nova);
    cout << ">> Encomenda [" << nova.codigo << "] cadastrada com sucesso!\n";
}

// Requisito 3 e 4: Simulação de carregamento para um plano
ResultadoPlano executarPlano(
    const string& nomePlano,
    const vector<Encomenda>& todasEncomendas,
    const Veiculo& veiculo,
    bool ordenarPorMenorVolume
) {
    ResultadoPlano resultado;
    resultado.nomePlano = nomePlano;
    resultado.pesoRestante = veiculo.capPeso;
    resultado.volumeRestante = veiculo.capVolume;
    resultado.qtdPrioritariasAtendidas = 0;

    // Filtra apenas as encomendas do destino atendido pelo veículo
    vector<Encomenda> candidatas;
    for (const auto& e : todasEncomendas) {
        if (e.destino == veiculo.destinoAtendido) {
            candidatas.push_back(e);
        }
    }

    // Ordenação conforme regras do Plano A e Plano B
    sort(candidatas.begin(), candidatas.end(), [ordenarPorMenorVolume](const Encomenda& a, const Encomenda& b) {
        // 1. Prioritárias sempre vêm primeiro em ambos os planos
        if (a.prioritaria != b.prioritaria) {
            return a.prioritaria > b.prioritaria;
        }
        // 2. Critério específico dentro da mesma categoria
        if (ordenarPorMenorVolume) {
            // Plano B: menor volume primeiro; desempate pela ordem de cadastro
            if (a.volume != b.volume) {
                return a.volume < b.volume;
            }
            return a.ordemCadastro < b.ordemCadastro;
        } else {
            // Plano A: ordem de cadastro
            return a.ordemCadastro < b.ordemCadastro;
        }
    });

    // Avaliação de restrições (Requisito 4)
    const double EPS = 1e-9;

    for (const auto& e : candidatas) {
        if (e.precisaRefrigeracao && !veiculo.temRefrigeracao) {
            resultado.pendentes.push_back({e, "Falta de refrigeracao"});
        } else if (e.peso > resultado.pesoRestante + EPS) {
            resultado.pendentes.push_back({e, "Excesso de peso"});
        } else if (e.volume > resultado.volumeRestante + EPS) {
            resultado.pendentes.push_back({e, "Excesso de volume"});
        } else {
            // Encomenda aprovada
            resultado.selecionadas.push_back(e);
            resultado.pesoRestante -= e.peso;
            resultado.volumeRestante -= e.volume;
            if (resultado.pesoRestante < EPS) resultado.pesoRestante = 0.0;
            if (resultado.volumeRestante < EPS) resultado.volumeRestante = 0.0;
            if (e.prioritaria) {
                resultado.qtdPrioritariasAtendidas++;
            }
        }
    }

    return resultado;
}

// Requisito 5: Exibição detalhada de um plano sem <iomanip>
void exibirResultadoPlano(const ResultadoPlano& plano) {
    cout << "\n======================================================\n";
    cout << " " << plano.nomePlano << "\n";
    cout << "======================================================\n";

    cout << ">> ENCOMENDAS SELECIONADAS (" << plano.selecionadas.size() << "):\n";
    if (plano.selecionadas.empty()) {
        cout << "   Nenhuma encomenda selecionada.\n";
    } else {
        for (const auto& e : plano.selecionadas) {
            cout << "   - [" << e.codigo << "] "
                 << (e.prioritaria ? "[PRIORITARIA] " : "[Normal]      ")
                 << "Peso: " << e.peso << " kg | Vol: " << e.volume << " m3 | "
                 << (e.precisaRefrigeracao ? "Refrig: Sim" : "Refrig: Nao") << "\n";
        }
    }

    cout << "\n>> ENCOMENDAS PENDENTES / NAO ATENDIDAS (" << plano.pendentes.size() << "):\n";
    if (plano.pendentes.empty()) {
        cout << "   Nenhuma encomenda pendente para este destino.\n";
    } else {
        for (const auto& item : plano.pendentes) {
            cout << "   - [" << item.encomenda.codigo << "] "
                 << (item.encomenda.prioritaria ? "[PRIORITARIA] " : "[Normal]      ")
                 << "Peso: " << item.encomenda.peso << " kg | Vol: " << item.encomenda.volume << " m3 "
                 << "-> Motivo: " << item.motivo << "\n";
        }
    }

    cout << "\n>> RESUMO DO VEICULO APOS CARREGAMENTO:\n";
    cout << "   Prioritarias atendidas : " << plano.qtdPrioritariasAtendidas << "\n";
    cout << "   Total atendidas        : " << plano.selecionadas.size() << "\n";
    cout << "   Peso restante          : " << plano.pesoRestante << " kg\n";
    cout << "   Volume restante        : " << plano.volumeRestante << " m3\n";
}

// Requisito 5: Recomendação entre Plano A e Plano B
void exibirRecomendacao(const ResultadoPlano& planoA, const ResultadoPlano& planoB) {
    cout << "\n******************************************************\n";
    cout << " RECOMENDACAO DO SISTEMA\n";
    cout << "******************************************************\n";

    if (planoA.qtdPrioritariasAtendidas > planoB.qtdPrioritariasAtendidas) {
        cout << "Recomendado: PLANO A (atendeu mais encomendas prioritarias: "
             << planoA.qtdPrioritariasAtendidas << " vs " << planoB.qtdPrioritariasAtendidas << ").\n";
    } else if (planoB.qtdPrioritariasAtendidas > planoA.qtdPrioritariasAtendidas) {
        cout << "Recomendado: PLANO B (atendeu mais encomendas prioritarias: "
             << planoB.qtdPrioritariasAtendidas << " vs " << planoA.qtdPrioritariasAtendidas << ").\n";
    } else {
        // Empate em prioritárias -> desempate pelo total de encomendas
        if (planoA.selecionadas.size() > planoB.selecionadas.size()) {
            cout << "Recomendado: PLANO A (empate em prioritarias, mas atendeu mais encomendas no total: "
                 << planoA.selecionadas.size() << " vs " << planoB.selecionadas.size() << ").\n";
        } else if (planoB.selecionadas.size() > planoA.selecionadas.size()) {
            cout << "Recomendado: PLANO B (empate em prioritarias, mas atendeu mais encomendas no total: "
                 << planoB.selecionadas.size() << " vs " << planoA.selecionadas.size() << ").\n";
        } else {
            cout << "Os planos sao EQUIVALENTES (mesma quantidade de prioritarias e de encomendas totais).\n";
        }
    }
    cout << "******************************************************\n";
}

// Carrega o cenário exato do Teste de Aceitação
void executarTesteAceitacao(vector<Encomenda>& encomendas, int& contadorOrdem) {
    encomendas.clear();
    contadorOrdem = 0;

    encomendas.push_back({"P1", "Centro", 50.0, 0.70, true, false, ++contadorOrdem});
    encomendas.push_back({"P2", "Centro", 30.0, 0.45, true, false, ++contadorOrdem});
    encomendas.push_back({"P3", "Centro", 30.0, 0.45, true, false, ++contadorOrdem});

    Veiculo v{"Centro", 100.0, 1.0, false};

    cout << "\n[TESTE DE ACEITACAO CARREGADO]\n";
    cout << "Veiculo: Destino = Centro | Cap. Peso = 100 kg | Cap. Volume = 1 m3 | Refrig = Nao\n";
    cout << "Encomendas: P1 (50kg, 0.70m3), P2 (30kg, 0.45m3), P3 (30kg, 0.45m3) - Todas Prioritarias.\n";

    ResultadoPlano planoA = executarPlano("PLANO A (Ordem de Cadastro)", encomendas, v, false);
    ResultadoPlano planoB = executarPlano("PLANO B (Menor Volume Primeiro)", encomendas, v, true);

    exibirResultadoPlano(planoA);
    exibirResultadoPlano(planoB);
    exibirRecomendacao(planoA, planoB);
}

int main() {
    vector<Encomenda> encomendas;
    int contadorOrdem = 0;
    int opcao;

    do {
        cout << "\n=== SISTEMA DE PLANEJAMENTO DE CARGAS ===\n";
        cout << "1. Cadastrar encomenda\n";
        cout << "2. Listar encomendas cadastradas\n";
        cout << "3. Simular planos para um veiculo\n";
        cout << "4. Executar Teste de Aceitacao automatico\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opcao: ";

        if (!(cin >> opcao)) {
            limparBuffer();
            continue;
        }
        limparBuffer();

        switch (opcao) {
            case 1:
                cadastrarEncomenda(encomendas, contadorOrdem);
                break;

            case 2:
                cout << "\n--- ENCOMENDAS CADASTRADAS (" << encomendas.size() << ") ---\n";
                for (const auto& e : encomendas) {
                    cout << "#" << e.ordemCadastro << " [" << e.codigo << "] Destino: " << e.destino
                         << " | Peso: " << e.peso << " kg"
                         << " | Vol: " << e.volume << " m3"
                         << " | Prioritaria: " << (e.prioritaria ? "Sim" : "Nao")
                         << " | Refrig: " << (e.precisaRefrigeracao ? "Sim" : "Nao") << "\n";
                }
                break;

            case 3: {
                if (encomendas.empty()) {
                    cout << "[Aviso] Cadastre ao menos uma encomenda antes de simular.\n";
                    break;
                }
                Veiculo v;
                do {
                    cout << "Destino atendido pelo veiculo: ";
                    getline(cin, v.destinoAtendido);
                } while (v.destinoAtendido.empty());

                v.capPeso = lerDoublePositivo("Capacidade de peso do veiculo (kg): ");
                v.capVolume = lerDoublePositivo("Capacidade de volume do veiculo (m3): ");
                v.temRefrigeracao = lerSimNao("O veiculo possui refrigeracao?");

                ResultadoPlano planoA = executarPlano("PLANO A (Ordem de Cadastro)", encomendas, v, false);
                ResultadoPlano planoB = executarPlano("PLANO B (Menor Volume Primeiro)", encomendas, v, true);

                exibirResultadoPlano(planoA);
                exibirResultadoPlano(planoB);
                exibirRecomendacao(planoA, planoB);
                break;
            }

            case 4:
                executarTesteAceitacao(encomendas, contadorOrdem);
                break;

            case 0:
                cout << "Encerrando sistema...\n";
                break;

            default:
                cout << "[Erro] Opcao invalida.\n";
        }
    } while (opcao != 0);

    return 0;
}
