#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

Pixel** createImage(int width, int height) {
  cout << "Start createImage... " << endl;
  
  // Create a one dimensional array on the heap of pointers to Pixels 
  //    that has width elements (i.e. the number of columns)
  Pixel** image = new Pixel*[width];
  
  bool fail = false;
  
  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    image[i] = new Pixel[height];
    
    if (image[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }
  
  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image[i]; // deleting nullptr is not a problem
    }
    delete [] image; // dlete array of pointers
    return nullptr;
  }
  
  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = { 0, 0, 0 };
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel** image, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete [] image[i]; // delete each individual array placed on the heap
  }
  delete [] image;
  image = nullptr;
}

int* createSeam(int length) {
	int* seam = new int[length];
	for (int i = 0; i < length; i++) {
		seam[i] = 0;
	}
	return seam;
}

void deleteSeam(int* seam) {
	delete [] seam; 
}
bool loadImage(string filename, Pixel** image, int width, int height) {
	ifstream ifs (filename);
	if (!ifs.is_open()) {
		cout << "Error: failed to open input file - " << filename << endl;
		return false;
	}
	char type[3];
	ifs >> type;
	if ((toupper(type[0]) != 'P') || (type[1] != '3')) {
	cout << "Error: type is " << type << " instead of P3" << endl;
	ifs.close();
	return false;
	}
	int w = 0, h = 0;
	ifs >> w >> h;
	if (ifs.fail()) {
			cout << "Error: read non-integer value" << endl;
			ifs.close();
			return false;
		}
	if (w != width) {
		cout << "Error: input width (" << width << ") does not match value in file (" << w << ")" << endl;
		ifs.close();
		return false;
	}
    if (h != height) {
		cout << "Error: input height (" << height << ") does not match value in file (" << h << ")" << endl;
		ifs.close();
		return false;
    }

    int colorMax = 0;
    ifs >> colorMax;
    if (colorMax != 255) {
		ifs.close();
		return false;
    }
	int countTot = w * h * 3;
	int countNum = 0;
	int newVal = 0;
	for (int row = 0; row < height; ++row) {
	for (int col = 0; col < width; ++col) {
		ifs >> image[col][row].r >> image[col][row].g >> image[col][row].b;
		countNum += 3;
		if (ifs.fail()&&countNum < countTot) {
			cout << "Error: read non-integer value" << endl;
			ifs.close();
			return false;
		}
		if ((image[col][row].r < 0 || image[col][row].r > 255)) {
			cout << "Error: invalid color value " << image[col][row].r << endl;
			ifs.close();
			return false;
		} else if ((image[col][row].g < 0 || image[col][row].g > 255)) {
			cout << "Error: invalid color value " << image[col][row].g << endl;
			ifs.close();
			return false;
		} else if ((image[col][row].b < 0 || image[col][row].b > 255)) {
			cout << "Error: invalid color value " << image[col][row].b << endl;
			ifs.close();
			return false;
		}
		if (ifs.fail()) {
			cout << "Error: not enough color values" << endl;
			ifs.close();
			return false;
		}
		}
		}
		ifs.ignore();
		if (ifs.eof()) {
			ifs.close();
			return true;
		}
		ifs >> newVal;
		if (!ifs.eof()&&newVal>1) {
			cout << "Error: too many color values" << endl;
			ifs.close();
			return false;
		}
	ifs.close();
	return true;
}

bool outputImage(string filename, Pixel** image, int width, int height) {
	ofstream outfile;
	outfile.open(filename);
	if (!outfile) { 
	cout << "Error: failed to open output file - " << filename;
	return false;
	}
	outfile << "P3" << endl;
	outfile << width << " " << height << " " << endl;
	outfile << 255 << endl;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			outfile << image[j][i].r << " " << image[j][i].g << " " << image[j][i].b << " ";
			}
		outfile << endl;
		}
	return true;
}

