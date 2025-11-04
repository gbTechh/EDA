## DIFERENCIAS CLAVE

| Aspecto                        | Lossy Counting                    | Lossy + Heap     | Space-Saving          |
| ------------------------------ | --------------------------------- | ---------------- | --------------------- |
| **Capacidad HashMap/Counters** | Crece y se poda                   | Crece y se poda  | Fija (K elementos)    |
| **Heap**                       | No usa                            | Top-K elementos  | Todos los K elementos |
| **Reemplazo**                  | Poda periódica                    | No hay reemplazo | Reemplazo inmediato   |
| **Memoria pico**               | Variable (~10-50 palabras)        | Variable + heap  | Fija (K palabras)     |
| **Garantía Top-K**             | Probabilística                    | Probabilística   | Determinística        |
| **Complejidad obtener Top-K**  | O(M log M) donde M=tamaño HashMap | O(K log K)       | O(K log K)            |

## EJEMPLO NUMÉRICO COMPLETO

### **Lossy Counting (epsilon=0.2, bucket_size=5)**

```
Procesamiento:
N=0-5:   bucket_id=1, HashMap crece libremente
N=5:     PODA: eliminar palabras con freq+error <= 1
N=5-10:  bucket_id=2, HashMap crece
N=10:    PODA: eliminar palabras con freq+error <= 2
...
N=35:    bucket_id=7

HashMap final: ~8 palabras
Memoria: ~600 bytes
```

### **Space-Saving (K=5)**

```
Procesamiento:
Siempre exactamente 5 palabras en memoria
Cada nuevo token:
  - Si existe: O(1) update + O(log 5) heap
  - Si no existe: O(1) replace + O(log 5) heap

Counters final: exactamente 5 palabras
Memoria: ~350 bytes (fija)
```
