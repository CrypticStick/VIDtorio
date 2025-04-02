#ifndef BLUEPRINT_H_
#define BLUEPRINT_H_

#include "arena.h"
#include "my_string.h"

// String builder max capacity (8 KB)
#define STEP_BUILDER_CAP 1<<13

typedef struct entity_icon_ {
    // "type" : "virtual"
    string *name;
} entity_icon;

typedef struct entity_ {
    string *name;
    entity_icon *icon;
    int entity_number;
    int position_x;
    int position_y;
    // if icon --> "show_in_chart" : true
} entity;

typedef struct entity_node_ {
    struct entity_node_ *next;
    entity *entity;
} entity_node;

typedef struct entity_list_ {
    entity_node *tail;
    entity_node *head;
} entity_list;

typedef struct blueprint_ {
    entity_list entities;
} blueprint;

typedef enum blueprint_stream_step_ {
    BLUEPRINT_PRE_ENTITIES_STEP = 0,
    BLUEPRINT_ENTITIES_STEP = 1,
    BLUEPRINT_POST_ENTITIES_STEP = 2,
    FINISHED_STEP = 3
} blueprint_stream_step;

typedef struct blueprint_stream_ {
    entity_node *cur_entity;
    blueprint_stream_step step;
    string to_append;
    string_buffer step_builder;
    string_buffer buf;
} blueprint_stream;

blueprint blueprint_new();

blueprint_stream blueprint_stream_new(blueprint blueprint, ptrdiff_t cap, arena *a);

string blueprint_stream_next(blueprint_stream *stream);

entity_icon *entity_icon_new(string *name, arena *a);

entity *entity_new(int entity_number, string *name, int pos_x, int pos_y, entity_icon *icon, arena *a);

string entity_string(entity *entity, string_buffer *buf, bool append_comma);

void blueprint_add_entity(blueprint *blueprint, entity *entity, arena *a);

#endif // BLUEPRINT_H_
