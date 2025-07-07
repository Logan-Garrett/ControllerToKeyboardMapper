#include <SDL.h>
#include <ApplicationServices/ApplicationServices.h>
#include <iostream>
#include <Carbon/Carbon.h>
#include <CoreGraphics/CGDisplayConfiguration.h>

constexpr bool DEBUG_FLAG = false; // Global Var for Debug.

// Need to load a Config File into a dictionary off the start that is easy for reconfigs or can be loaded as I so please.
// So far everything is hardcoded keybindings based on mac.

void Debug_Print(std::string debug_message) {
    if (DEBUG_FLAG == true) {
        std::cout << debug_message << std::endl;
    }
}

void SimulateRightClick() {
    CGEventRef ev = CGEventCreate(NULL);
    CGPoint pos = CGEventGetLocation(ev);
    CFRelease(ev);

    // Down + Up
    CGEventPost(kCGHIDEventTap, CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, pos, kCGMouseButtonRight));
    CGEventPost(kCGHIDEventTap, CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, pos, kCGMouseButtonRight));
}

void PressKey(CGKeyCode keycode) {
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, keycode, true);  // true = key down
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, keycode, false);   // false = key up

    CGEventPost(kCGHIDEventTap, keyDown);
    CGEventPost(kCGHIDEventTap, keyUp);

    CFRelease(keyDown);
    CFRelease(keyUp);
}

void HoldKey(CGKeyCode keycode) {
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, keycode, true);  // true = key down
    CGEventPost(kCGHIDEventTap, keyDown);
    CFRelease(keyDown);
}

void ReleaseKey(CGKeyCode keycode) {
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, keycode, false);   // false = key up
    CGEventPost(kCGHIDEventTap, keyUp);
    CFRelease(keyUp);
}

// Handle Trigger Actions
bool ltClicked = false;
bool rtClicked = false;
void HandleTriggerActions(SDL_Joystick * joystick) {
    const int threshold = 16000;

    int ltValue = SDL_JoystickGetAxis(joystick, 4);
    int rtValue = SDL_JoystickGetAxis(joystick, 5);

    // LT Trigger
    if (ltValue > threshold) {
        if (ltClicked == false) {
            std::cout << "LT Trigger Pressed." << std::endl;
            // PressKey(kVK_ANSI_O);
            HoldKey(kVK_ANSI_O);
            ltClicked = true;
        }
    } else {
        ReleaseKey(kVK_ANSI_O);
        ltClicked = false;
    }

    // RT Trigger
    if (rtValue > threshold) {
        if (rtClicked == false) {
            std::cout << "RT Trigger Pressed." << std::endl;
            // PressKey(kVK_ANSI_P);
            HoldKey(kVK_ANSI_P);
            rtClicked = true;
        }
    } else {
        ReleaseKey(kVK_ANSI_P);
        rtClicked = false;
    }
}

void HandleRightJoyStick(SDL_Joystick* joystick) {
    const float DEADZONE = 8000.0f;
    const float MAX_AXIS = 32768.0f;
    const float SENSITIVITY = 15.0f; // tweak output speed

    Sint16 rx = SDL_JoystickGetAxis(joystick, 2);
    Sint16 ry = SDL_JoystickGetAxis(joystick, 3);

    float fx = rx;
    float fy = ry;
    float magnitude = sqrtf(fx * fx + fy * fy);

    if (magnitude > DEADZONE) {
        // Scaled radial deadzone
        float normMag = (magnitude - DEADZONE) / (MAX_AXIS - DEADZONE);
        float scale = normMag / magnitude;

        float dx = fx * scale * (SENSITIVITY);
        float dy = fy * scale * (SENSITIVITY);

        // Invert Y for macOS coordinate system
        dy = dy;

        CGEventRef ev = CGEventCreate(NULL);
        CGPoint pos = CGEventGetLocation(ev);
        CFRelease(ev);

        CGPoint newPos = CGPointMake(pos.x + dx, pos.y + dy);
        CGWarpMouseCursorPosition(newPos);
        CGAssociateMouseAndMouseCursorPosition(true);
    }
}



