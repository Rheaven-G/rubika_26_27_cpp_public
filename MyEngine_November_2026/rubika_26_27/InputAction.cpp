#include "InputAction.h"

// ----- PUBLIC
InputAction::InputAction(const std::string& name) : ActionName(name) {}

void InputAction::CheckEvents()
{
    if (Value != 0.f)
    {
        if (PreviousValue != 0.f)
        {
            HoldEvent();
        } else
        {
            PressEvent();
        }
    }
    
    if (Value == 0.f && PreviousValue != 0.f)
        ReleaseEvent();
    
    PreviousValue = Value;
}

void InputAction::SetActionName(const std::string& name) { ActionName = name; }

// Value
void InputAction::SetValue(float value)
{
    Value = value;
}
float InputAction::GetValue()
{
    return Value;
}
template <>
float InputAction::ReadValue<float>()
{
    return Value;
}
template <>
sf::Vector2f InputAction::ReadValue<sf::Vector2f>()
{
    return {Value, Value};
}

// Callbacks
void InputAction::RegisterCallback( const std::string& debugName, const Moment moment, 
    std::function<void(float)> callback, void* owner)
{
    Callbacks.push_back({ debugName, moment, std::move(callback), owner });
}
void InputAction::UnregisterCallbacks(void* owner)
{
    for (size_t i{0}; i < Callbacks.size(); i++)
    {
        if (Callbacks[i].Owner == owner)
        {
            Callbacks.erase(Callbacks.begin() + (int)i);
        }
    }
}

// ---- PRIVATE
// Event launch
void InputAction::PressEvent()
{
    for (auto& callback : Callbacks)
    {
        if (callback.Moment == Moment::OnPress)
            callback.Callback(Value);
        
    }
}
void InputAction::HoldEvent()
{
    for (auto& callback : Callbacks)
    {
        if (callback.Moment == Moment::OnHold)
            callback.Callback(Value);
        
    }
}
void InputAction::ReleaseEvent()
{
    for (auto& callback : Callbacks)
    {
        if (callback.Moment == Moment::OnRelease)
            callback.Callback(Value);
        
    }
}