#include "init.h"
#include "data.h"
#include "vector.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

CInit::CInit(Init _init)
    : topic(_init.k, _init.bucket_size, _init.tokens_ventana,
            _init.documentos_ventana),
      init(_init) {}

void CInit::procesar_ventana() {
  topic.iniciar_nueva_ventana();
  for (std::size_t i = 0; i < queue_ventana_actual.size(); i++) {
    std::string nameDoc = queue_ventana_actual[i];
    CVector<std::string> tokens = m_cache.find(nameDoc);
    // cout << "doc: " << nameDoc << endl;
  }
}

std::vector<std::string>
CInit::leer_documentos_de_carpeta(const std::string &carpeta_path) {
  std::vector<std::string> archivos;

  try {
    if (!fs::exists(carpeta_path)) {
      std::cerr << "Error: La carpeta '" << carpeta_path << "' no existe."
                << std::endl;
      return archivos;
    }

    for (const auto &entry : fs::directory_iterator(carpeta_path)) {
      if (entry.is_regular_file()) {
        std::string extension = entry.path().extension().string();
        // Solo procesar archivos de texto
        if (extension == ".txt" || extension == ".text" || extension == "") {
          archivos.push_back(entry.path().string());
        }
      }
    }

    std::cout << "Encontrados " << archivos.size() << " archivos en '"
              << carpeta_path << "'" << std::endl;

  } catch (const fs::filesystem_error &ex) {
    std::cerr << "Error accediendo a la carpeta: " << ex.what() << std::endl;
  }

  return archivos;
}

std::string CInit::leer_archivo(const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cerr << "Error: No se pudo abrir el archivo '" << file_path << "'"
              << std::endl;
    return "";
  }

  std::string contenido;
  std::string linea;
  while (std::getline(file, linea)) {
    contenido += linea + " ";
  }

  file.close();
  return contenido;
}

void CInit::run() {
  // vector<vector<string>> todos_documentos = {
  //     {"fútbol", "gol", "fútbol", "deporte"},           // Doc1
  //     {"gol", "partido", "fútbol", "estadio"},          // Doc2
  //     {"tenis", "fútbol", "golf", "deporte"},           // Doc3
  //     {"terremoto", "alerta", "sismo", "emergencia"},   // Doc4
  //     {"alerta", "tsunami", "terremoto", "riesgo"},     // Doc5
  //     {"fútbol", "tenis", "golf", "deporte"},           // Doc6
  //     {"crisis", "economía", "mercado", "dinero"},      // Doc7
  //     {"tecnología", "innovación", "digital", "futuro"} // Doc8
  // };

  std::string carpeta_docs = "docs";
  std::vector<std::string> archivos = leer_documentos_de_carpeta(carpeta_docs);
  if (archivos.empty()) {
    std::cout << "No se encontraron archivos en la carpeta 'docs'. Creando "
                 "archivos de ejemplo..."
              << std::endl;

    // Crear carpeta docs si no existe
    fs::create_directory("docs");

    // Crear algunos archivos de ejemplo
    std::vector<std::string> documentos_ejemplo = {
        "Rollins Goes 0-for-4 as Streak Ends PHILADELPHIA -- Jimmy Rollins was "
        "heading back to the clubhouse when Charlie Manuel put his arm around "
        "him and offered some encouraging words.",
        "Mets Beat Phillies 5-2 Behind Strong Pitching NEW YORK -- The New "
        "York Mets defeated the Philadelphia Phillies 5-2 on Saturday night.",
        "Yankees Win World Series in Game 7 Thriller NEW YORK -- The New York "
        "Yankees won their 27th World Series championship with a dramatic Game "
        "7 victory.",
        "Phillies Sign Free Agent Pitcher to 3-Year Deal PHILADELPHIA -- The "
        "Phillies have signed right-handed pitcher to a three-year contract "
        "worth $30 million.",
        "Eagles Prepare for Cowboys in NFC East Showdown PHILADELPHIA -- The "
        "Philadelphia Eagles are getting ready to face the Dallas Cowboys in a "
        "crucial division game."};

    for (size_t i = 0; i < documentos_ejemplo.size(); i++) {
      std::string nombre_archivo =
          "docs/documento_" + std::to_string(i + 1) + ".txt";
      std::ofstream file(nombre_archivo);
      if (file.is_open()) {
        file << documentos_ejemplo[i];
        file.close();
        archivos.push_back(nombre_archivo);
        std::cout << "Creado archivo de ejemplo: " << nombre_archivo
                  << std::endl;
      }
    }
  }
  for (std::size_t i = 0; i < archivos.size(); i++) {
    init.doc_count++;
    std::cout << "\n=== PROCESANDO DOCUMENTO " << init.doc_count
              << " ===" << std::endl;
    std::cout << "Archivo: " << archivos[i] << std::endl;
    std::string texto_completo = leer_archivo(archivos[i]);

    if (texto_completo.empty()) {
      std::cout << "Archivo vacío, saltando..." << std::endl;
      continue;
    }

    std::cout << "Texto original (primeros 100 chars): "
              << texto_completo.substr(0, 100) << "..." << std::endl;

    // Preprocesar el texto con Python
    CVector<std::string> v_tokens =
        preprocesador.preprocesar_texto(texto_completo);

    std::string nameDoc = "doc_" + to_string(init.doc_count);
    m_cache.ins(nameDoc, v_tokens);
    queue_ventana_actual.push_back(nameDoc);

    for (int i = 0; i < v_tokens.size(); i++) {
      topic.add_ventana(v_tokens[i]);
    }
    for (std::size_t c = 0; c < v_tokens.size(); c++) {
      topic.add_cementerio(v_tokens[c]);
    }

    if (init.doc_count >= init.documentos_ventana) {
      std::string doc = queue_ventana_actual[0];
      queue_ventana_actual.pop_front();
      CVector<std::string> tokens_rem = m_cache.find(doc);
      topic.rem_freq(tokens_rem);
      numVentana++;
    }
  }
}