// Handle Left & Right Joysticks.
bool llClicked = false;
bool lrClicked = false;
bool luClicked = false;
bool ldClicked = false;

void ControllerAxisAction(SDL_Event * event, SDL_Joystick * joystick) {
    const int DEADZONE = 8000;

    // Left Stick
    Sint16 lx = SDL_JoystickGetAxis(joystick, 0); // Axis 0: Left Stick X
    Sint16 ly = SDL_JoystickGetAxis(joystick, 1); // Axis 1: Left Stick Y

    // Left Stick Movement Stuff

    // Handle A (left)
    if (lx < -DEADZONE) {
        if (llClicked == false) {
            HoldKey(kVK_ANSI_A);
            llClicked = true;
        }
    } else {
        llClicked = false;
        ReleaseKey(kVK_ANSI_A);
    }

    // Handle D (right)
    if (lx > DEADZONE) {
        if (lrClicked == false) {
            HoldKey(kVK_ANSI_D);
            lrClicked = true;
        }
    } else {
        ReleaseKey(kVK_ANSI_D);
        lrClicked = false;
    }

    // Handle W (up)
    if (ly < -DEADZONE) {
        if (luClicked == false) {
            HoldKey(kVK_ANSI_W);
            luClicked = true;
        }
    } else {
        ReleaseKey(kVK_ANSI_W);
        luClicked = false;
    }

    // Handle S (down)
    if (ly > DEADZONE) {
        if (ldClicked == false) {
            HoldKey(kVK_ANSI_S);
            ldClicked = true;
        }
    } else {
        ReleaseKey(kVK_ANSI_S);
        ldClicked = false;
    }
}

// Handles all Button Actions or at least what registers as buttons.
void ControllerButtonAction(SDL_Event * event) {
    Debug_Print("Controller Button Action.");
    int button = event->cbutton.button;

    // Perform Action Based On Button
    switch(button) {
        case 0: // A Button
            std::cout << "Button (A) Pressed." << std::endl;
            PressKey(kVK_Space); // normally is kVK_ANSI_A but because A is a part of WASD this causes issues and needs to be registered as space.
            break;
        case 1: // B Button
            std::cout << "Button (B) Pressed." << std::endl;
            PressKey(kVK_ANSI_B);
            break;
        case 2: // X Button
            std::cout << "Button (X) Pressed." << std::endl;
            PressKey(kVK_ANSI_X);
            break;
        case 3: // Y Button
            std::cout << "Button (Y) Pressed." << std::endl;
            PressKey(kVK_ANSI_Y);
            break;
        case 4: // Back Button
            std::cout << "Back Button Pressed." << std::endl;
            PressKey(kVK_Escape);
            break;
        case 6: // Start Button
            std::cout << "Start Button Pressed." << std::endl;
            PressKey(kVK_Tab); // This may need to be adjusted. 
            break;
        case 7: // Left Joystick Down
            std::cout << "Left Joystick Pressed." << std::endl;
            PressKey(kVK_Shift);
            break;
        case 8: // Right Joystick Down
            std::cout << "Right Joystick Pressed." << std::endl;
            SimulateRightClick();
            break;
        case 9: // LB Button 
            // std::cout << "Left Bumper Pressed." << std::endl;
            PressKey(kVK_ANSI_Q);
            break;
        case 10: // RB Button
            std::cout << "Right Bumber Pressed." << std::endl;
            PressKey(kVK_ANSI_E);
            break;
        case 11: // Up D Pad
            std::cout << "Up D Pad Pressed." << std::endl;
            break;
        case 12: // Down D Pad
            std::cout << "Down D Pad Pressed." << std::endl;
            break;
        case 13: // Left D Pad
            std::cout << "Left D Pad Pressed." << std::endl;
            break;
        case 14: // Right D Pad
            std::cout << "Right D Pad Pressed." << std::endl;
            break;
        case 15: // Other (Possibly for Screen shots.)
            std::cout << "Screenshot Button Pressed." << std::endl;
            break;
        default:
            std::cout << "UnIdentified Button Pressed. " << button << std::endl;
            break;
    }
}

