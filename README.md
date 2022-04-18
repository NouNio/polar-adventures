# Polar Adventures :snowflake: :white_medium_square:
--- 
---
### Description
This is project designign and manufacturing a game using a low level GPU API. In particular the OpenGL framework was used. <br>
The project is done for the Computer Graphics course at TU Wien. <br>
<img src="https://github.com/NouNio/polar-adventures/blob/main/poster.jpg" width="250" height="375"> <br>
A poster done for the game project, as it could be displayed in advertisments or packaging.

### Documentation
The documentation is written in Latex and can be found on Overleaf [here](https://www.overleaf.com/read/gjymnhtjwbmf).

### Dependencies
---
- OpenGL v. >= 3.3
- GLFW
- GLEW
- glm
- assimp
- stb_image
- freetype
- inih
- bullet3
- boost
- irrKlang
- ...

It is recommended to resolve all dependencies using [vcpkg](https://vcpkg.io/en/index.html).
The package manager can be installed via: <br>
```shell
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
```
Then add vcpkg to PATH (Environment Variables -> Path -> add absolute path to the cloned vcpkg directory) <br>
and install any _package_ with:
```
vcpkg install package
```
Also make sure that vcpkg and VS are being connected with:
```
vcpkg integrate install
```
Assuming the above was done, the script add_depencies, will resolve all packages via vcpkg (Assuming the OS is WIN>=10 and 64-bit):
```
.\add_dependencies.bat
```
#### Notes & Issues
There is currently an [issue with vcpkg and assimp](https://github.com/microsoft/vcpkg/issues/21605), until the issue is resolved it needs to be built manually. <br>
<br>
Also there is an [issue with bullet and vcpkg](https://github.com/microsoft/vcpkg/issues/7877), where old submodules don't get included by vcpkg automatically, due to a missing include command.<br>
To resolve this, one needs to add `your-path-to-vcpkg\vcpkg\installed\x64-windows\include\bullet` to the _additional include directories_ in the VS project.

### References and Resources
[1]  [Learn OpenGL](https://learnopengl.com/About) by [Joey de Vries](https://github.com/JoeyDeVries) <br>
[2]  [Open game assets](https://opengameart.org/) by [Kenny](https://opengameart.org/users/kenney) <br>
[3]  [Open fonts](https://www.fontspace.com/category/pixel), specifically this [pixel font](https://www.fontspace.com/datcub-font-f69195) <br>
[4]  Nischwitz, Alfred, et al. Computergrafik: Band I des Standardwerks Computergrafik und Bildverarbeitung. Springer-Verlag, 2019. <br>
[5]  Nischwitz, Alfred, et al. Bildverarbeitung: Band II des Standardwerks Computergrafik und Bildverarbeitung. Springer-Verlag, 2020. <br>
[6]  Koster, Raph. Theory of fun for game design. O'Reilly, 2013. <br>
[7]  Kosarevsky, Sergey, et al. 3D Graphics Rendering Cookbook. Packt, 2021. <br>
[8]  a general [character controller guide](https://digitalrune.github.io/DigitalRune-Documentation/html/7cc27ced-9a65-4ddd-8b8e-fa817b7fe6b7.htm) by digital rune <br>
[9]  [opengl-tutorial](http://www.opengl-tutorial.org/) <br>
[10] a [web-video-series for bullet physics ](https://www.youtube.com/watch?v=wbu5MdsFYko) by thecplusplusguy <br>
[11] [bullet physics docs](https://pybullet.org/Bullet/BulletFull/index.html)
