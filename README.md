[![License](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://gitlab.xfce.org/thunar-plugins/thunar-media-tags-plugin/-/blob/master/COPYING)

thunar-media-tags-plugin
====================

 The Thunar Media Tags Plugin (thunar-media-tags-plugin) adds special features for media files to the Thunar File Manager.

Currently, these are:

  * A bulk renamer option, which allows users to rename multiple audio files at once, based on their tags (e.g. ID3 or OGG/Vorbis),
  * An audio tag editor which is reachable from the file properties page,
  * A special media file page for the file properties dialog, which displays detailed information about quality, length, etc.

----

### Homepage

[Thunar-media-tags-plugin documentation](https://docs.xfce.org/xfce/thunar/media-tags)

### Changelog

See [NEWS](https://gitlab.xfce.org/thunar-plugins/thunar-media-tags-plugin/-/blob/master/NEWS) for details on changes and fixes made in the current release.

### Source Code Repository

[Thunar-media-tags-plugin source code](https://gitlab.xfce.org/thunar-plugins/thunar-media-tags-plugin)

### Download a Release Tarball

[Thunar-media-tags-plugin archive](https://archive.xfce.org/src/thunar-plugins/thunar-media-tags-plugin)
    or
[Thunar-media-tags-plugin tags](https://gitlab.xfce.org/thunar-plugins/thunar-media-tags-plugin/-/tags)
### Installation

From source: 

    % cd thunar-media-tags-plugin
    % meson setup build
    % meson compile -C build
    % meson install -C build

From release tarball:

    % tar xf thunar-media-tags-plugin-<version>.tar.xz
    % cd thunar-media-tags-plugin-<version>
    % meson setup build
    % meson compile -C build
    % meson install -C build

### Reporting Bugs

Visit the [reporting bugs](https://docs.xfce.org/xfce/thunar/thunar-media-tags-plugin/bugs) page to view currently open bug reports and instructions on reporting new bugs or submitting bugfixes.

