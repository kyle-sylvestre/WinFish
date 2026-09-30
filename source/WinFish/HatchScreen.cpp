#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>

#include "HatchScreen.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "ProfileMgr.h"
#include "BubbleMgr.h"
#include "Res.h"

Sexy::HatchScreen::HatchScreen(WinFishApp* theApp, int thePetId)
{
	mApp = theApp;
	mBubbleMgr = new BubbleMgr();

	mBubbleMgr->mBubbleBounds = Rect(190, 220, 258, 200);
	mBubbleMgr->SetBubbleConfig(10, 5);
	mBubbleMgr->UpdateALot();
	m0x94 = 0;
	mPetId = thePetId;
	theApp->m0x884 = true;
	mX = 0;
	mY = 0;
	mWidth = mApp->mWidth;
	mHeight = mApp->mHeight;

	mContinueButton = MakeDialogButton2(99, this, lz("Please Wait..."), IMAGE_MAINBUTTON);
	mContinueButton->SetFont(FONT_JUNGLEFEVER12OUTLINE);
	mContinueButton->mMouseVisible = false;
	mContinueButton->mColors[0] = Color(255, 240, 0, 255);
	mContinueButton->Resize(186, 445, 264, mContinueButton->mHeight);
	mApp->mWidgetManager->BringToFront(mContinueButton);

	mMenuButton = MakeDialogButton2(100, this, lz("Menu"), IMAGE_MAINBUTTON);
	mMenuButton->Resize(525, 4, 80, mMenuButton->mHeight);
	if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 2)
		mMenuButton->mVisible = false;

	mOverlay = new HatchScreenOverlay();
	mOverlay->mScreen = this;
	mOverlay->mMouseVisible = false;
	mOverlay->mHasAlpha = true;
	mOverlay->mWidth = 640;
	mOverlay->mHeight = 480;

	m0x9c = 0;
	m0xa0 = 0;
	m0xa4 = 480;
	mApp->StopMusic();
	mApp->PlayMusic(2, 54);
}

Sexy::HatchScreen::~HatchScreen()
{
	if (mBubbleMgr)
		delete mBubbleMgr;
	if (mContinueButton)
		delete mContinueButton;
	if (mMenuButton)
		delete mMenuButton;
	if (mOverlay)
		delete mOverlay;
}


void Sexy::HatchScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mContinueButton);
	theWidgetManager->AddWidget(mMenuButton);
	theWidgetManager->AddWidget(mOverlay);
}

void Sexy::HatchScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mContinueButton);
	theWidgetManager->RemoveWidget(mMenuButton);
	theWidgetManager->RemoveWidget(mOverlay);
}

