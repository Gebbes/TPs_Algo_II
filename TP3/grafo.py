import random

class Grafo(object):

    def _init_(self, dirigido = False):
        """
        Parametros: dirigido
        Funcionamiento: Crea el grafo y se le asigna si es dirigido o no
        Post: grafo creado
        """
        self.grafo = {}
        self.es_dirigido = dirigido

    def vertice_pertenece(self, v):
        """
        Parametros: vertice
        Funcionamiento: verifica si que el vertice indicado exista en el grafo
        Post: grafo creado
        """
        return v in self.grafo

    def agregar_vertices(self, v):
        """
        Parametros: vertice
        Funcionamiento: agrega el vertice indicado al grafo
        Post: grafo creado
        """
        if not self.vertice_pertenece(v):
            self.grafo[v] = {}

    def borrar_vertice(self, vertice):
        """
        Parametros: vertice
        Funcionamiento: Borra el vertice que se le indique del grafo
        Post: grafo creado
        """
        if not self.vertice_pertenece(vertice):
            return False
        
        for v in self.grafo:
            if vertice in self.grafo[v]:
                del self.grafo[v][vertice]

        del self.grafo[vertice]
        return True


    def arista_pertenece(self, a, b):
        """
        Parametros: vertice_desde, vertice_hasta
        Funcionamiento: Verifica si la arista pertenece en el grafo
        Post: grafo creado
        """        
        if not ((a in self.grafo) and (b in self.grafo[a])):
            return False
        return self.grafo[a][b]


    def agregar_arista(self, a, b, peso = 1):
        """
        Parametros: vertice_desde, vertice_hasta, peso
        Funcionamiento: agrega una arista al grafo
        Post: grafo creado
        """
        if self.arista_pertenece(a,b):
            return False

        self.grafo[a][b] = peso
        if not self.es_dirigido:
            self.grafo[b][a] = peso
    
    def borrar_arista(self, a, b):
        """
        Parametros: vertice_desde, vertice_hasta
        Funcionamiento: borra la arista del grafo
        Post: grafo creado
        """
        if not self.arista_pertenece(a,b):
            return False
        
        del self.grafo[a][b]
        if not self.dirigido:
            del self.grafo[b][a]
    
    def peso_arista(self, v, w):
        """
        """
        if v in self.grafo:
            if w in self.grafo[v]:
                return self.grafo[v][w]
        return None

    def obtener_vertices(self):
        """
        """
        lista_vertices = []
        vertices = self.grafo
        for vertice in vertices:
            lista_vertices.append(vertice)
        return lista_vertices
    
    def vertice_aleatorio(self):

        if len(self.grafo) == 0:
            return None
        
        vertice_n = random.randint(0, len(self.grafo))
        i = 0
        for vertice in self.grafo:
            if i == vertice_n:
                return vertice_n
            i += 1
    
    def adyacentes(self, v):
        if not v in self.grafo:
            return None
        
        lista_adyacentes = []
        if self.vertice_pertenece(v):
            dict_adyacentes = self.grafo[v]
            for adyacente in dict_adyacentes:
                lista_adyacentes.append(adyacente)
        
        return lista_adyacentes


        
        