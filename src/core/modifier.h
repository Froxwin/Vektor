#ifndef VKTR_MODIFIER_H
#define VKTR_MODIFIER_H

#include "src/core/primitives.h"

typedef enum { VEKTOR_MODIFIER_BEVEL } VektorModifierType;

typedef struct VektorModifier {
    VektorModifierType type;
    bool enabled;
    bool dirty;
    void* parameters;

    VektorShape (*apply)(struct VektorModifier mod, VektorShape input);

} VektorModifier;

typedef struct VektorShapeNode {
    VektorShape base;
    VektorShape evaluated;

    VektorModifier* modifiers;
    size_t modifier_count;

    bool base_dirty;
} VektorShapeNode;

typedef struct VektorShapeNodeBuffer {
    VektorShapeNode* nodes;
    size_t count;
    size_t capacity;
} VektorShapeNodeBuffer;

VektorShapeNode vektor_shapenode_new(VektorShape shape);
VektorShape* vektor_shapenode_get_evaluated(VektorShapeNode* shapeNode);
void vektor_shapenode_modifier_add(VektorShapeNode* shapeNode,
                                   VektorModifier* mod);
void vektor_shapenode_modifier_remove(VektorShapeNode* shapeNode,
                                      VektorModifier* mod);
void vektor_shapenode_free(VektorShapeNode* shapeNode);

void vektor_shapenodebuf_add(VektorShapeNodeBuffer* buffer,
                             VektorShapeNode node);

#endif