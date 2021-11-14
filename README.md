# 3DModelViewer
This is a 3D model viewer based on OpenGL.  
The file to be displayed can be changed only by changing the macro definition of FILE in 3DModelViewer.cpp.  
If there is no normal vector in the .obj file or the origin normal vectors are not expected, the new normal vectors can be computed by defining the macro CALCULATE_NORMAL in 3DModelViewer.cpp.  
