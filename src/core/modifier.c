#include "modifier.h"
#include <glib.h>

VektorShapeNode vektor_shapenode_new(VektorShape* shape) {
    VektorShapeNode node = (VektorShapeNode){
        .base = shape, 
        .modifier_count = 0, 
        .evaluated = shape,
        .base_dirty = true
    };
    return node;
}

void vektor_shapenode_free(VektorShapeNode* shapeNode) {
    if(shapeNode->base == shapeNode->evaluated) {
        free(shapeNode->base); // avoid double free()
    } else {
        free(shapeNode->base);
        free(shapeNode->evaluated);
    }
    free(shapeNode->modifiers);
}

VektorShape* vektor_shapenode_get_evaluated(VektorShapeNode* shapeNode) {
    return shapeNode->evaluated;
}

VektorShape vektor_modifier_apply(VektorModifier* mod, VektorShape* input) {
    mod->cachedEvaluatedShape = mod->apply(mod, input);
    return mod->cachedEvaluatedShape;
}

// lots of copies by value here, could be problematic
void vektor_shapenode_update(VektorShapeNode* shapeNode) {
    // if the base is dirty, apply EVERY modifier
    if(shapeNode->base_dirty) {
        VektorShape* evaluated = shapeNode->base;
        for(size_t i = 0; i < shapeNode->modifier_count; i++) {
            *evaluated = vektor_modifier_apply(&shapeNode->modifiers[i], evaluated);
            shapeNode->modifiers[i].dirty = false;
        }
        shapeNode->evaluated = evaluated;
        shapeNode->base_dirty = false;
        return;
    }

    // if the base is not dirty, start applying modifiers upstream
    // starting from the first dirty
    bool encountered_dirty = false;
    
    for(size_t i = 0; i < shapeNode->modifier_count; i++) {
        if(shapeNode->modifiers[i].dirty) { encountered_dirty = true; }
        if(encountered_dirty) {
            if(i == 0) {
                vektor_modifier_apply(&shapeNode->modifiers[i], shapeNode->base);
            } else {
                vektor_modifier_apply(&shapeNode->modifiers[i], &shapeNode->modifiers[i - 1].cachedEvaluatedShape);
            }
            shapeNode->modifiers[i].dirty = false;
        }
    }

    if (encountered_dirty) {
        *shapeNode->evaluated = shapeNode->modifiers[shapeNode->modifier_count - 1].cachedEvaluatedShape;
    }

}

void vektor_shapenodebuf_add(VektorShapeNodeBuffer* buffer,
                             VektorShapeNode node) {
    if (buffer->count >= buffer->capacity) {
        buffer->capacity = buffer->capacity ? buffer->capacity * 2 : 4;
        buffer->nodes =
            realloc(buffer->nodes, sizeof(VektorShapeNode) * buffer->capacity);
    }
    buffer->nodes[buffer->count++] = node;

    if (buffer->count <= buffer->capacity / 4) {
        buffer->capacity /= 2;
        buffer->nodes =
            realloc(buffer->nodes, sizeof(VektorShapeNode) * buffer->capacity);
    }
}