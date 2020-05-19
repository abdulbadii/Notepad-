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


#include "ansiCharPanel.h"
#include "ScintillaEditView.h"
#include "localization.h"

void AnsiCharPanel::switchEncoding()	{

	int codepage = (*_ppEditView)->getCurrentBuffer()->getEncoding();
	_listView.resetValues(codepage);
}

INT_PTR CALLBACK AnsiCharPanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)	{

    switch (message)	{

        case WM_INITDIALOG :	{

			NativeLangSpeaker *pNativeSpeaker = param.getNativeLangSpeaker();
			generic_string valStr = pNativeSpeaker->getAttrNameStr(L"Dec", "AsciiInsertion", "ColumnVal");
			generic_string hexStr = pNativeSpeaker->getAttrNameStr(L"Hex", "AsciiInsertion", "ColumnHex");
			generic_string charStr = pNativeSpeaker->getAttrNameStr(L"Character", "AsciiInsertion", "ColumnChar");
			generic_string htmlNumberStr = pNativeSpeaker->getAttrNameStr(L"HTML Value", "AsciiInsertion", "ColumnHtmlNumber");
			generic_string htmlNameStr = pNativeSpeaker->getAttrNameStr(L"HTML Name", "AsciiInsertion", "ColumnHtmlName");

			_listView.addColumn(columnInfo(valStr, param._dpiManager.scaleX(45)));
			_listView.addColumn(columnInfo(hexStr, param._dpiManager.scaleX(45)));
			_listView.addColumn(columnInfo(charStr, param._dpiManager.scaleX(73)));
			_listView.addColumn(columnInfo(htmlNumberStr, param._dpiManager.scaleX(93)));
			_listView.addColumn(columnInfo(htmlNameStr, param._dpiManager.scaleX(85)));

			_listView.init(_hInst, _hSelf);
			int codepage = (*_ppEditView)->getCurrentBuffer()->getEncoding();
			_listView.setValues(codepage==-1?0:codepage);
			_listView.display();

            return TRUE;
        }

		case WM_NOTIFY:	{

			switch (((LPNMHDR)lParam)->code)	{

				case NM_DBLCLK:	{

					LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) lParam;
					LVHITTESTINFO pInfo;
					pInfo.pt = lpnmitem->ptAction;
					ListView_SubItemHitTest(_listView.getHSelf(), &pInfo);

					int i = pInfo.iItem;
					int j = pInfo.iSubItem;
					wchar_t buffer[10];
					LVITEM item;
					item.mask = LVIF_TEXT | LVIF_PARAM;
					item.iItem = i;
					item.iSubItem = j;
					item.cchTextMax = 10;
					item.pszText = buffer;
					ListView_GetItem(_listView.getHSelf(), &item);

					if (i == -1)
						return TRUE;

					if (j != 2)
						insertString(item.pszText);
					else
						insertChar(static_cast<unsigned char>(i));
					
					return TRUE;
				}

				case LVN_KEYDOWN:	{

					switch (((LPNMLVKEYDOWN)lParam)->wVKey)	{

						case VK_RETURN:	{

							int i = _listView.getSelectedIndex();

							if (i == -1)
								return TRUE;

							insertChar(static_cast<unsigned char>(i));
							return TRUE;
						}
						default:
							break;
					}
				}
				break;

				default:
					break;
			}
		}
		return TRUE;

        case WM_SIZE:	{

            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
			::MoveWindow(_listView.getHSelf(), 0, 0, width, height, TRUE);
            break;
        }

        default :
            return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
    }
	return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}

void AnsiCharPanel::insertChar(unsigned char char2insert) const
{
    char charStr[2];
    charStr[0] = char2insert;
    charStr[1] = '\0';
    wchar_t wCharStr[10];
    char multiByteStr[10];
	int codepage = (*_ppEditView)->getCurrentBuffer()->getEncoding();
	if (codepage == -1)	{

		bool isUnicode = ((*_ppEditView)->f(SCI_GETCODEPAGE) == SC_CP_UTF8);
		if (isUnicode)	{

			MultiByteToWideChar(0, 0, charStr, -1, wCharStr, _countof(wCharStr));
			WideCharToMultiByte(CP_UTF8, 0, wCharStr, -1, multiByteStr, sizeof(multiByteStr), NULL, NULL);
		}
		else	{ // ANSI

			multiByteStr[0] = charStr[0];
			multiByteStr[1] = charStr[1];
		}
	}
	else	{

		MultiByteToWideChar(codepage, 0, charStr, -1, wCharStr, _countof(wCharStr));
		WideCharToMultiByte(CP_UTF8, 0, wCharStr, -1, multiByteStr, sizeof(multiByteStr), NULL, NULL);
	}
	(*_ppEditView)->f(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(""));
	size_t len = (char2insert < 128) ? 1 : strlen(multiByteStr);
	(*_ppEditView)->f(SCI_ADDTEXT, len, reinterpret_cast<LPARAM>(multiByteStr));
	(*_ppEditView)->focus();
}

void AnsiCharPanel::insertString(LPWSTR string2insert) const
{
	char multiByteStr[10];
	int codepage = (*_ppEditView)->getCurrentBuffer()->getEncoding();
	if (codepage == -1)	{

		bool isUnicode = ((*_ppEditView)->f(SCI_GETCODEPAGE) == SC_CP_UTF8);
		if (isUnicode)	{

			WideCharToMultiByte(CP_UTF8, 0, string2insert, -1, multiByteStr, sizeof(multiByteStr), NULL, NULL);
		}
		else	{ // ANSI

			wcstombs(multiByteStr, string2insert, 10);
		}
	}
	else	{

		WideCharToMultiByte(CP_UTF8, 0, string2insert, -1, multiByteStr, sizeof(multiByteStr), NULL, NULL);
	}

	(*_ppEditView)->f(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(""));
	size_t len = strlen(multiByteStr);
	(*_ppEditView)->f(SCI_ADDTEXT, len, reinterpret_cast<LPARAM>(multiByteStr));
	(*_ppEditView)->focus();
}
