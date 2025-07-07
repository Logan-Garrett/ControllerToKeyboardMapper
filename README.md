# ControllerToKeyboardMapper

The main issue with this is it falls flat on its face when trying to run over RDP due to the way it handles Abolsute and relative mouse interactions for the right joystick when acting as a mouse.

g++ main.cpp -o MapController `sdl2-config --cflags --libs` -framework ApplicationServices -framework CoreGraphics
