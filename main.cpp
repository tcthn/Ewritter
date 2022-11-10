#include <windows.h>
#include "main.h"
#include <iostream>
#include <commdlg.h>
#include <tchar.h> 
#include <commctrl.h>
#include <richedit.h>
#include <map>
#include <fstream>
#include <string>
#include <strsafe.h>
#include <stdio.h>
#include <libloaderapi.h>
#include <mmsystem.h>
 
using namespace std;





HWND g_hwnd;
HWND g_hEdit;
HFONT g_hFont;
char* FileNameV;
HANDLE hFileV;
char* FileNameVU;
HANDLE hFileVU;
COLORREF g_editcolor=RGB(0,0,0);
HBRUSH g_hbrbackground=CreateSolidBrush(RGB(255,255,255));
BOOL LoadFile(HWND hEdit, LPSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF) {
			LPSTR pszFileText;
			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL) {
				DWORD dwRead;
				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL)) {
					pszFileText[dwFileSize] = 0; // Null terminator
					if(SetWindowText(hEdit, pszFileText))
						bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL SaveFile(HWND hEdit, LPSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD dwTextLength;
		dwTextLength = GetWindowTextLength(hEdit);
		if(dwTextLength > 0) {
			LPSTR pszText;
			pszText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
			if(pszText != NULL) {
				if(GetWindowText(hEdit, pszText, dwTextLength + 1)) {
					DWORD dwWritten;
					if(WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
						bSuccess = TRUE;
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL DoFileOpenSave(HWND hwnd, BOOL bSave) {
	OPENFILENAME ofn;
	char szFileName[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	szFileName[0] = 0;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Archivos de texto (*.txt)\0*.txt\0Todos los archivos (*.*)\0*.*\0Archivos Ewritter (*.ewritter)\0*.ewriter\00\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "ewritter";

	if(bSave) {
		ofn.Flags = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
		if(GetSaveFileName(&ofn)) {
			if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
				MessageBox(hwnd, "No se pudo guardar el archivo.", "Error",MB_OK|MB_ICONEXCLAMATION);
				return FALSE;
			}
		}
	} else {
		ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
		if(GetOpenFileName(&ofn)) {
			if(!LoadFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
				MessageBox(hwnd, "No se pudo cargar el archivo.", "Error",MB_OK|MB_ICONEXCLAMATION);
				return FALSE;
			}
		}
	}
	return TRUE;
 
}



void ChooseFontForEdit()
{
    CHOOSEFONT cf={sizeof(CHOOSEFONT)};
    LOGFONT lf;
    GetObject(g_hFont,sizeof(LOGFONT),&lf);
    cf.Flags=CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
    cf.hwndOwner=g_hwnd;
    cf.lpLogFont=&lf;
    cf.rgbColors=g_editcolor;
    if(!ChooseFont(&cf))
      return;
    HFONT hf=CreateFontIndirect(&lf);
    if(hf)
    {
        g_hFont=hf;
        SendMessage(g_hEdit,WM_SETFONT,(WPARAM)g_hFont,TRUE);
    }
    g_editcolor=cf.rgbColors;
 
           


         
    
}
 
 
 

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	HFONT hFont;
    HWND hEdit;
	switch(Message) {
		case WM_CREATE:
     hEdit =  CreateWindow("EDIT", "",WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,hwnd, (HMENU)IDC_MAIN_TEXT, GetModuleHandle(NULL), NULL);
			SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE,0));

	
			 

           hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
           SendMessage(hEdit,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(FALSE,0));

           g_hEdit=hEdit;
           g_hFont=hFont;

			break;
		case WM_SIZE:
			if(wParam != SIZE_MINIMIZED)
				MoveWindow(GetDlgItem(hwnd, IDC_MAIN_TEXT), 0, 0, LOWORD(lParam),HIWORD(lParam), TRUE);
			break;
		case WM_SETFOCUS:
			SetFocus(GetDlgItem(hwnd, IDC_MAIN_TEXT));
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case CM_FILE_OPEN:
					DoFileOpenSave(hwnd, FALSE);
			 

					break;
				case CM_FILE_SAVEAS:
					DoFileOpenSave(hwnd, TRUE);
					break;
				case CM_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case CM_ABOUT:
					MessageBox (NULL, "Editor texto creado por MannyR / Tu Espacio Con Tony / Utilizando la api de WIN32" , "Acerca De Ewriter", 0);
					break;
				case CM_FILE_CUT:
			  	SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_CUT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE,0));
				break;
				case CM_FILE_COPY:
			  	SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_COPY,(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE,0));
				break;
				case CM_FILE_PASTE:
			  	SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_PASTE,(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE,0));
				break;
				case CM_FILE_REDO:
			  	SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_UNDO,(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE,0));
				break;
				case CM_FILE_UNDO: 
			  	SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_UNDO,(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE,0));
				break;
				case CM_FILE_SELECTFONT: 
 
  ChooseFontForEdit();

				break;	
				
	case CM_FILE_TEST:
