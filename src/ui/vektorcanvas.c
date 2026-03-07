#include "epoxy/gl.h"
#include "gtk/gtk.h"

#include "../core/raster.h"
#include "src/core/primitives.h"
#include "uicontroller.h"
#include "vektorcanvas.h"
#include <epoxy/gl_generated.h>

#define VKTR_CANVAS_WIDTH 400
#define VKTR_CANVAS_HEIGHT 400
#define VKTR_CANVAS_SIZE (VKTR_CANVAS_WIDTH * VKTR_CANVAS_HEIGHT * 4)

char* read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0'; // null-terminate
    fclose(f);
    return buffer;
}

static GLuint shader_program;
static GLuint vao;
VertexBuffer vb;

static GLuint compile_shader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        g_error("Shader compile failed: %s", info);
    }
    return shader;
}

static void init_shader(void) {
    char* vert_src = read_file("./shaders/triangle.vert.glsl");
    char* frag_src = read_file("./shaders/triangle.frag.glsl");

    if (!vert_src || !frag_src)
        g_error("Failed to load shader files");

    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vert_src);
    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, frag_src);

    printf("%s\n", vert_src);
    printf("%s\n", frag_src);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex);
    glAttachShader(shader_program, fragment);
    glLinkProgram(shader_program);

    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(shader_program, 512, NULL, info);
        g_error("Shader link failed: %s", info);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

static void init_geometry(void) {
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, coords));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
                          (void*)offsetof(Vertex, color));

    glBindVertexArray(0);
}
static gboolean render(GtkGLArea* area, GdkGLContext* context,
                       VektorShapeBuffer* prims) {
    vektor_rasterize(&vb, prims);

    for (size_t i = 0; i < vb.count; i++) {
        printf("Vertex %zu: x=%f, y=%f\n", i, vb.vertices[i].coords.x,
               vb.vertices[i].coords.y);
    }

    glBufferData(GL_ARRAY_BUFFER, vb.count * sizeof(Vertex), vb.vertices,
                 GL_STATIC_DRAW);

    glUseProgram(shader_program);

    GLuint uProjectionLoc = glGetUniformLocation(shader_program, "uProjection");
    float projectionMatrix[16] = {1, 0, 0, 0, 0, 1, 0, 0,
                                  0, 0, 1, 0, 0, 0, 0, 1};
    glUniformMatrix4fv(uProjectionLoc, 1, GL_FALSE, projectionMatrix);

    glBindVertexArray(vao);
    glDisable(GL_CULL_FACE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, vb.count);
    GLenum err = glGetError();
    printf("OpenGL error: %x\n", err);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    return TRUE;
}

static void dump_gl_info(GtkGLArea* area) {
    gtk_gl_area_make_current(area);

    if (gtk_gl_area_get_error(area)) {
        g_warning("Failed to make GL context current");
        return;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* shading = glGetString(GL_SHADING_LANGUAGE_VERSION);

    g_debug("GL Vendor    : %s", vendor);
    g_debug("GL Renderer  : %s", renderer);
    g_debug("GL Version   : %s", version);
    g_debug("GLSL Version : %s", shading);

    GLint n;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    g_debug("Supported extensions (%d):", n);
    for (GLint i = 0; i < n; ++i) {
        g_debug("  %s", glGetStringi(GL_EXTENSIONS, i));
    }
}

static void realize(GtkGLArea* area, gpointer user_data) {
    gtk_gl_area_make_current(area);

    if (gtk_gl_area_get_error(area) != NULL)
        return;

    glEnable(GL_DEBUG_OUTPUT);
    dump_gl_info(area);
    init_shader();
    init_geometry();
}

void vektor_canvas_init(VektorWidgetState* state, VektorCanvas* canvasOut,
                        VektorShapeBuffer* prims) {
    canvasOut->canvasWidget = state->workspaceCanvas;
    canvasOut->width = VKTR_CANVAS_WIDTH;
    canvasOut->height = VKTR_CANVAS_HEIGHT;
    canvasOut->canvasPixels = g_malloc0(VKTR_CANVAS_SIZE);

    canvasOut->canvasPixelBytes =
        g_bytes_new(canvasOut->canvasPixels, VKTR_CANVAS_SIZE);
    canvasOut->canvasTexture = gdk_memory_texture_new(
        VKTR_CANVAS_WIDTH, VKTR_CANVAS_HEIGHT, GDK_MEMORY_R8G8B8A8,
        canvasOut->canvasPixelBytes, VKTR_CANVAS_WIDTH * 4);

    g_signal_connect(canvasOut->canvasWidget, "realize", G_CALLBACK(realize),
                     NULL);
    g_signal_connect(canvasOut->canvasWidget, "render", G_CALLBACK(render),
                     prims);
}
