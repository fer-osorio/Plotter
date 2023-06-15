/* main.cpp
 *
 * Copyright 2023 Fernando Osorio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <iostream>
#include "BMP/Bitmap.hpp"

using namespace std;

class Plotter : private Bitmap {
public:
    Plotter(const char *function, int width, int height = -1) :
    Bitmap(width, function, height), xy(has_xy()) {}

    bool xy;

    void plot(double ax, double ay , double bx = 0, double by = 0) {
        RGB color(0xFF, 0xFF, 0xFF);
        char fname[] = "/home/alexisfernandoosoriosarabio/Projects/Plotter/"
                       "Plot.bmp";
        if(xy) {
            //try {
                graphfxy(ax, ay, bx, by);
            //} catch(const char* e) {
                //cout << e << endl;
                //throw ;
            //}
        }
        else {
            //try {
                graphfx(color, ax, ay);
            //} catch(const char* e) {
                //cout << e << endl;
                //throw ;
            //}
        }
        save(fname);
    }
};

int main ()
{
    int i = 0;
    char exp[100], bf;
    double ax = -1, ay = -1, bx = 1, by = 1;

    cout << "\nEnter the function to plot:\n";
    while((bf = getchar()) != '\n') {
        if(bf != ' ') {
            if(bf == '=') i = 0;
            else exp[i++] = bf;
        }
    }
    exp[i] = 0;
    Plotter plotter(exp, 1024);

    cout << "\nEnter the plotting domain:\n";
    if(plotter.xy) {
        cout << "\nax = ";
        cin >> ax;
        cout << "\nay = ";
        cin >> ay;
        cout << "\nbx = ";
        cin >> bx;
        cout << "\nby = ";
        cin >> by;
    } else {
        cout << "\na = ";
        cin >> ax;
        cout << "\nb = ";
        cin >> ay;
    }
    plotter.plot(ax, ay, bx, by);
}
