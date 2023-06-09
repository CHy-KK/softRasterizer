#pragma once
#ifndef _MODEL_H_ 
#define _MODEL_H_
#include <iostream>
#include <vector>
#include "Vec.h"
#include "Matrix.h"
#include <fstream>
#include <sstream>
#include "GameObject.h"
#include "Shader.h"
using namespace std;


class Model: public GameObject
{
private:
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<Vec2f> uvs;
	vector<vector<Vec3i>> faces;    // faces 存储每个三角面的三个顶点，每个顶点使用一个vec3i来表示使用的：模型坐标index/uv坐标index/法线index
    shared_ptr<Shader> shader;

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
            else if (!line.compare(0, 4, "vt  ")) {
                iss >> trash >> trash >> f1 >> f2 >> f3;
                uvs.emplace_back(f1, f2);
            }
            else if (!line.compare(0, 4, "vn  ")) {
                iss >> trash >> trash >> f1 >> f2 >> f3;
                normals.emplace_back(f1, f2 ,f3);
            }
            else if (!line.compare(0, 2, "f ")) {
                std::vector<Vec3i> f;
                int itrash, iv, ivt, ivn;
                iss >> trash;
                while (iss >> iv >> trash >> ivt >> trash >> ivn) {
                    f.emplace_back(--iv, --ivt, --ivn); // in wavefront obj all indices start at 1, not zero
                }
                faces.push_back(f);
            }
        }
        std::cerr << "# v# " << vertices.size() << " f# " << faces.size() << std::endl;
	}

    int nverts() const { return vertices.size(); }
    int nfaces() const { return faces.size(); }
    Vec3f vert(int i) const { return vertices[i]; }
    Vec2f uv(int i) const { return uvs[i]; }
    Vec3f normal(int i) const { return normals[i]; }
    vector<Vec3i> face(int i) const { return faces[i]; }
    void setShader(shared_ptr<Shader> s) { shader = s; }
    shared_ptr<Shader> getShader() const { return shader; }

    //virtual Matrix transformMatrix();
    
};



#endif // 


