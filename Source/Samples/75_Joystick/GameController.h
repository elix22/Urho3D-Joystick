//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Urho3D/Core/Object.h>

using namespace Urho3D;

namespace Urho3D
{
class Controls;
}

//=============================================================================
//=============================================================================
const StringHash VAR_AXIS_0("VAR_AXIS_0");
const StringHash VAR_AXIS_1("VAR_AXIS_1");

const int BUTTON_A = (1<<0);
const int BUTTON_B = (1<<1);
const int BUTTON_X = (1<<2);
const int BUTTON_Y = (1<<3);

//=============================================================================
//=============================================================================
const float CAMERA_MIN_DIST = 1.0f;
const float CAMERA_INITIAL_DIST = 5.0f;
const float CAMERA_MAX_DIST = 20.0f;

//=============================================================================
//=============================================================================
class GameController : public Object
{
    URHO3D_OBJECT(GameController, Object);

public:
    GameController(Context* context);
    ~GameController();

    void UpdateControlInputs(Controls& controls);

    bool IsValid() const                { return joystickIndex_ != M_MAX_UNSIGNED; }
    void SetMinTolerance(float minVal)  { minTolerance_ = minVal; }
    float GetMinTolerance() const       { return minTolerance_; }

protected:
    bool CreateController();
    void RemoveScreenJoystick();
    void ClampValues(Vector2 &vec, float minVal);

protected:
    unsigned joystickIndex_;
    float minTolerance_;
};

