#include "init.h"

CInit::CInit(Init _init)
    : topic(_init.k, _init.bucket_size, _init.tokens_ventana,
            _init.documentos_ventana),
      init(_init) {}

void CInit::procesar_documento(const vector<string> &documento) {
  for (const string &token : documento) {
    topic.add_ventana(token);
  }
}

void CInit::reprocesar_ventana_completa() {

  topic.iniciar_nueva_ventana();

  for (const auto &documento : deque_ventana_actual) {
    for (const string &token : documento) {
      topic.add_ventana(token);
    }
  }
}

void CInit::run() {
  vector<vector<string>> todos_documentos = {
      {"fútbol", "gol", "fútbol"},
      {"gol", "partido", "basketball"},
      {"tenis", "fútbol", "golf"},
      {"terremoto", "alerta", "sismo"},
      {"alerta", "tsunami", "terremoto"},
      {"fútbol", "tenis", "golf"},
      {"crisis", "economía", "mercado"},
      {"tecnología", "innovación", "digital"},
      {"clima", "lluvia", "temperatura"},
      {"salud", "medicina", "hospital"},
      {"educación", "escuela", "estudiantes"},
      {"política", "gobierno", "elecciones"},
      {"deporte", "competencia", "atleta"},
      {"música", "concierto", "artista"},
      {"cine", "película", "actor"},
      {"viaje", "turismo", "avión"},
      {"comida", "restaurante", "receta"},
      {"negocio", "empresa", "empleo"},
      {"ciencia", "investigación", "descubrimiento"},
      {"animal", "naturaleza", "conservación"},
      {"coche", "carretera", "conducción"},
      {"casa", "hogar", "familia"},
      {"dinero", "inversión", "banco"},
      {"tiempo", "reloj", "horas"},
      {"agua", "río", "mar"},
      {"fuego", "calor", "llama"},
      {"tierra", "planeta", "naturaleza"},
      {"aire", "viento", "atmósfera"},
      {"sol", "luz", "calor"},
      {"luna", "noche", "estrellas"},
      {"ciudad", "urbano", "edificio"},
      {"campo", "rural", "agricultura"},
      {"ropa", "moda", "vestir"},
      {"juego", "diversión", "entretenimiento"},
      {"libro", "lectura", "biblioteca"},
      {"arte", "creatividad", "expresión"},
      {"historia", "pasado", "eventos"},
      {"futuro", "predicción", "tecnología"},
      {"amor", "relación", "familia"},
      {"amistad", "compañerismo", "confianza"},
      {"trabajo", "esfuerzo", "profesión"},
      {"éxito", "logro", "meta"},
      {"fracaso", "error", "aprendizaje"},
      {"paz", "armonía", "tranquilidad"},
      {"guerra", "conflicto", "batalla"},
      {"libertad", "derechos", "justicia"},
      {"seguridad", "protección", "policía"},
      {"peligro", "riesgo", "advertencia"},
      {"belleza", "estética", "atractivo"},
      {"feo", "desagradable", "horror"},
      {"felicidad", "alegría", "contento"},
      {"tristeza", "dolor", "pena"},
      {"miedo", "ansiedad", "preocupación"},
      {"sorpresa", "inesperado", "asombro"},
      {"enfado", "enojo", "ira"},
      {"calma", "paciencia", "serenidad"},
      {"energía", "fuerza", "potencia"},
      {"debilidad", "enfermedad", "vulnerabilidad"},
      {"inteligencia", "sabiduría", "conocimiento"},
      {"ignorancia", "desconocimiento", "confusión"},
      {"verdad", "realidad", "hechos"},
      {"mentira", "engaño", "falsedad"},
      {"riqueza", "abundancia", "prosperidad"},
      {"pobreza", "escasez", "necesidad"},
      {"joven", "juventud", "adolescente"},
      {"viejo", "vejez", "anciano"},
      {"niño", "infancia", "juego"},
      {"adulto", "madurez", "responsabilidad"},
      {"hombre", "masculino", "varón"},
      {"mujer", "femenino", "dama"},
      {"padre", "paternidad", "familia"},
      {"madre", "maternidad", "cuidado"},
      {"hijo", "descendencia", "herencia"},
      {"hermano", "familia", "parentesco"},
      {"amigo", "compañía", "apoyo"},
      {"enemigo", "oposición", "conflicto"},
      {"jefe", "autoridad", "liderazgo"},
      {"empleado", "trabajador", "subordinado"},
      {"profesor", "enseñanza", "educación"},
      {"estudiante", "aprendizaje", "escuela"},
      {"médico", "salud", "enfermedad"},
      {"paciente", "tratamiento", "cura"},
      {"ingeniero", "tecnología", "diseño"},
      {"científico", "investigación", "experimento"},
      {"artista", "creación", "expresión"},
      {"deportista", "atleta", "competencia"},
      {"cocinero", "comida", "receta"},
      {"escritor", "libro", "literatura"},
      {"músico", "instrumento", "melodía"},
      {"actor", "teatro", "película"},
      {"piloto", "avión", "vuelo"},
      {"conductor", "coche", "carretera"},
      {"agricultor", "campo", "cultivo"},
      {"pescador", "mar", "pez"},
      {"minero", "tierra", "mineral"},
      {"bombero", "fuego", "emergencia"},
      {"policía", "ley", "seguridad"},
      {"soldado", "ejército", "defensa"},
      {"marinero", "barco", "océano"},
      {"astronauta", "espacio", "nave"},
      {"periodista", "noticia", "medios"},
      {"abogado", "ley", "justicia"},
      {"juez", "tribunal", "sentencia"},
      {"político", "gobierno", "poder"},
      {"rey", "monarquía", "corona"},
      {"presidente", "democracia", "gobierno"},
      {"líder", "dirección", "influencia"},
      {"seguidor", "apoyo", "lealtad"}};

  for (int i = 0; i < todos_documentos.size(); i++) {
    cout << "=== AGREGANDO DOCUMENTO " << i << " ===" << endl;

    if (deque_ventana_actual.size() == init.documentos_ventana) {
      deque_ventana_actual.pop_front();
      reprocesar_ventana_completa();
    }

    vector<string> documento = todos_documentos[i];
    deque_ventana_actual.push_back(documento);

    procesar_documento(documento);

    for (const string &token : documento) {
      topic.add_cementerio(token);
    }

    // Mostrar estado actual (opcional)
    cout << "Ventana actual contiene " << deque_ventana_actual.size()
         << " documentos" << endl;
  }
}

void CInit::print() { topic.printVentanaActual(); }