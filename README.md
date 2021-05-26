# AudioStreamEffect
Apply audio effects in real time for easy comparison.  
Effects include  
    Change speed - nearest neighbor interpolation  
    Change speed - linear interpolation  
    Change speed - cubic interpolation  
    Change speed - windowed sinc interpolation  
    Low pass filter  
    High pass filter  
    Band pass filter
Selectable window functions for all kernel filters.  
    Rectangle  
    Triangle  
    Parzen  
    Hann  
    Blackman  
    Dolph-Chebyshev
Written in C++ using portaudio for audio I/O, glbindings, glfw and imgui for user interface and tinyfiledialogs for dialog handling. Currently only supports the 16-bit pcm wave format.
