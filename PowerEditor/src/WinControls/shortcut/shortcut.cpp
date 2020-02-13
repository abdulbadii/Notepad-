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


#include <memory>
#include <algorithm>
#include <array>
#include "shortcut.h"
#include "Parameters.h"
#include "ScintillaEditView.h"
#include "resource.h"
#include "Notepad_plus_Window.h"
#include "keys.h"

using namespace std;

const int KEY_STR_LEN = 16;

struct KeyIDNAME {
	const TCHAR * name;
	UCHAR id;
};

KeyIDNAME namedKeyArray[] = {
{L"None", VK_NULL},

{L"Backspace", VK_BACK},
{L"Tab", VK_TAB},
{L"Enter", VK_RETURN},
{L"Esc", VK_ESCAPE},
{L"Spacebar", VK_SPACE},

{L"Page up", VK_PRIOR},
{L"Page down", VK_NEXT},
{L"End", VK_END},
{L"Home", VK_HOME},
{L"Left", VK_LEFT},
{L"Up", VK_UP},
{L"Right", VK_RIGHT},
{L"Down", VK_DOWN},

{L"INS", VK_INSERT},
{L"DEL", VK_DELETE},

{L"0", VK_0},
{L"1", VK_1},
{L"2", VK_2},
{L"3", VK_3},
{L"4", VK_4},
{L"5", VK_5},
{L"6", VK_6},
{L"7", VK_7},
{L"8", VK_8},
{L"9", VK_9},
{L"A", VK_A},
{L"B", VK_B},
{L"C", VK_C},
{L"D", VK_D},
{L"E", VK_E},
{L"F", VK_F},
{L"G", VK_G},
{L"H", VK_H},
{L"I", VK_I},
{L"J", VK_J},
{L"K", VK_K},
{L"L", VK_L},
{L"M", VK_M},
{L"N", VK_N},
{L"O", VK_O},
{L"P", VK_P},
{L"Q", VK_Q},
{L"R", VK_R},
{L"S", VK_S},
{L"T", VK_T},
{L"U", VK_U},
{L"V", VK_V},
{L"W", VK_W},
{L"X", VK_X},
{L"Y", VK_Y},
{L"Z", VK_Z},

{L"Numpad 0", VK_NUMPAD0},
{L"Numpad 1", VK_NUMPAD1},
{L"Numpad 2", VK_NUMPAD2},
{L"Numpad 3", VK_NUMPAD3},
{L"Numpad 4", VK_NUMPAD4},
{L"Numpad 5", VK_NUMPAD5},
{L"Numpad 6", VK_NUMPAD6},
{L"Numpad 7", VK_NUMPAD7},
{L"Numpad 8", VK_NUMPAD8},
{L"Numpad 9", VK_NUMPAD9},
{L"Num *", VK_MULTIPLY},
{L"Num +", VK_ADD},
//{L"Num Enter", VK_SEPARATOR},	//this one doesnt seem to work
{L"Num -", VK_SUBTRACT},
{L"Num .", VK_DECIMAL},
{L"Num /", VK_DIVIDE},
{L"F1", VK_F1},
{L"F2", VK_F2},
{L"F3", VK_F3},
{L"F4", VK_F4},
{L"F5", VK_F5},
{L"F6", VK_F6},
{L"F7", VK_F7},
{L"F8", VK_F8},
{L"F9", VK_F9},
{L"F10", VK_F10},
{L"F11", VK_F11},
{L"F12", VK_F12},

{L"~", VK_OEM_3},
{L"-", VK_OEM_MINUS},
{L"=", VK_OEM_PLUS},
{L"[", VK_OEM_4},
{L"]", VK_OEM_6},
{L";", VK_OEM_1},
{L"'", VK_OEM_7},
{TEXT("\\"), VK_OEM_5},
{L",", VK_OEM_COMMA},
{L".", VK_OEM_PERIOD},
{L"/", VK_OEM_2},

{L"<>", VK_OEM_102},
};

#define nbKeys sizeof(namedKeyArray)/sizeof(KeyIDNAME)

generic_string Shortcut::toString() const
{
	generic_string sc = L"";
	if (!isEnabled())
		return sc;

	if (_keyCombo._isCtrl)
		sc += L"Ctrl+";
	if (_keyCombo._isAlt)
		sc += L"Alt+";
	if (_keyCombo._isShift)
		sc += L"Shift+";

	generic_string keyString;
	getKeyStrFromVal(_keyCombo._key, keyString);
	sc += keyString;
	return sc;
}

void Shortcut::setName(const TCHAR * name)
{
	lstrcpyn(_menuName, name, nameLenMax);
	lstrcpyn(_name, name, nameLenMax);
	int i = 0, j = 0;
	while (name[j] != 0 && i < nameLenMax)
	{
		if (name[j] != '&')
		{
			_name[i] = name[j];
			++i;
		}
		else //check if this ampersand is being escaped
		{
			if (name[j+1] == '&') //escaped ampersand
			{
				_name[i] = name[j];
				++i;
				++j;	//skip escaped ampersand
			}
		}
		++j;
	}
	_name[i] = 0;
}

generic_string ScintillaKeyMap::toString() const
{
	generic_string sc = L"";
	size_t nbCombos = getSize();
	for (size_t combo = 0; combo < nbCombos; ++combo)
	{
		sc += toString(combo);
		if (combo < nbCombos - 1)
			sc += L" or ";
	}
	return sc;
}

