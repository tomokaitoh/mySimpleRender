#pragma once
#include"myGUI.hpp"
#include"Mathematics.hpp"
#include"Model.hpp"
#include<cmath>
class Pipline;
namespace Graphics{
    Matrix4f modelTrans(Model& m);
    Matrix4f viewTrans(Camera& c);
    Matrix4f projTrans(Camera& c,int projectionMode);
    Matrix4f viewportTrans();
    Color lerpTexture(Pipline& p);

    void line(vec4f &v1,vec4f& v2,Color& color=Color_White);
    void scanLine(Pipline& p,int y,int x1,int x2);
    inline void calculateWeight(Pipline& p,float x,float y);
    void calculateEdge(Pipline& p,vec4f &v1,vec4f& v2,bool flag);
    void calculateMsaa(Pipline& p,int x,int y);
    inline void wireFrameTriangle(Pipline& p);
    void fill(Pipline& p);

};

#include"Graphics.cpp"
