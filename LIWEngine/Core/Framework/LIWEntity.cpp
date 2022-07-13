#include "LIWEntity.h"

void LIW::LIWEntity::AddComponent(LIWComponent* component, bool toActivate)
{
	m_components.emplace_back(component);
	component->SetEntity(this);
}

void LIW::LIWEntity::RemoveComponent(LIWComponent* component)
{
	auto itr = std::find(m_components.begin(), m_components.end(), component);
	if (itr != m_components.end()) {
		(*itr)->SetEntity(nullptr);
		m_components.erase(itr);
	}
}
