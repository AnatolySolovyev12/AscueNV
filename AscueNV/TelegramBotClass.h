#pragma once

#include <stdio.h>
#include <tgbot/tgbot.h>

class TelegramBotClass 
{


public:
	TelegramBotClass();

	~TelegramBotClass();

	TgBot::Bot* bot;

	TgBot::Message::Ptr messageTest;
	
private:
	
	

};


