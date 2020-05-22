// This file is part of Notepad++ project
// Copyright (C)2003 Don HO <don.h@free.fr>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// Note that the GPL places important restrictions on "derived works", yet
// it does not provide a detailed definition of that term.  To avoid
// misunderstandings, we consider an application to constitute a
// "derivative work" for the purpose of this license if it does any of the
// following:
// 1. Integrates source code from Notepad++.
// 2. Integrates/includes/aggregates Notepad++ into a proprietary executable
//    installer, such as those produced by InstallShield.
// 3. Links to a library or executes a program that does any of the above.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include <shlobj.h>
#include <uxtheme.h>

#include "AboutDlg.h"
#include "Parameters.h"
#include "localization.h"

#pragma warning(disable : 4996) // for GetVersion()

INT_PTR CALLBACK AboutDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)	{

	switch (message)	{

        case WM_INITDIALOG :	{

			HWND compileDateHandle = ::GetDlgItem(_hSelf, IDC_BUILD_DATETIME);
			generic_string buildTime = L"Build time : ";

			buildTime +=  wmc.char2wchar(__DATE__, CP_ACP);
			buildTime += L" - ";
			buildTime +=  wmc.char2wchar(__TIME__, CP_ACP);

			LPCTSTR bitness = param.isx64() ? L"(64-bit)" : L"(32-bit)";
			::SetDlgItemText(_hSelf, IDC_VERSION_BIT, bitness);

			::SendMessage(compileDateHandle, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(buildTime.c_str()));
			::EnableWindow(compileDateHandle, FALSE);

            HWND licenceEditHandle = ::GetDlgItem(_hSelf, IDC_LICENCE_EDIT);
			::SendMessage(licenceEditHandle, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(LICENCE_TXT));

            //_emailLink.init(_hInst, _hSelf);
			//_emailLink.create(::GetDlgItem(_hSelf, IDC_AUTHOR_NAME), L"mailto:don.h@free.fr");
			//_emailLink.create(::GetDlgItem(_hSelf, IDC_AUTHOR_NAME), L"https://notepad-plus-plus.org/news/v781-free-uyghur-edition/");

            _pageLink.init(_hInst, _hSelf);
            _pageLink.create(::GetDlgItem(_hSelf, IDC_HOME_ADDR), L"https://notepad-plus-plus.org/");

			getClientRect(_rc);

			ETDTProc enableDlgTheme = (ETDTProc)param.getEnableThemeDlgTexture();
			if (enableDlgTheme)	{

				enableDlgTheme(_hSelf, ETDT_ENABLETAB);
				redraw();
			}

			return TRUE;
		}

		case WM_DRAWITEM :	{

			HICON hIcon = (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_CHAMELEON), IMAGE_ICON, 64, 64, LR_DEFAULTSIZE);
			//HICON hIcon = (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_JESUISCHARLIE), IMAGE_ICON, 64, 64, LR_DEFAULTSIZE);
			//HICON hIcon = (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_GILETJAUNE), IMAGE_ICON, 64, 64, LR_DEFAULTSIZE);
			//HICON hIcon = (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_SAMESEXMARRIAGE), IMAGE_ICON, 64, 64, LR_DEFAULTSIZE);
			DRAWITEMSTRUCT *pdis = (DRAWITEMSTRUCT *)lParam;
			::DrawIconEx(pdis->hDC, 0, 0, hIcon, 64, 64, 0, NULL, DI_NORMAL);
			return TRUE;
		}

		case WM_COMMAND :	{

			switch (wParam)	{

				case IDCANCEL :
				case IDOK :
					display(false);
					return TRUE;

				default :
					break;
			}
		}

		case WM_DESTROY :	{

			return TRUE;
		}
	}
	return FALSE;
}

void AboutDlg::doDialog()	{

	if (!isCreated())
		create(IDD_ABOUTBOX);

    // Adjust the position of AboutBox
	goToCenter();
}


