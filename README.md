# surf_tes3mp

This fork of **TES3MP** adds **Source engine–style surfing physics** — similar to *Counter-Strike: Source*

https://www.youtube.com/watch?v=BzXkL_etyPo

https://discord.gg/TSqQTVexe5

---

## ⚙️ Configuration

This build **does not share** the usual openmw.cfg file in  
`Documents\My Games\OpenMW\`.

All configuration files are inside the included **`config`** folder.

The two main files are:
- `config/openmw.cfg` — game data and content list  
- `config/settings.cfg` — graphics, audio, and gameplay options  
- the `data files` folder contains mod files for a port of surf utopia

---

## 🧭 How to Set Up `openmw.cfg`

Open `config/openmw.cfg` in a text editor and scroll to the bottom.  
Edit the data path to match your Morrowind install, for example:

```ini
data="C:\GOG Games\Morrowind\Data Files"
data="./Data Files"
content=Morrowind.esm
content=Tribunal.esm
content=Bloodmoon.esm
content=first_ramp.omwaddon
```
📝 If your Morrowind is installed somewhere else, change the first data="..." line to point to your own Data Files folder.

## 🧙 If you use the setup wizard to select your game path

reopen config/openmw.cfg afterwards and make sure these lines are still at the bottom:
```ini
data="./Data Files"
content=Morrowind.esm
content=Tribunal.esm
content=Bloodmoon.esm
content=first_ramp.omwaddon
```
This ensures the included surf test map loads correctly.

🌐 Surf Server

The included `tes3mp-client-default.cfg` is already set up to connect to the public Surf server — just launch `tes3mp.exe`.

🏄‍♂️
That’s it — configure openmw.cfg, launch TES3MP, and start surfing



TES3MP
======

Copyright (c) 2008-2015, OpenMW Team  
Copyright (c) 2016-2022, David Cernat & Stanislav Zhukov

TES3MP is a project adding multiplayer functionality to [OpenMW](https://github.com/OpenMW/openmw), an open-source game engine that supports playing "The Elder Scrolls III: Morrowind" by Bethesda Softworks.

* TES3MP version: 0.8.1
* OpenMW version: 0.47.0
* License: GPLv3 with additional allowed terms (see [LICENSE](https://github.com/TES3MP/TES3MP/blob/master/LICENSE) for more information)

Font Licenses:
* DejaVuLGCSansMono.ttf: custom (see [files/mygui/DejaVuFontLicense.txt](https://github.com/TES3MP/TES3MP/blob/master/files/mygui/DejaVuFontLicense.txt) for more information)

Project status
--------------

[Version changelog](https://github.com/TES3MP/TES3MP/blob/master/tes3mp-changelog.md)

As of version 0.8.1, TES3MP is fully playable, providing very extensive player, NPC, world and quest synchronization, as well as state saving and loading, all of which are highly customizable via [serverside Lua scripts](https://github.com/TES3MP/CoreScripts).

Remaining gameplay problems mostly relate to AI and the fact that clientside script variables need to be placed on a synchronization whitelist to avoid packet spam.

TES3MP now also has a [VR branch](https://github.com/TES3MP/TES3MP/tree/0.8.1-vr) that combines its code with that of Mads Buvik Sandvei's [OpenMW VR](https://gitlab.com/madsbuvi/openmw).

Donations
---------------

You can benefit the project by donating on Patreon to our two developers, [David Cernat](https://www.patreon.com/davidcernat) and [Koncord](https://www.patreon.com/Koncord), as well as by supporting [OpenMW](https://openmw.org).

Contributing
---------------

Helping us with documentation, bug hunting and video showcases is always greatly appreciated.

For code contributions, it's best to start out with modestly sized fixes and features and work your way up. There are so many different possible implementations of more major features – many of which would cause undesirable code or vision conflicts with OpenMW – that those should be talked over in advance with the existing developers before effort is spent on them.

Feel free to contact the [team members](https://github.com/TES3MP/TES3MP/blob/master/tes3mp-credits.md) for any questions you might have.

Getting started
---------------

* [Quickstart guide](https://github.com/TES3MP/TES3MP/wiki/Quickstart-guide)
* [Steam group](https://steamcommunity.com/groups/mwmulti) and its [detailed FAQ](https://steamcommunity.com/groups/mwmulti/discussions/1/353916184342480541/)
* [TES3MP section on OpenMW forums](https://forum.openmw.org/viewforum.php?f=45)
* [Discord server](https://discord.gg/ECJk293)
* [Subreddit](https://www.reddit.com/r/tes3mp)
* [Known issues and bug reports](https://github.com/TES3MP/TES3MP/issues)
