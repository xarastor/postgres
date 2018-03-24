/*-------------------------------------------------------------------------
 *
 * planner.h
 *	  prototypes for planner.c.
 *
 *
 * Portions Copyright (c) 1996-2018, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/planner.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PLANNER_H
#define PLANNER_H

#include "nodes/plannodes.h"
#include "nodes/relation.h"

/*
 * Transitivity include
 */

#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include <ctype.h>

#define MAP_BY_VAL 0
#define MAP_BY_REF 1

typedef struct mapitem
{
    char* key;
    void* val;
    int type;
} MI;

typedef struct map
{
    int size;
    MI* items;
} M;

typedef struct Array{
    char ** array;
    // int *array;
    size_t used;
    size_t size;
} Array;
//struct mapitem;
//struct map;
//struct Array;
//struct val;
//struct predicate;
//struct graph_node;
//
//enum condition_type;

typedef enum {
    MORE,           // >
    LESS,           // <
    MORE_OR_EQUAL,  // >=
    LESS_OR_EQUAL,  // <=
    NOTHING_COND    // NULL
} condition_type;

typedef enum {
    NUMERIC,
    VARIABLE
} node_type;

typedef struct val {
    node_type type;
    char variable_name[16];
    int constant;
} val;

typedef struct predicate {
    val * rval;
    val * lval;
    condition_type cond;
} predicate;

typedef struct graph_node {
    struct map* m;
} graph_node;


M* mapNew(void);
int a2i(const char *s);
int numbers_only(const char *s);
int isContant (struct map* map, int offset);
int getConstants (struct map* map, int offset);
int getMapSize (struct map* map);
void mapAdd(char* key, void* val, struct map* map);
void mapDynAdd(char* key, void* val, struct map* map);
void* mapGet(char* key, struct map* map);
void mapClose(struct map* map);
void initArray(struct Array *a, size_t initialSize);
void insertArray(struct Array *a, char * element);
void freeArray(struct Array *a);
char* itoa(int value, char * result, int base);

val * make_val_from_constant (int constant);
val * make_val_from_str_constant (char * constant);
val * make_val_from_str (char * variable_name);
predicate * make_predicate_with_str_int(char * constant, condition_type cond, char * variable_name);
predicate * make_predicate_with_int (int constant, condition_type cond, char * variable_name);
predicate * make_predicate (char * variable_name1, condition_type cond, char * variable_name2);
condition_type str_to_cond_type (char * cond);
char * cond_type_to_str (condition_type cond);
void print_graph_node (graph_node * graph, char * key);
void make_empty_graph_node (graph_node * graph, char * key);
condition_type reverse_op (condition_type op);
condition_type resolve_ops (condition_type op1, condition_type op2);
condition_type * near_ops (condition_type op);
void addDeps (graph_node * graph,  char * name, predicate * sample);
char * predicate_to_str (predicate * p);
void injectInGraph (graph_node * graph, predicate * sample);
void genDepsGraph (graph_node * graph, predicate * samples[], int count);
void optimazeGraph (graph_node * graph, char ** out_str, int * offset);

char* extract_Var(Var* var);
char* extract_OpVar(OpExpr* var);
int extract_int_Const(Const* var);
predicate* extractOp(OpExpr* oExpr);
void extract(List* list, int extract_index);
void extract_root(List* list);


/*
 * Transitivity end
 */


/* Hook for plugins to get control in planner() */
typedef PlannedStmt *(*planner_hook_type) (Query *parse,
										   int cursorOptions,
										   ParamListInfo boundParams);
extern PGDLLIMPORT planner_hook_type planner_hook;

/* Hook for plugins to get control when grouping_planner() plans upper rels */
typedef void (*create_upper_paths_hook_type) (PlannerInfo *root,
											  UpperRelationKind stage,
											  RelOptInfo *input_rel,
											  RelOptInfo *output_rel);
extern PGDLLIMPORT create_upper_paths_hook_type create_upper_paths_hook;


extern PlannedStmt *planner(Query *parse, int cursorOptions,
		ParamListInfo boundParams);
extern PlannedStmt *standard_planner(Query *parse, int cursorOptions,
				 ParamListInfo boundParams);

extern PlannerInfo *subquery_planner(PlannerGlobal *glob, Query *parse,
				 PlannerInfo *parent_root,
				 bool hasRecursion, double tuple_fraction);

extern bool is_dummy_plan(Plan *plan);

extern RowMarkType select_rowmark_type(RangeTblEntry *rte,
					LockClauseStrength strength);

extern void mark_partial_aggref(Aggref *agg, AggSplit aggsplit);

extern Path *get_cheapest_fractional_path(RelOptInfo *rel,
							 double tuple_fraction);

extern Expr *expression_planner(Expr *expr);

extern Expr *preprocess_phv_expression(PlannerInfo *root, Expr *expr);

extern bool plan_cluster_use_sort(Oid tableOid, Oid indexOid);
extern int	plan_create_index_workers(Oid tableOid, Oid indexOid);

extern List *get_partitioned_child_rels(PlannerInfo *root, Index rti,
						   bool *part_cols_updated);
extern List *get_partitioned_child_rels_for_join(PlannerInfo *root,
									Relids join_relids);

#endif							/* PLANNER_H */