void Sexy::HatchScreen::Update()
{
	mBubbleMgr->Update();
	MarkDirty();
	m0x9c = mApp->mSeed->Next() % 4;
	m0xa0 = mApp->mSeed->Next() % 2;

	if (!mIsDown || m0x94 > 139)
	{
		if (m0x94 == 141)
		{
			if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
				mApp->PlaySample(SOUND_EVILLAFF);
			else
				mApp->PlaySample(SOUND_HATCH);
		}
	}
	else
	{
		m0x94 = 140;
	}

	if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
	{
		if (m0x94 == 28 || m0x94 == 48 || m0x94 == 68)
			mApp->PlaySample(SOUND_BUY);
		if(m0x94 == 88)
			mApp->PlaySample(SOUND_GROW);
	}

	if (m0x94 >= 220 && mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
	{
		int aVal = m0x94 % 16;
		if (aVal <= 3)
			m0xa4--;
		else if (aVal > 7 && aVal < 12)
			m0xa4++;
	}
	else if(m0x94 >= 140 && mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
	{
		if (m0x94 < 161)
			m0xa4 -= 12;
		else if (m0x94 < 171)
			m0xa4 -= 8;
		else if (m0x94 < 181)
			m0xa4 -= 5;
		else if (m0x94 < 191)
			m0xa4 -= 2;
		else
			m0xa4--;
	}

	if (m0x94 == 170)
	{
		mContinueButton->mLabel = lz("Click Here to Continue");
		mContinueButton->mMouseVisible = true;
	}

	m0x94++;
}

void Sexy::HatchScreen::Draw(Graphics* g)
{
	g->DrawImage(IMAGE_HATCHSCREEN, 0, 0);
	g->DrawImageBox(Rect(20, 0, 600, IMAGE_SCREENTITLE->mHeight), IMAGE_SCREENTITLE);

	if (mMenuButton->mVisible)
		g->DrawImageBox(Rect(mMenuButton->mX - 1, mMenuButton->mY - 1, mMenuButton->mWidth + 2, IMAGE_SCREENTITLEHOLE->mHeight), IMAGE_SCREENTITLEHOLE);
	
	g->SetFont(FONT_JUNGLEFEVER17OUTLINE);
	g->SetColor(Color(0xff, 200, 0, 0xff));

	g->DrawString(lz("You have found:"), 215, 25);

	int aShakeTimer = m0x94 % 32;
	int anYOffset = 0;
	if (aShakeTimer < 4)
		anYOffset = 0;
	else if (aShakeTimer < 8)
		anYOffset = -1;
	else if (aShakeTimer < 12)
		anYOffset = -2;
	else if (aShakeTimer < 16)
		anYOffset = -1;
	else if (aShakeTimer < 20)
		anYOffset = 0;
	else if (aShakeTimer < 24)
		anYOffset = 1;
	else if (aShakeTimer < 28)
		anYOffset = 2;
	else if (aShakeTimer < 32)
		anYOffset = 1;

	Graphics g2(*g);
	if (m0x94 < 81)
	{
		int aCel = 0;
		if (m0x94 > 55)
			aCel = (m0x94 - 56) / 2;
		g->DrawImageCel(IMAGE_EGGCRACK1, 258, anYOffset + 60, aCel);
	}
	else // 136 - 364
	{
		if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
		{
			int aCel = 0;
			if (m0x94 >= 140)
				aCel = 9;
			else if (m0x94 > 119)
				aCel = (m0x94 - 120) / 2;

			if (m0x94 > 160)
			{
				m0x9c = 0;
				m0xa0 = 0;
			}
			g->DrawImageCel(IMAGE_EGGCRACK2, m0x9c + 258, m0xa0 + 60 + anYOffset, aCel);
		}
		else if (m0x94 < 141)
		{
			int aCel = 0;
			if (m0x94 >	 119)
				aCel = (m0x94 - 120) / 2;
			g->DrawImageCel(IMAGE_EGGCRACK2, m0x9c + 258, m0xa0 + 60 + anYOffset, aCel);
		}
		else // 166
		{
			if (mPetId == PET_AMP)
			{
				g2.ClipRect(236, 100, 160, 60);
			}
			else
			{
				int anYClip = 0;
				if (mPetId == PET_CLYDE || mPetId == PET_STINKY)
					anYClip = 100;
				else if (mPetId == PET_NIKO)
					anYClip = 70;
				else
					anYClip = 90;
				g2.ClipRect(278, anYClip, 80, 80);
			}
			int aXOffset = (m0x94 % 20) / 2;
			if (mPetId == PET_SEYMOUR || mPetId == PET_SHRAPNEL ||
				mPetId == PET_CLYDE || mPetId == PET_RHUBARB || mPetId == PET_BRINKLEY)
				aXOffset = (m0x94 % 40) / 4;
			else if (mPetId == PET_VERT || mPetId == PET_NIKO)
			{
				aXOffset = m0x94 % 20;
				if (aXOffset > 9)
					aXOffset = 19 - aXOffset;
			}
			Image* anImage = IMAGE_STINKY;
			int aX = 278;
			int aY = 90;
			if (mPetId != PET_AMP)
				aXOffset *= 80;
			else
				aXOffset *= 160;

			switch (mPetId)
			{
			case PET_STINKY:
				aY = 100;
				anImage = IMAGE_STINKY;
				break;
			case PET_NIKO:
				aY = 70;
				anImage = IMAGE_NIKO;
				break;
			case PET_ITCHY:
				anImage = IMAGE_ITCHY;
				break;
			case PET_PREGO:
				anImage = IMAGE_PREGO;
				break;
			case PET_ZORF:
				anImage = IMAGE_ZORF;
				break;
			case PET_CLYDE:
				aY = 100;
				anImage = IMAGE_CLYDE;
				break;
			case PET_VERT:
				anImage = IMAGE_VERT;
				break;
			case PET_RUFUS:
				anImage = IMAGE_RUFUS;
				break;
			case PET_MERYL:
				anImage = IMAGE_MERYL;
				break;
			case PET_WADSWORTH:
				anImage = IMAGE_WADSWORTH;
				break;
			case PET_SEYMOUR:
				anImage = IMAGE_SEYMOUR;
				break;
			case PET_SHRAPNEL:
				anImage = IMAGE_SHRAPNEL;
				break;
			case PET_GUMBO:
				anImage = IMAGE_GUMBO;
				break;
			case PET_BLIP:
				anImage = IMAGE_BLIP;
				break;
			case PET_RHUBARB:
				anImage = IMAGE_RHUBARB;
				break;
			case PET_NIMBUS:
				anImage = IMAGE_NIMBUS;
				break;
			case PET_AMP:
				aY = 100;
				aX = 236;
				anImage = IMAGE_AMP;
				break;
			case PET_GASH:
				anImage = IMAGE_GASH;
				break;
			case PET_ANGIE:
				anImage = IMAGE_ANGIE;
				break;
			case PET_PRESTO:
				anImage = IMAGE_PRESTO;
				break;
			case PET_BRINKLEY:
				anImage = IMAGE_BRINKLEY;
				break;
			case PET_NOSTRADAMUS:
				anImage = IMAGE_NOSTRADAMUS;
				break;
			case PET_STANLEY:
				anImage = IMAGE_STANLEY;
				break;
			case PET_WALTER:
				anImage = IMAGE_WALTER;
				break;
			}

			g2.DrawImage(anImage, aX - aXOffset, aY);
			g2.SetColorizeImages(false);

			SexyString aPetDesc = "";
			switch (mPetId)
			{
			case PET_STINKY:
				aPetDesc = lz("STINKY the Snail");
				break;
			case PET_NIKO:
				aPetDesc = lz("NIKO the Oyster");
				break;
			case PET_ITCHY:
				aPetDesc = lz("ITCHY the Swordfish");
				break;
			case PET_PREGO:
				aPetDesc = lz("PREGO the Momma Fish");
				break;
			case PET_ZORF:
				aPetDesc = lz("ZORF the Sea Horse");
				break;
			case PET_CLYDE:
				aPetDesc = lz("CLYDE the Jellyfish");
				break;
			case PET_VERT:
				aPetDesc = lz("VERT the Skeleton");
				break;
			case PET_RUFUS:
				aPetDesc = lz("RUFUS the Fiddler Crab");
				break;
			case PET_MERYL:
				aPetDesc = lz("MERYL the Mermaid");
				break;
			case PET_WADSWORTH:
				aPetDesc = lz("WADSWORTH the Whale");
				break;
			case PET_SEYMOUR:
				aPetDesc = lz("SEYMOUR the Turtle");
				break;
			case PET_SHRAPNEL:
				aPetDesc = lz("SHRAPNEL the Robot Fish");
				break;
			case PET_GUMBO:
				aPetDesc = lz("GUMBO the Angler");
				break;
			case PET_BLIP:
				aPetDesc = lz("BLIP the Porpoise");
				break;
			case PET_RHUBARB:
				aPetDesc = lz("RHUBARB the Hermit Crab");
				break;
			case PET_NIMBUS:
				aPetDesc = lz("NIMBUS the Manta Ray");
				break;
			case PET_AMP:
				aPetDesc = lz("AMP the Electric Eel");
				break;
			case PET_GASH:
				aPetDesc = lz("GASH the Shark");
				break;
			case PET_ANGIE:
				aPetDesc = lz("ANGIE the Angelfish");
				break;
			case PET_PRESTO:
				aPetDesc = lz("PRESTO the Tadpole");
				break;
			case PET_BRINKLEY: // 20 (0x14)
				aPetDesc = lz("BRINKLEY");
				break;
			case PET_NOSTRADAMUS:
				aPetDesc = lz("NOSTRADAMUS the Nose");
				break;
			case PET_STANLEY:
				aPetDesc = lz("STANLEY the Startlingly");
				break;
			case PET_WALTER:
				aPetDesc = lz("WALTER the Penguin");
				break;
			case 999:
				aPetDesc = lz("Evil Alien Mastermind");
				break;
			default:
				break;
			}
			g->SetColor(Color(255, 200, 0));
			g->SetFont(FONT_JUNGLEFEVER15OUTLINE);
			int aYDesc = 260;
			if (mPetId == PET_RHUBARB || mPetId == PET_WADSWORTH ||
				mPetId == PET_SHRAPNEL || mPetId == PET_NOSTRADAMUS)
				g->SetFont(FONT_JUNGLEFEVER12OUTLINE);
			else if (mPetId == PET_BRINKLEY || mPetId == PET_STANLEY)
			{
				g->SetFont(FONT_JUNGLEFEVER12OUTLINE);
				SexyString aMoreDesc = "";
				if (mPetId == PET_BRINKLEY)
					aMoreDesc = lz("the Scuba Diving Elephant");
				else if (mPetId == PET_STANLEY)
					aMoreDesc = lz("Small Sea Serpent");

				int aStrWdth = g->GetFont()->StringWidth(aMoreDesc);
				g->DrawString(aMoreDesc, (650 - aStrWdth) / 2 - 3, 265);
				aYDesc = 245;
			}

			int aStrWdth = g->GetFont()->StringWidth(aPetDesc);
			g->DrawString(aPetDesc, (650 - aStrWdth) / 2 - 3, aYDesc);
		}
	} // 364

	if (m0x94 > 170)
	{
		g->SetColor(Color::White);
		g->SetFont(FONT_JUNGLEFEVER10OUTLINE);

		SexyString aStr1 = "";
		SexyString aStr2 = "";
		SexyString aStr3 = "";
		SexyString aStr4 = "";

		switch (mPetId)
		{
		case PET_STINKY:
			aStr1 = lz("STINKY roams around the");
			aStr2 = lz("bottom of your tank, catching");
			aStr3 = lz("any coins you may have missed.");
			break;
		case PET_NIKO:
			aStr1 = lz("NIKO produces pearls that");
			aStr2 = lz("you can click on for a");
			aStr3 = lz("hefty sum of money.");
			break;
		case PET_ITCHY:
			aStr1 = lz("ITCHY helps you by attacking");
			aStr2 = lz("aliens when they appear.");
			break;
		case PET_PREGO:
			aStr1 = lz("PREGO helps populate your");
			aStr2 = lz("tank by giving birth to a new");
			aStr3 = lz("baby guppy every so often.");
			break;
		case PET_ZORF:
			aStr1 = lz("ZORF gives you a hand in");
			aStr2 = lz("keeping your fish fed.");
			break;
		case PET_CLYDE:
			aStr1 = lz("CLYDE drifts slowly through");
			aStr2 = lz("your tank, collecting any");
			aStr3 = lz("coins it passes by.");
			break;
		case PET_VERT:
			aStr1 = lz("VERT drops gold coins just like");
			aStr2 = lz("a large guppy, but doesn\'t need");
			aStr3 = lz("fish food to survive.");
			break;
		case PET_RUFUS:
			aStr1 = lz("RUFUS does heavy damage to");
			aStr2 = lz("enemies you\'ve lured to the");
			aStr3 = lz("bottom of the tank.");
			break;
		case PET_MERYL:
			aStr1 = lz("MERYL\'s song cheers up all the");
			aStr2 = lz("guppies in the tank, making");
			aStr3 = lz("them drop coins faster.");
			break;
		case PET_WADSWORTH:
			aStr1 = lz("WADSWORTH helps by sheltering");
			aStr2 = lz("your baby and medium guppies");
			aStr3 = lz("from hungry aliens.");
			break;
		case PET_SEYMOUR:
			aStr1 = lz("SEYMOUR\'s presence makes all");
			aStr2 = lz("coins and diamonds drift");
			aStr3 = lz("at a slower rate.");
			break;
		case PET_SHRAPNEL:
			aStr1 = lz("SHRAPNEL drops bombs that");
			aStr2 = lz("blow up fish on contact but");
			aStr3 = lz("give lots of cash when clicked.");
			break;
		case PET_GUMBO:
			aStr1 = lz("GUMBO attracts guppies using");
			aStr2 = lz("the lantern on his head,");
			aStr3 = lz("luring them away from aliens.");
			break;
		case PET_BLIP:
			aStr1 = lz("BLIP provides you with info");
			aStr2 = lz("that helps you better combat");
			aStr3 = lz("aliens and keep your fish fed.");
			break;
		case PET_RHUBARB:
			aStr1 = lz("RHUBARB snaps his claws at");
			aStr2 = lz("fish, keeping them off the");
			aStr3 = lz("bottom of your tank.");
			break;
		case PET_NIMBUS:
			aStr1 = lz("NIMBUS tosses any coins or");
			aStr2 = lz("food he catches back up");
			aStr3 = lz("toward the top of the tank.");
			break;
		case PET_AMP:
			aStr1 = lz("AMP can electrocute your");
			aStr2 = lz("entire tank, killing your fish");
			aStr3 = lz("and turning them into diamonds.");
			break;
		case PET_GASH:
			aStr1 = lz("GASH viciously attacks aliens,");
			aStr2 = lz("but will snack on one of");
			aStr3 = lz("your guppies from time to time.");
			break;
		case PET_ANGIE:
			aStr1 = lz("ANGIE has the ability to");
			aStr2 = lz("resurrect dead fish.");
			break;
		case PET_PRESTO:
			aStr1 = lz("PRESTO has the ability to");
			aStr2 = lz("metamorph into any of");
			aStr3 = lz("your other pets.");
			break;
		case PET_BRINKLEY:
			aStr1 = lz("Likes: peach muffins, all");
			aStr2 = lz("things brown and sticky");
			aStr3 = lz("Dislikes: arugula");
			break;
		case PET_NOSTRADAMUS:
			aStr1 = lz("Little known fact:  NOSTRADAMUS");
			aStr2 = lz("is the long lost nose of ex-president");
			aStr3 = lz("Rutherford B. Hayes");
			break;
		case PET_STANLEY:
			aStr1 = lz("STANLEY knows no fear.. except");
			aStr2 = lz("that of badgers, aprons,");
			aStr3 = lz("and badgers wearing aprons.");
			break;
		case PET_WALTER:
			aStr1 = lz("Choosing this pet donates all");
			aStr2 = lz("proceeds to the Falafel");
			aStr3 = lz("Foundation. Free the falafels!");
			break;
		case 999:
			aStr1 = lz("EVIL ALIEN MASTERMIND actually");
			aStr2 = lz("isn\'t very helpful at all.  Unless");
			aStr3 = lz("you consider devouring the entire");
			aStr4 = lz("contents of your fishtank helpful.");
			g->SetColor(Color::White);
			g->DrawString(aStr4, (650 - g->GetFont()->StringWidth(aStr4)) / 2 - 3, 360);
			break;
		default:
			break;
		}

		g->DrawString(aStr1, (650 - g->GetFont()->StringWidth(aStr1)) / 2 - 3, 300);
		g->DrawString(aStr2, (650 - g->GetFont()->StringWidth(aStr2)) / 2 - 3, 320);
		g->DrawString(aStr3, (650 - g->GetFont()->StringWidth(aStr3)) / 2 - 3, 340);

		g->SetColor(Color(255, 255, 100));

		if (mPetId == PET_PRESTO)
		{
			WriteWordWrapped(g, Rect(180, 373, 284, 0), lz("You have been awarded 5000 shells\nfor defeating the Final Boss!"), -1, 0);
		}
		else
		{
			g->DrawString(lz("Your game has been saved."), 221, 390);
		}

		if (mPetId == 999)
		{
			g->SetColor(Color(255, 200, 0));
			SexyString aStr = lz("Evil Alien Mastermind");
			g->SetFont(FONT_JUNGLEFEVER15OUTLINE);
			int aStrWdth = g->GetFont()->StringWidth(aStr);
			g->DrawString(aStr, (650 - aStrWdth) / 2 - 3, 260);
		}
	}
}

void Sexy::HatchScreen::DrawOverlay(Graphics* g)
{
	g->DrawImage(IMAGE_HATCHREFLECTION, 240, 60);
	if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
	{
		if (m0x94 > 139)
		{
			Graphics g2(*g);
			g2.ClipRect(238, m0xa4, 160, 160);
			int aFrame = (m0x94 % 20) / 2;
			g2.DrawImage(IMAGE_BOSS, 238 - (aFrame * 160), m0xa4);
		}
	}
	else if(m0x94 > 139 && m0x94 < 200)
	{
		int aShardValues[16] = { -10, -15, 3, -13, 10, 5, 14, 12, 1, -11, -14, 12, -15, 11, 13, -1 };

		for (int i = 0; i < 8; i++)
		{
			int anOffset = m0x94 - 140;
			g->DrawImageCel(IMAGE_EGGSHARDS, aShardValues[i + 8] * anOffset + 268, aShardValues[i] * anOffset + 70, i);
		}
	}
}

void Sexy::HatchScreen::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::HatchScreen::ButtonDepress(int theId)
{
	if (theId == 99)
	{
		mApp->RemoveHatchScreen();
		if (mApp->mGameMode == GAMEMODE_TIME_TRIAL || mApp->mGameMode == GAMEMODE_CHALLENGE)
			mApp->LeaveGameBoard();
		else if (mApp->mCurrentProfile->mTank != 5 || mApp->mCurrentProfile->mLevel != 2)
			mApp->SwitchToBoard(false, false);
		else
			mApp->SwitchToInterludeScreen();
	}
	else if (theId == 100)
	{
		mApp->RemoveHatchScreen();
		if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 2)
			mApp->SwitchToBoard(false, false);
		else
			mApp->SwitchToGameSelector();
	}
}

void Sexy::HatchScreenOverlay::Draw(Graphics* g)
{
	mScreen->DrawOverlay(g);
}
