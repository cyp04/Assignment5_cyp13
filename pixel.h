#ifndef PIXEL_H
#define PIXEL_H

// One piexel contains three color intensities and its screen coordinates.
struct _pixel
{

	float r; // Red intensity, from 0.0 to 1.0
	float g; // Green intensity, from 0.0 to 1.0
	float b; // Blue intensity, from 0.0 to 1.0
	float x; // Horizontal position: 0 through 511 for this image.
	float y; // Vertical position: 0 through 255 for this image.
};
typedef struct _pixel Pixel; // Allosw the shorter type name Pixel

#endif // End of the PIXEL_H header guard