generic_string ScintillaKeyMap::toString(size_t index) const
{
	generic_string sc = L"";
	if (!isEnabled())
		return sc;

	KeyCombo kc = _keyCombos[index];
	if (kc._isCtrl)
		sc += L"Ctrl+";
	if (kc._isAlt)
		sc += L"Alt+";
	if (kc._isShift)
		sc += L"Shift+";

	generic_string keyString;
	getKeyStrFromVal(kc._key, keyString);
	sc += keyString;
	return sc;
}

KeyCombo ScintillaKeyMap::getKeyComboByIndex(size_t index) const
{
	return _keyCombos[index];
}

void ScintillaKeyMap::setKeyComboByIndex(int index, KeyCombo combo)
{
	if (combo._key == 0 && (_size > 1))
	{	//remove the item if possible
		_keyCombos.erase(_keyCombos.begin() + index);
	}
	_keyCombos[index] = combo;
}

void ScintillaKeyMap::removeKeyComboByIndex(size_t index)
{
	if (_size > 1 && index < _size)
	{
		_keyCombos.erase(_keyCombos.begin() + index);
		_size--;
	}
}

int ScintillaKeyMap::addKeyCombo(KeyCombo combo)
{	//returns index where key is added, or -1 when invalid
	if (combo._key == 0)	//do not allow to add disabled keycombos
		return -1;
	if (!isEnabled())
	{	//disabled, override current combo with new enabled one
		_keyCombos[0] = combo;
		return 0;
	}

	for (size_t i = 0; i < _size; ++i)
	{	//if already in the list do not add it
		KeyCombo & kc = _keyCombos[i];
		if (combo._key == kc._key && combo._isCtrl == kc._isCtrl && combo._isAlt == kc._isAlt && combo._isShift == kc._isShift)
			return static_cast<int32_t>(i);	//already in the list
	}
	_keyCombos.push_back(combo);
	++_size;
	return static_cast<int32_t>(_size - 1);
}

bool ScintillaKeyMap::isEnabled() const
{
	return (_keyCombos[0]._key != 0);
}

size_t ScintillaKeyMap::getSize() const
{
	return _size;
}

void getKeyStrFromVal(UCHAR keyVal, generic_string & str)
{
	str = L"";
	bool found = false;
	int i;
	for (i = 0; i < nbKeys; ++i)
	{
		if (keyVal == namedKeyArray[i].id)
		{
			found = true;
			break;
		}
	}
	if (found)
		str = namedKeyArray[i].name;
	else 
		str = L"Unlisted";
}

void getNameStrFromCmd(DWORD cmd, generic_string & str)
{
	if ((cmd >= ID_MACRO) && (cmd < ID_MACRO_LIMIT))
	{
		vector<MacroShortcut> & theMacros = (NppParameters::getInstance()).getMacroList();
		int i = cmd - ID_MACRO;
		str = theMacros[i].getName();
	}
	else if ((cmd >= ID_USER_CMD) && (cmd < ID_USER_CMD_LIMIT))
	{
		vector<UserCommand> & userCommands = (NppParameters::getInstance()).getUserCommandList();
		int i = cmd - ID_USER_CMD;
		str = userCommands[i].getName();
	}
	else if ((cmd >= ID_PLUGINS_CMD) && (cmd < ID_PLUGINS_CMD_LIMIT))
	{
		vector<PluginCmdShortcut> & pluginCmds = (NppParameters::getInstance()).getPluginCommandList();
		size_t i = 0;
		for (size_t j = 0, len = pluginCmds.size(); j < len ; ++j)
		{
			if (pluginCmds[j].getID() == cmd)
			{
				i = j;
				break;
			}
		}
		str = pluginCmds[i].getName();
	}
	else
	{
		HWND hNotepad_plus = ::FindWindow(Notepad_plus_Window::getClassName(), NULL);
		const int commandSize = 64;
		TCHAR cmdName[commandSize];
		HMENU m = reinterpret_cast<HMENU>(::SendMessage(hNotepad_plus, NPPM_INTERNAL_GETMENU, 0, 0));
		int nbChar = ::GetMenuString(m, cmd, cmdName, commandSize, MF_BYCOMMAND);
		if (!nbChar)
			return;
		bool fin = false;
		int j = 0;
		size_t len = lstrlen(cmdName);
		for (size_t i = 0 ; i < len; ++i)
		{
			switch(cmdName[i])
			{
				case '\t':
					cmdName[j] = '\0';
					fin = true;
					break;

				case '&':
					break;

				default :
					cmdName[j++] = cmdName[i];
			}
			if (fin)
				break;
		}
		cmdName[j] = '\0';
		str = cmdName;
	}
	return;
}

void Shortcut::updateConflictState(const bool endSession) const
{
	if (endSession)
	{
		// Clean up message for detached dialogs: save Macros/RunCommands
		::SendMessage(_hParent, NPPM_INTERNAL_FINDKEYCONFLICTS, 0, 0);
		return;
	}

	// Check for conflicts
	bool isConflict = false;
	::SendMessage(_hParent, NPPM_INTERNAL_FINDKEYCONFLICTS,
				  reinterpret_cast<WPARAM>(&_keyCombo), reinterpret_cast<LPARAM>(&isConflict));
	::ShowWindow(::GetDlgItem(_hSelf, IDC_CONFLICT_STATIC), isConflict ? SW_SHOW : SW_HIDE);
}

