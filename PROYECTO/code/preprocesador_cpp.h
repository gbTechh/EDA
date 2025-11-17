#ifndef PREPROCESADOR_CPP_H
#define PREPROCESADOR_CPP_H

#include "vector.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <unordered_set>

class PreprocesadorCPP {
private:
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
      // Palabras deportivas comunes
      "game", "team", "player", "season", "league", "point", "play", "win",
      "loss", "match", "score", "goal", "field", "court",
      // Otros comunes
      "all", "any", "both", "each", "few", "more", "most", "other", "some",
      "such", "no", "not", "only", "own", "same", "than", "too"};

  std::unordered_map<std::string, std::string> lematizador = {
      // Sustantivos plurales
      {"players", "player"},
      {"teams", "team"},
      {"games", "game"},
      {"seasons", "season"},
      {"leagues", "league"},
      {"points", "point"},
      {"matches", "match"},
      {"scores", "score"},
      {"goals", "goal"},
      {"fields", "field"},
      {"courts", "court"},
      {"countries", "country"},
      {"cities", "city"},
      {"states", "state"},
      {"years", "year"},
      {"months", "month"},
      {"days", "day"},
      {"times", "time"},
      {"ways", "way"},
      {"men", "man"},
      {"women", "woman"},
      {"children", "child"},
      {"people", "person"},

      // Verbos comunes (mantengo solo los más esenciales para el ejemplo)
      {"playing", "play"},
      {"played", "play"},
      {"plays", "play"},
      {"winning", "win"},
      {"won", "win"},
      {"wins", "win"},
      {"losing", "lose"},
      {"lost", "lose"},
      {"loses", "lose"},
      {"scoring", "score"},
      {"scored", "score"},
      {"going", "go"},
      {"went", "go"},
      {"goes", "go"},
      {"getting", "get"},
      {"got", "get"},
      {"gets", "get"},
      {"making", "make"},
      {"made", "make"},
      {"makes", "make"},
      {"taking", "take"},
      {"took", "take"},
      {"takes", "take"}};

  // Función auxiliar para reemplazar ends_with
  bool termina_con(const std::string &str, const std::string &suffix) {
    if (str.length() < suffix.length())
      return false;
    return str.compare(str.length() - suffix.length(), suffix.length(),
                       suffix) == 0;
  }

  std::string limpiar_caracteres_extraños(const std::string &texto) {
    std::string resultado;
    for (char c : texto) {
      // Mantener letras, números, apostrofes y guiones
      if (std::isalnum(c) || c == '\'' || c == '-') {
        resultado += c;
      } else {
        // Reemplazar otros caracteres por espacio
        resultado += ' ';
      }
    }
    return resultado;
  }

  std::string a_minusculas(const std::string &texto) {
    std::string resultado = texto;
    std::transform(resultado.begin(), resultado.end(), resultado.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return resultado;
  }

  std::string aplicar_reglas_stemming(const std::string &palabra) {
    if (palabra.length() <= 3)
      return palabra;

    // Buscar en el diccionario de lematización primero
    auto it = lematizador.find(palabra);
    if (it != lematizador.end()) {
      return it->second;
    }

    // Reglas básicas de stemming
    std::string stemmed = palabra;

    // Reglas para plurales - USANDO termina_con EN LUGAR DE ends_with
    if (stemmed.length() > 3) {
      if (stemmed.back() == 's') {
        if (termina_con(stemmed, "sses")) {
          stemmed = stemmed.substr(0, stemmed.length() - 2);
        } else if (termina_con(stemmed, "ies")) {
          stemmed = stemmed.substr(0, stemmed.length() - 3) + "y";
        } else if (termina_con(stemmed, "es") &&
                   (termina_con(stemmed, "ches") ||
                    termina_con(stemmed, "shes") ||
                    termina_con(stemmed, "xes") ||
                    termina_con(stemmed, "zes"))) {
          stemmed = stemmed.substr(0, stemmed.length() - 2);
        } else if (stemmed[stemmed.length() - 2] != 's') {
          stemmed = stemmed.substr(0, stemmed.length() - 1);
        }
      }
    }

    // Reglas para verbos - USANDO termina_con EN LUGAR DE ends_with
    if (stemmed.length() > 4) {
      if (termina_con(stemmed, "ing")) {
        if (termina_con(stemmed, "ying")) {
          stemmed = stemmed.substr(0, stemmed.length() - 3) + "ie";
        } else {
          stemmed = stemmed.substr(0, stemmed.length() - 3);
        }
      } else if (termina_con(stemmed, "ed")) {
        if (termina_con(stemmed, "ied")) {
          stemmed = stemmed.substr(0, stemmed.length() - 3) + "y";
        } else {
          stemmed = stemmed.substr(0, stemmed.length() - 2);
        }
      }
    }

    return stemmed;
  }

  bool es_palabra_valida(const std::string &palabra) {
    if (palabra.length() < 3)
      return false;
    if (stopwords.find(palabra) != stopwords.end())
      return false;

    // Verificar que sea principalmente alfabética
    int letras_count = 0;
    for (char c : palabra) {
      if (std::isalpha(c))
        letras_count++;
    }
    return letras_count >= (palabra.length() * 0.7); // 70% letras
  }

public:
  CVector<std::string> preprocesar_texto(const std::string &texto) {
    CVector<std::string> tokens;

    if (texto.empty())
      return tokens;

    // 1. Limpiar caracteres extraños
    std::string texto_limpio = limpiar_caracteres_extraños(texto);

    // 2. Convertir a minúsculas
    texto_limpio = a_minusculas(texto_limpio);

    // 3. Tokenización
    std::string token;
    for (char c : texto_limpio) {
      if (std::isalnum(c) || c == '\'' || c == '-') {
        token += c;
      } else if (!token.empty()) {
        // 4. Aplicar lematización/stemming
        std::string token_procesado = aplicar_reglas_stemming(token);

        // 5. Filtrar palabras válidas
        if (es_palabra_valida(token_procesado)) {
          tokens.push_back(token_procesado);
        }
        token.clear();
      }
    }

    // Último token
    if (!token.empty()) {
      std::string token_procesado = aplicar_reglas_stemming(token);
      if (es_palabra_valida(token_procesado)) {
        tokens.push_back(token_procesado);
      }
    }

    return tokens;
  }
};

#endif