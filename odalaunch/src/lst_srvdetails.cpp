// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 2006-2012 by The Odamex Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//   List control for handling extra server details
//
//-----------------------------------------------------------------------------

#include <algorithm>

#include "lst_srvdetails.h"
#include "str_utils.h"

#include <wx/settings.h>

using namespace odalpapi;

IMPLEMENT_DYNAMIC_CLASS(LstOdaSrvDetails, wxListCtrl)

typedef enum
{
	srvdetails_field_name
	,srvdetails_field_value

	,max_srvdetails_fields
} srvdetails_fields_t;

LstOdaSrvDetails::LstOdaSrvDetails()
{
	ItemShade = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	BgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

	Header = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
	HeaderText = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
}

// Adjusts the width of the name and value columns to the longest item
void LstOdaSrvDetails::ResizeNameValueColumns()
{
	SetColumnWidth(srvdetails_field_name, wxLIST_AUTOSIZE);
	SetColumnWidth(srvdetails_field_value, wxLIST_AUTOSIZE);
}

void LstOdaSrvDetails::InsertHeader(const wxString& Name,
                                    wxColor NameColor,
                                    wxColor NameBGColor)
{
	wxListItem ListItem;

	ListItem.SetMask(wxLIST_MASK_TEXT);

	// Name Column
	ListItem.SetText(Name);
	ListItem.SetColumn(srvdetails_field_name);
	ListItem.SetId(InsertItem(GetItemCount(), ListItem.GetText()));

	if(NameColor == wxNullColour)
		NameColor = HeaderText;

	if(NameBGColor == wxNullColour)
		NameBGColor = Header;

	ListItem.SetBackgroundColour(NameBGColor);
	ListItem.SetTextColour(NameColor);
	SetItem(ListItem);
}

void LstOdaSrvDetails::InsertLine(const wxString& Name, const wxString& Value)
{
	size_t i = 0;
	wxString Str;
	wxListItem ListItem;

	ListItem.SetMask(wxLIST_MASK_TEXT);

	// Name Column
	ListItem.SetText(Name);
	ListItem.SetColumn(srvdetails_field_name);
	ListItem.SetId(InsertItem(GetItemCount(), ListItem.GetText()));

	if(BGItemAlternator == BgColor)
	{
		BGItemAlternator = ItemShade;
	}
	else
		BGItemAlternator = BgColor;

	ListItem.SetBackgroundColour(BGItemAlternator);

	SetItem(ListItem);

	// Value Column
	// Detect newlines and break on them
	while(i < Value.Length())
	{
		// Insert new line
		if(Value[i] == wxT('\\') &&
		        (i + 1 < Value.Length() && Value[i+1] == wxT('n')))
		{
			ListItem.SetColumn(srvdetails_field_value);
			ListItem.SetText(Str);
			SetItem(ListItem);

			ListItem.SetColumn(srvdetails_field_name);
			ListItem.SetId(InsertItem(GetItemCount(), wxT("")));
			ListItem.SetText(wxT(""));
			SetItem(ListItem);

			Str.Clear();

			++i;
			++i;

			continue;
		}

		Str += Value[i];

		++i;
	}

	if(i == Value.Length())
	{
		ListItem.SetColumn(srvdetails_field_value);
		ListItem.SetText(Str);
		SetItem(ListItem);
	}
}

static bool CvarCompare(const Cvar_t& a, const Cvar_t& b)
{
	return a.Name < b.Name;
}

void LstOdaSrvDetails::ToggleGameStatusSection(const Server& In)
{
	wxString TimeLeft;
	bool addtime = false;
	bool addsl = false;

	if(In.Info.TimeLimit)
	{
		if(In.Info.TimeLeft)
		{
			TimeLeft = wxString::Format(wxT("%.2d:%.2d"),
			                            In.Info.TimeLeft / 60, In.Info.TimeLeft % 60);
		}
		else
			TimeLeft = wxT("00:00");

		addtime = true;
	}

	if(In.Info.GameType == GT_TeamDeathmatch ||
	        In.Info.GameType == GT_CaptureTheFlag)
	{
		addsl = true;
	}

	if(addtime || addsl)
	{
		InsertLine(wxT(""), wxT(""));
		InsertHeader(wxT("Game Status"));

		if(addtime)
			InsertLine(wxT("Time left (HH:MM)"), TimeLeft);

		if(addsl)
		{
			InsertLine(wxT("Score Limit"), wxString::Format(wxT("%u"),
			           In.Info.ScoreLimit));
		}
	}
}

