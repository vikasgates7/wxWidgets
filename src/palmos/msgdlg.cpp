/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/msgdlg.cpp
// Purpose:     wxMessageDialog
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/msgdlg.h"
#endif

#include <Loader.h>
#include <Form.h>

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

int wxMessageDialog::ShowModal()
{
    int AlertID=1000;
    int Result=0;
    int wxResult=wxID_OK;
    const long style = GetMessageDialogStyle();

    // Handle to the currently running application database
    DmOpenRef    AppDB;
    SysGetModuleDatabase(SysGetRefNum(), NULL, &AppDB);

    // Translate wx styles into Palm OS styles
    if (style & wxYES_NO)
    {
        if (style & wxCANCEL)
            AlertID=1300; // Yes No Cancel
        else
            AlertID=1200; // Yes No
    }
    if (style & wxOK)
    {
        if (style & wxCANCEL)
            AlertID=1100; // Ok Cancel
        else
            AlertID=1000; // Ok
    }

    // Add the icon styles
    if (style & wxICON_EXCLAMATION)
        AlertID=AlertID+0; // Warning
    else if (style & wxICON_HAND)
        AlertID=AlertID+1; // Error
    else if (style & wxICON_INFORMATION)
        AlertID=AlertID+2; // Information
    else if (style & wxICON_QUESTION)
        AlertID=AlertID+3; // Confirmation

    // The Palm OS Dialog API does not support custom titles in a dialog box.
    // So we have to set the title by manipulating the resource.

    // Get the alert resource
    char *AlertPtr;
    MemHandle AlertHandle;
    AlertHandle=DmGetResource(AppDB,'Talt',AlertID);

    AlertPtr=(char *)MemHandleLock(AlertHandle);
    AlertPtr+=8;

    // Clear out any old title.  This must be done with a static array of chars
    // because using MemSet is not supported on resources and could result in
    // crashes or unpredictable behaviour.
    char ClearTitle[25]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
    MemMove(AlertPtr,&ClearTitle,25);

    // Get the title length and make sure it is not too long
    int TitleLength=m_caption.length();
    if(TitleLength>25)
        TitleLength=25;

    // Center the title in the window
    int BufferLength=(25-TitleLength)/2;
    AlertPtr+=BufferLength;

    // Copy the title
    MemMove(AlertPtr,m_caption.c_str(),TitleLength);

    // Release the resource
    MemHandleUnlock(AlertHandle);
    DmReleaseResource(AlertHandle);

    // Display the dialog
    Result=FrmCustomAlert(AppDB,AlertID,GetFullMessage().c_str(),"","");

    // Convert the Palm OS result to wxResult
    if(AlertID<1100)
    {
        // Ok
        wxResult=wxID_OK;
    }
    else if(AlertID<1200)
    {
        // Ok Cancel
        if(Result==0)
            wxResult=wxID_OK;
        else
            wxResult=wxID_CANCEL;
    }
    else if(AlertID<1300)
    {
        // Yes No
        if(Result==0)
            wxResult=wxID_YES;
        else
            wxResult=wxID_NO;
    }
    else
    {
        // Yes No Cancel
        if(Result==0)
            wxResult=wxID_YES;
        else if(Result==1)
            wxResult=wxID_NO;
        else
            wxResult=wxID_CANCEL;
    }

    return wxResult;
}
