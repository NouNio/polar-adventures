:: vcpkg list

:: add all the necessary packages, assuming os is win-64. if your os is 32, just swapping the numbers should suffice
:: note that there is currently an error with ASSIMP in vcpkg, so that one needs to be resolved manually
set list=glfw3, glew, glm, stb, freetype, inih, bullet3

(for %%p in (%list%) do (
	vcpkg install %%p:x64-windows
))