INT_PTR CALLBACK Shortcut::run_dlgProc(UINT Message, WPARAM wParam, LPARAM) 
{
	switch (Message)
	{
		case WM_INITDIALOG :
		{
			::SetDlgItemText(_hSelf, IDC_NAME_EDIT, getMenuName());	//display the menu name, with ampersands
			if (!_canModifyName)
				::SendDlgItemMessage(_hSelf, IDC_NAME_EDIT, EM_SETREADONLY, TRUE, 0);
			auto textlen = ::SendDlgItemMessage(_hSelf, IDC_NAME_EDIT, WM_GETTEXTLENGTH, 0, 0);

			::SendDlgItemMessage(_hSelf, IDC_CTRL_CHECK, BM_SETCHECK, _keyCombo._isCtrl?BST_CHECKED:BST_UNCHECKED, 0);
			::SendDlgItemMessage(_hSelf, IDC_ALT_CHECK, BM_SETCHECK, _keyCombo._isAlt?BST_CHECKED:BST_UNCHECKED, 0);
			::SendDlgItemMessage(_hSelf, IDC_SHIFT_CHECK, BM_SETCHECK, _keyCombo._isShift?BST_CHECKED:BST_UNCHECKED, 0);
			::EnableWindow(::GetDlgItem(_hSelf, IDOK), isValid() && (textlen > 0 || !_canModifyName));
			int iFound = -1;
			for (size_t i = 0 ; i < nbKeys ; ++i)
			{
				::SendDlgItemMessage(_hSelf, IDC_KEY_COMBO, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(namedKeyArray[i].name));

				if (_keyCombo._key == namedKeyArray[i].id)
					iFound = static_cast<int32_t>(i);
			}

			if (iFound != -1)
				::SendDlgItemMessage(_hSelf, IDC_KEY_COMBO, CB_SETCURSEL, iFound, 0);
			
			// Hide this warning on startup
			::ShowWindow(::GetDlgItem(_hSelf, IDC_WARNING_STATIC), SW_HIDE);
			
			updateConflictState();
			NativeLangSpeaker* nativeLangSpeaker = NppParameters::getInstance().getNativeLangSpeaker();
			nativeLangSpeaker->changeDlgLang(_hSelf, "ShortcutMapperSubDialg");
			goToCenter();
			return TRUE;
		}

		case WM_COMMAND : 
		{
			auto textlen = ::SendDlgItemMessage(_hSelf, IDC_NAME_EDIT, WM_GETTEXTLENGTH, 0, 0);
			switch (wParam)
			{
				case IDC_CTRL_CHECK :
					_keyCombo._isCtrl = BST_CHECKED == ::SendDlgItemMessage(_hSelf, static_cast<int32_t>(wParam), BM_GETCHECK, 0, 0);
					::EnableWindow(::GetDlgItem(_hSelf, IDOK), isValid() && (textlen > 0 || !_canModifyName));
					updateConflictState();
					return TRUE;

				case IDC_ALT_CHECK :
					_keyCombo._isAlt = BST_CHECKED == ::SendDlgItemMessage(_hSelf, static_cast<int32_t>(wParam), BM_GETCHECK, 0, 0);
					::EnableWindow(::GetDlgItem(_hSelf, IDOK), isValid() && (textlen > 0 || !_canModifyName));
					updateConflictState();
					return TRUE;

				case IDC_SHIFT_CHECK :
					_keyCombo._isShift = BST_CHECKED == ::SendDlgItemMessage(_hSelf, static_cast<int32_t>(wParam), BM_GETCHECK, 0, 0);
					updateConflictState();
					return TRUE;

				case IDOK :
					if (!isEnabled())
					{
						_keyCombo._isCtrl = _keyCombo._isAlt = _keyCombo._isShift = false;
					}

					if (_canModifyName)
					{
						TCHAR editName[nameLenMax];
						::SendDlgItemMessage(_hSelf, IDC_NAME_EDIT, WM_GETTEXT, nameLenMax, reinterpret_cast<LPARAM>(editName));
						setName(editName);
					}
					::EndDialog(_hSelf, 0);
					updateConflictState(true);
					return TRUE;

				case IDCANCEL :
					::EndDialog(_hSelf, -1);
					updateConflictState(true);
					return TRUE;

				default:
					if (HIWORD(wParam) == EN_CHANGE)
					{
						if (LOWORD(wParam) == IDC_NAME_EDIT)
						{
							::EnableWindow(::GetDlgItem(_hSelf, IDOK), isValid() && (textlen > 0 || !_canModifyName));
							return TRUE;
						}
					}
					else if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						if (LOWORD(wParam) == IDC_KEY_COMBO)
						{
							auto i = ::SendDlgItemMessage(_hSelf, LOWORD(wParam), CB_GETCURSEL, 0, 0);
							_keyCombo._key = namedKeyArray[i].id;
							::EnableWindow(::GetDlgItem(_hSelf, IDOK), isValid() && (textlen > 0 || !_canModifyName));
							::ShowWindow(::GetDlgItem(_hSelf, IDC_WARNING_STATIC), isEnabled()?SW_HIDE:SW_SHOW);
							updateConflictState();
							return TRUE;
						}
					}
					return FALSE;
			}
		}
		default :
			return FALSE;
	}
}

