#include <fstream>

using namespace std;

struct RGB {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    RGB(char r, char g, char b):
    red(r), green(r), blue(b) {}
};

class Bitmap {
    struct FileHeader {
        char bm[2]; // [B,M] for .bmp files.
        int size;
        short reserved1;
        short reserved2;
        int offset;
    }fh;

    struct ImageHeader {
        int size;
        int Width;
        int Height;
        short Planes;
        short BitsPerPixel;
        int Compression;
        int SizeOfBitmap;
        int HorzResolution;
        int VertResolution;
        int ColorsUsed;
        int ColorsImportant;
    }ih;
    char *data;
    // -Pointers to the data; this will allow us to manipulate
    //  the data as if it where in a matrix.
    RGB **img;
    // -The point (X,Y) will describe the position of the
    //  pencil at the moment of drawing something.
    int X, Y;

public:
	// -Building from a file.
    Bitmap(const char* fname);
	// -White image.
    Bitmap(int width, int height = -1);

	// -If not exist, creates a new file naming it with the string
	//  pointed by the fname pointer. In other case just overwrites
	//  the file.
    void save(const char *fname);

    ~Bitmap();

	// -Paints the entire image with the color passed.
    void clear(RGB color);

    // -Sets the values of (X, Y);
    void moveTo(int x, int y);

	// -Draw a line from (X, Y) to (x, y).
	// -At the end, updates the point (X, Y)
	//  with (x,y), i.e. (Y,Y) <- (x,y).
    void lineTo(RGB color, int x, int y);

    // -Function R -> R.
    double f(double x);

    // -Function R^2 -> R.
    double f(double x, double y);

	// -Graph of the f function.
    void graphfx(const RGB &color, double a, double b);

    // -Graph of the f function.
    void graphfxy(double ax, double ay, double bx, double by);

    friend ostream& operator << (ostream &stream, const Bitmap &bmp);
};
