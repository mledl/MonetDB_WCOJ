#include "sql_hypergraph.h"

/*
 * Key function that calculates the number of relations a specific attributes is involved in.
 */
static int
vertice_realtion_count(Vertice *vertice) {
    return list_length(vertice->attributes);
}

Hypergraph * 
hypergraph_create(sql_allocator *sa, sql_rel *original_rel) {
    Hypergraph *hg = SA_NEW(sa, Hypergraph);

    if(!hg)
        return NULL;
    
    *hg = (Hypergraph) {
        .original_rel = original_rel,
        .partition = -1,
    };

    return hg;
}

Attribute *
attribute_create(sql_allocator *sa, const char *tname, const char *cname) {
	Attribute *attr = SA_NEW(sa, Attribute);

	if (!attr)
		return NULL;

	*attr = (Attribute) {
		.tname = tname,
        .cname = cname,
	};

	return attr;
}

Vertice *
vertice_create(sql_allocator *sa, const char *cname, list *attributes) {
    Vertice *vertice = SA_NEW(sa, Vertice);

    if(!vertice)
        return NULL;

    *vertice = (Vertice) {
        .cname = cname,
        .attributes = attributes,
    };

    return vertice;
}

Edge *
edge_create(sql_allocator *sa, const char *tname, list *attributes) {
    Edge *edge = SA_NEW(sa, Edge);

    if(!edge)
        return NULL;
    
    *edge = (Edge) {
        .tname = tname,
        .attributes = attributes,
    };

    return edge;
}

void
print_hypergraph(stream *fd, Hypergraph *hg) {
    node *n = NULL, *m = NULL;
    Edge *edge = NULL;

    mnstr_printf(fd, "Hypergraph Summary\n");
    mnstr_printf(fd, "Vertices: { ");

    for(n = hg->vertices->h; n; n = n->next) {
        Vertice *vertice = n->data;
        mnstr_printf(fd, "%s( ",vertice->cname);

        for(node *nattr = vertice->attributes->h; nattr; nattr = nattr->next) {
            Attribute *attribute = nattr->data;
            mnstr_printf(fd, "%s", attribute->tname);
            if(nattr->next) {
                mnstr_printf(fd, ", ");
            }
        }
        if(n->next) {
            mnstr_printf(fd, " ), ");
        } else {
            mnstr_printf(fd, " ) } ");

        }
    }
    mnstr_printf(fd, " cnt=%d\n", hg->nr_vertices);
    mnstr_printf(fd, "Edges: {\n");

    for(n = hg->edges->h; n; n = n->next) {
        Edge *edge = n->data;
        list *attributes = edge->attributes;
       
        mnstr_printf(fd, "%s={ ", edge->tname);
        for(m = attributes->h; m; m = m->next) {
            Attribute *attribute = m->data;
            mnstr_printf(fd, "%s", attribute->cname);
            if(m->next) {
                mnstr_printf(fd, ", ");
            } else {
                mnstr_printf(fd, " },\n");
            }
        }
    }

    mnstr_printf(fd, "} cnt=%d\n", hg->nr_edges);
}

Attribute *
get_attribute_from_hypergraph(Hypergraph *hg, const char *tname, const char *cname) {
    node *n = NULL;

    for (n = hg->vertices->h; n; n = n->next) {
        Vertice *vertice = n->data;

        if (strcmp(vertice->cname, cname) == 0) {
            return get_attribute_from_vertice(vertice, tname);
        }
    }

    return NULL;
}

Attribute *
get_attribute_from_edge(Edge *edge, const char *cname) {
    node *n = NULL;

    for (n = edge->attributes->h; n; n = n->next) {
        Attribute *attribute = n->data;

        if (strcmp(attribute->cname, cname) == 0) {
            return attribute;
        }
    }

    return NULL;
}

Attribute *
get_attribute_from_vertice(Vertice *vertice, const char *tname) {
    node *n = NULL;

    for (n = vertice->attributes->h; n; n = n->next) {
        Attribute *attribute = n->data;

        if (strcmp(attribute->tname, tname) == 0) {
            return attribute;
        }
    }
}


Vertice *
get_vertice_from_hypergraph(Hypergraph *hg, const char *cname) {
    node *n = NULL;

    for (n = hg->vertices->h; n; n = n->next) {
        Vertice *vertice = n->data;

        if (strcmp(vertice->cname, cname) == 0) {
            return vertice;
        }
    }

    return NULL;
}

void
update_attribute_statement(Hypergraph *hg, const char *tname, const char *cname, stmt *s) {
    Attribute *attr = get_attribute_from_hypergraph(hg, tname, cname);
    attr->current_stmt = s;
}

void 
update_attribute_intersection_statement(Edge *edge, const char *cname, stmt *s) {
    Attribute *attr = get_attribute_from_edge(edge, cname);
    attr->intersection_stmt = s;
}


stmt *
get_statement_from_edge(Edge *edge, const char *cname) {
    Attribute *attr = get_attribute_from_edge(edge, cname);
    return attr != NULL ? attr->current_stmt : NULL;
}

void
partition_hypergraph(mvc *sql, Hypergraph *hg, Hypergraph *hg_i, Hypergraph *hg_j) {
    list *vertices_i = sa_list(sql->sa), *vertices_j = sa_list(sql->sa), *tmp = NULL;
    list *edges_i = sa_list(sql->sa), *edges_j = sa_list(sql->sa);
    Vertice *vert = NULL;

    // find an arbitrary partition 1 <= |I| < |V| & |I| + |J| = |V|
    // for stake of simplicity we take |I| = 1
    // first sort the vertices in descending order of number of relations to ensure that the partiotion I 
    // consists of a vertices with relation count >= 2 in order to ensure intersection produced
    tmp = list_sort(hg->vertices, (fkeyvalue)&vertice_realtion_count, NULL);

    // just take the very first node for vertices I
    tmp = list_partition(tmp, vertices_i, vertices_j, 1);

    if (!tmp) {
        TRC_ERROR(SQL_EXECUTION, "Failed to partition hypergraph vertices!\n");
    }
    assert(tmp);

    // after partitioning the vertices, we now have to deal with the corresponding edges
    for (node *en = hg->edges->h; en; en = en->next) {
        Edge *edge = en->data;

        // handle edges of sub hypergraph i
        for (node *nvi = vertices_i->h; nvi; nvi = nvi->next) {
            Vertice *v = nvi->data;
            if (get_attribute_from_edge(edge, v->cname)) {
                list_append(edges_i, edge);
                break;
            }
        }

        // handle edges of sub hypergraph j
        for (node *nvj = vertices_j->h; nvj; nvj = nvj->next) {
            Vertice *v = nvj->data;
            if (get_attribute_from_edge(edge, v->cname)) {
                list_append(edges_j, edge);
                break;
            }
        }
    }

    // set fields of both sub hypergraphs
    hg_i->vertices = vertices_i;
    hg_i->edges = edges_i;
    hg_i->nr_vertices = vertices_i->cnt;
    hg_i->nr_edges = edges_i->cnt;
    hg_i->partition = 0;
    hg_j->vertices = vertices_j;
    hg_j->edges = edges_j;
    hg_j->nr_vertices = vertices_j->cnt;
    hg_j->nr_edges = edges_j->cnt;
    hg_j->partition = 1;
}