void CInit::runtest() {
  std::string carpeta_docs = "docs";
  std::vector<std::string> archivos_reales =
      leer_documentos_de_carpeta(carpeta_docs);

  if (archivos_reales.empty()) {
    // ... (Lógica para crear archivos de ejemplo si la carpeta está vacía) ...
    std::cout << "No se encontraron archivos en la carpeta 'docs'. Creando "
                 "archivos de ejemplo..."
              << std::endl;
    fs::create_directory("docs");
    std::vector<std::string> documentos_ejemplo = {
        "Rollins Goes 0-for-4 as Streak Ends PHILADELPHIA -- Jimmy Rollins was "
        "heading back to the clubhouse when Charlie Manuel put his arm around "
        "him and offered some encouraging words.",
        "Mets Beat Phillies 5-2 Behind Strong Pitching NEW YORK -- The New "
        "York Mets defeated the Philadelphia Phillies 5-2 on Saturday night.",
        "Yankees Win World Series in Game 7 Thriller NEW YORK -- The New York "
        "Yankees won their 27th World Series championship with a dramatic Game "
        "7 victory.",
        "Phillies Sign Free Agent Pitcher to 3-Year Deal PHILADELPHIA -- The "
        "Phillies have signed right-handed pitcher to a three-year contract "
        "worth $30 million.",
        "Eagles Prepare for Cowboys in NFC East Showdown PHILADELPHIA -- The "
        "Philadelphia Eagles are getting ready to face the Dallas Cowboys in a "
        "crucial division game."};

    for (size_t i = 0; i < documentos_ejemplo.size(); i++) {
      std::string nombre_archivo =
          "docs/documento_" + std::to_string(i + 1) + ".txt";
      std::ofstream file(nombre_archivo);
      if (file.is_open()) {
        file << documentos_ejemplo[i];
        file.close();
        archivos_reales.push_back(nombre_archivo);
        std::cout << "Creado archivo de ejemplo: " << nombre_archivo
                  << std::endl;
      }
    }
  }

  // --- Lógica de simulación para 1 millón de documentos ---
  const long long OBJETIVO_DOCUMENTOS = 10;
  long long documentos_procesados_totales = 0;
  size_t num_archivos_reales = archivos_reales.size();

  if (num_archivos_reales == 0) {
    std::cerr << "Error: No hay archivos reales que procesar." << std::endl;
    return;
  }

  // Calcular el número de ciclos completos necesarios
  long long ciclos_necesarios =
      std::ceil((double)OBJETIVO_DOCUMENTOS / num_archivos_reales);

  std::cout << "Se necesitaran aproximadamente " << ciclos_necesarios
            << " ciclos de " << num_archivos_reales
            << " archivos para llegar a " << OBJETIVO_DOCUMENTOS << std::endl;

  for (long long ciclo = 0; ciclo < ciclos_necesarios; ++ciclo) {
    for (size_t i = 0; i < num_archivos_reales; ++i) {

      if (documentos_procesados_totales >= OBJETIVO_DOCUMENTOS) {
        break;
      }

      std::string ruta_archivo_actual = archivos_reales[i];

      init.doc_count++;
      documentos_procesados_totales++;

      std::string nameDoc = "doc_" + std::to_string(init.doc_count);

      std::cout << "\n=== PROCESANDO DOCUMENTO " << init.doc_count << std::endl;
      std::string texto_completo = leer_archivo(archivos[i]);
      CVector<std::string> v_tokens =
          preprocesador.preprocesar_texto(texto_completo);

      m_cache.ins(nameDoc, v_tokens);
      queue_ventana_actual.push_back(nameDoc);

      try {
        v_tokens = m_cache.find(nameDoc);

      } catch (...) {
        std::string texto_completo = leer_archivo(ruta_archivo_actual);

        if (texto_completo.empty()) {
          std::cout << "Archivo vacío o error de lectura, saltando..."
                    << std::endl;
          continue;
        }

        // Preprocesar el texto con Python
        CVector<std::string> v_tokens =
            preprocesador.preprocesar_texto(texto_completo);

        m_cache.ins(nameDoc, v_tokens);
      }
      cout << "DOC: " << nameDoc << endl;
      queue_ventana_actual.push_back(nameDoc);
      for (int i = 0; i < v_tokens.size(); i++) {
        cout << "TOKEN: [ " << v_tokens[i] << " , ";
        topic.add_ventana(v_tokens[i]);
      }
      cout << "]\n";
      for (std::size_t c = 0; c < v_tokens.size(); c++) {
        topic.add_cementerio(v_tokens[c]);
      }

      if (init.doc_count >= init.documentos_ventana) {
        std::string doc = queue_ventana_actual[0];
        queue_ventana_actual.pop_front();
        CVector<std::string> tokens_rem = m_cache.find(doc);
        topic.rem_freq(tokens_rem);
        numVentana++;
      }
    }
    if (documentos_procesados_totales >= OBJETIVO_DOCUMENTOS) {
      break; // Salir del bucle exterior también
    }
  }

  // Procesa cualquier ventana restante al final del proceso
  while (queue_ventana_actual.size() >= init.documentos_ventana) {
    std::cout << "==== PROCESANDO VENTANA FINAL " << (numVentana + 1)
              << " ====" << std::endl;
    procesar_ventana();
    queue_ventana_actual.pop_front();
    numVentana++;
  }

  std::cout << "Procesamiento completado. Total de documentos simulados: "
            << documentos_procesados_totales << std::endl;
}

void CInit::print() { topic.printVentanaActual(); }