#include "stdafx.h"
#include "UCommandDialogs.h"
#include "CvsPrefs.h"
#include "MacCvsConstant.h"

namespace {

  inline UInt32 GetCommandDialogToggleKey()
  {
    return /*gCvsPrefs.HideCommandDlgUseShift() ? shiftKey :*/ optionKey;
  }
  
  inline bool HideCommandDialogToggleKeyPressed()
  {
    return (GetCurrentKeyModifiers() & GetCommandDialogToggleKey()) != 0;
  }
  
  inline bool ShiftKeyDown()
  {
    return (GetCurrentKeyModifiers() & (GetCommandDialogToggleKey() == shiftKey ? optionKey : shiftKey)) != 0;
  }
  
} // end of anonymous namespace


StOptionalDialogHandler::StOptionalDialogHandler(
                ResIDT			inDialogResID,
                bool        inSkipDialogByDefault,
                LCommander*		inSuper) : Inherited(inDialogResID, inSuper),
           m_SkipDialogValue(inSkipDialogByDefault),
           m_ShowDialog(!inSkipDialogByDefault || HideCommandDialogToggleKeyPressed() || ShiftKeyDown())
{
}

StOptionalDialogHandler::StOptionalDialogHandler(
                LWindow*		inWindow,
                bool        inSkipDialogByDefault,
                LCommander*		inSuper) : Inherited(inWindow, inSuper),
           m_SkipDialogValue(inSkipDialogByDefault),
           m_ShowDialog(!inSkipDialogByDefault || HideCommandDialogToggleKeyPressed() || ShiftKeyDown())
{
}

void StOptionalDialogHandler::ShowDialog()
{
  LWindow*    dialog(this->GetDialog());
  if ( dialog )
  {
    // set up the 'hide' checkbox
    //
    // - value
    DoDataExchange(dialog, item_HideCmdDlg, m_SkipDialogValue, true);
    // - and descriptor
    char    title[256];
    title[0] = sprintf(&title[1], "Show only when %s key is down", GetCommandDialogToggleKey() == shiftKey ? "shift" : "option");
		dialog->SetDescriptorForPaneID(item_HideCmdDlg, (unsigned char*)title);
    
    if ( m_ShowDialog ) dialog->Show();
  }
}

MessageT StOptionalDialogHandler::DoDialog() // Override
{
  // if we're to skip the dialog, return msg_OK immediately
  //
  if ( !m_ShowDialog ) return msg_OK;
  
  // otherwise, let StDialogHandler do it's job
  //
  MessageT  result(Inherited::DoDialog());
  if ( result == msg_OK )
  {
    // update value of "hide dialog" flag
    //
    DoDataExchange(this->GetDialog(), item_HideCmdDlg, m_SkipDialogValue, false);
  }
  return result;
}
