// Graph calculator.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "linalg.h"
#include "model.h"
#include "render.h"
#include <thread>
#include <stdio.h>
using namespace std;


#define MAX_LOADSTRING 100


const COLORREF rgbRed = 0x000000FF;
const COLORREF rgbGreen = 0x0000FF00;
const COLORREF rgbBlue = 0x00FF0000;
const COLORREF rgbBlack = 0x00000000;
const COLORREF rgbWhite = 0x00FFFFFF;
const COLORREF rgbGray = 0x00A0A0A0;

// 전역 변수:


int WIDTH = 1280, HEIGHT = 720;

renderstruct rs(WIDTH,HEIGHT);
physics model;
int fps=15;

HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

#ifdef _DEBUG
    AllocConsole();
#else
    AttachConsole(ATTACH_PARENT_PROCESS);
#endif

    FILE* pCout;
    freopen_s(&pCout, "conout$", "w", stdout); //returns 0

    printf_s("test\n");
    

    polygon temp;
    temp.p[0] = { 100,-20,20 };
    temp.p[1] = { 100,0,-20 };
    temp.p[2] = { 100,20,20 };
    temp.set_dir();
    rs.p.push_back(temp);

    temp.p[0] = { 100,20,20 };
    temp.p[1] = { 100,0,-20 };
    temp.p[2] = { 130,0,0 };
    temp.set_dir();
    rs.p.push_back(temp);

    temp.p[0] = { 100,-20,20 };
    temp.p[1] = { 130,0,0 };
    temp.p[2] = { 100,0,-20 };
    temp.set_dir();
    rs.p.push_back(temp);

    temp.p[0] = { 100,20,20 };
    temp.p[1] = { 130,0,0 };
    temp.p[2] = { 100,-20,20 };
    temp.set_dir();
    rs.p.push_back(temp);

    //thread _t1(model_loop,model,60);
    thread _t2(render_loop,&rs,15);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RENDER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RENDER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    _t2.join();

    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_RENDER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RENDER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

    rs.assign_window(hWnd);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_PAINT:
            rs.render();
        break;
        case WM_KEYDOWN:
            rs.move_camera(wParam);  
        break;
        case WM_CHAR:
            rs.move_camera(wParam);
        break;
        case WM_MOUSEWHEEL:
 
        break;
        case WM_SIZE:
        {
            WIDTH= LOWORD(lParam);
            HEIGHT= HIWORD(lParam);
            rs.on_resize(WIDTH, HEIGHT);
        }
        break;
        case WM_LBUTTONDOWN:
        
        break;
        case WM_LBUTTONUP:
        
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            FreeConsole();
            rs.quit();
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}