#pragma once
#ifndef _MODEL_H_ 
#define _MODEL_H_
#include <iostream>
#include <vector>
#include "Vec.h"
#include <fstream>
#include <sstream>
using namespace std;

class Model
{
private:
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<Vec2f> uvs;
	vector<vector<int>> faces;
public:
	Model(const char* filename) {
		std::ifstream in; 
        in.open(filename, std::ifstream::in);
        if (in.fail()) return;
        std::string line;
        while (!in.eof()) {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            float f1, f2, f3;
            if (!line.compare(0, 2, "v ")) {
                iss >> trash >> f1 >> f2 >> f3;
                vertices.emplace_back(f1, f2, f3);
            }
            else if (!line.compare(0, 2, "vt")) {
                iss >> trash >> f1 >> f2;
                uvs.emplace_back(f1, f2);
            }
            else if (!line.compare(0, 2, "vn")) {
                iss >> trash >> f1 >> f2 >> f3;
                normals.emplace_back(f1, f2 ,f3);
            }
            else if (!line.compare(0, 2, "f ")) {
                std::vector<int> f;
                int itrash, iv, ivt, ivn;
                iss >> trash;
                while (iss >> iv >> trash >> ivt >> trash >> ivn) {
                    iv--; // in wavefront obj all indices start at 1, not zero
                    f.push_back(iv);
                }
                faces.push_back(f);
            }
        }
        std::cerr << "# v# " << vertices.size() << " f# " << faces.size() << std::endl;
	}

    int nverts() { return vertices.size(); }
    int nfaces() { return faces.size(); }
    Vec3f vert(int i) { return vertices[i]; }
    vector<int> face(int i) { return faces[i]; }
};

#endif // 


