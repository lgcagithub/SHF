#include "Title.h"
#include "VideoPlayer.h"
#include "TitleTeam.h"
#include "../Script/Script.h"
#include "MainScene.h"

Title::Title()
{
	drawFullScreen = true;
}

Title::~Title()
{
	freeResource();
}

void Title::playTitleBGM()
{
	char * s;
	int len = PakFile::readFile("music\\ks64.mp3", &s);
	if (s != NULL && len > 0)
	{
		engine->loadBGM(s, len);
		engine->playBGM();
		delete s;
		s = NULL;
	}	
}

void Title::init()
{
	initFromIni("ini\\ui\\title\\Window.ini");
	initBtn = addButton("ini\\ui\\title\\InitBtn.ini");
	exitBtn = addButton("ini\\ui\\title\\ExitBtn.ini");
	loadBtn = addButton("ini\\ui\\title\\LoadBtn.ini");
	teamBtn = addButton("ini\\ui\\title\\TeamBtn.ini");
	initBtn->stretch = true;
	exitBtn->stretch = true;
	loadBtn->stretch = true;
	teamBtn->stretch = true;
	if (impImage != NULL)
	{
		IMP::clearIMPImage(impImage);
		delete impImage;
		impImage = NULL;
	}
	char * s;
	int len = PakFile::readFile("mpc\\ui\\title\\title.bmp", &s);
	if (len > 0)
	{ 
		impImage = new IMPImage;
		impImage->frameCount = 1;
		impImage->frame.resize(1);
		impImage->frame[0].data = s;
		impImage->frame[0].dataLen = len;
	}
}

void Title::freeResource()
{
	delete initBtn;
	initBtn = NULL;
	delete exitBtn;
	exitBtn = NULL;
	delete loadBtn;
	loadBtn = NULL;
	delete teamBtn;
	teamBtn = NULL;
	if (impImage != NULL)
	{
		IMP::clearIMPImage(impImage);
		delete impImage;
		impImage = NULL;
	}
	result = erNone;
	removeAllChild();
}

void Title::onEvent()
{
	if (exitBtn != NULL && exitBtn->getResult(erClick))
	{
		engine->stopBGM();
		running = false;
	}
	if (teamBtn != NULL && teamBtn->getResult(erClick))
	{
		engine->stopBGM();
		TitleTeam * tt = new TitleTeam;
		tt->run();
		delete tt;
		playTitleBGM();
	}
	if (initBtn != NULL && initBtn->getResult(erClick))
	{
		engine->stopBGM();
		//重新开始
		MainScene * ms = new MainScene(0);
		int ret = ms->run();
		delete ms;
		if (ret & erExit)
		{
			running = false;
		}
		else
		{
			playTitleBGM();
		}
		
	}
	if (loadBtn != NULL && loadBtn->getResult(erClick))
	{
		//读取进度
		SaveLoad * sl = new SaveLoad(false, true);
		addChild(sl);
		sl->priority = 0;
		unsigned int ret = sl->run();
		if ((ret & erLoad) != 0)
		{
			int index = sl->index;
			engine->stopBGM();

			MainScene * ms = new MainScene(sl->index + 1);
			ret = ms->run();
			delete ms;
			if (ret & erExit)
			{
				running = false;
			}
			else
			{
				playTitleBGM();
			}		
		}
		removeChild(sl);
		delete sl;
		
	}
}

bool Title::onInitial()
{
	init();
	return true;
}

void Title::onExit()
{
}

void Title::onRun()
{
	VideoPlayer * vp = new VideoPlayer("video\\logo.avi");
	vp->run();
	delete vp;
	vp = new VideoPlayer("video\\title.avi");
	vp->run();
	delete vp;
	playTitleBGM();
}

bool Title::onHandleEvent(AEvent * e)
{
	if (e == NULL)
	{
		return false;
	}
	if (e->eventType == KEYDOWN)
	{
		if (e->eventData == KEY_F)
		{	
			if (engine->getKeyPress(KEY_LCTRL) || engine->getKeyPress(KEY_RCTRL))
			{ 
				engine->setWindowFullScreen(!engine->getWindowFullScreen());
				return true;
			}
		}
		if (e->eventData == KEY_M)
		{
			if (engine->getKeyPress(KEY_LCTRL) || engine->getKeyPress(KEY_RCTRL))
			{
				engine->setMouseHardware(!engine->getMouseHardware());
				return true;
			}
		}
	}
	else if (e->eventType == QUIT)
	{
		running = false;
		return true;
	}
	return false;
}