// return true if one of CommandShortcuts is deleted. Otherwise false.
void Accelerator::updateShortcuts() 
{
	const array<unsigned long, 3> incrFindAccIds = { IDM_SEARCH_FINDNEXT, IDM_SEARCH_FINDPREV, IDM_SEARCH_FINDINCREMENT };

	NppParameters& nppParam = NppParameters::getInstance();

	vector<CommandShortcut> & shortcuts = nppParam.getUserShortcuts();
	vector<MacroShortcut> & macros  = nppParam.getMacroList();
	vector<UserCommand> & userCommands = nppParam.getUserCommandList();
	vector<PluginCmdShortcut> & pluginCommands = nppParam.getPluginCommandList();

	size_t nbMenu = shortcuts.size();
	size_t nbMacro = macros.size();
	size_t nbUserCmd = userCommands.size();
	size_t nbPluginCmd = pluginCommands.size();

	delete [] _pAccelArray;
	_pAccelArray = new ACCEL[nbMenu+nbMacro+nbUserCmd+nbPluginCmd];
	vector<ACCEL> incrFindAcc;

	ACCEL *pSearchFindAccel = nullptr;
	int offset = 0;
	size_t i = 0;
	//no validation performed, it might be that invalid shortcuts are being used by default. Allows user to 'hack', might be a good thing
	for (i = 0; i < nbMenu; ++i)
	{
		if (shortcuts[i].isEnabled())
		{
			_pAccelArray[offset].cmd = static_cast<WORD>(shortcuts[i].getID());
			_pAccelArray[offset].fVirt = shortcuts[i].getAcceleratorModifiers();
			_pAccelArray[offset].key = shortcuts[i].getKeyCombo()._key;

			// Special extra handling for shortcuts shared by Incremental Find dialog
			if (std::find(incrFindAccIds.begin(), incrFindAccIds.end(), shortcuts[i].getID()) != incrFindAccIds.end())
				incrFindAcc.push_back(_pAccelArray[offset]);

			if (shortcuts[i].getID() == IDM_SEARCH_FIND)
				pSearchFindAccel = &_pAccelArray[offset];

			++offset;
		}
	}

	for (i = 0; i < nbMacro; ++i)
	{
		if (macros[i].isEnabled()) 
		{
			_pAccelArray[offset].cmd = (WORD)(macros[i].getID());
			_pAccelArray[offset].fVirt = macros[i].getAcceleratorModifiers();
			_pAccelArray[offset].key = macros[i].getKeyCombo()._key;
			++offset;
		}
	}

	for (i = 0; i < nbUserCmd; ++i)
	{
		if (userCommands[i].isEnabled())
		{
			_pAccelArray[offset].cmd = (WORD)(userCommands[i].getID());
			_pAccelArray[offset].fVirt = userCommands[i].getAcceleratorModifiers();
			_pAccelArray[offset].key = userCommands[i].getKeyCombo()._key;
			++offset;
		}
	}

	for (i = 0; i < nbPluginCmd; ++i)
	{
		if (pluginCommands[i].isEnabled())
		{
			_pAccelArray[offset].cmd = (WORD)(pluginCommands[i].getID());
			_pAccelArray[offset].fVirt = pluginCommands[i].getAcceleratorModifiers();
			_pAccelArray[offset].key = pluginCommands[i].getKeyCombo()._key;
			++offset;
		}
	}

	_nbAccelItems = offset;

	updateFullMenu();
	
	//update the table
	if (_hAccTable)
		::DestroyAcceleratorTable(_hAccTable);
	_hAccTable = ::CreateAcceleratorTable(_pAccelArray, _nbAccelItems);

	if (_hIncFindAccTab)
		::DestroyAcceleratorTable(_hIncFindAccTab);

	size_t nb = incrFindAcc.size();
	ACCEL *tmpIncrFindAccelArray = new ACCEL[nb];
	for (i = 0; i < nb; ++i)
	{
		tmpIncrFindAccelArray[i] = incrFindAcc[i];
	}
	_hIncFindAccTab = ::CreateAcceleratorTable(tmpIncrFindAccelArray, static_cast<int32_t>(nb));
	delete [] tmpIncrFindAccelArray;

	if (_hIncFindAccTab)
		::DestroyAcceleratorTable(_hIncFindAccTab);


	if (_hFindAccTab)
		::DestroyAcceleratorTable(_hFindAccTab);
	if (pSearchFindAccel != nullptr)
	{
		ACCEL *tmpFindAccelArray = new ACCEL[1];
		tmpFindAccelArray[0] = *pSearchFindAccel;
		_hFindAccTab = ::CreateAcceleratorTable(tmpFindAccelArray, 1);
		delete[] tmpFindAccelArray;
	}

	return;
}

void Accelerator::updateFullMenu()
{
	NppParameters& nppParam = NppParameters::getInstance();
	vector<CommandShortcut> commands = nppParam.getUserShortcuts();
	for (size_t i = 0; i < commands.size(); ++i)
	{
		updateMenuItemByCommand(commands[i]);
	}

	vector<MacroShortcut> mcommands = nppParam.getMacroList();
	for (size_t i = 0; i < mcommands.size(); ++i)
	{
		updateMenuItemByCommand(mcommands[i]);
	}

	vector<UserCommand> ucommands = nppParam.getUserCommandList();
	for (size_t i = 0; i < ucommands.size(); ++i)
	{
		updateMenuItemByCommand(ucommands[i]);
	}

	vector<PluginCmdShortcut> pcommands = nppParam.getPluginCommandList();
	for (size_t i = 0; i < pcommands.size(); ++i)
	{
		updateMenuItemByCommand(pcommands[i]);
	}

	::DrawMenuBar(_hMenuParent);
}

