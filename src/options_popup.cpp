//#include "options_popup.hpp"
//#include "world_init.hpp"
//
//
//OptionsPopup::OptionsPopup()
//{
//}
//
//OptionsPopup::~OptionsPopup()
//{
//}
//
//void OptionsPopup::displayOptions(int screen)
//{
//	Screen s_enum = (Screen)screen;
//	createText("Options Menu", vec2(gameInfo.width/2.f, gameInfo.height/3.6), 1.0f, Colour::theme_blue_3, s_enum);
//	vec2 center = { gameInfo.width / 2.f, gameInfo.height / 2.f };
//	Entity back = createBox(center, {gameInfo.width/4.5f, gameInfo.height / 2.f});
//	Entity front = createBox(center, { gameInfo.width / 4.5f - 20.f, gameInfo.height / 2.f - 20.f });
//	//Entity testButton = createButton("HELP", center, 1.5f, { gameInfo.width / 4.5f, gameInfo.height / 2.f }, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), s_enum);
//	
//	registry.colours.insert(back, Colour::theme_blue_2);
//	registry.colours.insert(front, Colour::white);
//	
//	registry.screens.insert(back, s_enum);
//	registry.screens.insert(front, s_enum);
//	//registry.screens.insert(testButton, s_enum);
//}
