#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
    int V;
    int **dest;
    int **weight;
    int *edgeCount;
} Graph;

Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    
    graph->dest = (int**)malloc(V * sizeof(int*));
    graph->weight = (int**)malloc(V * sizeof(int*));
    graph->edgeCount = (int*)calloc(V, sizeof(int));

    for (int i = 0; i < V; ++i) {
        graph->dest[i] = (int*)malloc(V * sizeof(int));
        graph->weight[i] = (int*)malloc(V * sizeof(int));
    }
    return graph;
}

void addEdge(Graph* graph, int u, int v, int w) {
    int index = graph->edgeCount[u];
    graph->dest[u][index] = v;
    graph->weight[u][index] = w;
    graph->edgeCount[u]++;
}

void printGraph(Graph* graph) {
    printf("Adjacency list Representation :\n");
    for (int u = 0; u < graph->V; ++u) {
        printf("%d -> ", u);
        for (int i = 0; i < graph->edgeCount[u]; ++i) {
            printf("(%d,%d) ", graph->dest[u][i], graph->weight[u][i]);
        }
        printf("\n");
    }
    printf("\n");
}

void freeGraph(Graph* graph) {
    for (int i = 0; i < graph->V; ++i) {
        free(graph->dest[i]);
        free(graph->weight[i]);
    }
    free(graph->dest);
    free(graph->weight);
    free(graph->edgeCount);
    free(graph);
}

typedef struct {
    int vertex;
    int dist;
} PQNode;

typedef struct {
    PQNode* data;
    int size;
    int capacity;
} MinHeap;

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->data = (PQNode*)malloc(capacity * sizeof(PQNode));
    return minHeap;
}

void swapNode(PQNode* a, PQNode* b) {
    PQNode t = *a; *a = *b; *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->data[left].dist < minHeap->data[smallest].dist)
        smallest = left;
    if (right < minHeap->size && minHeap->data[right].dist < minHeap->data[smallest].dist)
        smallest = right;

    if (smallest != idx) {
        swapNode(&minHeap->data[smallest], &minHeap->data[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

PQNode extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap)) {
        PQNode emptyNode = {-1, -1};
        return emptyNode;
    }
    if (minHeap->size == 1) {
        minHeap->size--;
        return minHeap->data[0];
    }

    PQNode root = minHeap->data[0];
    minHeap->data[0] = minHeap->data[minHeap->size - 1];
    minHeap->size--;
    minHeapify(minHeap, 0);

    return root;
}

void insert(MinHeap* minHeap, int vertex, int dist) {
    if (minHeap->size == minHeap->capacity) {
        minHeap->capacity *= 2;
        minHeap->data = (PQNode*)realloc(minHeap->data, minHeap->capacity * sizeof(PQNode));
    }

    minHeap->size++;
    int i = minHeap->size - 1;
    minHeap->data[i].vertex = vertex;
    minHeap->data[i].dist = dist;

    while (i != 0 && minHeap->data[(i - 1) / 2].dist > minHeap->data[i].dist) {
        swapNode(&minHeap->data[i], &minHeap->data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Helper function to print the path recursively
void printPath(int parent[], int j) {
    if (parent[j] == -1) {
        printf("%d ", j);
        return;
    }
    printPath(parent, parent[j]);
    printf("%d ", j);
}

void dijkstra(Graph* graph, int src) {
    int V = graph->V;
    int* dist = (int*)malloc(V * sizeof(int));
    int* parent = (int*)malloc(V * sizeof(int)); // Array to track the shortest path

    for (int i = 0; i < V; ++i) {
        dist[i] = INT_MAX;
        parent[i] = -1; 
    }

    MinHeap* minHeap = createMinHeap(V * V); 

    insert(minHeap, src, 0);
    dist[src] = 0;

    while (!isEmpty(minHeap)) {
        PQNode minNode = extractMin(minHeap);
        int u = minNode.vertex;
        int d = minNode.dist;

        if (d > dist[u]) continue;

        for (int i = 0; i < graph->edgeCount[u]; ++i) {
            int v = graph->dest[u][i];
            int weight = graph->weight[u][i];

            if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u; // Track the predecessor node
                insert(minHeap, v, dist[v]); 
            }
        }
    }

    printf("\nShortest Paths from source %d:\n\n", src);
    for (int i = 0; i < V; ++i) {
        printf("Destination: %d\n", i);
        if (dist[i] == INT_MAX) {
            printf("Cost: INF\n");
            printf("Path: No path\n\n");
        } else {
            printf("Cost: %d\n", dist[i]);
            printf("Path : ");
            printPath(parent, i);
            printf("\n\n");
        }
    }

    free(dist);
    free(parent);
    free(minHeap->data);
    free(minHeap);
}

int main() {
    FILE *file = fopen("graph.txt", "r");
    if (file == NULL) {
        printf("Error: Could not open graph.txt.\n");
        return 1;
    }

    int V;
    if (fscanf(file, "%d", &V) != 1) {
        printf("Error: Could not read vertices.\n");
        fclose(file);
        return 1;
    }

    Graph* graph = createGraph(V);
    int u, v, w;

    while (fscanf(file, "%d %d %d", &u, &v, &w) == 3) {
        if (u >= 0 && u < V && v >= 0 && v < V) {
            addEdge(graph, u, v, w);
        }
    }
    fclose(file);

    printGraph(graph);

    int src;
    printf("Enter source vertex: ");
    if (scanf("%d", &src) != 1 || src < 0 || src >= V) {
        printf("\nInvalid source vertex. Defaulting to 0.\n");
        src = 0;
    }

    dijkstra(graph, src);

    freeGraph(graph);
    return 0;
}
