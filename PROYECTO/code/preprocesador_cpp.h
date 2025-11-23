#ifndef PREPROCESADOR_CPP_H
#define PREPROCESADOR_CPP_H

#include "vector.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

class PreprocesadorCPP {
private:
  // ============= STOPWORDS =============
  std::unordered_set<std::string> stopwords = {
      // Artículos
      "the", "a", "an",
      // Conjunciones
      "and", "or", "but", "nor", "yet", "so",
      // Preposiciones
      "in", "on", "at", "to", "for", "of", "with", "by", "as", "from", "into",
      "during", "including", "until", "against", "among", "throughout",
      "despite", "towards", "upon", "concerning",
      // Verbos auxiliares
      "is", "was", "are", "were", "be", "been", "being", "have", "has", "had",
      "do", "does", "did", "will", "would", "could", "should", "may", "might",
      "must", "shall",
      // Pronombres
      "i", "you", "he", "she", "it", "we", "they", "me", "him", "her", "us",
      "them", "my", "your", "his", "its", "our", "their", "mine", "yours",
      "hers", "ours", "theirs",
      // Demostrativos
      "this", "that", "these", "those",
      // Adverbios comunes
      "very", "really", "quite", "too", "so", "just", "only", "now", "then",
      "here", "there", "when", "where", "why", "how",
      // Otros comunes
      "all", "any", "both", "each", "few", "more", "most", "other", "some",
      "such", "no", "what", "say", "not", "only", "own", "same", "than", "too"};

  // ============= DICCIONARIO DE LEMAS =============
  std::unordered_map<std::string, std::string> lematizador;

  // (Opcional) Guardar palabras desconocidas
  std::unordered_set<std::string> palabras_desconocidas;

  //============= CARGAR DICCIONARIO EXTERNO ============
public:
  void cargar_diccionario(const std::string &ruta) {
    std::ifstream file(ruta);
    if (!file.is_open()) {
      std::cerr << "Error leyendo: " << ruta << "\n";
      return;
    }

    std::string linea, palabra, lema;
    while (std::getline(file, linea)) {
      std::istringstream ss(linea);
      if (ss >> palabra >> lema) {
        lematizador[palabra] = lema;
      }
    }
  }

private:
  //============= UTILIDADES ============
  bool termina_con(const std::string &str, const std::string &suffix) {
    if (str.size() < suffix.size())
      return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
  }

  bool es_consonante(char c) {
    c = std::tolower(c);
    return (c >= 'a' && c <= 'z') &&
           !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
  }

  std::string limpiar_caracteres(const std::string &texto) {
    std::string r;
    for (char c : texto)
      r += (std::isalnum(c) || c == '-') ? c : ' ';
    return r;
  }

  std::string a_minusculas(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
  }

  //============== STEMMING SIMPLE =================
  std::string stemming_simple(std::string w) {
    const CVector<std::string> sufijos = {"ing", "ed", "es", "s"};

    for (std::size_t i = 0; i < sufijos.size(); i++) {
      if (w.size() > sufijos[i].size() && termina_con(w, sufijos[i])) {
        w = w.substr(0, w.size() - sufijos[i].size());
        break;
      }
    }

    // quitar doble consonante final: running -> run
    if (w.size() >= 2 && w[w.size() - 1] == w[w.size() - 2] &&
        es_consonante(w[w.size() - 1])) {
      w = w.substr(0, w.size() - 1);
    }
    return w;
  }

  //============== LEMATIZACIÓN + STEMMING =================
  std::string aplicar_lematizacion(const std::string &palabra) {
    auto it = lematizador.find(palabra);
    if (it != lematizador.end())
      return it->second;

    palabras_desconocidas.insert(palabra); // opcional

    return stemming_simple(palabra);
  }

  //============== FILTRO DE PALABRAS =================
  bool es_valida(const std::string &p) {
    if (p.size() < 3)
      return false;
    if (stopwords.count(p))
      return false;

    int letras = 0;
    for (char c : p)
      if (std::isalpha(c))
        letras++;

    return letras >= p.size() * 0.7;
  }

public:
  //============== MÉTODO PRINCIPAL =================
  CVector<std::string> preprocesar_texto(const std::string &texto) {
    CVector<std::string> tokens;
    if (texto.empty())
      return tokens;

    std::string limpio = a_minusculas(limpiar_caracteres(texto));

    std::string token = "";
    for (char c : limpio) {
      if (std::isalnum(c) || c == '-')
        token += c;
      else if (!token.empty()) {
        std::string t = aplicar_lematizacion(token);
        if (es_valida(t))
          tokens.push_back(t);
        token.clear();
      }
    }

    if (!token.empty()) {
      std::string t = aplicar_lematizacion(token);
      if (es_valida(t))
        tokens.push_back(t);
    }

    return tokens;
  }
};

#endif
