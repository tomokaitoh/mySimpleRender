#pragma once
#include<windows.h>
#include<commdlg.h>
#include<string>
#include<vector>
#include<map>
#include<thread>
#include"tools.hpp"

/* Color类
 * -- bitmap中一个像素颜色
 * -- R,G,B 的范围均为[0,255]
 */
#pragma pack(1)
struct Color{
    BYTE r,g,b;
    Color operator+(const Color& a) const{ return Color(r+a.r,g+a.g,b+a.b); }
    Color operator*(float a)        const{ return Color(r*a,g*a,b*a); }
    
    Color(BYTE _r,BYTE _g,BYTE _b):r(_r),g(_g),b(_b){}
    Color() = default;
} 
Color_White = Color(255,255,255),
Color_Black = Color(0,0,0);

/* Window类
 * -- 
 */
class SingletonWindow{
public:
    int width,height;

    void init(int _width,int _height,const char *title);
    void setPixel(int x,int y,Color color);
    Color& getPixel(int x,int y);
    void setText(int y,std::string text);
    void drawFrame();
    void mainLoop();
    void clear();

    SingletonWindow() = default;
    ~SingletonWindow();

private:
    HWND        hWnd            ;
    HDC         hWndDC          ;
    Color*      frameBuffer     ;
    int         frameBufferSize ;
    HBITMAP     hframeBuffer    ;
    HDC         hframeBufferDC  ;
    BITMAPINFO  bmpInfo         ;

    std::vector<std::pair<int,std::string>> texts;
    friend std::string selectFileDialog();
} Window;
std::thread GUIthread;

/* KeyBoard类
 * -- 
 */
class SingletonKeyBoard{
public:
    void setHotkey(WPARAM key,void(*func)());
    void clickKey(WPARAM key);
private:
    std::map<WPARAM,void(*)()> hotkeys;
} KeyBoard;

std::string selectFileDialog(){
    char szBuffer[MAX_PATH] = {0};   
    OPENFILENAMEA ofn= {0};
    ofn.lStructSize = sizeof(ofn);   
    ofn.hwndOwner = Window.hWnd;   
    ofn.lpstrFilter = "模型文件(*.obj)\0*.obj\0所有文件(*.*)\0*.*\0";//要选择的文件后缀   
    ofn.lpstrInitialDir = ".\\resources";//默认的文件路径   
    ofn.lpstrFile = szBuffer;//存放文件的缓冲区   
    ofn.nMaxFile = sizeof(szBuffer)/sizeof(*szBuffer);   
    ofn.nFilterIndex = 0;   
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER ;//标志如果是多选要加上OFN_ALLOWMULTISELECT  
    BOOL bSel = GetOpenFileNameA(&ofn);
    return std::string(szBuffer);
}

#include"myGUI.cpp"