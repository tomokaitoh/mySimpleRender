#pragma once
#include<cmath>
#include<iostream>

//向量定义
struct vec4f{
    float x,y,z,w;
    
    vec4f(float _x,float _y,float _z,float _w):x(_x),y(_y),z(_z),w(_w){}
    vec4f() = default;

    float  len()       { return sqrt((*this)*(*this)); }
    vec4f& normalize() { return (*this/=len()); }
    vec4f  normalized(){ return *this/len(); }
 
    vec4f  operator+  (const vec4f& a) { return vec4f(a.x+x, a.y+y, a.z+z, a.w+w); }
    vec4f& operator+= (const vec4f& a) { x+=a.x; y+=a.y; z+=a.z; w+=a.w;return *this;}
 
    vec4f  operator-  (const vec4f& a) { return vec4f(x-a.x, y-a.y, z-a.z, w-a.w); }
    vec4f& operator-= (const vec4f& a) { x-=a.x; y-=a.y; z-=a.z; w-=a.w;return *this;}

    float  operator*  (const vec4f& a) { return a.x*x + a.y*y + a.z*z + a.w*w; }
    vec4f  operator*  (float a)        { return vec4f(x*a,y*a,z*a,w*a); }
    vec4f& operator*= (float a)        { x*=a; y*=a; z*=a;w*=a; return *this;}

    vec4f  operator/  (float a)        { return vec4f(x/a, y/a, z/a, w/a); }
    vec4f& operator/= (float a)        { x/=a; y/=a; z/=a;w/=a; return *this; }


    friend std::ostream& operator<<(std::ostream& os,const vec4f& a){
        return os<<"("<<a.x<<","<<a.y<<","<<a.z<<","<<a.w<<")";
    }
};

struct vec2f{
    float x,y;
    vec2f(float _x,float _y):x(_x),y(_y){}
    vec2f() = default;
};

//矩阵定义
class Matrix4f{
public:
    float m[16];

    Matrix4f(float v);
    Matrix4f(float _m[]);
    Matrix4f() = default;

    Matrix4f& transpose();
    vec4f     operator*(const vec4f& a);
    Matrix4f  operator*(const Matrix4f& a);
    friend std::ostream& operator<< (std::ostream& os,const vec4f& a);

    static Matrix4f identityMatrix;
};


//常用函数
namespace Math{
    static float PI = 3.14159265;
    inline float distance(vec4f a,vec4f b);
    template<typename T> inline T clamp(T a,T min,T max);
    inline vec4f cross(vec4f a,vec4f b);
    template<typename T> inline T lerp(T a,T b,float i);
}

#include"Mathematics.cpp"
