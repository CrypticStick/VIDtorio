#include <stdio.h>
#include "blueprint_generator.h"
#include "display_renderer.h"
#include "factorio_ui.h"
#include "arena.h"

// Global memory reference
static arena g_arena;

// Processing memory reference (resets to the global reference on Process_Init)
static arena proc_arena;

// Compressed blueprint string (for copying to javascript)
static string_buffer encoded_blueprint;

int main() {
    // Allocate 64 MB for general use.
    // Note that SDL may allocate additional memory,
    // but this will not affect the existing arena.
    g_arena = arena_new(1<<26);

    // Use 2 MB for the blueprint string output
    encoded_blueprint = string_buffer_new(1<<21, &g_arena);

    const char *error = Renderer_Init();
    if (error) {
        printf("Error: %s\n", error);
        return 1;
    }

    // Use additional space necessary to prepare all icon data
    if (!Load_Icons(&g_arena)) {
        printf("Error: failed to load game icons\n");
        return 1;
    }

    return 0;
}

/**
 * Allocates space for the incoming frame data and returns a pointer to the new buffer.
 * 
 * Do not use the global arena until the incoming pixel data has been fully processed!
 * \param num_pixels The total number of pixels in the data stream (in 32-bit RGBA format)
 * \returns A pointer to the newly created buffer
 */
uint32_t *Process_Init(ptrdiff_t num_pixels) {
    // Reset processing arena to the global arena state
    proc_arena = g_arena;
    return a_malloc(&proc_arena, uint32_t, num_pixels, NO_ZERO);
}

/**
 * Generates an encoded blueprint string from frame data and renders the resulting map view.
 * \param data A stream of pixel data (in 32-bit RGBA format)
 * \param width The width (in pixels) of the provided image
 * \param height The height (in pixels) of the provided image
 * \param res The target resolution for rendering the display icons
 * \param fmt The target rendering flags, or 0 for none
 * \returns An encoded blueprint string, or NULL on failure
 */
uint8_t *Process_Image(RGBA* data, int width, int height, int spacing, Factorio_Icon_Resolution res, Display_Render_Flags fmt) {
    // Copy processing arena to create a temporary arena
    arena scratch = proc_arena;

    // Generate and render the icon frame
    Factorio_Icon_Frame icon_frame = Generate_Icon_Frame(data, width, height, spacing, fmt, &scratch);
    Render_Icon_Frame(icon_frame, res);

    // Generate the blueprint
    blueprint frame_blueprint = Generate_Image_Blueprint(icon_frame, &scratch);

    // Generate the compressed blueprint string
    string_buffer_clear(&encoded_blueprint);
    Encode_Blueprint(frame_blueprint, &encoded_blueprint, scratch);
    if (string_buffer_terminate(&encoded_blueprint)) {
        return encoded_blueprint.str.data;
    } else {
        printf("Error: the encoded blueprint is too large for the string buffer\n");
        return NULL;
    }
}