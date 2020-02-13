//this file is part of notepad++
//Copyright (C)2003 Don HO ( donho@altern.org )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#pragma once

#include <oleacc.h>
#include "Common.h"
#include "RunDlg_rc.h"

#define CURRENTWORD_MAXLENGTH 2048

const TCHAR fullCurrentPath[] = L"FULL_CURRENT_PATH";
const TCHAR currentDirectory[] = L"CURRENT_DIRECTORY";
const TCHAR onlyFileName[] = L"FILE_NAME";
const TCHAR fileNamePart[] = L"NAME_PART";
const TCHAR fileExtPart[] = L"EXT_PART";
const TCHAR currentWord[] = L"CURRENT_WORD";
const TCHAR nppDir[] = L"NPP_DIRECTORY";
const TCHAR nppFullFilePath[] = L"NPP_FULL_FILE_PATH";
const TCHAR currentLine[] = L"CURRENT_LINE";
const TCHAR currentColumn[] = L"CURRENT_COLUMN";

int whichVar(TCHAR *str);
void expandNppEnvironmentStrs(const TCHAR *strSrc, TCHAR *stringDest, size_t strDestLen, HWND hWnd);

class Command {
public :
	Command() = default;
	explicit Command(TCHAR *cmd) : _cmdLine(cmd){};
	explicit Command(const generic_string& cmd) : _cmdLine(cmd){};
	HINSTANCE run(HWND hWnd);
	HINSTANCE run(HWND hWnd, const TCHAR* cwd);

protected :
	generic_string _cmdLine;
private :
	void extractArgs(TCHAR *cmd2Exec, size_t cmd2ExecLen, TCHAR *args, size_t argsLen, const TCHAR *cmdEntier);
};

class RunDlg : public Command, public StaticDialog
{
public :
	RunDlg() = default;

	void doDialog(bool isRTL = false);
    virtual void destroy() {};

protected :
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

private :
	void addTextToCombo(const TCHAR *txt2Add) const;
	void removeTextFromCombo(const TCHAR *txt2Remove) const;
};

