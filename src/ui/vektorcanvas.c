#include "epoxy/gl.h"
#include "gtk/gtk.h"

#include "../core/raster.h"
#include "uicontroller.h"
#include "vektorcanvas.h"

#define VKTR_CANVAS_WIDTH 400
#define VKTR_CANVAS_HEIGHT 400
#define VKTR_CANVAS_SIZE (VKTR_CANVAS_WIDTH * VKTR_CANVAS_HEIGHT * 4)

static GLuint shader_program;
static GLuint vao;

static const char* vertex_shader_src =
    "#version 300 es\n" // <- ES version
    "layout(location = 0) in vec2 aPos;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\n";

static const char* fragment_shader_src =
    "#version 300 es\n"          // <- ES version
    "precision mediump float;\n" // required in ES for fragment color
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

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
    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

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
    // Vertices for a rectangle in NDC coordinates
    float vertices[] = {
        -0.5f, -0.5f, // bottom-left
        0.5f,  -0.5f, // bottom-right
        0.5f,  0.5f,  // top-right
        -0.5f, 0.5f   // top-left
    };
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    GLuint vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

static gboolean render(GtkGLArea* area, GdkGLContext* context) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    return TRUE;
}

static void realize(GtkGLArea* area, gpointer user_data) {
    gtk_gl_area_make_current(area);

    if (gtk_gl_area_get_error(area) != NULL)
        return; // context creation failed

    init_shader();
    init_geometry();
}

void vektor_canvas_init(VektorWidgetState* state, VektorCanvas* canvasOut) {
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
                     NULL);
    // gtk_picture_set_paintable(canvasOut->canvasWidget,
    //                           GDK_PAINTABLE(canvasOut->canvasTexture));
    // gtk_picture_set_content_fit(GTK_PICTURE(canvasOut->canvasWidget),
    //                             GTK_CONTENT_FIT_CONTAIN);
    // g_object_unref(bytes);
}

/* Generate new texture based on canvasPixels*/
void vektor_canvas_update(VektorCanvas* canvas) {
    g_bytes_unref(canvas->canvasPixelBytes);
    canvas->canvasPixelBytes =
        g_bytes_new(canvas->canvasPixels, VKTR_CANVAS_SIZE);

    g_object_unref(canvas->canvasTexture);
    canvas->canvasTexture = gdk_memory_texture_new(
        canvas->width, canvas->height, GDK_MEMORY_R8G8B8A8,
        canvas->canvasPixelBytes, canvas->width * 4);

    // gtk_picture_set_paintable(canvas->canvasWidget,
    //                           GDK_PAINTABLE(canvas->canvasTexture));
}

void vektor_canvas_fill(VektorCanvas* canvas, VektorColor color) {
    for (int x = 0; x < VKTR_CANVAS_WIDTH; x++) {
        for (int y = 0; y < VKTR_CANVAS_HEIGHT; y++) {
            int i = (y * VKTR_CANVAS_WIDTH + x) * 4;
            canvas->canvasPixels[i + 0] = color.r;
            canvas->canvasPixels[i + 1] = color.g;
            canvas->canvasPixels[i + 2] = color.b;
            canvas->canvasPixels[i + 3] = color.a;
        }
    }
}

void vektor_canvas_drawfrom(VektorFramebuffer* fb, VektorCanvas* target) {
    for (int x = 0; x < fb->width; x++) {
        for (int y = 0; y < fb->height; y++) {

            int i = (y * fb->width + x) * 4;
            target->canvasPixels[i + 0] = (guchar)fb->pixels[i + 0];
            target->canvasPixels[i + 1] = (guchar)fb->pixels[i + 1];
            target->canvasPixels[i + 2] = (guchar)fb->pixels[i + 2];
            target->canvasPixels[i + 3] = (guchar)fb->pixels[i + 3];
        }
    }
}