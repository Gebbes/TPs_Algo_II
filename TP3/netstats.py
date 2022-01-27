#!/usr/bin/python3
import os
import sys
from grafo import Grafo
from biblioteca import *
from random import shuffle
import csv


def listar_operaciones():
    print("rango\ncamino\nnavegacion\nciclo\nclustering\ndiametro\n", end="")


def printear_camino(camino: list):
    largo = len(camino)
    for i in range(largo):
        if i != 0:
            print(" -> ", camino[i], end = "", sep ="")
        else:
            print(camino[i], end="", sep ="")
    print("")


def printear_camino_2(camino : list):
    largo = len(camino)
    for i in range(largo):
        if i != 0:
            print(", ", camino[i], end = "")
        else:
            print(camino[i], end="")
    print("")


def bajar_datos(archivo: str, self: Grafo):
    if not ( os.path.exists(archivo)):
        return False
    with open(archivo, mode ='r', newline='', encoding="UTF-8") as archivo_tsv:
        tsv_reader = csv.reader(archivo_tsv, delimiter='\t')
        for linea in tsv_reader:
            self.agregar_vertices(linea[0])
            for parte in linea:
                if( parte not in self.grafo):
                    self.agregar_vertices( parte)
                if parte != linea[0]:
                    self.agregar_arista(linea[0], parte)


def rango(self: Grafo, origen, n):
    padre, distancia = bfs_corte_rango(self, origen, int(n))
    cont = 0
    for d in distancia:
        if(distancia[d] == int(n)):
            cont += 1
        
    print(cont)


def navegacion(self: Grafo, v):
    links = []
    visitados = {}
    if (not self.vertice_pertenece(v)):
        print("ERROR")
        return

    visitados[v] = True
    links.append(v)

    for i in range(20):
        ady = self.adyacentes(v)
        if len(ady) != 0:
            if( ady[0] not in visitados):
                links.append(ady[0])
                v = ady[0]
    
    printear_camino(links)       


def clustering(self : Grafo, pagina, opcional):

    if ( pagina and not self.vertice_pertenece(pagina)):         # if (pagina != None) and...
        print("ERROR")
        return 

    clustering = 0

    if pagina:
        clustering = obtener_clustering(self, pagina)
        print(clustering)
        return    
    
    for v in self.grafo:
        clustering += obtener_clustering(self, v)
    
    clustering_total = round(clustering/(len(self.grafo)), 3)
    
    print(clustering_total)


def camino_mas_corto(self: Grafo, origen, destino):
    padres, distancia = bfs_corte_encontrado(self, origen, destino)

    if(padres == None):
        if distancia == None:
            print("Destino no existe en el grafo")
            return
        if distancia == 0:
            print(origen)
            return
        if distancia == "inf":
            print("No se encontro recorrido")
            return
    
    camino = reconstruir_camino(padres, origen, destino)

    printear_camino(camino)
    print("Costo: ", distancia)


def diametro(self:Grafo, diametro_grafo : list):
    if(len(diametro_grafo) != 0):
        camino_max = diametro_grafo
        maximo = len(diametro_grafo)
    else:
        maximo, camino_max = diametro_general(self)
    
    printear_camino(camino_max)
    print("Costo: ", maximo, sep= "")
    return camino_max


def encontrar_ciclo_h(self: Grafo, actual, camino:list, inicio, n):
    camino.append(actual)
    adyacentes = self.adyacentes(actual)
    if (len(camino) == n ):
        if inicio in adyacentes:
            camino.append(inicio)
        else:
            camino.remove(actual)
        return camino
    
    for adyacente in adyacentes:
        if adyacente not in camino:
            camino_parcial = encontrar_ciclo_h(self, adyacente, camino, inicio, n)
            if(len(camino_parcial) == n + 1):
                return camino_parcial
    
    camino.remove(actual)
    return camino


def ciclo_hamiltoniano(self: Grafo, inicio, n):
    camino = encontrar_ciclo_h(self, inicio, [], inicio, int(n))
    if(len(camino) == 0):
        print("No se encontro recorrido")
        return
    printear_camino(camino)




def menu():
    grafo = Grafo()
    grafo._init_(True)

    bajar_datos(sys.argv[1], grafo)

    diametro_grafo = []

    opcion = sys.stdin.readline()
    while opcion != "":

        partes = opcion.replace("\n", "").split(" ")
        comando = partes[0]
        partes.pop(0)
        partes = " ".join(partes).split(",")

        for i in range(len(partes)):
            partes[i] = partes[i].strip()


        if comando == "listar_operaciones":
            listar_operaciones()
        elif comando == "navegacion":
            navegacion(grafo, partes[0])
        elif comando == "rango":
            rango(grafo, partes[0], partes[1])
        elif comando == "clustering": 
            if (len(partes) == 1):                        #No entendimos para que el parametro "opcional"
                clustering(grafo, partes[0], None)        #pero de querer usarse, se podria usar
            else:
                clustering(grafo, partes[0], partes[1])         
        elif comando == "mas_importantes":
            pass
        elif comando == "camino":
            camino_mas_corto(grafo, partes[0], partes[1])
        elif comando == "diametro":
            diametro_grafo = diametro(grafo, diametro_grafo)
        elif comando == "conectados":
            pass
        elif comando == "ciclo":
            sys.setrecursionlimit(100000)
            ciclo_hamiltoniano(grafo, partes[0], partes[1])
        else:
            print("La opcion elegida no es valida")
        opcion = sys.stdin.readline()

menu()


