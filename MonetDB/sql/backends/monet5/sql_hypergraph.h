#ifndef _HYPERGRAPH_H_
#define _HYPERGRAPH_H_

#include "monetdb_config.h"
#include "sql_relation.h"
#include "sql_statement.h"

typedef struct
{
    sql_rel *original_rel;  // pointer to the original join relation structure
    list *vertices;         // list of attributes within the hypergraph
    list *edges;
    int nr_vertices;        // number of attributes in the hypergraph
    int nr_edges;           // number of edges in the hypergraph
    int partition;          // indicates if current hypergraph represents partition I (= 0) or J (= 1), if -1, not partitioned so far
} Hypergraph;

typedef struct 
{
    const char *tname;          // name of the table the attribute belongs to
    const char *cname;          // name of the column itself
    sql_subtype bat_type;       // represents the attribute`s type
    stmt *bat_proj_stmt;        // statement representing a projection of retrieved bat
    stmt *current_stmt;         // represents the current stmt the attribute is involved in
    stmt *intersection_stmt;    // represemts the statement returned after intersecting all attributes of same kind
} Attribute;

typedef struct
{
    const char *cname;      // name of the column itself
    list *attributes;       // list of same attributes accross different tables
    stmt *tmp_result;       // represents intermediate results of specific attribute
    stmt *result_stmt;      // represents the result for current attribute represented by its column name
    stmt *tmp_candidate;    // represents a bat of oids that will be used to project a single string to bat consisting of multiple entries of this specific string
} Vertice;

typedef struct 
{
    const char *tname;      // name of the corresponding table
    list *attributes;       // list of attributes of a hyper edge
    sql_rel *rel;           // the sql relation corresponding to this hyper edge
} Edge;

extern Hypergraph *hypergraph_create(sql_allocator *sa, sql_rel *original_rel);
extern Attribute *attribute_create(sql_allocator *sa, const char *tname, const char *cname);
extern Vertice *vertice_create(sql_allocator *sa, const char *cname, list *attributes);
extern Edge *edge_create(sql_allocator *sa, const char *tname, list *attributes);
extern void print_hypergraph(stream *fd, Hypergraph *hg);
extern Attribute *get_attribute_from_hypergraph(Hypergraph *hg, const char *tname, const char *cname);
extern Attribute *get_attribute_from_edge(Edge *edge, const char *cname);
extern Attribute *get_attribute_from_vertice(Vertice *vertice, const char *tname);
Vertice *get_vertice_from_hypergraph(Hypergraph *hg, const char *cname);
extern void update_attribute_statement(Hypergraph *hg, const char *tname, const char *cname, stmt *s);
extern void update_attribute_intersection_statement(Edge *edge, const char *cname, stmt *s);
extern stmt *get_statement_from_edge(Edge *edge, const char *cname);
extern void partition_hypergraph(mvc *sql, Hypergraph *hg, Hypergraph *hg_i, Hypergraph *hg_j);

#endif