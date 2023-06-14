#include"Bitmap.hpp"

using namespace std;

Bitmap::Bitmap(const char* fname) : X(0), Y(0) {
    ifstream file;
    file.open(fname, ios::binary);
    int i, j;
    if(file.is_open()) {
        file.read((char*)fh.bm, 2);
        if(fh.bm[0] == 'B' && fh.bm[1] == 'M') {
            // -File Header.
            file.read((char*)&fh.size, 4);
            file.read((char*)&fh.reserved1, 2);
            file.read((char*)&fh.reserved2, 2);
            file.read((char*)&fh.offset, 4);
            // -Image Header.
            file.read((char*)&ih.size, 4);
            file.read((char*)&ih.Width, 4);
            file.read((char*)&ih.Height, 4);
            file.read((char*)&ih.Planes, 2);
            file.read((char*)&ih.BitsPerPixel, 2);
            file.read((char*)&ih.Compression, 4);
            file.read((char*)&ih.SizeOfBitmap, 4);
            file.read((char*)&ih.HorzResolution, 4);
            file.read((char*)&ih.VertResolution, 4);
            file.read((char*)&ih.ColorsUsed, 4);
            file.read((char*)&ih.ColorsImportant, 4);
            data = new char[ih.SizeOfBitmap];
            file.read((char*)data, ih.SizeOfBitmap);

            // -Matrix of dimension (ih.Height, ih.Width) where
            //  each of its elements is 'RGB color' of 3 bytes.
            for(i = ih.Height - 1, j = 0; i >= 0; i--, j++) {
                img[j] = (RGB*)&data[3 * i * ih.Width];
            }
        } else {
            throw "Not a valid bitmap file.";
        }
    } else {
        throw "File could not be opened/created.";
    }
}

Bitmap::Bitmap(int width, int height) : X(0), Y(0) {
    int i, j, h, w = width, s;
    w -= w % 4;
    h = height;
    if(h == -1) h = w;
    s = h * w * 3;
    // -File Header.
    fh.bm[0] = 'B';
    fh.bm[1] = 'M';
    fh.size = s + 54;
    fh.reserved1 = 0;
    fh.reserved2 = 0;
    fh.offset = 54;
    // -Image Header.
    ih.size = 40;
    ih.Width = w;
    ih.Height = h;
    ih.Planes = 1;
    ih.BitsPerPixel = 24;
    ih.Compression = 0;
    ih.SizeOfBitmap = s;
    ih.HorzResolution = 3780; // Why??
    ih.VertResolution = 3780; // Why??
    ih.ColorsUsed = 0;
    ih.ColorsImportant = 0;
    data = new char[ih.SizeOfBitmap];

    // -Matrix of dimension (ih.Height, ih.Width) where
    //  each of its elements is 'RGB color' of 3 bytes.
    for(i = 0; i < ih.SizeOfBitmap; i++) data[i] = 0xFF;

    img = new RGB*[h];
    for(i = h - 1, j = 0; i >= 0; i--, j++) {
        img[j] = (RGB*)&data[3 * i * w];
    }
}

void Bitmap::save(const char *fname) {
    ofstream file;
    file.open(fname, ios::binary);
    if(file.is_open()) {
        file.write((char*)fh.bm, 2);
        if(fh.bm[0] == 'B' && fh.bm[1] == 'M') {
            // -File Header.
            file.write((char*)&fh.size, 4);
            file.write((char*)&fh.reserved1, 2);
            file.write((char*)&fh.reserved2, 2);
            file.write((char*)&fh.offset, 4);
            // -Image Header.
            file.write((char*)&ih.size, 4);
            file.write((char*)&ih.Width, 4);
            file.write((char*)&ih.Height, 4);
            file.write((char*)&ih.Planes, 2);
            file.write((char*)&ih.BitsPerPixel, 2);
            file.write((char*)&ih.Compression, 4);
            file.write((char*)&ih.SizeOfBitmap, 4);
            file.write((char*)&ih.HorzResolution, 4);
            file.write((char*)&ih.VertResolution, 4);
            file.write((char*)&ih.ColorsUsed, 4);
            file.write((char*)&ih.ColorsImportant, 4);
            file.write((char*)data, ih.SizeOfBitmap);
        } else {
            throw "Not a valid bitmap file.";
        }
    } else {
        throw "File could not be written.";
    }
}

Bitmap::~Bitmap() {
    if(data != NULL) delete data;
    if(img  != NULL) delete img;
}

void Bitmap::clear(RGB color) {
    int i, j;
    for(i = 0; i < ih.Height; i++)
        for(j = 0; j < ih.Width; j++)
            img[i][j] = color;
}

void Bitmap::moveTo(int x, int y) {
    X = x; Y = y;
}