void Accelerator::updateMenuItemByCommand(const CommandShortcut& csc)
{
	int cmdID = csc.getID();
	
	// Ensure that the menu item checks set prior to this update remain in affect.
	// Ensure that the menu item state is also maintained
	UINT cmdFlags = GetMenuState(_hAccelMenu, cmdID, MF_BYCOMMAND );
	cmdFlags = MF_BYCOMMAND | ((cmdFlags&MF_CHECKED) ? MF_CHECKED : MF_UNCHECKED) | ((cmdFlags&MF_DISABLED) ? MF_DISABLED : MF_ENABLED);
	::ModifyMenu(_hAccelMenu, cmdID, cmdFlags, cmdID, csc.toMenuItemString().c_str());
}

recordedMacroStep::recordedMacroStep(int iMessage, uptr_t wParam, uptr_t lParam, int codepage)
	: _message(iMessage), _wParameter(wParam), _lParameter(lParam), _macroType(mtUseLParameter)
{ 
	if (_lParameter)
	{
		switch (_message)
		{
			case SCI_SETTEXT :
			case SCI_REPLACESEL :
			case SCI_REPLACETARGET :
			case SCI_REPLACETARGETRE :
			case SCI_SEARCHINTARGET :
			case SCI_ADDTEXT :
			case SCI_ADDSTYLEDTEXT :
			case SCI_INSERTTEXT :
			case SCI_APPENDTEXT :
			case SCI_SETWORDCHARS :
			case SCI_SETWHITESPACECHARS :
			case SCI_SETSTYLINGEX :
			case SCI_TEXTWIDTH :
			case SCI_STYLESETFONT :
			case SCI_SEARCHNEXT :
			case SCI_SEARCHPREV :
			case IDFINDWHAT:
			case IDREPLACEWITH:
			case IDD_FINDINFILES_DIR_COMBO:
			case IDD_FINDINFILES_FILTERS_COMBO:
			{
				char *ch = reinterpret_cast<char *>(_lParameter);
				TCHAR tch[2];
				::MultiByteToWideChar(codepage, 0, ch, -1, tch, 2);
				_sParameter = *tch;
				_macroType = mtUseSParameter;
				_lParameter = 0;
			}
			break;

				
			default : // for all other messages, use _lParameter "as is"
				break;
		}
	}
}

