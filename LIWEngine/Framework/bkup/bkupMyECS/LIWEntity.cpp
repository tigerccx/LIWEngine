#include "LIWEntity.h"

void LIW::LIWEntity::AddComponent(LIWComponent* component, bool toActivate)
{
	m_components.emplace_back(component);
	component->SetEntity(this);
	if (toActivate) {
		component->Activate();
	}
}

void LIW::LIWEntity::RemoveComponent(LIWComponent* component)
{
	auto itr = std::find(m_components.begin(), m_components.end(), component);
	if (itr != m_components.end()) {
		(*itr)->SetEntity(nullptr);
		m_components.erase(itr);
	}
}

void LIW::LIWEntity::AddModel(LIWModel* model)
{
	m_models.emplace_back(model);
	model->SetEntity(this);
}

void LIW::LIWEntity::RemoveModel(LIWModel* model)
{
	auto itr = std::find(m_models.begin(), m_models.end(), model);
	if (itr != m_models.end()) {
		(*itr)->SetEntity(nullptr);
		m_models.erase(itr);
	}
}
