#include <SexyAppFramework/Slider.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/DialogButton.h>

#include "CheatsDialog.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "Res.h"
#include "ProfileMgr.h"

using namespace Sexy;

Sexy::CheatsDialog::CheatsDialog(WinFishApp* theApp, bool theFlag)
	: MoneyDialog(theApp, IMAGE_DIALOG, IMAGE_DIALOGBUTTON, DIALOG_CHEATS, true, "Cheats",
		"", "OK", BUTTONS_FOOTER)
{
	mApp = theApp;
	SetColor(3, Color(0xff, 0xff, 100));
	mFlag = theFlag;

	for (int i = 0; i < SDL_arraysize(theApp->mBoard->mCheatCodes); i++)
	{
		Cheat aCheat = {};
		aCheat.mID = i;
		aCheat.mCode = theApp->mBoard->mCheatCodes[i]->mCheatString;
		aCheat.mCheckbox = MakeCheckbox(i, this, mApp->mCurrentProfile->GetCheatFlag(i));
		mCheats.push_back(aCheat);
	}
}

Sexy::CheatsDialog::~CheatsDialog()
{
	for (Cheat &aIter : mCheats)
		delete aIter.mCheckbox;
}

void Sexy::CheatsDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	MoneyDialog::AddedToManager(theWidgetManager);
	for (Cheat &aIter : mCheats)
		theWidgetManager->AddWidget(aIter.mCheckbox);
}

void Sexy::CheatsDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	MoneyDialog::RemovedFromManager(theWidgetManager);
	for (Cheat &aIter : mCheats)
		theWidgetManager->RemoveWidget(aIter.mCheckbox);
}

void Sexy::CheatsDialog::Draw(Graphics* g)
{
	MoneyDialog::Draw(g);
	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	g->SetColorizeImages(true);
	g->SetColor(mColors[4]);
	for (Cheat &aIter : mCheats)
	{
		g->DrawString(aIter.mCode.c_str(), 
					  aIter.mCheckbox->mX - mX + 43, 
					  aIter.mCheckbox->mY - mY + 24);
	}
	g->SetColorizeImages(false);
}

void Sexy::CheatsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	const int BUTTON_WIDTH = 45;
	const int BUTTON_HEIGHT = 46;
	int aIndex = 0;
	int aY = 66;
	MoneyDialog::Resize(theX, theY, theWidth, theHeight);
	for (Cheat &aIter : mCheats)
	{
		int aX = (aIndex % 2 == 0) ? 33 : 300;
		aIter.mCheckbox->Resize(mX + aX, mY + aY, BUTTON_WIDTH, BUTTON_HEIGHT);
		if (aIndex % 2 == 1)
		{
			aY += BUTTON_HEIGHT;
		}
		aIndex += 1;
	}
}

void Sexy::CheatsDialog::CheckboxChecked(int theId, bool checked)
{
	mApp->mBoard->DoCheatCode(theId);
	for (Cheat &aIter : mCheats)
	{
		aIter.mCheckbox->mChecked = mApp->mCurrentProfile->GetCheatFlag(aIter.mID);
	}
}
