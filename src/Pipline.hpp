#pragma once
#include<ctime>
#include<cmath>
#include"myGUI.hpp"
#include"Model.hpp"
#include"tools.hpp"
#define OrthogonalMode 0
#define PerspectiveMode 1
#define WireframeMode 0
#define RasterizationMode 1
#define Msaa9x 9
#define Msaa5x 5
class Pipline;

/* Opinion类 
 * -- 渲染模型时的参数
 * -- 投影模式，渲染模式，抗锯齿开关
 */
class RenderOpinion{
public:
    int projectionMode,renderMode;
    int MsaaMode;
    bool isAutoMoveCamera;
    void displayInfo();
    void switchProjectionMode();
    void switchRenderMode();
    void switchMsaa();
    void switchAutoMoveCamera();
    RenderOpinion() = default;
};


//为了避免图形函数参数过多，将一些必要的数据单独作为一个类
class TriangleTmp{
public:
    float weight[3];
    float s;
    vec4f v1,v2,v3; //三角形顶点
    vec2f vt1,vt2,vt3; //三角形纹理坐标
    Texture* tex; //三角形纹理

    int ymin,ymax;
    std::vector<std::pair<int,int>> edges;

    void set(Pipline& p,Triangle& t);
    void setEdge(int y,int x,bool flag);
};

/* Pipline类 
 * -- 渲染流水线
 * -- 应用程序阶段，几何阶段，光栅化阶段，像素处理阶段
 */
class Pipline{
public:
    RenderOpinion opinion;
    TriangleTmp triangleNow;

    void init();
    void setModel(Model& m);
    Model& getModel();
    void setCamera(Camera& c);
    Camera& getCamera();
    float getZbuffer(int x,int y);
    void setZbuffer(int x,int y,float z);
    void setMsaa(int x,int y,int i);
    float getMsaa(int x,int y);

    void clearBuffer();
    void displayInfo();

    void applicationStage();
    void geometryStage();
    void rasterizationStage();
    void pixelsStage();

    Pipline() = default;
    ~Pipline();
    
private:
    Model *model;
    Camera *camera;

    std::vector<vec4f> transVertices;

    float *zBuffer;
    bool *MsaaBuffer;
    int zBufferSize,MsaaBufferSize;

    time_t frameStartTime;
    friend TriangleTmp;
    friend void Graphics::fill(Pipline& p);
};

#include"Pipline.cpp"