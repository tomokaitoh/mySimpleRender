#pragma once
#include<vector>
#include<string>
#include<fstream>
#include<regex>
#include<map>
#include"myGUI.hpp"
#include"Mathematics.hpp"

class Texture{
public:
    int width,height;
    void open(const char *path);
    Texture(const char *path);
    Texture(const Texture& t);
    Texture():pixels(NULL){};
    ~Texture();
    Color& getPixel(int x,int y);
private:
    Color *pixels;
};

class Triangle{
public:
    int vert[3];
    int tex[3];
    int normal[3];
    int textureID;

    Triangle(std::smatch &res,int texID);
    Triangle() = default;
};
/* Model类
 *--模型的顶点，纹理坐标，法线，纹理，三角形
 */
class Model{
public:
    std::vector<vec4f> vertices;
    std::vector<vec2f> texCoords;
    std::vector<vec4f> normals;
    std::vector<Texture> textures;
    std::vector<Triangle> triangles;
    bool isOpen;
    vec4f pos;

    void open(const char* path);
    void scale(float value);
    void displayInfo();

    Model(const char* file,int scaleValue=1);
    Model():isOpen(false),pos(0,0,0,0){};
};

class Camera{
public:
    vec4f pos,up,right,direction;
    float zNear,zFar,aspect,fov;

    void lookat(vec4f target);
    void moveY(float step);

    Camera(vec4f _pos,vec4f _direction,vec4f _up):
        pos(_pos),
        direction(_direction),
        up(_up),
        right(Math::cross(_direction,_up))
    {};
    Camera() = default;
};
#include"Model.cpp"