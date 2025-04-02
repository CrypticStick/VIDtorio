#include "blueprint.h"

static const string BLUEPRINT_PRE_ENTITIES = str("{\"blueprint\":{\"icons\":[{\"signal\":{\"name\":\"display-panel\"},\"index\":1}],\"entities\":[");
static const string BLUEPRINT_POST_ENTITIES = str("],\"item\":\"blueprint\",\"version\":562949955977217}}");

blueprint blueprint_new() {
    blueprint new_blueprint = {0};
    return new_blueprint;
}

blueprint_stream blueprint_stream_new(blueprint blueprint, ptrdiff_t cap, arena *a) {
    blueprint_stream new_stream = {0};
    new_stream.cur_entity = blueprint.entities.tail;
    new_stream.buf = string_buffer_new(cap, a);
    new_stream.step_builder = string_buffer_new(STEP_BUILDER_CAP, a);
    return new_stream;
}

string blueprint_stream_next(blueprint_stream *stream) {
    ptrdiff_t buf_rem;
    ptrdiff_t buf_written;
    string_buffer_clear(&stream->buf);
    do {
        if (stream->to_append.len == 0) {
            // Determine the next string to output
            switch (stream->step) {
                case BLUEPRINT_PRE_ENTITIES_STEP:
                    stream->to_append = BLUEPRINT_PRE_ENTITIES;
                    stream->step += 1;
                    break;
                case BLUEPRINT_ENTITIES_STEP:
                    if (stream->cur_entity == NULL) {
                        stream->step += 1;
                    } else {
                        stream->to_append = entity_string(
                            stream->cur_entity->entity,
                            &stream->step_builder,
                            stream->cur_entity->next != NULL);
                        stream->cur_entity = stream->cur_entity->next;
                    }
                    break;
                case BLUEPRINT_POST_ENTITIES_STEP:
                    stream->to_append = BLUEPRINT_POST_ENTITIES;
                    stream->step += 1;
                    break;
                case FINISHED_STEP:
                    // Nothing left to output
                    return stream->buf.str;
            }
        }
        // Output the current string
        buf_written = string_buffer_add_str(&stream->buf, stream->to_append);
        stream->to_append = string_after(stream->to_append, buf_written);
        // Loop while everything fits in the output
    } while (stream->to_append.len == 0);
    return stream->buf.str;
}

entity_icon *entity_icon_new(string *name, arena *a) {
    entity_icon *new_entity_icon = a_malloc(a, entity_icon);
    new_entity_icon->name = name;
    return new_entity_icon;
}

entity *entity_new(int entity_number, string *name, int pos_x, int pos_y, entity_icon *icon, arena *a) {
    entity *new_entity = a_malloc(a, entity);
    new_entity->entity_number = entity_number;
    new_entity->name = name;
    new_entity->position_x = pos_x;
    new_entity->position_y = pos_y;
    new_entity->icon = icon;
    return new_entity;
}

string entity_string(entity *entity, string_buffer *buf, bool append_comma) {
    string_buffer_clear(buf);
    // entity_number
    string_buffer_add_str(buf, string("{\"entity_number\":"));
    string_buffer_add_int(buf, entity->entity_number);
    // name
    string_buffer_add_str(buf, string(",\"name\":\""));
    string_buffer_add_str(buf, *entity->name);
    // position: x
    string_buffer_add_str(buf, string("\",\"position\":{\"x\":"));
    string_buffer_add_int(buf, entity->position_x);
    // position: y
    string_buffer_add_str(buf, string(",\"y\":"));
    string_buffer_add_int(buf, entity->position_y);
    string_buffer_add_str(buf, string("}"));
    if (entity->icon) {
        // entity: type
        // entity: name
        string_buffer_add_str(buf, string(",\"icon\": {\"type\": \"virtual\",\"name\": \""));
        string_buffer_add_str(buf, *entity->icon->name);
        // show_in_chart
        string_buffer_add_str(buf, string("\"},\"show_in_chart\": true"));
    }
    // for comma-seperated list of entities
    if (append_comma) {
        string_buffer_add_str(buf, string("},"));
    } else {
        string_buffer_add_str(buf, string("}"));
    }
    return buf->str;
}

void blueprint_add_entity(blueprint *blueprint, entity *entity, arena *a) {
    entity_node *new_entity_node = a_malloc(a, entity_node);
    new_entity_node->entity = entity;
    if (blueprint->entities.head) {
        blueprint->entities.head->next = new_entity_node;
    } else {
        blueprint->entities.tail = new_entity_node;
    }
    blueprint->entities.head = new_entity_node;
}
