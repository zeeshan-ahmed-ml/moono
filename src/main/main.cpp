/* ////////////////////////////////////////////////////////////

File Name: main.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include "yspng.h"
#include "Uno.h"
#include "fslazywindow.h"
#include "ysglfontdata.h"

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
private:
	Uno game;
	mutable int wid,hei;
	int mx,my,lb,mb,rb, evt;
	YsRawPngDecoder background;
	int numPlayers;
	bool beginGame;
	bool gameNotCreated;
	SimpleBitmap mooNOLogoPng;

	void DisplayMooNOLogo() const;
	void LoadMooNOLogo();

	void DisplayBackground() const;
	void LoadBackground();
	void DrawArrowButton(int x, int y, int dx, int dy, int dir) const;
	void DisplayMenu();
	void ChangePlayerCount(int mx, int my);
	// void CheckBeginClick();
	void CheckQuitButtonClick();
	void CheckNewGameButtonClick();
	void CheckGameLevel();
protected:
	bool needRedraw;

public:
	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &OPT) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool UserWantToCloseProgram(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
};

FsLazyWindowApplication::FsLazyWindowApplication()
	: game{}, numPlayers{2}, beginGame{false}, gameNotCreated{true}
{
	LoadBackground();
	LoadMooNOLogo();
	needRedraw=false;
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
}
/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	FsGetWindowSize(wid,hei);
	// game.gameLevel = 'h'; // temp placeholder before implementing graphic for level selection by user
	
	needRedraw=false;
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{	
	
	if(beginGame && gameNotCreated)
	{
		game.SetupGame(numPlayers);
		gameNotCreated = false;
	}

	auto key=FsInkey();
	evt=FsGetMouseEvent(lb,mb,rb,mx,my);
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	
	if(evt==FSMOUSEEVENT_LBUTTONDOWN)
	{
		if(!beginGame)
		{
			CheckQuitButtonClick();
			ChangePlayerCount(mx,my);
			// CheckBeginClick();
			CheckGameLevel();
		}
		game.SetClickFlagTrue();
		game.GetXYClickLoc(mx,my);

		
		if(!gameNotCreated && !game.HasGameEnded())
		{
			CheckQuitButtonClick();
			CheckNewGameButtonClick();
		}
	}
	if(!gameNotCreated && !game.HasGameEnded())
	{
		game.Play();
	}

	if(!gameNotCreated && game.HasGameEnded())
	{
		game.DrawGameState();
		if(evt==FSMOUSEEVENT_LBUTTONDOWN)
		{
			CheckQuitButtonClick();
			CheckNewGameButtonClick();
		}
		
		
	}

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	DisplayBackground();
	
	if (gameNotCreated)
	{
		DisplayMooNOLogo();
		DisplayMenu();
	}
	else
	{
		game.DrawGameState();
	
	}
		


	FsSwapBuffers();
	needRedraw=false;
	
}
/* virtual */ bool FsLazyWindowApplication::UserWantToCloseProgram(void)
{
	return true; // Returning true will just close the program.
}
/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	return FsLazyWindowApplicationBase::MustTerminate();
}
/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 10;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	return needRedraw;
}


static FsLazyWindowApplication *appPtr=nullptr;

/* static */ FsLazyWindowApplicationBase *FsLazyWindowApplicationBase::GetApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new FsLazyWindowApplication;
	}
	return appPtr;
}
void FsLazyWindowApplication::LoadBackground()
{
    if(YSERR == background.Decode("menu.png"))
    {
        std::cout << "Error: Failed to read menu background!" << std::endl;
        return;
    }
    background.Flip();
}


void FsLazyWindowApplication::DisplayBackground() const
{
	glRasterPos2i(0, hei-1);
	glDrawPixels(wid,hei,GL_RGBA, GL_UNSIGNED_BYTE, background.rgba);
}

