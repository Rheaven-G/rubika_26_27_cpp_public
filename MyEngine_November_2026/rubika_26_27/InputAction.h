#pragma once

#include <functional>
#include <string>
#include <SFML/System/Vector2.hpp>

class InputAction
{
public:
	InputAction() = default;
	InputAction(const std::string& name);
	void CheckEvents();
	void SetActionName(const std::string& name);
	void SetValue(float value);
	float GetValue();
	template <typename T>
	T ReadValue()=delete;
	template <>
	float ReadValue<float>();
	template<>
	sf::Vector2f ReadValue<sf::Vector2f>();
	
	enum Moment
	{
		OnPress,
		OnHold,
		OnRelease
	};
	void RegisterCallback(const std::string& debugName, const Moment moment, 
		std::function<void(float)> callback, void* owner);
	void UnregisterCallbacks(void* owner);
	
private:
	struct sCallbackInfo
	{
		std::string DebugName;
		Moment Moment;
		std::function<void(float)> Callback;
		void* Owner = nullptr;
	};

	std::string ActionName;
	std::size_t ActionNameHash;
	float Value = 0.f;
	float PreviousValue = 0.f;
	std::vector<sCallbackInfo> Callbacks;
	
	void PressEvent();
	void HoldEvent();
	void ReleaseEvent();
	
public:
	const std::vector<sCallbackInfo>& GetCallbacks() const { return Callbacks; }
};
