#include "graph.h"


void first_visit(const Graph *g,int start,int *d) {
    for(int i=0;i<g->n;++i) d[i]=-1;
    d[start]=0; std::queue<int> q; q.push(start);
    while(!q.empty()) {
        int u=q.front(); q.pop();
        for(int v=0;v<g->n;++v) if(g->a[u*g->n+v]>0 && d[v]<0) {
            d[v]=d[u]+g->a[u*g->n+v]; q.push(v);
        }
    }
}
void dijkstra(const Graph *g,int start,int *d) {
    int *used=(int *)allocate(g->n,sizeof(int));
    for(int i=0;i<g->n;++i) d[i]=-1;
    d[start]=0;
    for(int k=0;k<g->n;++k) {
        int u=-1;
        for(int v=0;v<g->n;++v)
            if(!used[v] && d[v]>=0 && (u<0 || d[v]<d[u])) u=v;
        if(u<0) break;
        used[u]=1;
        for(int v=0;v<g->n;++v) {
            int w=g->a[u*g->n+v];
            if(w>0 && (d[v]<0 || d[v]>d[u]+w)) d[v]=d[u]+w;
        }
    }
    free(used);
}


void metrics(const Graph *g) {
    int *all=(int *)allocate((size_t)g->n*g->n,sizeof(int));
    int *ecc=(int *)allocate(g->n,sizeof(int));
    bool all_finite=true; int radius=INT_MAX, diameter=0;
    for(int u=0;u<g->n;++u) {
        int *d=all+u*g->n; dijkstra(g,u,d);
        int e=0;
        for(int v=0;v<g->n;++v) {
            if(d[v]<0) {e=-1; break;}
            if(d[v]>e) e=d[v];
        }
        ecc[u]=e;
        if(e<0) all_finite=false;
        else {if(e<radius) radius=e; if(e>diameter) diameter=e;}
    }
    puts("Матрица кратчайших расстояний (-1 = INF):");
    for(int u=0;u<g->n;++u) {
        printf("%d:",u+1);
        for(int v=0;v<g->n;++v) printf("%4d",all[u*g->n+v]);
        puts("");
    }
    printf("Эксцентриситеты:");
    for(int u=0;u<g->n;++u) {
        if(ecc[u]<0) printf(" INF"); else printf(" %d",ecc[u]);
    }
    puts("");
    if(radius==INT_MAX) puts("Радиус: INF"); else printf("Радиус: %d\n",radius);
    if(!all_finite) puts("Диаметр: INF"); else printf("Диаметр: %d\n",diameter);
    printf("Центральные вершины:");
    for(int u=0;u<g->n;++u)
        if(radius==INT_MAX ? ecc[u]<0 : ecc[u]==radius) printf(" %d",u+1);
    printf("\nПериферийные вершины:");
    for(int u=0;u<g->n;++u)
        if(all_finite ? ecc[u]==diameter : ecc[u]<0) printf(" %d",u+1);
    puts("");
    if(!all_finite) puts("Используется расширенная метрика с INF для недостижимых пар.");
    free(ecc); free(all);
}
int main(int argc,char **argv) {
    srand((unsigned)time(NULL));
    Options o=options(argc,argv);
    if(o.bench) {fputs("В ЛР10 замеры не требуются.\n",stderr); return 1;}
    Graph g=load_graph(&o,true,true);
    printf("ЛР10. n=%d, p=%d%%, start=%d\n",g.n,o.p,o.start);
    printf("Граф: %s, %s\n",o.directed ? "ориентированный" : "неориентированный",o.weighted ? "взвешенный" : "невзвешенный");
    print_matrix(&g);
    int *d=(int *)allocate(g.n,sizeof(int));
    first_visit(&g,o.start-1,d); print_vector("Первое посещение (методичка):",d,g.n);
    dijkstra(&g,o.start-1,d); print_vector("Кратчайшие расстояния (Дейкстра):",d,g.n);
    metrics(&g); free(d); graph_free(&g); return 0;
}
