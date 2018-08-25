//
// Created by zzzhr on 2018/8/23.
//

#ifndef FUCK_COMMON_ID_H
#define FUCK_COMMON_ID_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
enum
{
    ID_CLEARLOG = wxID_HIGHEST+1,
    ID_BACKGROUND_COLOUR,
    ID_FOREGROUND_COLOUR,
    ID_STYLE_MENU,

    // file menu
    //ID_SINGLE,        wxDV_SINGLE==0 so it's always present
            ID_MULTIPLE,
    ID_ROW_LINES,
    ID_HORIZ_RULES,
    ID_VERT_RULES,

    ID_EXIT = wxID_EXIT,

    // about menu
            ID_ABOUT = wxID_ABOUT,


    // control IDs

    ID_MUSIC_CTRL       = 50,
    ID_ATTR_CTRL        = 51,

    ID_ADD_MOZART       = 100,
    ID_DELETE_SEL       = 101,
    ID_DELETE_YEAR      = 102,
    ID_SELECT_NINTH     = 103,
    ID_COLLAPSE         = 104,
    ID_EXPAND           = 105,
    ID_SHOW_CURRENT,
    ID_SET_NINTH_CURRENT,

    ID_PREPEND_LIST     = 200,
    ID_DELETE_LIST      = 201,
    ID_GOTO             = 202,
    ID_ADD_MANY         = 203,
    ID_HIDE_ATTRIBUTES  = 204,
    ID_SHOW_ATTRIBUTES  = 205,

    // Fourth page.
            ID_DELETE_TREE_ITEM = 400,
    ID_DELETE_ALL_TREE_ITEMS = 401,
    ID_ADD_TREE_ITEM = 402,
    ID_ADD_TREE_CONTAINER_ITEM = 403
};
#endif //FUCK_COMMON_ID_H