int energy(Pixel** image, int x, int y, int width, int height) { 
	int gradX, gradY, sumGrad = 0;
	int rx = 0, gx = 0, bx = 0, ry = 0, gy = 0, by = 0;
	int maxW = width - 1;
	int maxH = height - 1;
	if (y == 0 && x == 0) { //both equal 0,0 
		rx = abs(image[1][0].r - image[maxW][0].r);
		gx = abs(image[1][0].g - image[maxW][0].g);
		bx = abs(image[1][0].b - image[maxW][0].b);
		ry = abs(image[0][1].r - image[0][maxH].r);
		gy = abs(image[0][1].g - image[0][maxH].g);
		by = abs(image[0][1].b - image[0][maxH].b);
	} else if (y == maxH && x == maxW) { //bottom right corner
		rx = abs(image[x-1][y].r - image[0][y].r);
		gx = abs(image[x-1][y].g - image[0][y].g);
		bx = abs(image[x-1][y].b - image[0][y].b);
		ry = abs(image[x][y-1].r - image[x][0].r);
		gy = abs(image[x][y-1].g - image[x][0].g);
		by = abs(image[x][y-1].b - image[x][0].b);
	} else if (y == 0 && x == maxW) { //top right corner
		rx = abs(image[x-1][y].r - image[0][y].r);
		gx = abs(image[x-1][y].g - image[0][y].g);
		bx = abs(image[x-1][y].b - image[0][y].b);
		ry = abs(image[x][1].r - image[x][maxH].r);
		gy = abs(image[x][1].g - image[x][maxH].g);
		by = abs(image[x][1].b - image[x][maxH].b);
	} else if (y == maxH && x == 0) {//bottom left corner
		rx = abs(image[x+1][y].r - image[maxW][y].r);
		gx = abs(image[x+1][y].g - image[maxW][y].g);
		bx = abs(image[x+1][y].b - image[maxW][y].b);
		ry = abs(image[x][0].r - image[x][y-1].r);
		gy = abs(image[x][0].g - image[x][y-1].g);
		by = abs(image[x][0].b - image[x][y-1].b);
	} else if (x == 0) { //leftmost column
		rx = abs(image[1][y].r - image[maxW][y].r);
		gx = abs(image[1][y].g - image[maxW][y].g);
		bx = abs(image[1][y].b - image[maxW][y].b);
		ry = abs(image[x][y+1].r - image[x][y-1].r);
		gy = abs(image[x][y+1].g - image[x][y-1].g);
		by = abs(image[x][y+1].b - image[x][y-1].b);
	} else if (y == 0) { //top row
		rx = abs(image[x+1][y].r - image[x-1][y].r);
		gx = abs(image[x+1][y].g - image[x-1][y].g);
		bx = abs(image[x+1][y].b - image[x-1][y].b);
		ry = abs(image[x][1].r - image[x][maxH].r);
		gy = abs(image[x][1].g - image[x][maxH].g);
		by = abs(image[x][1].b - image[x][maxH].b);
	} else if (y == maxH) { //bottom row
		rx = abs(image[x+1][y].r - image[x-1][y].r);
		gx = abs(image[x+1][y].g - image[x-1][y].g);
		bx = abs(image[x+1][y].b - image[x-1][y].b);
		ry = abs(image[x][y-1].r - image[x][0].r);
		gy = abs(image[x][y-1].g - image[x][0].g);
		by = abs(image[x][y-1].b - image[x][0].b);
	} else if (x == maxW) { //rightmost column
		rx = abs(image[x-1][y].r - image[0][y].r);
		gx = abs(image[x-1][y].g - image[0][y].g);
		bx = abs(image[x-1][y].b - image[0][y].b);
		ry = abs(image[x][y+1].r - image[x][y-1].r);
		gy = abs(image[x][y+1].g - image[x][y-1].g);
		by = abs(image[x][y+1].b - image[x][y-1].b);
	} else { //middle
		rx = abs(image[x+1][y].r - image[x-1][y].r);
		gx = abs(image[x+1][y].g - image[x-1][y].g);
		bx = abs(image[x+1][y].b - image[x-1][y].b);
		ry = abs(image[x][y+1].r - image[x][y-1].r);
		gy = abs(image[x][y+1].g - image[x][y-1].g);
		by = abs(image[x][y+1].b - image[x][y-1].b);
	}
	gradX = (rx*rx) + (gx*gx) + (bx*bx);
	gradY = (ry*ry) + (gy*gy) + (by*by);
	sumGrad = gradX + gradY;
	return sumGrad;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
    seam[0] = start_col;
    int energyTot = energy(image, start_col, 0,width, height);
    int energyMin = 0;
    int engL, engM, engR = 0;
    for (int i = 1; i < height; ++i) {
        if (start_col==width-1) {
            engR = energy(image, start_col-1, i, width, height);
            engM = energy(image, start_col, i, width, height);
            if (engR<engM) {
                start_col-=1;
            }
        } else if (start_col==0) {
            engL = energy(image, 1, i, width, height);
            engM = energy(image, 0, i, width, height);
            if (engL<engM) {
                start_col+=1;
            }
        } else {
            engR = energy(image, start_col-1, i, width, height);
            engM = energy(image, start_col, i, width, height);
            engL = energy(image, start_col+1, i, width, height);
            if ((engL<=engR)&&(engL<engM)) {
                start_col+=1;
            } else if (engR<engM) {
                start_col-=1;
            }
        }
        seam[i] = start_col;
        energyMin = energy(image, start_col, i, width, height);
        energyTot+=energyMin;
    }
    return energyTot;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
    seam[0] = start_row;
    int energyTot = energy(image, 0,start_row,width, height);
    int energyMin = 0;
    int engT, engM, engB = 0;
    for (int i = 1; i < width; ++i) {
        if (start_row==height-1) {
            engT = energy(image, i, start_row - 1, width, height);
            engM = energy(image, i, start_row, width, height);
            if (engT<engM) {
                start_row-=1;
            }
        } else if (start_row==0) {
            engM = energy(image, i, 0, width, height);
            engB = energy(image, i, 1, width, height);
            if (engB<engM) {
                start_row+=1;
            }
        } else {
            engT = energy(image, i, start_row-1, width, height);
            engM = energy(image, i, start_row, width, height);
            engB = energy(image, i, start_row+1, width, height);
            if ((engT<=engB)&&(engT<engM)) {
                start_row -= 1;
            } else if (engB<engM) {
                start_row+=1;
            }
        }
        seam[i] = start_row;
        energyMin=energy(image, i, start_row, width, height);
        energyTot+=energyMin;
    }
    return energyTot;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
	int* verSeam = createSeam(height);
	int minEng = loadVerticalSeam(image,0,width,height,verSeam);
	int colEng = 0;
	for (int i = width-1; i > 0; --i) {
		int* tempVerSeam = createSeam(height);
		colEng = loadVerticalSeam(image, i, width, height, tempVerSeam);
		if (colEng<minEng) {
			minEng=loadVerticalSeam(image, i, width, height, verSeam);
		}
		deleteSeam(tempVerSeam);
	}
    return verSeam;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
	int* horSeam = createSeam(width);
	int minEng = loadHorizontalSeam(image,0,width,height,horSeam);
	int rowEng = 0;
	for (int i = height-1; i > 0; --i) {
		int* tempHorSeam = createSeam(width);
		rowEng = loadHorizontalSeam(image, i, width, height, tempHorSeam);
		if (rowEng<minEng) {
			minEng=loadHorizontalSeam(image, i, width, height, horSeam);
		}
		deleteSeam(tempHorSeam);
	}
    return horSeam;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
	for (int row = 0; row < height; ++row) {
		for (int col = verticalSeam[row]; col < width-1; ++col) {
			image[col][row].r = image[col+1][row].r;
			image[col][row].g = image[col+1][row].g;
			image[col][row].b = image[col+1][row].b;
		}
	}
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
	for (int col = 0; col < width; ++col) {
		for (int row = horizontalSeam[col]; row < height-1; ++row) {
			image[col][row].r = image[col][row+1].r;
			image[col][row].g = image[col][row+1].g;
			image[col][row].b = image[col][row+1].b;
		}
	}
}