// code comes from Scintilla's Editor.cxx:
// void Editor::NotifyMacroRecord(unsigned int iMessage, uptr_t wParam, sptr_t lParam)
bool recordedMacroStep::isMacroable() const
{
	// Enumerates all macroable messages
	switch (_message)
	{
		case SCI_REPLACESEL: // (<unused>, const char *text)
		case SCI_ADDTEXT:    // (int length, const char *s)
		case SCI_INSERTTEXT: // (int pos, const char *text)
		case SCI_APPENDTEXT: // (int length, const char *s)
		case SCI_SEARCHNEXT: // (int searchFlags, const char *text)
		case SCI_SEARCHPREV: // (int searchFlags, const char *text)
		{
			if (_macroType == mtUseSParameter)
				return true;
			else
				return false;
		}

		case SCI_GOTOLINE:   // (int line)
		case SCI_GOTOPOS:    // (int position)
		case SCI_SETSELECTIONMODE:  // (int mode)
		case SCI_CUT:
		case SCI_COPY:
		case SCI_PASTE:
		case SCI_CLEAR:
		case SCI_CLEARALL:
		case SCI_SELECTALL:
		case SCI_SEARCHANCHOR:
		case SCI_LINEDOWN:
		case SCI_LINEDOWNEXTEND:
		case SCI_PARADOWN:
		case SCI_PARADOWNEXTEND:
		case SCI_LINEUP:
		case SCI_LINEUPEXTEND:
		case SCI_PARAUP:
		case SCI_PARAUPEXTEND:
		case SCI_CHARLEFT:
		case SCI_CHARLEFTEXTEND:
		case SCI_CHARRIGHT:
		case SCI_CHARRIGHTEXTEND:
		case SCI_WORDLEFT:
		case SCI_WORDLEFTEXTEND:
		case SCI_WORDRIGHT:
		case SCI_WORDRIGHTEXTEND:
		case SCI_WORDPARTLEFT:
		case SCI_WORDPARTLEFTEXTEND:
		case SCI_WORDPARTRIGHT:
		case SCI_WORDPARTRIGHTEXTEND:
		case SCI_WORDLEFTEND:
		case SCI_WORDLEFTENDEXTEND:
		case SCI_WORDRIGHTEND:
		case SCI_WORDRIGHTENDEXTEND:
		case SCI_HOME:
		case SCI_HOMEEXTEND:
		case SCI_LINEEND:
		case SCI_LINEENDEXTEND:
		case SCI_HOMEWRAP:
		case SCI_HOMEWRAPEXTEND:
		case SCI_LINEENDWRAP:
		case SCI_LINEENDWRAPEXTEND:
		case SCI_DOCUMENTSTART:
		case SCI_DOCUMENTSTARTEXTEND:
		case SCI_DOCUMENTEND:
		case SCI_DOCUMENTENDEXTEND:
		case SCI_STUTTEREDPAGEUP:
		case SCI_STUTTEREDPAGEUPEXTEND:
		case SCI_STUTTEREDPAGEDOWN:
		case SCI_STUTTEREDPAGEDOWNEXTEND:
		case SCI_PAGEUP:
		case SCI_PAGEUPEXTEND:
		case SCI_PAGEDOWN:
		case SCI_PAGEDOWNEXTEND:
		case SCI_EDITTOGGLEOVERTYPE:
		case SCI_CANCEL:
		case SCI_DELETEBACK:
		case SCI_TAB:
		case SCI_BACKTAB:
		case SCI_FORMFEED:
		case SCI_VCHOME:
		case SCI_VCHOMEEXTEND:
		case SCI_VCHOMEWRAP:
		case SCI_VCHOMEWRAPEXTEND:
		case SCI_VCHOMEDISPLAY:
		case SCI_VCHOMEDISPLAYEXTEND:
		case SCI_DELWORDLEFT:
		case SCI_DELWORDRIGHT:
		case SCI_DELWORDRIGHTEND:
		case SCI_DELLINELEFT:
		case SCI_DELLINERIGHT:
		case SCI_LINECOPY:
		case SCI_LINECUT:
		case SCI_LINEDELETE:
		case SCI_LINETRANSPOSE:
		case SCI_LINEDUPLICATE:
		case SCI_LOWERCASE:
		case SCI_UPPERCASE:
		case SCI_LINESCROLLDOWN:
		case SCI_LINESCROLLUP:
		case SCI_DELETEBACKNOTLINE:
		case SCI_HOMEDISPLAY:
		case SCI_HOMEDISPLAYEXTEND:
		case SCI_LINEENDDISPLAY:
		case SCI_LINEENDDISPLAYEXTEND:
		case SCI_LINEDOWNRECTEXTEND:
		case SCI_LINEUPRECTEXTEND:
		case SCI_CHARLEFTRECTEXTEND:
		case SCI_CHARRIGHTRECTEXTEND:
		case SCI_HOMERECTEXTEND:
		case SCI_VCHOMERECTEXTEND:
		case SCI_LINEENDRECTEXTEND:
		case SCI_PAGEUPRECTEXTEND:
		case SCI_PAGEDOWNRECTEXTEND:
		case SCI_SELECTIONDUPLICATE:
		case SCI_COPYALLOWLINE:
		case SCI_VERTICALCENTRECARET:
		case SCI_MOVESELECTEDLINESUP:
		case SCI_MOVESELECTEDLINESDOWN:
		case SCI_SCROLLTOSTART:
		case SCI_SCROLLTOEND:
		case SCI_SETVIRTUALSPACEOPTIONS:
		{
			if (_macroType == mtUseLParameter)
				return true;
			else
				return false;
		}

		// Filter out all others like display changes. Also, newlines are redundant
		// with char insert messages.
		case SCI_NEWLINE:
		default:
			return false;
	}
}

void recordedMacroStep::PlayBack(Window* pNotepad, ScintillaEditView *pEditView)
{
	if (_macroType == mtMenuCommand)
	{
		::SendMessage(pNotepad->getHSelf(), WM_COMMAND, _wParameter, 0);
	}
	else
	{
		// Ensure it's macroable message before send it
		if (!isMacroable())
			return;

		if (_macroType == mtUseSParameter) 
		{
			int byteBufferLength = ::WideCharToMultiByte(static_cast<UINT>(pEditView->execute(SCI_GETCODEPAGE)), 0, _sParameter.c_str(), -1, NULL, 0, NULL, NULL);
			auto byteBuffer = std::make_unique< char[] >(byteBufferLength);
			::WideCharToMultiByte(static_cast<UINT>(pEditView->execute(SCI_GETCODEPAGE)), 0, _sParameter.c_str(), -1, byteBuffer.get(), byteBufferLength, NULL, NULL);
			auto lParam = reinterpret_cast<LPARAM>(byteBuffer.get());
			pEditView->execute(_message, _wParameter, lParam);
		}
		else
		{
			pEditView->execute(_message, _wParameter, _lParameter);
		}

		// If text content has been modified in Scintilla,
		// then notify Notepad++
		if ( (_message == SCI_SETTEXT)
			|| (_message == SCI_REPLACESEL) 
			|| (_message == SCI_ADDTEXT) 
			|| (_message == SCI_ADDSTYLEDTEXT) 
			|| (_message == SCI_INSERTTEXT) 
			|| (_message == SCI_APPENDTEXT) )
		{
			SCNotification scnN;
			scnN.nmhdr.code = SCN_CHARADDED;
			scnN.nmhdr.hwndFrom = pEditView->getHSelf();
			scnN.nmhdr.idFrom = 0;
			if (_sParameter.empty())
				scnN.ch = 0;
			else
				scnN.ch = _sParameter.at(0);

			::SendMessage(pNotepad->getHSelf(), WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&scnN));
		}
	}
}

void ScintillaAccelerator::init(vector<HWND> * vScintillas, HMENU hMenu, HWND menuParent)
{
	_hAccelMenu = hMenu;
	_hMenuParent = menuParent;
	size_t nbScintilla = vScintillas->size();
	for (size_t i = 0; i < nbScintilla; ++i)
	{
		_vScintillas.push_back(vScintillas->at(i));
	}
}

