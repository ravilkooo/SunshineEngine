<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/ravilkooo/">
    <img src="logo/SunshineLogo_transparent.png" alt="Logo" width="256" height="256">
  </a>

<h2 align="center">Sunshine Engine</h2>

  <p align="center">
    Game Engine
    <br />
    <a href="https://github.com/ravilkooo/SunshineEngine"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/ravilkooo/">View Demo</a>
    &middot;
    <a href="https://github.com/ravilkooo/SunshineEngine/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    &middot;
    <a href="https://github.com/ravilkooo/SunshineEngine/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->
![2026-01-23_14-35](https://github.com/user-attachments/assets/bf6cb502-2bc3-48a5-a56a-103341e29493)

Sunshine Engine is a **3D game engine** built with **C++**.

### Core Features

**Graphics**
- Deferred rendering system with multiple render passes
- Support for meshes, textures
- Particle system with compute shaders
- Shadow mapping and lighting
- Debug visualization (perception radii, emitters, icons)

**Physics**
- Jolt Physics integration for collision detection and rigid body dynamics
- Physics components on game objects

**Scripting**
- **Lua integration** via Sol2 for game logic
- Component-based scripting system
- Lua bindings for engine systems (camera, input, perception, physics,...)

**Audio**
- FMOD integration for sound playback and management

**Game Objects & Components**
- GameObject-Component architecture
- Built-in components: Transform, Render (hidden), Mesh, Physics, Lua, Perception, Behaviour
- Player object with camera control and input systems
- JSON serialization for scenes and objects

**AI & Perception**
- Perception component for NPC awareness (sight, hearing)
- Behavior tree system via Lua
- Debug visualization of perception radii

**Editor**
- **SunshineEditor** - ImGui-based world editor
- Scene management and object editing
- Content browser
- Properties panel for game objects
- Camera preview with miniature viewport

**Input System**
- Robust input manager with edge detection (pressed/released/held)
- Support for keyboard and mouse
- Key-to-Lua-function mapping for player controllers

### Architecture

- **SunshineEngine** (LIB) - Core engine systems
- **SunshineEditor** (EXE) - Editor application
- **Projects** folder - Game projects using the engine
- Modular rendering with render passes and techniques

The engine is designed for **indie game development** with a focus on **scripting-driven gameplay**.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With

* C++
* DirectX

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started


### Prerequisites

* <a href="https://cmake.org/"> CMake </a> (3.20+)
* Visual Studio 2022 or JetBrains Rider

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/ravilkooo/SunshineEngine.git
   ```
2. Configure projects using CMake
  
    **Option 1**

    Run `configure_solution.bat`

    **Option 2**
    
    Or open cmd in repo folder and run this commands:
   ```sh
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```
3. Open project `Sunshine.sln`
4. Build `ALL_BUILD` (see in *CMakePredefinedTargets*)
5. Copy fmodL.dll file to build: from `$RepoFolder$\ThirdParty\FMOD\api\lib\x64` to `$RepoFolder$\build\bin\Debug`
6. Copy fmod.dll file to build: from `$RepoFolder$\ThirdParty\FMOD\api\lib\x64` to `$RepoFolder$\build\bin\Release`
7. '*Set as Startup Project*' on `SunshineEditor` (VS 2022)
8. Run Editor in *Debug* or *Release* configuration

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

Make simple games with our Sunshine Engine!

Link to wiki:
https://github.com/ravilkooo/SunshineEngine/wiki

Link to videoguide:
[YouTube]([https://youtu.be/PQfZMqGmwHU](https://youtube.com/playlist?list=PLYaUtvngCOWkHuXNp-mbLQ_tkmgm7sHEm&si=J-wMjmR6qrSwSEhl))

[VK]([https://vk.com/video359543554_456240406](https://vkvideo.ru/playlist/359543554_2))


<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [x] Add Graphics module
- [x] Add Physics and Collision module (Jolt)
- [x] Add audio module (fmod)
- [x] Lua integration
- [x] World Editor
- [x] PlayerController
- [x] Perception system
- [x] Behavior action-pattern system
- [ ] Navigation mesh

See the [open issues](https://github.com/ravilkooo/SunshineEngine/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Top contributors:

<a href="https://github.com/ravilkooo/SunshineEngine/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=ravilkooo/SunshineEngine" alt="contrib.rocks image" />
</a>


<!-- LICENSE -->
## License


<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

[Ravil](https://github.com/ravilkooo) - ravilka.zftsh@yandex.ru

[Maria20Maria20](https://github.com/Maria20Maria20)

Project Link: [https://github.com/ravilkooo/SunshineEngine](https://github.com/ravilkooo/SunshineEngine)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/ravilkooo/SunshineEngine.svg?style=for-the-badge
[contributors-url]: https://github.com/ravilkooo/SunshineEngine/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/ravilkooo/SunshineEngine.svg?style=for-the-badge
[forks-url]: https://github.com/ravilkooo/SunshineEngine/network/members
[stars-shield]: https://img.shields.io/github/stars/ravilkooo/SunshineEngine.svg?style=for-the-badge
[stars-url]: https://github.com/ravilkooo/SunshineEngine/stargazers
[issues-shield]: https://img.shields.io/github/issues/ravilkooo/SunshineEngine.svg?style=for-the-badge
[issues-url]: https://github.com/ravilkooo/SunshineEngine/issues
[license-shield]: https://img.shields.io/github/license/ravilkooo/SunshineEngine.svg?style=for-the-badge
[license-url]: https://github.com/ravilkooo/SunshineEngine/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username
[product-screenshot]: logo/SunshineLogo.png
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com 
