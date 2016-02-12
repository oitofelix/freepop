---
title: oitofelix - MININIM
description: >
  MININIM: The Advanced Prince of Persia Engine (a childhood dream)
tags: >
  C, Allegro, Prince of Persia
license: CC BY-SA 4.0
layout: oitofelix-homepage
base: http://oitofelix.github.io
#base_local: http://localhost:4000
---
<div id="markdown" markdown="1">
## MININIM

<div>
<img style="border: 0px;" src="{{ page.base_local }}{{ site.baseurl }}/mininim.png"
alt="MININIM logo">
<img src="{{ page.base_local }}{{ site.baseurl }}/game-play.png">
</div>

<blockquote markdown="1">
_Menininho_ is the Brazilian Portuguese word for _male kid_, and
_mininim_ is how it's informally pronounced throughout the country.
_Kid_ is the term used by _Jordan Mechner_<sup>1</sup> when referring
to the main character of his most famous game --- __Prince of Persia__
--- in its source code and documentation, because the character's
movements were derived from rotoscoping footage of Mechner's little
brother.  __MININIM__ is the _Advanced Prince of Persia Engine_, written
from scratch by computer programmer and free software activist _Bruno
Félix Rezende Ribeiro_ (`oitofelix`).  Bruno had a childhood dream of
deciphering the secrets of his favorite game that introduced him to
computing --- a dream dreamt when he was just a kid, a dream realized
now that he's still a _mininim_ deep in his heart.<sup>2</sup>
</blockquote>

__MININIM__ is the _Advanced Prince of Persia Engine_ --- a childhood
dream, the free software engine for _Jordan Mechner_'s masterpiece,
developed from scratch by _Bruno Félix Rezende Ribeiro_ (`oitofelix`).
_MININIM is free software_; you can redistribute it and/or modify it
under the terms of the
[GNU General Public License](https://www.gnu.org/licenses/gpl.html) as
published by the Free Software Foundation; either version 3, or (at
your option) any later version.

__MININIM__ aims to have detailed documentation on all matters
concerning the original 1989 Prince of Persia world, ranging from its
design concepts to the engine's practical use and development.
_MININIM documentation is free_; you can redistribute it and/or modify
it under the terms of the
[GNU Free Documentation Licence](https://www.gnu.org/licenses/fdl.html)
as published by the Free Software Foundation --- with no Invariant
Sections; either version 1.3, or (at your option) any later version.

### Download

MININIM currently supports _GNU/Linux_ and _Microsoft Windows_
operating systems.

If you use _GNU/Linux_, you have to build from source.  This should be
quite straightforward, provided you install the _Allegro 5_
development libraries using your distribution's package manager.  The
usual incantation `./configure && make` is often enough.  The
installation step (`make install`) is optional as MININIM is able to
run directly from its source directory.

- [0.9.1 release source code]({{ page.base_local }}{{ site.baseurl }}/mininim-0.9.1.tar.gz)

If you are under _Microsoft Windows_ a MinGW build is readily
available.  No installation is required, just extract the zip archive
and run `mininim.exe`.  The PDF build of the manual is bundled as
well.

- [0.9.1 MinGW build]({{ page.base_local }}{{ site.baseurl }}/mininim-0.9.1-win32.zip)


If you just want to have a look in the latest release documentation,
you can download the manual in PDF format separately.

- [0.9.1 PDF manual]({{ page.base_local }}{{ site.baseurl }}/mininim-0.9.1.pdf)

If you are interested in the latest features, not present yet in the
latest release, you can build from the VCS repository.  Build
instructions for both GNU/Linux and MinGW are available in the
documentation.

- [VCS repository](http://github.com/oitofelix/mininim/)


<hr>

<p markdown="1">
<small><sup>1</sup> Jordan Mechner (born June 4, 1964) is an American
video game designer, author, screenwriter, and filmmaker, best known
for creating the Prince of Persia video game franchise and the 8-bit
computer game Karateka.</small>
</p>

<p markdown="1">
<small><sup>2</sup> Of course, none of this is reason enough to name a
game engine "MININIM", if not by the fact that it's a really cool
palindrome!</small>
</p>

</div>