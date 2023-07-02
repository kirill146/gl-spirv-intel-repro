# gl-spirv-intel-repro
More info [here](https://community.intel.com/t5/Graphics/OpenGL-SPIR-V-compiler-failed-to-link-shaders/m-p/1501085).

This sample uses glfw for creating an OpenGL context and GLAD for loading GL functions.

Shaders are built with
```
glslc -fshader-stage=frag ps.glsl -o ps.spirv
glslc -fshader-stage=vert vs.glsl -o vs.spirv
```

Output of the program:
```
Picked Intel(R) RaptorLake-S Mobile Graphics Controller
Debug callback:
         source: SHADER_COMPILER
         type: ERROR
         id: 0x1
         severity: HIGH
         message: SHADER_ID_LINK error has been generated. GLSL link failed for program 5, "":
Info log is empty
Cannot link program
```
