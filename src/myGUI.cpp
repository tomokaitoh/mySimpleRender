#pragma once
#include"myGUI.hpp"

//窗口消息的回调函数
LRESULT CALLBACK wndProcCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_DESTROY:PostQuitMessage(0);return 0;
    case WM_KEYDOWN:
        tlogf("key:%c\n",wParam);
        KeyBoard.clickKey(wParam);
    return 0;
    default:return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

//初始化窗口
void SingletonWindow::init(int _width,int _height,const char *title){
    if(this->frameBuffer) return; //禁止重新初始化窗口
    width = _width;
    height = _height;
    texts.reserve(20);

    //注册窗口
    HINSTANCE hInstance = GetModuleHandle(0);
    WNDCLASSEX wc;
    wc.cbSize        = sizeof(WNDCLASSEX)               ;
    wc.style         = 0                                ;
    wc.cbClsExtra    = 0                                ;
    wc.cbWndExtra    = 0                                ;
    wc.lpfnWndProc   = wndProcCallback                  ;                           
    wc.hInstance     = hInstance                        ;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION)  ;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW)      ;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1)         ;
    wc.lpszMenuName  = NULL                             ;
    wc.lpszClassName = TEXT("MyGLClassName")            ;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION)  ;

    if(!RegisterClassEx(&wc)){
        MessageBoxA(NULL, "Window Registration Failed!", "Error!",MB_ICONEXCLAMATION | MB_OK);
        exit(0);
    }
    //创建窗口
    hWnd = CreateWindowExA(
            WS_EX_CLIENTEDGE,
            "MyGLClassName",
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            NULL, NULL, hInstance, NULL);

    if(hWnd == NULL){
        MessageBoxA(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        exit(0);
    }
    hWndDC = GetDC(hWnd);

    //frameBuffer初始化
    frameBufferSize =  sizeof(Color)*width*height                    ;
    frameBuffer     = (Color*)malloc(frameBufferSize)                ;
	hframeBuffer    = CreateCompatibleBitmap(hWndDC, width, height)  ;
	hframeBufferDC  = CreateCompatibleDC(hWndDC)                     ;

	bmpInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER)      ;   
	bmpInfo.bmiHeader.biWidth       = width                         ;   
	bmpInfo.bmiHeader.biHeight      = height                        ;   
	bmpInfo.bmiHeader.biPlanes      = 1                             ;   
	bmpInfo.bmiHeader.biCompression = BI_RGB                        ;   
	bmpInfo.bmiHeader.biBitCount    = 24                            ;
    GetDIBits(hWndDC, hframeBuffer, 0, height, frameBuffer, &bmpInfo, DIB_RGB_COLORS);  
	SelectObject(hframeBufferDC, hframeBuffer);
    PAINTSTRUCT ps;
    BeginPaint(hWnd,&ps);
    SelectObject(hframeBufferDC,GetStockObject(SYSTEM_FONT));
    SetBkMode(hframeBufferDC,TRANSPARENT);
    SetTextColor(hframeBufferDC,RGB(200,200,200));

    //显示窗口
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);
    tlogf("Window.size: %d,%d\n",Window.width,Window.height);
}

//设置像素颜色
void SingletonWindow::setPixel(int x,int y,Color color){
    if(((unsigned int)x)<width && ((unsigned int)y)<height)
        frameBuffer[x+y*width] = color;
}
Color& SingletonWindow::getPixel(int x,int y){
    return frameBuffer[x+y*width];
}

//设置文本
void  SingletonWindow::setText(int y,std::string text){
    texts.emplace_back(y,text);
}

//将framebuffer和texts的内容绘制到窗口上
void SingletonWindow::drawFrame(){
    SetDIBits(hframeBufferDC,hframeBuffer,0,height,frameBuffer,&bmpInfo,DIB_RGB_COLORS);
    for(auto text:texts) TextOutA(hframeBufferDC,5,text.first,text.second.c_str(),text.second.length());
    BitBlt(hWndDC,0,0,width,height,hframeBufferDC,0,0,SRCCOPY);
}

//清除framebuffer的内容
void SingletonWindow::clear(){
    memset(frameBuffer,0,frameBufferSize);
    texts.clear();
}

//窗口消息循环
void SingletonWindow::mainLoop(){
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
//释放资源
SingletonWindow::~SingletonWindow(){
    if(!hWnd) return;
    free(frameBuffer);
    ReleaseDC(hWnd, hWndDC); 
	DeleteDC(hframeBufferDC);
	DeleteObject(hframeBuffer);
}

//KeyBoard类方法
void SingletonKeyBoard::setHotkey(WPARAM key,void(*func)()){
    hotkeys[key] = func;
}

void SingletonKeyBoard::clickKey(WPARAM key){
    if(hotkeys.count(key)) hotkeys[key]();
}