int main(int argc, char* argv[]) {
    // extra initial work
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;


    // Initialize SDL and confirm that it was successful.
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << std::endl;
        SDL_Quit();
        return 1;
    } else {
        std::cout << "SDL initialized successfully!" << std::endl;
    }

    // Check for Connected Controllers.
    int numJoysticks = SDL_NumJoysticks();
    if (numJoysticks < 1) {
        std::cerr << "No joysticks connected!" << std::endl;
        SDL_Quit();
        return 1;
    } else {
        std::cout << "Number of joysticks connected: " << numJoysticks << std::endl;
    }

    // Get the Joystick Id.
    SDL_Joystick* joystick = SDL_JoystickOpen(0);
    if (joystick == nullptr) {
        std::cerr << "Failed to open joystick! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    } else {
        std::cout << "Joystick opened successfully!" << std::endl;
    }

    // Spit out product info
    std::cout << "Joystick Name: " << SDL_JoystickName(joystick) << std::endl;
    std::cout << "Joystick Num Axes: " << SDL_JoystickNumAxes(joystick) << std::endl;
    std::cout << "Joystick Num Buttons: " << SDL_JoystickNumButtons(joystick) << std::endl;
    std::cout << "Joystick Num Hats: " << SDL_JoystickNumHats(joystick) << std::endl;
    std::cout << "Joystick Num Balls: " << SDL_JoystickNumBalls(joystick) << std::endl;
    std::cout << "Joystick Vendor ID: " << SDL_JoystickGetVendor(joystick) << std::endl;
    std::cout << "Joystick Product ID: " << SDL_JoystickGetProduct(joystick) << std::endl;
    std::cout << "Joystick Product Version: " << SDL_JoystickGetProductVersion(joystick) << std::endl;

    // Now start the event loop to handle joystick events.
    SDL_Event event;
    bool running = true;
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST)) / (double)SDL_GetPerformanceFrequency(); // milliseconds
        // deltaTime /= 1000.0;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_DISPLAYEVENT:
                    std::cout << "SDL_DISPLAYEVENT" << std::endl;
                    break;
                case SDL_WINDOWEVENT:
                    std::cout << "SDL_WINDOWEVENT" << std::endl;
                    break;
                case SDL_KEYDOWN:
                    std::cout << "SDL_KEYDOWN: keycode=" << event.key.keysym.sym << std::endl;
                    break;
                case SDL_KEYUP:
                    std::cout << "SDL_KEYUP: keycode=" << event.key.keysym.sym << std::endl;
                    break;
                case SDL_TEXTEDITING:
                    std::cout << "SDL_TEXTEDITING" << std::endl;
                    break;
                case SDL_TEXTEDITING_EXT:
                    std::cout << "SDL_TEXTEDITING_EXT" << std::endl;
                    break;
                case SDL_TEXTINPUT:
                    std::cout << "SDL_TEXTINPUT" << std::endl;
                    break;
                case SDL_MOUSEMOTION:
                    std::cout << "SDL_MOUSEMOTION" << std::endl;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    std::cout << "SDL_MOUSEBUTTON: button=" << static_cast<int>(event.button.button) << std::endl;
                    break;
                case SDL_MOUSEWHEEL:
                    std::cout << "SDL_MOUSEWHEEL" << std::endl;
                    break;
                case SDL_JOYAXISMOTION:
                    // std::cout << "SDL_JOYAXISMOTION" << std::endl; //Prints too much.
                    // ControllerAxisAction(&event, joystick);
                    // Left Join stick only since it acts as keys.
                    if (event.jaxis.axis < 2) {
                        ControllerAxisAction(&event, joystick);
                    }
                    break;
                case SDL_JOYBALLMOTION:
                    std::cout << "SDL_JOYBALLMOTION" << std::endl;
                    break;
                case SDL_JOYHATMOTION:
                    std::cout << "SDL_JOYHATMOTION" << std::endl;
                    break;
                case SDL_JOYBUTTONDOWN:
                    break;
                case SDL_JOYBUTTONUP:
                    ControllerButtonAction(&event);
                    break;
                case SDL_JOYDEVICEADDED:
                    std::cout << "SDL_JOYDEVICEADDED" << std::endl;
                    break;
                case SDL_JOYDEVICEREMOVED:
                    std::cout << "SDL_JOYDEVICEREMOVED" << std::endl;
                    break;
                case SDL_JOYBATTERYUPDATED:
                    std::cout << "SDL_JOYBATTERYUPDATED" << std::endl;
                    break;
                case SDL_CONTROLLERAXISMOTION:
                    std::cout << "SDL_CONTROLLERAXISMOTION: axis=" << static_cast<int>(event.caxis.axis)
                            << " value=" << event.caxis.value << std::endl;
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                case SDL_CONTROLLERBUTTONUP:
                    std::cout << "SDL_CONTROLLERBUTTON: button=" << static_cast<int>(event.cbutton.button)
                            << " state=" << static_cast<int>(event.cbutton.state) << std::endl;
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                    std::cout << "SDL_CONTROLLERDEVICEADDED" << std::endl;
                    break;
                case SDL_CONTROLLERDEVICEREMOVED:
                    std::cout << "SDL_CONTROLLERDEVICEREMOVED" << std::endl;
                    break;
                case SDL_CONTROLLERTOUCHPADDOWN:
                case SDL_CONTROLLERTOUCHPADMOTION:
                case SDL_CONTROLLERTOUCHPADUP:
                    std::cout << "SDL_CONTROLLERTOUCHPAD" << std::endl;
                    break;
                case SDL_CONTROLLERSENSORUPDATE:
                    std::cout << "SDL_CONTROLLERSENSORUPDATE" << std::endl;
                    break;
                case SDL_AUDIODEVICEADDED:
                case SDL_AUDIODEVICEREMOVED:
                    std::cout << "SDL_AUDIODEVICE" << std::endl;
                    break;
                case SDL_SENSORUPDATE:
                    std::cout << "SDL_SENSORUPDATE" << std::endl;
                    break;
                case SDL_QUIT:
                    std::cout << "SDL_QUIT" << std::endl;
                    running = false;
                    break;
                case SDL_USEREVENT:
                    std::cout << "SDL_USEREVENT" << std::endl;
                    break;
                case SDL_SYSWMEVENT:
                    std::cout << "SDL_SYSWMEVENT" << std::endl;
                    break;
                case SDL_FINGERDOWN:
                case SDL_FINGERMOTION:
                case SDL_FINGERUP:
                    std::cout << "SDL_TOUCHFINGER" << std::endl;
                    break;
                case SDL_MULTIGESTURE:
                    std::cout << "SDL_MULTIGESTURE" << std::endl;
                    break;
                case SDL_DOLLARGESTURE:
                case SDL_DOLLARRECORD:
                    std::cout << "SDL_DOLLARGESTURE" << std::endl;
                    break;
                case SDL_DROPFILE:
                case SDL_DROPTEXT:
                case SDL_DROPBEGIN:
                case SDL_DROPCOMPLETE:
                    std::cout << "SDL_DROP" << std::endl;
                    break;
                default:
                    std::cout << "UNKNOWN EVENT TYPE: " << event.type << std::endl;
                    break;
            }
            // Delay on Actions.
            // SDL_Delay(16); // 60DPS response time.
            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < 16) {
                SDL_Delay(16 - frameTime); // Cap at ~60 FPS
            }
        }

        // Handle LT and RT Triggers
        HandleTriggerActions(joystick);

        // Handle Right Joystick Fake Mouse Actions
        HandleRightJoyStick(joystick);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime); // Cap at ~60 FPS
        }
    }

    // Cleanup code
    SDL_JoystickClose(joystick);
    SDL_Quit();
    return 0;
}
