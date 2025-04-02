#ifndef BLUEPRINT_GENERATOR_H_
#define BLUEPRINT_GENERATOR_H_

#include "arena.h"
#include "factorio_ui.h"
#include "blueprint.h"
#include "my_string.h"

// Compression chunk size (16 KB)
#define Z_CHUNK 2<<14

/**
 * Generates a blueprint for displaying an image.
 */
blueprint Generate_Image_Blueprint(Factorio_Icon_Frame icon_frame, arena *a);

/**
 * Generates a blueprint for displaying a video.
 */
blueprint Generate_Video_Blueprint(Factorio_Icon_Video icon_video, arena *a);

/**
* Encodes the given string in base64 format, appending it to the provided buffer.
* \param src Data to be encoded
* \param dest String buffer to store the encoded data
* \returns The length of the encoded string (0 on error)
*/
ptrdiff_t Base64_Encode(string src, string_buffer* dest);

/**
 * Generates an encoded string from a blueprint.
 */
void Encode_Blueprint(blueprint blueprint, string_buffer *out, arena scratch);

#endif // BLUEPRINT_GENERATOR_H_