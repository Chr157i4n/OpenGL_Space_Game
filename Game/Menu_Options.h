#pragma once

#include "Menu.h"

class Menu_Options : public Menu
{
public:

	Menu_Options();

	void toggleVsync();

	void setVolume();

	void toggleShadows();

	void changeShadowMapResolution();

	void changeEnvMapResolution();

private:
	UI_Element_Button *pB_vsync, *pB_shadow, *pB_back;
	UI_Element_Slider *sL_volume, *sL_shadowres, * sL_envres;
	UI_Element_Dropdown *dD_Resolution;
};

