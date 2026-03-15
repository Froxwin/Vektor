#include "../modifier.h"

static VektorShape vektor_m_identity_apply(VektorModifier* m, VektorShape* input) {
    return *input;
}

VektorModifier vektor_m_identity_new() {
    return (VektorModifier) {
        .type = VEKTOR_MODIFIER_IDENTITY,
        .enabled = true,
        .dirty = true,
        .parameters = NULL,
        .apply = vektor_m_identity_apply
    };
}

