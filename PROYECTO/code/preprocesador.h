#ifndef PREPROCESADOR_CPP_H
#define PREPROCESADOR_CPP_H

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class PreprocesadorCPP {
public:
  std::vector<std::string> preprocesar_texto(const std::string &texto) {
    std::string comando = "python3 preprocesador_simple.py ";
    comando += escape_shell_argument(texto);

    // std::cout << "Executing Python preprocessing..." << std::endl;
    int resultado = system(comando.c_str());

    if (resultado != 0) {
      std::cerr << "Error in Python preprocessing" << std::endl;
      return {};
    }

    return leer_tokens_desde_json();
  }

private:
  std::string escape_shell_argument(const std::string &arg) {
    std::string result = "\"";
    for (char c : arg) {
      if (c == '"' || c == '\\' || c == '`' || c == '$') {
        result += '\\';
      }
      result += c;
    }
    result += "\"";
    return result;
  }

  std::vector<std::string> leer_tokens_desde_json() {
    std::vector<std::string> tokens;
    std::ifstream file("/tmp/processed_tokens.json");

    if (!file.is_open()) {
      std::cerr << "Could not open JSON file" << std::endl;
      return tokens;
    }

    std::string linea, contenido;
    while (std::getline(file, linea)) {
      contenido += linea;
    }
    file.close();

    size_t start = contenido.find('[');
    size_t end = contenido.find(']');

    if (start == std::string::npos || end == std::string::npos) {
      return tokens;
    }

    std::string array_content = contenido.substr(start + 1, end - start - 1);

    size_t pos = 0;
    while ((pos = array_content.find('"', pos)) != std::string::npos) {
      size_t end_quote = array_content.find('"', pos + 1);
      if (end_quote != std::string::npos) {
        std::string token = array_content.substr(pos + 1, end_quote - pos - 1);
        if (!token.empty()) {
          tokens.push_back(token);
        }
        pos = end_quote + 1;
      } else {
        break;
      }
    }

    // std::cout << "Processed tokens: " << tokens.size() << std::endl;
    return tokens;
  }
};

#endif