#include <CherrySoda/Components/Logic/Alarm.h>

#include <CherrySoda/Engine.h>
#include <CherrySoda/Entity.h>
#include <CherrySoda/Util/Log.h>

using cherrysoda::Alarm;
using cherrysoda::AlarmMode;
using cherrysoda::Engine;
using cherrysoda::Entity;
using cherrysoda::STL;

Alarm* Alarm::Create(AlarmMode mode, STL::Action<> onComplete, float duration/* = 1.f*/, bool start/* = false*/)
{
	Alarm* alarm = nullptr;
	if (STL::Count(m_cached) == 0) {
		alarm = new Alarm();
	}
	else {
		alarm = STL::Pop(m_cached);
	}
	alarm->Init(mode, onComplete, duration, start);
	return alarm;
}

Alarm* Alarm::Set(Entity* entity, float duration, STL::Action<> onComplete, AlarmMode alarmMode/* = Alarm::AlarmMode::Oneshot*/)
{
	Alarm* alarm = Alarm::Create(alarmMode, onComplete, duration, true);
	entity->Add(alarm);
	return alarm;
}

void Alarm::Init(AlarmMode mode, STL::Action<> onComplete, float duration/* = 1.f*/, bool start/* = false*/)
{
#ifndef NDEBUG
	if (duration <= 0) {
		CHERRYSODA_ASSERT(false, "Alarm duration cannot be <= 0\n");
	}
#endif
	m_mode = mode;
	m_duration = duration;
	m_onComplete = onComplete;	

	Active(false);
	m_timeLeft = 0.f;

	if (start) {
		Start();
	}
}

void Alarm::Update()
{
	m_timeLeft -= Engine::Instance()->DeltaTime(); 
	if (m_timeLeft <= 0) {
		m_timeLeft = 0.f;
		if (m_onComplete != nullptr) {
			m_onComplete();
		}	
		if (m_mode == AlarmMode::Looping) {
			Start();
		}
		else if (m_mode == AlarmMode::Oneshot) {
			RemoveSelf();
		}
		else if (m_timeLeft <= 0) {
			Active(false);
		}
	}
}

void Alarm::Removed(Entity* entity)
{
	base::Removed(entity);
	STL::Push(m_cached, this);
}

void Alarm::Start()
{
	Active(true);
	m_timeLeft = m_duration;	
}

void Alarm::Start(float duration)
{
#ifndef NDEBUG
	if (duration <= 0) {
		CHERRYSODA_ASSERT(false, "Alarm duration cannot be <= 0\n");
	}
#endif

	m_duration = duration;
	Start();
}

void Alarm::Stop()
{
	Active(false);
}