INT_PTR CALLBACK DebugInfoDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM /*lParam*/)	{

	switch (message)	{

		case WM_INITDIALOG:	{
			// Notepad++ version
			_debugInfoStr = NOTEPAD_PLUS_VERSION;
			_debugInfoStr += param.isx64() ? L"   (64-bit)" : L"   (32-bit)";
			_debugInfoStr += L"\r\n";

			// Build time
			_debugInfoStr += L"Build time : ";
			generic_string buildTime;
			buildTime += wmc.char2wchar(__DATE__, CP_ACP);
			buildTime += L" - ";
			buildTime += wmc.char2wchar(__TIME__, CP_ACP);
			_debugInfoStr += buildTime;
			_debugInfoStr += L"\r\n";

			// Binary path
			_debugInfoStr += L"Path : ";
			TCHAR nppFullPath[MAX_PATH];
			::GetModuleFileName(NULL, nppFullPath, MAX_PATH);
			_debugInfoStr += nppFullPath;
			_debugInfoStr += L"\r\n";

			//Size
			HANDLE hItself = ::CreateFile(
			reinterpret_cast<LPCWSTR>(nppFullPath),
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
			
			if (hItself != INVALID_HANDLE_VALUE)	{
				LARGE_INTEGER size;
				if (!GetFileSizeEx(hItself, &size))	{
					_debugInfoStr += L"Size can't be known\r\n ";
					CloseHandle(hItself);
				}
				else{
				_debugInfoStr += L"Size : ";
				_debugInfoStr += commafyInt(size.QuadPart);
				_debugInfoStr += L" bytes\r\n";
				CloseHandle(hItself);
			}
			}

			// Administrator mode
			_debugInfoStr += L"As Administrator : ";
			_debugInfoStr += (_isAdmin ? L"Yes" : L"No");
			_debugInfoStr += L"\r\n";

			// local conf
			_debugInfoStr += L"XML Configuration Files: On ";
			bool doLocalConf = param.isLocal();
			_debugInfoStr += (doLocalConf ? L"Installation Directory\r\n" : L"%APPDATA%\\Notepad++\r\n");

			// OS information
			HKEY hKey;
			DWORD dataSize = 0;
			
			TCHAR szProductName[96] = {'\0'};
			TCHAR szCurrentBuildNumber[32] = {'\0'};
			TCHAR szReleaseId[32] = {'\0'};
			DWORD dwUBR = 0;
			TCHAR szUBR[12] = L"0";

			// NOTE: RegQueryValueExW is not guaranteed to return null-terminated strings
			if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS)	{

				dataSize = sizeof(szProductName);
				RegQueryValueExW(hKey, L"ProductName", NULL, NULL, reinterpret_cast<LPBYTE>(szProductName), &dataSize);
				szProductName[sizeof(szProductName) / sizeof(TCHAR) - 1] = '\0';

				dataSize = sizeof(szReleaseId);
				RegQueryValueExW(hKey, L"ReleaseId", NULL, NULL, reinterpret_cast<LPBYTE>(szReleaseId), &dataSize);
				szReleaseId[sizeof(szReleaseId) / sizeof(TCHAR) - 1] = '\0';
				
				dataSize = sizeof(szCurrentBuildNumber);
				RegQueryValueExW(hKey, L"CurrentBuildNumber", NULL, NULL, reinterpret_cast<LPBYTE>(szCurrentBuildNumber), &dataSize);
				szCurrentBuildNumber[sizeof(szCurrentBuildNumber) / sizeof(TCHAR) - 1] = '\0';
				
				dataSize = sizeof(DWORD);
				if (RegQueryValueExW(hKey, L"UBR", NULL, NULL, reinterpret_cast<LPBYTE>(&dwUBR), &dataSize) == ERROR_SUCCESS)	{

					generic_sprintf(szUBR, L"%u", dwUBR);
				}
				
				RegCloseKey(hKey);
			}

			// Get alternative OS information
			if (szProductName[0] == '\0')	{

				generic_sprintf(szProductName, L"%s", param.getWinVersionStr().c_str());
			}
			if (szCurrentBuildNumber[0] == '\0')	{

				DWORD dwVersion = GetVersion();
				if (dwVersion < 0x80000000)	{

					generic_sprintf(szCurrentBuildNumber, L"%u", HIWORD(dwVersion));
				}
			}
			
			_debugInfoStr += L"OS Name : ";
			_debugInfoStr += szProductName;
			_debugInfoStr += L" (";
			_debugInfoStr += param.getWinVerBitStr();
			_debugInfoStr += L") ";
			_debugInfoStr += L"\r\n";
			
			if (szReleaseId[0] != '\0')	{

				_debugInfoStr += L"OS Version : ";
				_debugInfoStr += szReleaseId;
				_debugInfoStr += L"\r\n";
			}

			if (szCurrentBuildNumber[0] != '\0')	{

				_debugInfoStr += L"OS Build : ";
				_debugInfoStr += szCurrentBuildNumber;
				_debugInfoStr += L".";
				_debugInfoStr += szUBR;
				_debugInfoStr += L"\r\n";
			}

			// Detect WINE
			PWINEGETVERSION pWGV = (PWINEGETVERSION)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "wine_get_version");
			if (pWGV)	{

				TCHAR szWINEVersion[32];
				generic_sprintf(szWINEVersion, L"%hs", pWGV());

				_debugInfoStr += L"WINE : ";
				_debugInfoStr += szWINEVersion;
				_debugInfoStr += L"\r\n";
			}

			// Plugins
			_debugInfoStr += L"Plugins : ";
			_debugInfoStr += _loadedPlugins.length() ? _loadedPlugins : L"none";
			_debugInfoStr += L"\r\n";

			::SendMessage(::GetDlgItem(_hSelf, IDC_DEBUGINFO_EDIT), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(_debugInfoStr.c_str()));

			_copyToClipboardLink.init(_hInst, _hSelf);
			_copyToClipboardLink.create(::GetDlgItem(_hSelf, IDC_DEBUGINFO_COPYLINK), IDC_DEBUGINFO_COPYLINK);

			getClientRect(_rc);
			return TRUE;
		}

		case WM_COMMAND:	{

			switch (wParam)	{

				case IDCANCEL:
				case IDOK:
					display(false);
					return TRUE;

				case IDC_DEBUGINFO_COPYLINK:	{

					if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)	{

						// Copy to clipboard
						str2Clipboard(_debugInfoStr, _hSelf);
					}
					return TRUE;
				}
				default:
					break;
			}
		}

		case WM_DESTROY:	{

			return TRUE;
		}
	}
	return FALSE;
}

