#include "LIWComponent.h"
#include "LIWGame.h"

void LIW::LIWComponent::Register()
{
	LIWGame::instance.m_componentManager.RegisterComponent(this);
}

void LIW::LIWComponent::Unregister()
{
	LIWGame::instance.m_componentManager.UnregisterComponent(this);
}