void Bitmap::lineTo(RGB color, int x, int y) {
    // Using y = mx+b equation.
    int x1, x2, y1, y2;
    int i, j;
    double b, m;

    // -Setting starting and end point.
    if(x > X) {
        x1 = X; x2 = x;
        y1 = Y; y2 = y;
    } else {
        x1 = x; x2 = X;
        y1 = y; y2 = Y;
    }

    if(x1 == x2) { // -Vertical line.
        if(y1 > y2) {
            // -We can use x2 as a buffer because its value is
            //  the same as x1, so we are loosing nothing.
            x2 = y2; y2 = y1; y1 = x2;
        }
        for(i = y1, j = x1; i <= y2; i++) {
            if(j >= 0 && j < ih.Width && i >= 0 && i < ih.Height) {
                img[i][j] = color;
            }
        }
        // -Updating the position of the 'pencil'.
        X = x; Y = y;
        return;
    }
    m = (y2 - y1) / (double)(x2 - x1);

    if(m >= -1.0 && m <= 1.0) {
        b = y1 - m * x1;
        for(i = x1; i <= x2; i++) {
            j = m * i + b;
            if(i >= 0 && i < ih.Width && j >= 0 && j < ih.Height) {
                img[j][i] = color;
            }
        }
    } else {
        // -Resetting starting and end point.
        if(y > Y) {
            y1 = Y; y2 = y;
            x1 = X; x2 = x;
        } else {
            y1 = y; y2 = Y;
            x1 = x; x2 = X;
        }
        m = (x2 - x1) / (double)(y2 - y1); // <- Case x2 = x1 needs attention.
        b = x1 - m * y1;
        for(i = y1; i <= y2; i++) {
            j = m * i + b;
            if(i >= 0 && i < ih.Width && j >= 0 && j < ih.Height) {
                img[i][j] = color;
            }
        }
    }
    // -Updating the position of the 'pencil'.
    X = x; Y = y;
}

void Bitmap::graphfx(const RGB &color, double a, double b, FUNCTION1 f) {
    double *fx, deltaX;
    double x, max, min;
    int i, j;
    fx = new double[ih.Width];
    deltaX = (b - a) / ih.Width;
    for(i = 0, x = a; i < ih.Width; i++, x += deltaX) {
        fx[i] = f(x);
    }
    max = min = fx[0];
    for(i = 0; i < ih.Width; i++) {
        if(fx[i] > max) max = fx[i];
        if(fx[i] < min) min = fx[i];
    }
    x = max - min;
    for(i = 0; i < ih.Width; i++) {
        fx[i] = (fx[i] - min) * (ih.Height - 1) / x;
    }
    // -Setting starting point.
    j = ih.Height - fx[0] - 1;
    moveTo(0, j);
    // -Drawing f graph.
    for(i = 1; i < ih.Width; i++) {
        j = ih.Height - fx[i] - 1;
        lineTo(color, i, j);
    }
    delete[] fx;
}

void Bitmap::graphfxy(double ax,double ay,double bx,double by,FUNCTION2 f) {
    RGB color(0,0,0);
    double **fxy, deltaX, deltaY;
    double x, y, max, min;
    int i, j;
    //int RED = 0xFF, GREEN = RED << 8, BLUE = GREEN << 8;

    fxy = new double*[ih.Height];
    for(i = 0; i < ih.Height; i++) fxy[i] = new double[ih.Width];

    deltaX = (bx - ax) / ih.Width;
    deltaY = (by - ay) / ih.Height;
    for(i = 0, y = ay; i < ih.Height; i++, y += deltaY)
        for(j = 0, x = ax; j < ih.Width; j++, x += deltaX)
            fxy[i][j] = f(x,y);

    max = min = fxy[0][0];
    for(i = 0; i < ih.Height; i++) {
        for(j = 0; j < ih.Width; j++) {
            if(fxy[i][j] > max) max = fxy[i][j];
            if(fxy[i][j] < min) min = fxy[i][j];
        }
    }
    x = max - min;
    for(i = 0; i < ih.Height; i++) {
        for(j = 0; j < ih.Width; j++) {
            fxy[i][j] = (fxy[i][j] - min) * 255.0 / x;
            //fxy[i][j] = (fxy[i][j] - min) * 0xFFFFFF / x;
        }
    }
    // -Drawing f graph.
    for(i = 0; i < ih.Height; i++) {
        for(j = 0; j < ih.Width; j++) {
            /*color.red   =  RED   & (int)fxy[i][j];
            color.green = (GREEN & (int)fxy[i][j]) >> 8;
            color.blue  = (BLUE  & (int)fxy[i][j]) >> 16;*/
            color.red     = fxy[i][j] * 0.5 + 126;
            color.green   = fxy[i][j];
            color.blue    = fxy[i][j];
            img[i][j] = color;
        }
    }
    for(i = 0; i < ih.Height; i++) delete[] fxy[i];
    delete[] fxy;
}

ostream& operator << (ostream &stream, const Bitmap &bmp) {
    stream << "File Header: ";
    stream << "\n\tbm: " << bmp.fh.bm[0] << bmp.fh.bm[1];
    stream << "\n\tsize: " << bmp.fh.size;
    stream << "\n\treserved1: " << bmp.fh.reserved1;
    stream << "\n\treserved2: " << bmp.fh.reserved2;
    stream << "\n\toffset: " << bmp.fh.offset;

    stream << "\nImage Header: ";
    stream << "\n\tsize: " << bmp.ih.size;
    stream << "\n\twidth: " << bmp.ih.Width;
    stream << "\n\theight: " << bmp.ih.Height;
    stream << "\n\tplanes: " << bmp.ih.Planes;
    stream << "\n\tbits per pixel: " << bmp.ih.BitsPerPixel;
    stream << "\n\tcompression: " << bmp.ih.Compression;
    stream << "\n\timage size: " << bmp.ih.SizeOfBitmap;
    stream << "\n\thorizontal resolution: " << bmp.ih.HorzResolution;
    stream << "\n\tvertical resolution: " << bmp.ih.VertResolution;
    stream << "\n\tcolors used: " << bmp.ih.ColorsUsed;
    stream << "\n\tcolors important: " << bmp.ih.ColorsImportant;

    return stream;
}
    