void ScintillaAccelerator::updateKeys() 
{
	NppParameters& nppParam = NppParameters::getInstance();
	vector<ScintillaKeyMap> & map = nppParam.getScintillaKeyList();
	size_t mapSize = map.size();
	size_t index;
	size_t nb = nbScintillas();
	for (size_t i = 0; i < nb; ++i)
	{
		::SendMessage(_vScintillas[i], SCI_CLEARALLCMDKEYS, 0, 0);
		for (int32_t j = static_cast<int32_t>(mapSize) - 1; j >= 0; j--) //reverse order, top of the list has highest priority
		{	
			ScintillaKeyMap skm = map[j];
			if (skm.isEnabled()) 
			{		//no validating, scintilla accepts more keys
				size_t size = skm.getSize();
				for (index = 0; index < size; ++index)
					::SendMessage(_vScintillas[i], SCI_ASSIGNCMDKEY, skm.toKeyDef(index), skm.getScintillaKeyID());
			}
			if (skm.getMenuCmdID() != 0) 
			{
				updateMenuItemByID(skm, skm.getMenuCmdID());
			}
			if (j == 0)	//j is unsigned, so default method doesnt work
				break;
		}
	}
}

void ScintillaAccelerator::updateMenuItemByID(const ScintillaKeyMap& skm, int id)
{
	const int commandSize = 64;
	TCHAR cmdName[commandSize];
	::GetMenuString(_hAccelMenu, id, cmdName, commandSize, MF_BYCOMMAND);
	int i = 0;
	while (cmdName[i] != 0)
	{
		if (cmdName[i] == '\t')
		{
			cmdName[i] = 0;
			break;
		}
		++i;
	}
	generic_string menuItem = cmdName;
	if (skm.isEnabled())
	{
		menuItem += L"\t";
		//menuItem += L"Sc:";	//sc: scintilla shortcut
		menuItem += skm.toString();
	}
	::ModifyMenu(_hAccelMenu, id, MF_BYCOMMAND, id, menuItem.c_str());
	::DrawMenuBar(_hMenuParent);
}

//This procedure uses _keyCombo as a temp. variable to store current settings which can then later be applied (by pressing OK)
void ScintillaKeyMap::applyToCurrentIndex()
{
	int index = static_cast<int>(::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_GETCURSEL, 0, 0));
	if (index == LB_ERR)
		return;
	setKeyComboByIndex(static_cast<int>(index), _keyCombo);
	updateListItem(index);
	::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_SETCURSEL, index, 0);

}

void ScintillaKeyMap::validateDialog()
{
	bool valid = isValid();	//current combo valid?
	bool isDisabling = _keyCombo._key == 0;	//true if this keycombo were to disable the shortcut
	bool isDisabled = !isEnabled();	//true if this shortcut already is 
	bool isDuplicate = false; //true if already in the list

	for (size_t i = 0; i < _size; ++i) 
	{
		if (_keyCombo._key   == _keyCombos[i]._key   && _keyCombo._isCtrl  == _keyCombos[i]._isCtrl &&
			_keyCombo._isAlt == _keyCombos[i]._isAlt && _keyCombo._isShift == _keyCombos[i]._isShift)
		{
			isDuplicate = true;
			break;
		}
	}

	::EnableWindow(::GetDlgItem(_hSelf, IDC_BUTTON_ADD), valid && !isDisabling && !isDuplicate);
	::EnableWindow(::GetDlgItem(_hSelf, IDC_BUTTON_APPLY), valid && (!isDisabling || _size == 1) && !isDuplicate);
	::EnableWindow(::GetDlgItem(_hSelf, IDC_BUTTON_RMVE), (_size > 1)?TRUE:FALSE);
	::ShowWindow(::GetDlgItem(_hSelf, IDC_WARNING_STATIC), isDisabled?SW_SHOW:SW_HIDE);
	updateConflictState();
}

void ScintillaKeyMap::showCurrentSettings()
{
	auto keyIndex = ::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_GETCURSEL, 0, 0);
	_keyCombo = _keyCombos[keyIndex];
	::SendDlgItemMessage(_hSelf, IDC_CTRL_CHECK,	BM_SETCHECK, _keyCombo._isCtrl?BST_CHECKED:BST_UNCHECKED, 0);
	::SendDlgItemMessage(_hSelf, IDC_ALT_CHECK,		BM_SETCHECK, _keyCombo._isAlt?BST_CHECKED:BST_UNCHECKED, 0);
	::SendDlgItemMessage(_hSelf, IDC_SHIFT_CHECK,	BM_SETCHECK, _keyCombo._isShift?BST_CHECKED:BST_UNCHECKED, 0);
	for (size_t i = 0 ; i < nbKeys ; ++i)
	{
		if (_keyCombo._key == namedKeyArray[i].id)
		{
			::SendDlgItemMessage(_hSelf, IDC_KEY_COMBO, CB_SETCURSEL, i, 0);
			break;
		}
	}
}

void ScintillaKeyMap::updateListItem(int index)
{
	::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_INSERTSTRING, index, reinterpret_cast<LPARAM>(toString(index).c_str()));
	::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_DELETESTRING, index+1, 0);
}

