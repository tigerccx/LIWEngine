#include "TestGame.h"

int TestGame::Initialise()
{
	TestRenderer* renderer = new TestRenderer(*(m_currentEnvironment->m_window));
	m_standardSystems.emplace_back(renderer);

	LIWEntity* entity = LIWEntityManager::instance.GetNewEntity();
	entity->AddComponent(LIWComponentManager<TestComponent0>::instance.GetNewComponent());

	LIWGame::instance = this;
	return 0;
}
