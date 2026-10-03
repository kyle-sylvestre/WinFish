#ifndef __CHEATSDIALOG_H__
#define __CHEATSDIALOG_H__

#include "MoneyDialog.h"
#include "SexyAppFramework/CheckboxListener.h"

namespace Sexy
{
	class WinFishApp;
	class Checkbox;
	class Slider;

	class CheatsDialog : public MoneyDialog, public CheckboxListener
	{
	public:
		struct Cheat
		{
			std::string mCode;
			int mID;
			Checkbox *mCheckbox;
		};
		WinFishApp* mApp;
		bool mFlag;
		std::vector<Cheat> mCheats;

	public:
		CheatsDialog(WinFishApp* theApp, bool theFlag);
		virtual ~CheatsDialog();
		virtual void			AddedToManager(WidgetManager* theWidgetManager);
		virtual void			RemovedFromManager(WidgetManager* theWidgetManager);
		virtual void			Draw(Graphics* g);
		virtual void			Resize(int theX, int theY, int theWidth, int theHeight);
		//virtual int				GetPreferredHeight(int theWidth);
		virtual void			CheckboxChecked(int theId, bool checked);
	};
}

#endif