void LstOdaSrvDetails::LoadDetailsFromServer(const Server& In)
{
	DeleteAllItems();
	DeleteAllColumns();

	if(In.GotResponse() == false)
		return;

	// Begin adding data to the control
	InsertColumn(srvdetails_field_name, wxT(""), wxLIST_FORMAT_LEFT, 150);
	InsertColumn(srvdetails_field_value, wxT(""), wxLIST_FORMAT_LEFT, 150);

	// Version
	InsertLine(wxT("Version"), wxString::Format(wxT("%u.%u.%u-r%u"),
	           In.Info.VersionMajor,
	           In.Info.VersionMinor,
	           In.Info.VersionPatch,
	           In.Info.VersionRevision));

	InsertLine(wxT("QP Version"), wxString::Format(wxT("%u"),
	           In.Info.VersionProtocol));

	// Add this section only if its needed
	ToggleGameStatusSection(In);

	// Patch (BEX/DEH) files
	InsertLine(wxT(""), wxT(""));
	InsertHeader(wxT("BEX/DEH Files"));

	if(In.Info.Patches.empty())
	{
		InsertLine(wxT("None"), wxT(""));
	}
	else
	{
		size_t i = 0;
		size_t PatchesCount = In.Info.Patches.size();

		wxString Current, Next;

		// A while loop is used to format this correctly
		while(i < PatchesCount)
		{
			Current = stdstr_towxstr(In.Info.Patches[i]);

			++i;

			if(i < PatchesCount)
				Next = stdstr_towxstr(In.Info.Patches[i]);

			++i;

			InsertLine(Current, Next);

			Current = wxT("");
			Next = wxT("");
		}
	}

	// Sort cvars ascending
	std::vector<Cvar_t> Cvars = In.Info.Cvars;

	sort(Cvars.begin(), Cvars.end(), CvarCompare);

	// Cvars that are enabled
	InsertLine(wxT(""), wxT(""));
	InsertHeader(wxT("Cvars Enabled"));

	for(size_t i = 0; i < Cvars.size(); ++i)
	{
		if(Cvars[i].Type == CVARTYPE_BOOL)
			InsertLine(stdstr_towxstr(Cvars[i].Name), wxT(""));
	}

	// Gameplay settings
	InsertLine(wxT(""), wxT(""));
	InsertHeader(wxT("Gameplay Variables"));

	for(size_t i = 0; i < Cvars.size(); ++i)
	{
		wxString Name = stdstr_towxstr(Cvars[i].Name);

		switch(Cvars[i].Type)
		{
		case CVARTYPE_BYTE:
		{
			wxString Value;

			Value = wxString::Format(wxT("%d"), Cvars[i].i8);

			InsertLine(Name, Value);
		}
		break;

		case CVARTYPE_WORD:
		{
			wxString Value;

			Value = wxString::Format(wxT("%d"), Cvars[i].i16);

			InsertLine(Name, Value);
		}
		break;

		case CVARTYPE_INT:
		{
			wxString Value;

			Value = wxString::Format(wxT("%d"), Cvars[i].i32);

			InsertLine(Name, Value);
		}
		break;

		case CVARTYPE_FLOAT:
		case CVARTYPE_STRING:
		{
			InsertLine(Name, stdstr_towxstr(Cvars[i].Value));
		}
		break;

		case CVARTYPE_NONE:
		case CVARTYPE_MAX:
		default:
		{

		}
		break;
		}
	}

	// Resize the columns
	ResizeNameValueColumns();
}