const char* url = "http://192.168.137.1/Edev/version_new.txt";
const char* dst = "C:\\Edev\\Ewritter\\version_update.txt";
URLDownloadToFile(NULL,url,dst,0,NULL);	 
 ifstream File1("version_actual.txt", ios::in); //Abrimos los dos ficheros en modo de lectura
 ifstream File2("version_update.txt", ios::in); //

 if (File1.fail() || File2.fail()){ // en el caso de que uno de los dos ficheros no exista...
 MessageBox(NULL,"ERROR NO SE HA PODIDO LEER LOS ARCHIVOS","Error",0); 
 
 }
 else {//si ambos ficheros se han encontrado y se han abierto correctamente...
  ofstream comparacion("Comparacion.txt", ios::out); //declaramos y abrimos otro fichero en modo de escritura
  bool iguales = true;
  int cont = 0, linea = 0;
   

  while (!File1.eof() && !File2.eof()) { // mientras que ninguno de los dos ficheros se haya acabado
   ++linea;
   string a, b;
   getline(File1, a); // metemos en el string a la primera linea del fichero 1
   getline(File2, b); // metemos en el string b la primera linea del fichero 2
   if (a != b) { // si son diferentes....
    ++cont;
    
   MessageBox(NULL,"Hay actualización Disponible se descargara ahora mismo!","Actualizaciones",0);
   const char* url2 = "http://192.168.137.1/Edev/Updates/stable/Ewritter.zip";
const char* dst2 = "C:\\Edev\\Ewritter\\Update.zip";
   if(S_OK == URLDownloadToFile(NULL,url2,dst2,0,NULL)){
   	MessageBox(NULL,"Se ha descargado la actualización correctamente!","Actualización descargada",0);
   	STARTUPINFO startInfo = {0};
   	PROCESS_INFORMATION processInfo = {0};
   	CreateProcess(TEXT("C:\\Edev\\Ewritter\\update_ewritter.bat"),NULL,NULL,NULL,FALSE,0,NULL,NULL,&startInfo,&processInfo);
   	
   	
   }else{
   	MessageBox(NULL,"No se ha podido descargar la actualización chequea tu internet","Error al actualizar",0);
   }
    iguales = false;
    
   }else{
   MessageBox(NULL,"No hay actualizaciones disponibles","Actualizaciones",0);
   iguales = true;
  }
}
  
  
   
   
  
  
 
} 
 
 

				break;	
				
		 
        
			}
			break;
		case WM_CLOSE:
			
			   
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}






int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
    wc.hIcon = LoadIcon(hInstance, "A");
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = "MAINMENU";
	wc.lpszClassName = "WindowClass";
	wc.hIconSm       = LoadIcon(hInstance,"A"); /* A is name used by project icons */

	if(!RegisterClassEx(&wc)) {
		MessageBox(0,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Ewritter by MannyR",WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		320,240,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL) {
		MessageBox(0, "Window Creation Failed!", "Error!",MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
		return 0;
	}

	ShowWindow(hwnd,1);
	UpdateWindow(hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

