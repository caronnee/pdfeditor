#ifndef ___IMAGE_MODULE__
#define ___IMAGE_MODULE__
#include <QWidget>
#include "ui_insertImage.h"
#include <kernel/pdfedit-core-dev.h>
#include "typedefs.h"
#include <png.h>

struct LoadedPng
{
	int x, y;

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	bool load(const std::string& file)
	{
		char header[8];    // 8 is the maximum size that can be checked

		/* open file and test for it being a png */
		FILE *fp = fopen(file.c_str(), "rb");
		if (!fp)
			//abort_("[read_png_file] File %s could not be opened for reading", file_name);
			return false;
		fread(header, 1, 8, fp);
		if (png_sig_cmp((unsigned char *)header, 0, 8))
			return false;
			//abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


		/* initialize stuff */
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (!png_ptr)
			//abort_("[read_png_file] png_create_read_struct failed");
			return false;

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
			//abort_("[read_png_file] png_create_info_struct failed");
			return false;

		if (setjmp(png_jmpbuf(png_ptr)))
			//abort_("[read_png_file] Error during init_io");
			return false;

		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);

		png_read_info(png_ptr, info_ptr);

		width = png_get_image_width(png_ptr, info_ptr);
		height = png_get_image_height(png_ptr, info_ptr);
		color_type = png_get_color_type(png_ptr, info_ptr);
		bit_depth = png_get_bit_depth(png_ptr, info_ptr);

		number_of_passes = png_set_interlace_handling(png_ptr);
		png_read_update_info(png_ptr, info_ptr);


		/* read file */
		if (setjmp(png_jmpbuf(png_ptr)))
			//abort_("[read_png_file] Error during read_image");
			return false;

		row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
		for (y=0; y<height; y++)
			row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

		png_read_image(png_ptr, row_pointers);

		fclose(fp);
	}
};
class InsertImage : public QWidget
{
	Q_OBJECT

	Ui::Image ui;
	//to, kde sa pridava obrazok, je uz v matici
public:
	InsertImage(QWidget * parent);
public slots:
	void apply();
	void rotationCm(int angle);
	void setImagePath();
	void setPosition(float f1, float f2);
signals:
	void insertImage(PdfOp op); //jake casti
};
#endif

