#ifndef DISPLAY_RENDERER_H_
#define DISPLAY_RENDERER_H_

#include "factorio_ui.h"

/**
 * Initializes the display renderer.
 * \returns NULL on success, or an error message on failure
 */
const char *Renderer_Init();

/**
 * Gets the current renderer.
 * \returns an instance of the current renderer (may be NULL)
 */
SDL_Renderer *Get_Renderer();

/**
 * Renders the given icon frame to the canvas.
 */
void Render_Icon_Frame(Factorio_Icon_Frame icon_frame, Factorio_Icon_Resolution resolution);

/**
 * Closes and cleans up the display simulation.
 */
void Renderer_Close();

#endif // DISPLAY_RENDERER_H_