INT_PTR CALLBACK ScintillaKeyMap::run_dlgProc(UINT Message, WPARAM wParam, LPARAM) 
{
	
	switch (Message)
	{
		case WM_INITDIALOG :
		{
			::SetDlgItemText(_hSelf, IDC_NAME_EDIT, _name);
			_keyCombo = _keyCombos[0];

			for (size_t i = 0 ; i < nbKeys ; ++i)
			{
				::SendDlgItemMessage(_hSelf, IDC_KEY_COMBO, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(namedKeyArray[i].name));
			}

			for (size_t i = 0; i < _size; ++i)
			{
				::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(toString(i).c_str()));
			}
			::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_SETCURSEL, 0, 0);

			showCurrentSettings();
			validateDialog();

			// Hide this warning on startup
			::ShowWindow(::GetDlgItem(_hSelf, IDC_WARNING_STATIC), SW_HIDE);

			NativeLangSpeaker* nativeLangSpeaker = NppParameters::getInstance().getNativeLangSpeaker();
			nativeLangSpeaker->changeDlgLang(_hSelf, "ShortcutMapperSubDialg");
			goToCenter();
			return TRUE;
		}

		case WM_COMMAND : 
		{
			switch (wParam)
			{
				case IDC_CTRL_CHECK :
					_keyCombo._isCtrl = BST_CHECKED == ::SendDlgItemMessage(_hSelf, static_cast<int32_t>(wParam), BM_GETCHECK, 0, 0);
					validateDialog();
					return TRUE;

				case IDC_ALT_CHECK :
					_keyCombo._isAlt = BST_CHECKED == ::SendDlgItemMessage(_hSelf, static_cast<int32_t>(wParam), BM_GETCHECK, 0, 0);
					validateDialog();
					return TRUE;

				case IDC_SHIFT_CHECK :
					_keyCombo._isShift = BST_CHECKED == ::SendDlgItemMessage(_hSelf, static_cast<int32_t>(wParam), BM_GETCHECK, 0, 0);
					validateDialog();
					return TRUE;

				case IDOK :
					//Cleanup
					_keyCombo._key = 0;
					_keyCombo._isCtrl = _keyCombo._isAlt = _keyCombo._isShift = false;
					::EndDialog(_hSelf, 0);
					return TRUE;

				case IDCANCEL :
					::EndDialog(_hSelf, -1);
					return TRUE;

				case IDC_BUTTON_ADD: 
				{
					size_t oldsize = _size;
					int res = addKeyCombo(_keyCombo);
					if (res > -1)
					{
						if (res == static_cast<int32_t>(oldsize))
						{
							::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_INSERTSTRING, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(toString(res).c_str()));
						}
						else
						{	//update current generic_string, can happen if it was disabled
							updateListItem(res);
						}
						::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_SETCURSEL, res, 0);
					}
					showCurrentSettings();
					validateDialog();
					return TRUE; 
				}

				case IDC_BUTTON_RMVE:
				{
					if (_size == 1)	//cannot delete last shortcut
						return TRUE;
					auto i = ::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_GETCURSEL, 0, 0);
					removeKeyComboByIndex(i);
					::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_DELETESTRING, i, 0);
					if (static_cast<size_t>(i) == _size)
						i = _size - 1;
					::SendDlgItemMessage(_hSelf, IDC_LIST_KEYS, LB_SETCURSEL, i, 0);
					showCurrentSettings();
					validateDialog();
					return TRUE; 
				}

				case IDC_BUTTON_APPLY:
				{
					applyToCurrentIndex();
					validateDialog();
					return TRUE;
				}

				default:
					if (HIWORD(wParam) == CBN_SELCHANGE || HIWORD(wParam) == LBN_SELCHANGE)
					{
						switch(LOWORD(wParam))
						{
							case IDC_KEY_COMBO:
							{
								auto i = ::SendDlgItemMessage(_hSelf, IDC_KEY_COMBO, CB_GETCURSEL, 0, 0);
								_keyCombo._key = namedKeyArray[i].id;
								::ShowWindow(::GetDlgItem(_hSelf, IDC_WARNING_STATIC), isEnabled() ? SW_HIDE : SW_SHOW);
								validateDialog();
								return TRUE;
							}
							case IDC_LIST_KEYS:
							{
								showCurrentSettings();
								validateDialog();
								return TRUE;
							}
						}
					}
					return FALSE;
			}
		}
		default :
			return FALSE;
	}

	//return FALSE;
}

CommandShortcut::CommandShortcut(const Shortcut& sc, long id) :	Shortcut(sc), _id(id)
{
	if ( _id < IDM_EDIT)
		_category = L"File";
	else if ( _id < IDM_SEARCH)
		_category = L"Edit";
	else if (_id >= IDM_EDIT_AUTOCOMPLETE and _id <= IDM_EDIT_AUTOCOMPLETE_PATH)
		_category = L"Edit";
	else if ( _id < IDM_VIEW)
		_category = L"Search";
	else if ( _id < IDM_FORMAT)
		_category = L"View";
	else if ( _id >= IDM_VIEW_GOTO_ANOTHER_VIEW and _id <= IDM_VIEW_LOAD_IN_NEW_INSTANCE)
		_category = L"View";
	else if ( _id < IDM_LANG)
		_category = L"Format";
	else if ( _id < IDM_ABOUT)
		_category = L"Lang";
	else if ( _id < IDM_SETTING)
		_category = L"About";
	else if ( _id < IDM_TOOL)
		_category = L"Setting";
	else if ( _id < IDM_EXECUTE)
		_category = L"Tool";
	else
		_category = L"Execute";
}