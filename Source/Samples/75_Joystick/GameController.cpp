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

#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/Log.h>

#include "GameController.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
const unsigned maxButtonQueries = 32;
const unsigned maxAxisCnt = 4;

//=============================================================================
//=============================================================================
GameController::GameController(Context* context)
    : Object(context)
    , joystickIndex_(M_MAX_UNSIGNED)
    , minTolerance_(0.2f)

{
    stickNames_.Push("lftStick");
    stickNames_.Push("rgtStick");

    CreateController();
}

GameController::~GameController()
{
}

void GameController::RemoveScreenJoystick()
{
    Input* input = GetSubsystem<Input>();

    for ( unsigned i = 0; i < input->GetNumJoysticks(); ++i )
    {
        JoystickState *joystick = input->GetJoystickByIndex(i);

        if (joystick->screenJoystick_)
        {
            input->RemoveScreenJoystick(joystick->joystickID_);
            break;
        }
    }
}

bool GameController::CreateController()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Input* input = GetSubsystem<Input>();

    // create or detect joystick
    if (GetPlatform() == "Android" || GetPlatform() == "iOS")
    {
        // remove the default screen joystick created in the Sample::InitTouchInput() fn.
        //**note** this would not be required if you don't inherit your app from the Sample class
        RemoveScreenJoystick();

        // and create our own
        XMLFile *layout = cache->GetResource<XMLFile>("ScreenJoystick/ScreenJoystick.xml");
        joystickIndex_ = (unsigned)input->AddScreenJoystick(layout, cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    }
    else
    {
        // get the 1st controller joystick detected
        for ( unsigned i = 0; i < input->GetNumJoysticks(); ++i )
        {
            JoystickState *joystick = input->GetJoystickByIndex(i);

            if (joystick->IsController())
            {
                joystickIndex_ = joystick->joystickID_;
                break;
            }
        }
    }

    //#define DUMP_JOYSTICK_INFO
    #ifdef DUMP_JOYSTICK_INFO
    if (joystickIndex_ != M_MAX_UNSIGNED)
    {
        JoystickState *joystick = input->GetJoystick(joystickIndex_);
        URHO3D_LOGINFO(ToString("\n----------------\njoystick name='%s', btns=%u, axes=%u, hats=%u, isctrl=%u\n----------------",
                                !joystick->name_.Empty()?joystick->name_.CString():"null",
                                joystick->GetNumButtons(),
                                joystick->GetNumAxes(),
                                joystick->GetNumHats(),
                                joystick->IsController()?1:0
                                ));
    }
    #endif


    return (joystickIndex_ != M_MAX_UNSIGNED);
}

void GameController::UpdateControlInputs(Controls& controls)
{
    // clear button
    controls.buttons_ = 0;

    if (joystickIndex_ != M_MAX_UNSIGNED)
    {
        JoystickState *joystick = GetSubsystem<Input>()->GetJoystick(joystickIndex_);

        if (joystick)
        {
            // buttons
            for ( unsigned i = 0; i < joystick->GetNumButtons() && i < maxButtonQueries; ++i )
            {
                controls.Set((1<<i), joystick->GetButtonDown(i));
            }

            // axis
            for ( unsigned i = 0; i < maxAxisCnt; i += 2 )
            {
                Vector2 val(joystick->GetAxisPosition(i), joystick->GetAxisPosition(i+1));

                ClampValues(val, minTolerance_);

                controls.extraData_[stickNames_[i/2]] = val;
            }
        }
    }
}

void GameController::ClampValues(Vector2 &vec, float minVal)
{
    if (Abs(vec.x_) < minVal)
    {
        vec.x_ = 0.0f;
    }
    if (Abs(vec.y_) < minVal)
    {
        vec.y_ = 0.0f;
    }

    // diagonal pts between x and y axis results in magnitude > 1, normalize
    if (vec.Length() > 1.0f)
    {
        vec.Normalize();
    }
}

