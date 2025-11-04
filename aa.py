def SplitNode(R):
    # SN1: ENCONTRAR MEJOR DIVISIÓN
    linea_division = Partition(R)  # Ej: línea en x=5
    
    # Crear dos nuevos nodos
    n1 = Nodo(area=parte_izquierda)
    n2 = Nodo(area=parte_derecha)
    
    # SN2: REPARTIR ELEMENTOS
    for cada elemento E en R:
        if E cabe_completamente_en(n1):
            n1.agregar(E)
        elif E cabe_completamente_en(n2):
            n2.agregar(E)
        else:  # E cruza la línea de división
            if R es hoja:
                # Fragmentar el objeto
                pedazo1, pedazo2 = cortar(E, linea_division)
                n1.agregar(pedazo1)
                n2.agregar(pedazo2)
            else:  # R es nodo interno
                # Dividir recursivamente el hijo
                hijo1, hijo2 = SplitNode(E.hijo)
                n1.agregar(hijo1)
                n2.agregar(hijo2)
    
    # SN3: PROPAGAR HACIA ARRIBA
    if R es raíz:
        nueva_raiz = Nodo()
        nueva_raiz.agregar(n1)
        nueva_raiz.agregar(n2)
        return nueva_raiz
    else:
        padre = R.padre
        padre.remover(R)
        padre.agregar(n1)
        padre.agregar(n2)
        
        if padre.tiene_demasiados_elementos():
            return SplitNode(padre)  # Propagación recursiva
        else:
            return n1, n2

def Partition(nodo):
    # Probar división horizontal
    costo_x = Sweep(nodo, eje='x')
    
    # Probar división vertical  
    costo_y = Sweep(nodo, eje='y')
    
    # Elegir la mejor
    if costo_x < costo_y:
        return linea_vertical_en(mejor_x)
    else:
        return linea_horizontal_en(mejor_y)

def Sweep(nodo, eje):
    # Ordenar elementos por el eje elegido
    if eje == 'x':
        elementos_ordenados = sorted(nodo.elementos, key=lambda e: e.x_min)
    else:
        elementos_ordenados = sorted(nodo.elementos, key=lambda e: e.y_min)
    
    # Encontrar punto donde tenemos ~M/2 elementos
    punto_division = elementos_ordenados[M//2].coordenada_fin
    
    # Calcular costo de esta división
    costo = 0
    for elemento in nodo.elementos:
        if elemento cruza punto_division:
            costo += 1  # Penalizar por cada split necesario
    
    return costo