void DebugInfoDlg::doDialog()	{

	if (!isCreated())
		create(IDD_DEBUGINFOBOX);

	// Adjust the position of AboutBox
	goToCenter();
}

void DoSaveOrNotBox::doDialog(bool isRTL)	{

	
	if (isRTL)	{

		DLGTEMPLATE *pMyDlgTemplate = NULL;
		HGLOBAL hMyDlgTemplate = makeRTLResource(IDD_DOSAVEORNOTBOX, &pMyDlgTemplate);
		::DialogBoxIndirectParam(_hInst, pMyDlgTemplate, _hParent, dlgProc, reinterpret_cast<LPARAM>(this));
		::GlobalFree(hMyDlgTemplate);
	}
	else
		::DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_DOSAVEORNOTBOX), _hParent, dlgProc, reinterpret_cast<LPARAM>(this));
}

void DoSaveOrNotBox::changeLang()	{

	generic_string msg;
	generic_string defaultMessage = L"Save file \"$STR_REPLACE$\" ?";
	NativeLangSpeaker* nativeLangSpeaker = param.getNativeLangSpeaker();

	if (nativeLangSpeaker->changeDlgLang(_hSelf, "DoSaveOrNot"))	{

		const unsigned char len = 255;
		TCHAR text[len];
		::GetDlgItemText(_hSelf, IDC_DOSAVEORNOTTEX, text, len);
		msg = text;
	}

	if (msg.empty())
		msg = defaultMessage;

	msg = stringReplace(msg, L"$STR_REPLACE$", _fn);
	::SetDlgItemText(_hSelf, IDC_DOSAVEORNOTTEX, msg.c_str());
}

INT_PTR CALLBACK DoSaveOrNotBox::run_dlgProc(UINT message, WPARAM wParam, LPARAM /*lParam*/)	{

	switch (message)	{

		case WM_INITDIALOG :	{

			changeLang();
			::EnableWindow(::GetDlgItem(_hSelf, IDRETRY), _isMulti);
			::EnableWindow(::GetDlgItem(_hSelf, IDIGNORE), _isMulti);
			goToCenter();
			return TRUE;
		}

		case WM_COMMAND:	{

			switch (LOWORD(wParam))	{

				case IDCANCEL:	{

					::EndDialog(_hSelf, -1);
					clickedButtonId = IDCANCEL;
					return TRUE;
				}

				case IDYES:	{

					::EndDialog(_hSelf, 0);
					clickedButtonId = IDYES;
					return TRUE;
				}

				case IDNO:	{

					::EndDialog(_hSelf, 0);
					clickedButtonId = IDNO;
					return TRUE;
				}

				case IDIGNORE:	{

					::EndDialog(_hSelf, 0);
					clickedButtonId = IDIGNORE;
					return TRUE;
				}

				case IDRETRY:	{

					::EndDialog(_hSelf, 0);
					clickedButtonId = IDRETRY;
					return TRUE;
				}
			}
		}
		default:
			return FALSE;
	}
}