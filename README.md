# CSP + CAN + FreeRTOS

**Introduction**

The code in this repo should be a perfect blank slate from which to start with FreeRTOS & CSP implemented on an STM32 board. Specifically, an STM32F746ZGT.

**How to Use**

I used the STM32CUBEIDE IDE to autogenerate a lot of the code (as well as the .ioc file) so it is recommended that you use that IDE when building upon this template.

Do not forget to link the "libcsp.a" static library located within the "csp_build" folder, as well as adding all the include directories within "csp_build".

**How to Compile the CSP Library Yourself**

If you do not wish to use the included compiled CSP library (perhaps you want to use an updated version), you may compile your own. The instructions to do so are documented in the official CSP repo but here are some things I learned which hopefully help you somewhat:

To build a static library for CSP you need to use the WAF build system or meson (though I never had any luck with meson). As I mentioned, this is all documented within the official CSP repo (https://github.com/libcsp/libcsp). I remember having to fiddle with the 'wscript' file. I also remember having to ensure that both the build script and the STM32 code used hard floats (or at least just the same type of float).

I would say that is all the wisdom I can offer on this matter.

**Notes**

I hope you enjoy this template! If you have any issues, you can raise an issue on this repo and I will do my best to answer it or direct you to someone who can :)

