#ifndef __UCommandDialogs__
#define __UCommandDialogs__

#include <UModalDialogs.h>

//! helper class to handle optional dialogs being hidden
//  if requested by user:
//  A checkbox pane within the dialog tells whether to hide
//  the dialog.
//  If the checkbox is not checked, the dialog is hidden <b>only</b> when
//  the appropriate modifier is pressed.
//  If the checkbox is checked, the dialog is hidden <b>unless</b> the
//  appropriate modifier is pressed.
class StOptionalDialogHandler : public StDialogHandler
{
  typedef StDialogHandler Inherited;
public:
  StOptionalDialogHandler(
                ResIDT			inDialogResID,
                bool        inSkipDialogByDefault,
                LCommander*		inSuper);

  StOptionalDialogHandler(
                LWindow*		inWindow,
                bool        inSkipDialogByDefault,
                LCommander*		inSuper);

  //! Show the dialog - use this instead of LWindow::Show
  //  to make sure the dialog is shown only if it's not
  //  to be hidden.
  //
  void ShowDialog();
  
  //! Overrides standard behavior to immediately return msg_OK
  //  if dialog is to be skipped.
  //
  virtual MessageT DoDialog(); // Override
  
  //! Return current setting of "hide dialog" value
  //
  bool GetHideCommandDlg() const { return m_SkipDialogValue; }
  
protected:
  bool      m_SkipDialogValue;
  bool      m_ShowDialog;
};

#endif // __UCommandDialogs__
