// =================================================================================
//	CTextView.h						©1996-2000 Metrowerks Inc. All rights reserved.
// =================================================================================

#ifndef _H_CTextView
#define _H_CTextView
#pragma once

#include <LTextEditView.h>

class CTextView : public LTextEditView {

public:
			enum { class_ID = FOUR_CHAR_CODE('TxtV') };
	
							CTextView(
								LStream*		inStream);
	virtual					~CTextView();
	
	virtual void			UserChangedText();
	
	virtual void			SavePlace(
								LStream*		outPlace);
	virtual void			RestorePlace(
								LStream*		inPlace);

			Boolean			IsDirty() const
								{
									return mIsDirty;
								}
			void			SetDirty(
								Boolean			inDirty)
								{
									mIsDirty = inDirty;
								}
								
protected:
			bool			mIsDirty;

private:
							CTextView();
							CTextView(const CTextView& inOriginal);
			CTextView&		operator=(const CTextView& inRhs);
};

#endif // _H_CTextView