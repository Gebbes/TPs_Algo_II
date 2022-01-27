from grafo import Grafo
from collections import deque


def reconstruir_camino(padres: dict, origen, destino):
    pila = []
    pila.append(destino)
    vertice = destino
    while vertice != origen:
        pila.append(padres[vertice])
        vertice = padres[vertice]
    camino = []
    while(len(pila) != 0):
        camino.append(pila.pop())
    return camino


def bfs(grafo: Grafo, origen):
    visitados = {}
    padre = {}
    orden = {}
    cola = deque()

    visitados[origen] = True
    padre[origen] = None
    orden[origen] = 0
    cola.append(origen)

    while len(cola) != 0:
        v = cola.popleft()
        ady = grafo.adyacentes(v)
        
        for w in ady:
            if w not in visitados:
                cola.append(w)
                visitados[w] = True
                padre[w] = v
                orden[w] = orden[v] + 1
    
    return padre, orden


def bfs_corte_rango(grafo: Grafo, origen, rango):
    visitados = {}
    padre = {}
    orden = {}
    cola = deque()

    visitados[origen] = True
    padre[origen] = None
    orden[origen] = 0
    cola.append(origen)

    while len(cola) != 0:
        v = cola.popleft()
        if orden[v] <= rango:
            ady = grafo.adyacentes(v)
            for w in ady:
                if w not in visitados:
                    cola.append(w)
                    visitados[w] = True
                    padre[w] = v
                    orden[w] = orden[v] + 1
        
    
    return padre, orden


def bfs_corte_encontrado(self: Grafo, origen, destino):
    if( destino not in self.grafo):
        return None, None
    if(origen == destino):
        return None, 0
    visitados = {}
    padre = {}
    orden = {}
    cola = deque()

    visitados[origen] = True
    padre[origen] = None
    orden[origen] = 0
    cola.append(origen)

    while len(cola) != 0:
        v = cola.popleft()
        ady = self.adyacentes(v)
        
        for w in ady:
            if w not in visitados:
                cola.append(w)
                visitados[w] = True
                padre[w] = v
                orden[w] = orden[v] + 1
            if w == destino:
                return padre, orden[w]

    return None, "inf"


def obtener_maximo_orden(orden: dict):
    maximo = 0
    vertice_max = None
    for vertice in orden:
        if orden[vertice] > maximo:
            maximo = orden[vertice]
            vertice_max = vertice
    return maximo, vertice_max


def diametro_general(self: Grafo):
    maximo = 0
    camino_max = []
    vertices = self.obtener_vertices()
    for vertice in vertices:
        padres, orden = bfs(self, vertice)
        maximo_local, vertice_max_local = obtener_maximo_orden(orden)
        if maximo_local > maximo:
            maximo = maximo_local
            camino_max = reconstruir_camino(padres, vertice, vertice_max_local)
    return maximo, camino_max


def obtener_clustering(grafo: Grafo, origen):
    ady = grafo.adyacentes(origen)

    if len(ady) < 2:
        return 0

    aristas_ady = 0
    largo_ady = len(ady)

    for v in ady:
        for w in grafo.adyacentes(v):
            if(w in ady):
                aristas_ady += 1
    
    return aristas_ady / (largo_ady * (largo_ady-1))
  
