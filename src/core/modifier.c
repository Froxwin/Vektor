#include "modifier.h"

VektorShapeNode vektor_shapenode_new(VektorShape shape) {
    VektorShapeNode node = (VektorShapeNode){
        .base = shape, .modifier_count = 0, .evaluated = shape};
    return node;
}

VektorShape* vektor_shapenode_get_evaluated(VektorShapeNode* shapeNode) {

    return &shapeNode->evaluated;
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