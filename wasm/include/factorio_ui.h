#ifndef FACTORIO_UI_H_
#define FACTORIO_UI_H_

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "arena.h"
#include "colors.h"
#include "my_string.h"

typedef struct Factorio_Icon_ {
    string name;
    Oklab avg_oklab;
    SDL_Texture *texture;
} Factorio_Icon;

typedef struct Factorio_Icon_Frame_ {
    int16_t *icon_ids;
    int width;
    int height;
    int spacing;
} Factorio_Icon_Frame;

typedef struct Factorio_Icon_Video_ {
    Factorio_Icon_Frame *frames;
    int num_frames;
    int ticks_per_frame;
} Factorio_Icon_Video;

typedef enum Display_Render_Flags_ {
    DISPLAY_RENDER_DEFAULT = 0x00,
    DISPLAY_RENDER_BINARY = 0x01,
    DISPLAY_RENDER_DITHERING = 0x02,
    DISPLAY_RENDER_TRANSPARENT = 0x04
} Display_Render_Flags;

typedef enum Factorio_Icon_Resolution_ {
    RES_VERY_LARGE = 0,
    RES_LARGE = 1,
    RES_MEDIUM = 2,
    RES_SMALL = 3
} Factorio_Icon_Resolution;

/**
 * Gets the bounding box for an icon based on the desired resolution.
 */
SDL_Rect Get_Icon_Rect(Factorio_Icon_Resolution resolution);

/**
 * Loads Factorio icons from the local file system.
 * \returns true on success or false on failure
 */
bool Load_Icons(arena *a);

/**
 * Frees all resources from the saved icon list.
 */
void Free_Icons();

/**
 * Finds the icon that matches the provided color best.
 * \param oklab The target icon color (in Oklab format)
 * \returns A pointer to the best matching icon.
 */
int16_t Get_Icon_By_Color(Oklab oklab);

/**
 * Finds the icon that matches the provided color best
 * (either black or white).
 * \param oklab The target icon color (in Oklab format)
 * \returns A pointer to the best matching icon.
 */
int16_t Get_Icon_By_Value(Oklab oklab);

/**
 * Gets the specified icon.
 * \param icon_id The id of the target icon
 * \returns A pointer to the requested icon (may be NULL)
 */
Factorio_Icon *Get_Icon(int16_t icon_id);

/**
 * Generates a row-wise array of Factorio icons from the provided frame data.
 * \param data A stream of pixel data (in 32-bit RGBA format)
 * \param width The width (in pixels) of the provided image
 * \param height The height (in pixels) of the provided image
 * \param fmt The target rendering flags, or 0 for none
 * \param arena The arena for allocating necessary data structures
 * \returns A row-wise array of Factorio icon ids.
 */
Factorio_Icon_Frame Generate_Icon_Frame(RGBA* data, int width, int height, int spacing, Display_Render_Flags fmt, arena *a);

/**
 * Generates an array of Factorio icon frames from the provided frame data.
 * \param data A stream of pixel data (in 32-bit RGBA format)
 * \param width The width (in pixels) of each frame
 * \param height The height (in pixels) of each frame
 * \param num_frames The number of frames in the pixel stream
 * \param ticks_per_frame The delay between frames (in ticks, or 1/60 sec intervals)
 * \param fmt The target rendering flags, or 0 for none
 * \param arena The arena for allocating necessary data structures
 * \returns A row-wise array of Factorio icon ids.
 */
Factorio_Icon_Video Generate_Icon_Video(RGBA* data, int width, int height, int spacing, int num_frames, int ticks_per_frame, Display_Render_Flags fmt, arena *a);

#endif // FACTORIO_UI_H_