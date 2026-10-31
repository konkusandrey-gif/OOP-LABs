#ifndef MODULE1_H
#define MODULE1_H

#include <windows.h>
#include <string>

// Функция вызова диалога 1
// Принимает хэндл родительского окна и возвращает введенную строку черезoutText
bool ShowDialog1(HWND hWndParent, std::wstring& outText);

#endif // MODULE1_H