void FsLazyWindowApplication::LoadMooNOLogo()
{
	if (!mooNOLogoPng.LoadPng("moonologo3.png"))
	{
		std::cout << "Error: Failed to read moono logo" << std::endl;
		std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
		return;
	}
	mooNOLogoPng.Invert();
}

void FsLazyWindowApplication::DisplayMooNOLogo() const
{
	auto bmpPtr = mooNOLogoPng.GetBitmapPointer();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glRasterPos2d(wid / 2 - 120, hei/2 - 200);
	glDrawPixels(280, 110, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bmpPtr);
	glDisable(GL_BLEND);
}





void FsLazyWindowApplication::DrawArrowButton(int x, int y, int dx, int dy, int dir) const
{
	glColor3i(0,0,0);
	glBegin(GL_QUADS);
	glVertex2i(x,y);
	glVertex2i(x+dx, y);
	glVertex2i(x+dx, y+dy);
	glVertex2i(x, y+dy);
	glEnd();

	glColor3f(255,0,0);
	glBegin(GL_TRIANGLES);
	if(dir == 1)
	{
		glVertex2i(x,y+dy);
		glVertex2i(x+dx,y+dy);
		glVertex2i(x+dx/2,y);
	}
	else
	{
		glVertex2i(x,y);
		glVertex2i(x+dx,y);
		glVertex2i(x+dx/2,y+dy);		
	}
	glEnd();

}

void FsLazyWindowApplication::DisplayMenu()
{
	// arrow button locations
	int x1 = 885;
	int y1 = 325;
	int dx = 50;
	int dy = 30;
	
	glColor3f(255,0,0);
	glRasterPos2i(350, 400);
	YsGlDrawFontBitmap20x32("Select number of opponents:");
	
	glRasterPos2i(900, 400);
	YsGlDrawFontBitmap20x32(std::to_string(numPlayers-1).c_str());

	DrawArrowButton(x1,y1,dx,dy,1);
	DrawArrowButton(x1,y1+85,dx,dy,0);

	glColor3f(255,100,100);
	glRasterPos2i(350,600);
	YsGlDrawFontBitmap20x32("Easy");

	glColor3f(255,100,100);
	glRasterPos2i(580,600);
	YsGlDrawFontBitmap20x32("Medium");	

	glColor3f(255,100,100);
	glRasterPos2i(860,600);
	YsGlDrawFontBitmap20x32("Hard");	



	game.DisplayQuitButton();
}

void FsLazyWindowApplication::ChangePlayerCount(int mx, int my)
{
	if(numPlayers < 10 && mx >= 885 && mx <=935 && my >= 325 && my <=355)
		numPlayers++;
	else if(numPlayers > 2 && mx >= 885 && mx <=935 && my >= 410 && my <=440)
		numPlayers--;
}

void FsLazyWindowApplication::CheckGameLevel()
{
	if(mx >= 350 && mx <=430 && my >= 568 && my <=600)
	{
		std::cout << "Easy button clicked!!!!" << std::endl;
		game.gameLevel = 'e';
		beginGame = true;		
	}

	if(mx >= 580 && mx <=700 && my >= 568 && my <=600)
	{
		std::cout << "Medium button clicked!!!!" << std::endl;
		game.gameLevel = 'm';
		beginGame = true;		
	}

	if(mx >= 860 && mx <=940 && my >= 568 && my <=600)
	{
		std::cout << "Hard button clicked!!!!" << std::endl;
		game.gameLevel = 'h';
		beginGame = true;		
	}
}

void FsLazyWindowApplication::CheckQuitButtonClick()
{
	if(mx > 1100 && mx < 1180 && my > 15 && my < 50)
	{
		SetMustTerminate(true);	
	}
}

void FsLazyWindowApplication::CheckNewGameButtonClick()
{
	if(mx > 1010 && mx < 1180 && my > 45 && my < 100)
	{
		beginGame = false;
		gameNotCreated = true;
	}
}
