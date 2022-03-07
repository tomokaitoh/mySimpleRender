#pragma once
#include"Mathematics.hpp"

//Matrix4f类
Matrix4f::Matrix4f(float _m[]){
    for(int i=0;i<16;++i) 
        m[i] = _m[i];
}
Matrix4f::Matrix4f(float v){
    for(int i=0;i<16;++i) 
        m[i] = v;
}
Matrix4f& Matrix4f::transpose(){
    std::swap(m[1],m[4]); std::swap(m[2],m[8]); std::swap(m[3] ,m[12]);
    std::swap(m[6],m[9]); std::swap(m[7],m[13]); std::swap(m[11],m[14]);
    return *this;
}

vec4f Matrix4f::operator*(const vec4f& a){
    return vec4f(
        a.x*m[0]  + a.y*m[1]  + a.z*m[2]  + a.w*m[3] ,
        a.x*m[4]  + a.y*m[5]  + a.z*m[6]  + a.w*m[7] ,
        a.x*m[8]  + a.y*m[9]  + a.z*m[10] + a.w*m[11],
        a.x*m[12] + a.y*m[13] + a.z*m[14] + a.w*m[15]
    );
}
Matrix4f Matrix4f::operator*(const Matrix4f& a){
    Matrix4f result;
    result.m[0]  = m[0] *a.m[0] + m[1] *a.m[4] + m[2]*a.m[8]   + m[3]*a.m[12]  ;
    result.m[1]  = m[0] *a.m[1] + m[1] *a.m[5] + m[2]*a.m[9]   + m[3]*a.m[13]  ;
    result.m[2]  = m[0] *a.m[2] + m[1] *a.m[6] + m[2]*a.m[10]  + m[3]*a.m[14]  ;
    result.m[3]  = m[0] *a.m[3] + m[1] *a.m[7] + m[2]*a.m[11]  + m[3]*a.m[15]  ;
    result.m[4]  = m[4] *a.m[0] + m[5] *a.m[4] + m[6]*a.m[8]   + m[7]*a.m[12]  ;
    result.m[5]  = m[4] *a.m[1] + m[5] *a.m[5] + m[6]*a.m[9]   + m[7]*a.m[13]  ;
    result.m[6]  = m[4] *a.m[2] + m[5] *a.m[6] + m[6]*a.m[10]  + m[7]*a.m[14]  ;
    result.m[7]  = m[4] *a.m[3] + m[5] *a.m[7] + m[6]*a.m[11]  + m[7]*a.m[15]  ;
    result.m[8]  = m[8] *a.m[0] + m[9] *a.m[4] + m[10]*a.m[8]  + m[11]*a.m[12] ;
    result.m[9]  = m[8] *a.m[1] + m[9] *a.m[5] + m[10]*a.m[9]  + m[11]*a.m[13] ;
    result.m[10] = m[8] *a.m[2] + m[9] *a.m[6] + m[10]*a.m[10] + m[11]*a.m[14] ; 
    result.m[11] = m[8] *a.m[3] + m[9] *a.m[7] + m[10]*a.m[11] + m[11]*a.m[15] ; 
    result.m[12] = m[12]*a.m[0] + m[13]*a.m[4] + m[14]*a.m[8]  + m[15]*a.m[12] ; 
    result.m[13] = m[12]*a.m[1] + m[13]*a.m[5] + m[14]*a.m[9]  + m[15]*a.m[13] ; 
    result.m[14] = m[12]*a.m[2] + m[13]*a.m[6] + m[14]*a.m[10] + m[15]*a.m[14] ;  
    result.m[15] = m[12]*a.m[3] + m[13]*a.m[7] + m[14]*a.m[11] + m[15]*a.m[15] ;
    return result;
}
std::ostream& operator<< (std::ostream& os,const Matrix4f& a){
    os<<"[";
    for(int i=0;i<4;++i){
        for(int j=0;j<4;++j) os<<a.m[i*4+j]<<" ,";
        if(i!=3) os<<std::endl;
    }
    return os<<"]\n";
}

Matrix4f Matrix4f::identityMatrix((float[16]){1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1});


inline float Math::distance(vec4f a,vec4f b){
    return (a-b).len();
}
template<typename T> inline T Math::clamp(T a,T min,T max){
    return a<min?min:(a>max?max:a);
}
inline vec4f Math::cross(vec4f a,vec4f b){
    return vec4f(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x,0);
}

template<typename T>inline T Math::lerp(T a, T b,float i){
    return b*i+a*(1-i);
}