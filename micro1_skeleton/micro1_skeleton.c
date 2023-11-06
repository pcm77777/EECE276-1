#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


#include <math.h>
#include <stdio.h>
#include <float.h>


void mirror_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void grayScale_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void sobelFiltering_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);

int main()
{
 
	int height;
	int width;
	int channel;

	char command;

 	printf("Take a picture? (y/n)\n");
 	scanf("%c", &command);

 	if(command == 'n')
 		exit(1);


 	printf("Cheeze !\r\n");
	system("libcamera-still --width 640 --height 480 -o image.bmp");
 	
 	
 	unsigned char* imgIn = stbi_load("image.bmp", &width, &height, &channel, 3);


	unsigned char* imgOut_mirror = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
	unsigned char* imgOut_grayScale = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
	unsigned char* imgOut_sobelFiltering = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
 	
	mirror_transform(imgIn, height, width, channel, imgOut_mirror);
	grayScale_transform(imgIn, height, width, channel, imgOut_grayScale);
	sobelFiltering_transform(imgOut_grayScale, height, width, channel, imgOut_sobelFiltering);


	stbi_write_bmp("image_mirror.bmp", width, height, channel, imgOut_mirror);
	stbi_write_bmp("image_grayScale.bmp", width, height, channel, imgOut_grayScale);
	stbi_write_bmp("image_sobelFiltering.bmp", width, height, channel, imgOut_sobelFiltering);
	
	stbi_image_free(imgIn);
	free(imgOut_mirror);
	free(imgOut_grayScale);
	free(imgOut_sobelFiltering);
 

	return 0;
}

void mirror_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			for (int c = 0; c < channel; c++) {
				int src_index = (y * width + x) * channel + c;
				int dest_index = (y * width + (width - 1 - x)) * channel + c;
				out[dest_index] = in[src_index];
			}
		}
	}

}

void grayScale_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int sum = 0;
			for (int c = 0; c < channel; c++) {
				int index = (y * width + x) * channel + c;
				sum += in[index];
			}
			int gray_value = sum / channel;
			for (int c = 0; c < channel; c++) {
				int index = (y * width + x) * channel + c;
				out[index] = (unsigned char)gray_value;
			}
		}
	}

}

void sobelFiltering_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	int sobel_kernel_x[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
	int sobel_kernel_y[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };

	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			for (int c = 0; c < channel; c++) {
				int sum_x = 0;
				int sum_y = 0;
				for (int ky = -1; ky <= 1; ky++) {
					for (int kx = -1; kx <= 1; kx++) {
						int index = ((y + ky) * width + (x + kx)) * channel + c;
						sum_x += in[index] * sobel_kernel_x[ky + 1][kx + 1];
						sum_y += in[index] * sobel_kernel_y[ky + 1][kx + 1];
					}
				}
				int magnitude = (int)sqrt(sum_x * sum_x + sum_y * sum_y);
				if (magnitude > 255) {
					magnitude = 255;
				}
				int index = (y * width + x) * channel + c;
				out[index] = (unsigned char)magnitude;
			}
		}
	}
	
}
