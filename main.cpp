#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

class Attivita {
public:
    Attivita(const std::string& nome, const std::string& descrizione, bool completata = false)
            : nome(nome), descrizione(descrizione), completata(completata) {
        if (nome.empty() || descrizione.empty()) {
            throw std::invalid_argument("Nome e descrizione non possono essere vuoti");
        }
    }

    std::string getNome() const { return nome; }
    std::string getDescrizione() const { return descrizione; }
    bool isCompletata() const { return completata; }

    void setNome(const std::string& nuovoNome) {
        if (nuovoNome.empty()) {
            throw std::invalid_argument("Il nuovo nome non può essere vuoto");
        }
        nome = nuovoNome;
    }

    void setDescrizione(const std::string& nuovaDescrizione) {
        if (nuovaDescrizione.empty()) {
            throw std::invalid_argument("La nuova descrizione non può essere vuota");
        }
        descrizione = nuovaDescrizione;
    }

    void setCompletata(bool nuovoStato) { completata = nuovoStato; }

    std::string toCSV() const {
        return nome + "," + descrizione + "," + (completata ? "1" : "0");
    }

    static Attivita fromCSV(const std::string& csvLine) {
        std::istringstream ss(csvLine);
        std::string nome, descrizione, completataStr;
        std::getline(ss, nome, ',');
        std::getline(ss, descrizione, ',');
        std::getline(ss, completataStr, ',');

        if (nome.empty() || descrizione.empty() || completataStr.empty()) {
            throw std::invalid_argument("CSV line is malformed: " + csvLine);
        }

        bool completata = (completataStr == "1");
        return Attivita(nome, descrizione, completata);
    }

private:
    std::string nome;
    std::string descrizione;
    bool completata;
};

class ListaAttivita {
public:
    void aggiungiAttivita(const Attivita& attivita) {
        elenco.push_back(attivita);
    }

    void mostraElenco() const {
        if (elenco.empty()) {
            std::cout << "Nessuna attività presente." << std::endl;
            return;
        }

        for (const auto& attivita : elenco) {
            std::cout << "Nome: " << attivita.getNome()
                      << ", Descrizione: " << attivita.getDescrizione()
                      << ", Completata: " << (attivita.isCompletata() ? "Sì" : "No") << std::endl;
        }
    }

    void contrassegnaComeCompletata(const std::string& nome) {
        for (auto& attivita : elenco) {
            if (attivita.getNome() == nome) {
                attivita.setCompletata(true);
                return;
            }
        }
        std::cerr << "Attività con nome '" << nome << "' non trovata." << std::endl;
    }

    void salvaSuDisco(const std::string& nomeFile) const {
        std::ofstream out(nomeFile);
        if (!out) {
            throw std::ios_base::failure("Errore nell'apertura del file per la scrittura: " + nomeFile);
        }

        for (const auto& attivita : elenco) {
            out << attivita.toCSV() << std::endl;
        }
    }

    void caricaDaDisco(const std::string& nomeFile) {
        std::ifstream in(nomeFile);
        if (!in) {
            throw std::ios_base::failure("Errore nell'apertura del file per la lettura: " + nomeFile);
        }

        elenco.clear();
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;  // Ignora righe vuote

            try {
                elenco.push_back(Attivita::fromCSV(line));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Errore nella lettura della linea: " << e.what() << std::endl;
            }
        }
    }

private:
    std::vector<Attivita> elenco;
};

int main() {
    ListaAttivita lista;
    std::string nome, descrizione;

    std::cout << "Inserisci le attività (digita 'fine' per terminare):" << std::endl;

    while (true) {
        std::cout << "Nome dell'attività: ";
        std::getline(std::cin, nome);
        if (nome == "fine") break;

        std::cout << "Descrizione dell'attività: ";
        std::getline(std::cin, descrizione);
        if (descrizione == "fine") break;

        try {
            lista.aggiungiAttivita(Attivita(nome, descrizione));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Errore: " << e.what() << std::endl;
        }
    }

    std::string nomeFile = "attivita.csv";

    try {
        lista.salvaSuDisco(nomeFile);
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Errore durante il salvataggio del file: " << e.what() << std::endl;
        return 1;
    }

    ListaAttivita nuovaLista;
    try {
        nuovaLista.caricaDaDisco(nomeFile);
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Errore durante il caricamento del file: " << e.what() << std::endl;
        return 1;
    }

    nuovaLista.mostraElenco();

    std::cout << "Vuoi contrassegnare un'attività come completata? (digita 'fine' per terminare):" << std::endl;
    while (true) {
        std::cout << "Nome dell'attività completata: ";
        std::getline(std::cin, nome);
        if (nome == "fine") break;

        nuovaLista.contrassegnaComeCompletata(nome);
    }

    nuovaLista.mostraElenco();

    return 0;
}
